#include "world/map/CMapObj.hpp"
#include "world/map/CMap.hpp"
#include "util/SFile.hpp"

TSHashTable<CMapObj, HASHKEY_STRI> CMapObj::mapObjHashtable;

// OFFSET: 0x7D80C0
bool CMapObj::Read(char* fileName) {
    SStrCopy(this->m_wmoName, fileName);

    SFile* file;
    if (!CMap::SafeOpen(fileName, &file)) {
        CMap::SafeOpen("world\\wmo\\Dungeon\\test\\missingwmo.wmo", &file);
        SStrCopy(this->m_wmoName, "world\\wmo\\Dungeon\\test\\missingwmo.wmo");
    }

    auto fileSize = SFile::GetFileSize(file, 0);
    this->wmoFileSize = fileSize;
    this->pWmoData = STORM_ALLOC(fileSize);
    auto asyncObject = AsyncFileReadAllocObject();
    this->asyncObject = asyncObject;
    asyncObject->file = file;
    this->asyncObject->userArg = this;
    this->asyncObject->buffer = this->pWmoData;
    this->asyncObject->size = this->wmoFileSize;
    this->asyncObject->userPostloadCallback = CMapObj::PostloadCallback;
    this->asyncObject->priority = 124;
    AsyncFileReadObject(this->asyncObject, 0);
    // HashTable::AddEntry(&CMapObj::s_asyncLoadQueue, (char *)this);
    return true;
}

// OFFSET: 0x7D7470
void CMapObj::Load() {
    SIffChunk* headerChunk = reinterpret_cast<SIffChunk*>(static_cast<char*>(this->pWmoData) + 12);
    this->header = headerChunk->Data<SMOHeader>();

    SIffChunk* textureNameListChunk = headerChunk->Next();
    this->textureNameList = textureNameListChunk->Data<char>();
    this->texturesSize = textureNameListChunk->size;

    SIffChunk* materialListChunk = textureNameListChunk->Next();
    this->materialList = materialListChunk->Data<SMOMaterial>();
    this->materialsCount = materialListChunk->size / sizeof(SMOMaterial);

    SIffChunk* groupNameListChunk = materialListChunk->Next();
    this->groupNameList = groupNameListChunk->Data<char>();
    this->groupNameSize = groupNameListChunk->size;

    SIffChunk* groupInfoChunk = groupNameListChunk->Next();
    this->groupInfo = groupInfoChunk->Data<SMOGroupInfo>();
    this->groupInfoCount = groupInfoChunk->size / sizeof(SMOGroupInfo);

    SIffChunk* skyboxChunk = groupInfoChunk->Next();
    this->skybox = skyboxChunk->Data<char>();
    if (this->skybox[0] == '\0')
        this->skybox = nullptr;

    if (!this->skybox && this->groupInfoCount) {
        for (uint32_t i = 0; i < this->groupInfoCount; i++) {
            this->groupInfo[i].flags &= ~0x40000;
        }
    }

    SIffChunk* portalVertexListChunk = skyboxChunk->Next();
    this->portalVertexList = portalVertexListChunk->Data<C3Vector>();
    this->planeVertCount = portalVertexListChunk->size / sizeof(C3Vector);

    SIffChunk* portalListChunk = portalVertexListChunk->Next();
    this->portalList = portalListChunk->Data<SMOPortal>();
    this->portalsCount = portalListChunk->size / sizeof(SMOPortal);

    for (int32_t i = 0; i < this->portalsCount; i++) {
        if (std::_Is_nan(this->portalList[i].plane.d)) {
            this->portalList[i].plane.n = { 0.0f, 0.0f, 1.0f };
            this->portalList[i].plane.d = 800000.0f;
        }
    }

    SIffChunk* portalRefListChunk = portalListChunk->Next();
    this->portalRefList = portalRefListChunk->Data<SMOPortalRef>();
    this->portalRefCount = portalRefListChunk->size / sizeof(SMOPortalRef);

    SIffChunk* visBlockVertListChunk = portalRefListChunk->Next();
    this->visBlockVertList = visBlockVertListChunk->Data<C3Vector>();
    this->visBlockVertCount = visBlockVertListChunk->size / sizeof(C3Vector);

    SIffChunk* visBlockListChunk = visBlockVertListChunk->Next();
    this->visBlockList = visBlockListChunk->Data<SMOVisibleBlock>();
    this->visBlockCount = visBlockListChunk->size / sizeof(SMOVisibleBlock);

    SIffChunk* lightListChunk = visBlockListChunk->Next();
    this->lightList = lightListChunk->Data<SMOLight>();
    this->lightsCount = lightListChunk->size / sizeof(SMOLight);

    SIffChunk* doodadSetListChunk = lightListChunk->Next();
    this->doodadSetList = doodadSetListChunk->Data<SMODoodadSet>();
    this->doodadSetCount = doodadSetListChunk->size / sizeof(SMODoodadSet);

    SIffChunk* doodadNameListChunk = doodadSetListChunk->Next();
    this->doodadNameList = doodadNameListChunk->Data<char>();
    this->doodadNameSize = doodadNameListChunk->size;

    SIffChunk* doodadDefListChunk = doodadNameListChunk->Next();
    this->doodadDefList = doodadDefListChunk->Data<SMODoodadDef>();
    this->doodadDefCount = doodadDefListChunk->size / sizeof(SMODoodadDef);

    SIffChunk* fogListChunk = doodadDefListChunk->Next();
    this->fogList = fogListChunk->Data<SMOFog>();
    this->fogsCount = fogListChunk->size / sizeof(SMOFog);

    SIffChunk* fileEnd = (SIffChunk*)((char*)this->pWmoData + this->wmoFileSize);
    SIffChunk* convexVolumeChunk = fogListChunk->Next();
    if (convexVolumeChunk < fileEnd && convexVolumeChunk->token == 'MCVP') {
        this->convexVolumePlanes = convexVolumeChunk->Data<C4Plane>();
        this->convexVolumePlaneCount = convexVolumeChunk->size / sizeof(C4Plane);
    }
}

void CMapObj::PrepareUpdate() {

}

// OFFSET: 0x7B0CC0
CMapObj* CMapObj::Create(char* fileName) {
    CMapObj* mapObj = CMapObj::mapObjHashtable.Ptr(fileName);
    if (mapObj) {
        mapObj->refCount++;
        return mapObj;
    }

    mapObj = CMap::AllocMapObj();
    if (!mapObj->Read(fileName)) {
        //NOP();
    }
    uint32_t hashval = SStrHashHT(fileName);
    CMapObj::mapObjHashtable.Insert(mapObj, hashval, fileName);
    mapObj->refCount = 1;
    return mapObj;
}

// OFFSET: 0x7D8050
void CMapObj::PostloadCallback(void* arg) {
    CMapObj* mapObj = static_cast<CMapObj*>(arg);

    AsyncFileReadDestroyObject(mapObj->asyncObject);
    mapObj->asyncObject = nullptr;

    //unk_1C4 = a1->unk_1C4;
    //if (unk_1C4) {
    //    unk_1C8 = a1->unk_1C8;
    //    if ((unk_1C8 & 1) == 0 && unk_1C8)
    //        v9 = (int32_t*)((char*)p_unk_1C4 + unk_1C8 - *(_DWORD*)(unk_1C4 + 4));
    //    else
    //        v9 = (_DWORD*)(unk_1C8 & 0xFFFFFFFE);
    //    *v9 = unk_1C4;
    //    *(_DWORD*)(*p_unk_1C4 + 4) = a1->unk_1C8;
    //    *p_unk_1C4 = 0;
    //    a1->unk_1C8 = 0;
    //}
    //savedregs = v10;
    mapObj->Load();
    //bn_CMapObj_CreateMaterials(a1);
    //a1->argb_color = a1->header->ambColor;
    //header = a1->header;
    //x = header->bounding_box.min.x;
    //header = (SMOHeader*)((char*)header + 36);
    //a1->bbox.min.x = x;
    //LODWORD(a1->bbox.min.y) = header->nGroups;
    //LODWORD(a1->bbox.min.z) = header->nPortals;
    //a1->bbox.max = *(C3Vector*)&header->nLights;
    //result = (CMapObjGroup*)a1->groupInfoCount;
    //v4 = 0;
    //a1->mapObjGroupCount = (int32_t)result;
    //if (result) {
    //    mapObjGroupArray = a1->mapObjGroupArray;
    //    do {
    //        result = (CMapObjGroup*)bn_CMap_AllocMapObjGroup();
    //        *mapObjGroupArray = result;
    //        ++v4;
    //        ++mapObjGroupArray;
    //    } while (v4 < a1->groupInfoCount);
    //}
    //a1->isGroupLoaded = 1;
}
