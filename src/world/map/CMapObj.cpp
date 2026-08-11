#include "world/map/CMapObj.hpp"
#include "world/map/CMap.hpp"
#include "util/SFile.hpp"
#include <async/AsyncFileRead.hpp>
#include "gx/CGxDevice.hpp"
#include "gx/Device.hpp"

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

// OFFSET: 0x7D85E0
void CMapObj::ReadGroup(uint32_t index, bool preLoad) {
    CMapObjGroup* mapObjGroup = this->mapObjGroupArray[index];

    char path[256];
    strcpy(path, this->m_wmoName);
    char* ext = strrchr(path, '.');
    SStrPrintf(ext, 0x100u, "_%03d", index);
    strcat(path, ".wmo");

    SFile* file;
    CMap::SafeOpen(path, &file);
    auto fileSize = SFile::GetFileSize(file, 0);
    mapObjGroup->fileSize = fileSize;
    mapObjGroup->filePtr = STORM_ALLOC(fileSize);
    mapObjGroup->parent = this;
    //if (!preLoad) {
    auto allocObject = AsyncFileReadAllocObject();
    allocObject->file = file;
    allocObject->buffer = mapObjGroup->filePtr;
    allocObject->size = mapObjGroup->fileSize;
    allocObject->userArg = mapObjGroup;
    allocObject->userPostloadCallback = CMapObjGroup::AsyncPostloadCallback;
    mapObjGroup->asyncObjPtr = allocObject;
    AsyncFileReadObject(allocObject, 0);
    //HashTable::AddEntry(&stru_ADFC58, (char*)v5);
    //} else {
    //    CMap::SafeRead(Str, index, v13, v5->fileSize);
    //    CMapObjGroup::Create(v5);
    //    SFileCloseFile((void*)index);
    //}
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

// OFFSET: 0x7AE1C0
void CMapObj::WaitLoad() {
    while (this->asyncObject) {
        AsyncFileReadWait(this->asyncObject);
    }
}

// OFFSET: 0x7AEAB0
void CMapObj::WaitLoadGroup(uint32_t index) {
    auto group = this->mapObjGroupArray[index];
    while (group->asyncObjPtr) {
        AsyncFileReadWait(group->asyncObjPtr);
    }
}

// OFFSET: 0x7AE5E0
void CMapObj::GetBounds(CAaSphere* sphere) {
    if(this->isGroupLoaded) {
        sphere->c.x = (this->bbox.t.x + this->bbox.b.x) * 0.5;
        sphere->c.y = this->bbox.t.y + this->bbox.b.y * 0.5;
        sphere->c.z = (this->bbox.t.z + this->bbox.b.z) * 0.5;
        auto v4 = this->bbox.t.z - sphere->c.z;
        auto v5 = this->bbox.t.y - sphere->c.y;
        auto v6 = v5 * v5 + v4 * v4;
        auto v7 = this->bbox.t.x - sphere->c.x;
        sphere->r = sqrt(v7 * v7 + v6);
    }
    else {
        sphere->c.x = 0.0;
        sphere->r = 0.0;
        sphere->c.y = 0.0;
        sphere->c.z = 0.0;
    }
}

// OFFSET: 0x7AE5E0
void CMapObj::GetBounds(CAaBox* box) {
    if (this->isGroupLoaded) {
        *box = this->bbox;
    } else {
        box->b.x = 0.0;
        box->b.y = 0.0;
        box->t.x = 0.0;
        box->b.z = 0.0;
        box->t.y = 0.0;
        box->t.z = 0.0;
    }
}

// OFFSET: 0x7AE670
void CMapObj::GetGroupBounds(CAaSphere* sphere, int32_t index) {
    if (this->isGroupLoaded) {
        auto v3 = &this->groupInfo[index];
        sphere->c.x = (v3->boundingBox.t.x + v3->boundingBox.b.x) * 0.5;
        sphere->c.y = (v3->boundingBox.t.y + v3->boundingBox.b.y) * 0.5;
        sphere->c.z = (v3->boundingBox.t.z + v3->boundingBox.b.z) * 0.5;
        auto v6 = v3->boundingBox.t.z - sphere->c.z;
        auto v7 = v3->boundingBox.t.y - sphere->c.y;
        auto v8 = v7 * v7 + v6 * v6;
        auto v9 = v3->boundingBox.t.x - sphere->c.x;
        sphere->r = sqrt(v9 * v9 + v8);
    } else {
        sphere->c.x = 0.0;
        sphere->r = 0.0;
        sphere->c.y = 0.0;
        sphere->c.z = 0.0;
    }
}

// OFFSET: 0x7AE670
void CMapObj::GetGroupBounds(CAaBox* box, int32_t index) {
    if (this->isGroupLoaded) {
        *box = this->groupInfo[index].boundingBox;
    } else {
        box->b.x = 0.0;
        box->b.y = 0.0;
        box->t.x = 0.0;
        box->b.z = 0.0;
        box->t.y = 0.0;
        box->t.z = 0.0;
    }
}

// OFFSET: 0x7AE7B0
uint32_t CMapObj::GetGroupFlags(int32_t index) {
    if (this->isGroupLoaded)
        return this->groupInfo[index].flags;
    return 0;
}

// OFFSET: 0x7AEA80
CMapObjGroup* CMapObj::GetGroup(int32_t index, bool a3) {
    if (!this->isGroupLoaded)
        return 0;
    auto result = this->mapObjGroupArray[index];
    if ((result->unkLoadedFlag & 1) == 0 && !a3)
        return 0;
    return result;
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
    mapObj->argb_color = mapObj->header->ambColor;
    mapObj->bbox = mapObj->header->bounding_box;
    mapObj->mapObjGroupCount = mapObj->groupInfoCount;
    if (mapObj->groupInfoCount) {
        for (int32_t i = 0; i < mapObj->groupInfoCount; i++) {
            mapObj->mapObjGroupArray[i] = CMap::AllocMapObjGroup();
        }
    }
    mapObj->isGroupLoaded = 1;
}

// OFFSET: 0x7AB1E0
void CMapObj::RenderGroupCollidable(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3) {
    if (a3)
        return;

    g_theGxDevicePtr->RsPush();
    int32_t polyFillOriginal = g_theGxDevicePtr->MasterEnable(GxMasterEnable_PolygonFill);
    g_theGxDevicePtr->RsSet(GxRs_Fog, 0);
    //SetRenderModeLight();
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, nullptr);
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, 1);
    g_theGxDevicePtr->RsSet(GxRs_BlendingMode, 0);
    //m_data = v7->m_appRenderStates.m_data;
    //v10 = CGxDevice::s_alphaRef[m_data[6].m_value.m_data.i[0]];
    //p_m_data = &v7->m_appRenderStates.m_data;
    //if (m_data[7].m_value.m_data.i[0] != v10) {
    //    CGxDevice::IRsDirty(v7, GxRs_AlphaRef);
    //    (*p_m_data)[7].m_value.m_data.i[0] = v10;
    //    v7 = g_theGxDevicePtr;
    //}
    g_theGxDevicePtr->RsSet(GxRs_MatDiffuse, 0x80CCCCCC);
    g_theGxDevicePtr->RsSet(GxRs_DepthWrite, 1);
    CMapObj::RenderGroupCollidableFaces(mapObjGroup);
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, 0);
    g_theGxDevicePtr->RsSet(GxRs_BlendingMode, 2);
    //v18 = v16->m_appRenderStates.m_data;
    //v19 = CGxDevice::s_alphaRef[v18[6].m_value.m_data.i[0]];
    //v20 = &v16->m_appRenderStates.m_data;
    //if (v18[7].m_value.m_data.i[0] != v19) {
    //    CGxDevice::IRsDirty(v16, GxRs_AlphaRef);
    //    (*v20)[7].m_value.m_data.i[0] = v19;
    //    v16 = g_theGxDevicePtr;
    //}
    g_theGxDevicePtr->RsSet(GxRs_MatDiffuse, 0x80111111);
    g_theGxDevicePtr->RsSet(GxRs_DepthWrite, 0);
    CMapObj::RenderGroupCollidableFaces(mapObjGroup);
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, polyFillOriginal);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x7A76C0
void CMapObj::RenderGroupCollidableFaces(CMapObjGroup* mapObjGroup) {
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 24, mapObjGroup->vertexListCount);
    C3Vector* vertexBuffer = (C3Vector*)g_theGxDevicePtr->BufLock(vertexBuf);

    for (int32_t i = 0; i < mapObjGroup->vertexListCount; i++) {
        *vertexBuffer++ = mapObjGroup->vertexList[i];
        *vertexBuffer++ = mapObjGroup->normalList[i];
    }

    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk1C = 1;
    GxPrimVertexPtr(vertexBuf, GxVBF_PN);

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, 3000);
    uint16_t* indexBuffer = (uint16_t*)g_theGxDevicePtr->BufLock(indexBuf);
    uint32_t indexCount = 0;

    for (int32_t i = 0; i < mapObjGroup->polyListSize; i++) {
        uint8_t flags = mapObjGroup->polyList[i].flags;
        if ((flags & 0x8) != 0 || ((flags & 0x20) != 0 && (flags & 0x4) == 0)) {
            const uint16_t* src = &mapObjGroup->indices[i * 3];

            *indexBuffer++ = *src++;
            *indexBuffer++ = *src++;
            *indexBuffer++ = *src++;
            indexCount += 3;

            if (indexCount == 3000) {
                indexCount = 0;

                g_theGxDevicePtr->BufUnlock(indexBuf, 0);
                indexBuf->unk1C = 1;
                GxPrimIndexPtr(indexBuf);

                CGxBatch batch;
                batch.m_maxIndex = mapObjGroup->vertexListCount;
                batch.m_primType = GxPrim_Triangles;
                batch.m_start = 0;
                batch.m_count = 3000;
                batch.m_minIndex = 0;
                g_theGxDevicePtr->Draw(&batch, 1);
                indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, 3000);
                indexBuffer = (uint16_t*)g_theGxDevicePtr->BufLock(indexBuf);
            }
        }
    }

    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk1C = 1;
    if (indexCount) {
        GxPrimIndexPtr(indexBuf);

        CGxBatch batch;
        batch.m_maxIndex = mapObjGroup->vertexListCount;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = indexCount;
        batch.m_minIndex = 0;
        g_theGxDevicePtr->Draw(&batch, 1);
    }
}
