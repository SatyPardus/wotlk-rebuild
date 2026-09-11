#include "world/map/CMapObj.hpp"
#include "world/map/CMap.hpp"
#include "util/SFile.hpp"
#include <async/AsyncFileRead.hpp>
#include "gx/CGxDevice.hpp"
#include "gx/Device.hpp"
#include "world/CWorld.hpp"
#include "world/CWorldScene.hpp"
#include <gx/RenderState.hpp>
#include <gx/Transform.hpp>
#include <world/CWorldMath.hpp>
#include <tempest/Intersect.hpp>
#include "world/daynight/DayNight.hpp"
#include "gx/shader/CShaderEffect.hpp"
#include <world/daynight/DNInfo.hpp>

TSHashTable<CMapObj, HASHKEY_STRI> CMapObj::mapObjHashtable;
uint32_t CMapObj::s_renderMode = 5;
RENDER_FUNC CMapObj::s_renderGroupExteriorFunc;
RENDER_FUNC CMapObj::s_renderGroupInteriorFunc;
RENDER_CALLBACK CMapObj::gRenderCallback;
void* CMapObj::gRenderUserParam;
CImVector CMapObj::s_lastSidnColor;
CShaderEffect* CMapObj::s_unifiedShaders[14];
int32_t CMapObj::s_lightingMode = -1;
static int32_t s_fogMode = -1; // dword_CFBEB0

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

// OFFSET: 0x7AEC30
int32_t CMapObj::GetDoodadSet(uint16_t index) {
    if (!this->isGroupLoaded || index >= this->doodadDefCount || !this->doodadSetCount)
        return -1;

    for (int32_t i = 0; i < this->doodadSetCount; i++) {
        SMODoodadSet* set = &this->doodadSetList[i];

        if (set->count && index >= set->startIdx && index <= set->startIdx + set->count - 1)
            return i;
    }

    return -1;
}

// OFFSET: 0x7AE7B0
uint32_t CMapObj::GetGroupFlags(int32_t index) {
    if (this->isGroupLoaded)
        return this->groupInfo[index].flags;
    return 0;
}

// OFFSET: 0x7AEB10
SMOGroupInfo* CMapObj::GetGroupInfo(int32_t index) {
    if (this->isGroupLoaded)
        return &this->groupInfo[index];
    return nullptr;
}

// OFFSET: 0x7AE4C0
bool CMapObj::IsGroupLoaded(int32_t index) {
    if (this->isGroupLoaded)
        return this->mapObjGroupArray[index]->unkLoadedFlag & 1;
    return 0;
}

// OFFSET: 0x7AE140
uint32_t CMapObj::CreateWmoIgnoreFlags(uint32_t a1) {
    uint32_t result = 0xEE;
    if ((a1 & 0x80u) != 0)
        return 0x2CA;
    if ((a1 & 0x10) != 0)
        result = 0xC6;
    if ((a1 & 0x20) != 0)
        result &= 0xDBu;
    if ((a1 & 0x40) == 0)
        result &= ~2u;
    if ((a1 & 0x4000) == 0)
        result &= ~0x40u;
    if ((a1 & 0x1000000) != 0) {
        if ((a1 & 0x40000000) != 0)
            return 0xFFFFFCDB;
        else
            return result | 0x100;
    }
    return result;
}

// OFFSET: 0x7AEAE0
char* CMapObj::GetGroupName(int32_t index) {
    if (!this->isGroupLoaded)
        return nullptr;
    auto result = this->mapObjGroupArray[index];
    if ((result->unkLoadedFlag & 1) == 0)
        return nullptr;
    return result->groupName;
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

// OFFSET: 0x7AFEE0
void CMapObj::Initialize() {
    CWorldScene::s_portalStamp = 0;
    CWorldScene::s_curMapObjDef = nullptr;
    CWorldScene::s_stampedMapObjDef = nullptr;
    //dword_ADFF50 = 2048;
    //if (stru_D1BEE8.m_count + 1024 > stru_D1BEE8.m_alloc)
    //    maybe_Liquid__AddConfigChangedCallback(&stru_D1BEE8, stru_D1BEE8.m_count + 1024);
    //stru_D1BEE8.m_chunk = 1024;
    CMapObj::s_unifiedShaders[7] = CShaderEffectManager::GetEffect("MapObjDiffuse");
    CMapObj::s_unifiedShaders[8] = CShaderEffectManager::GetEffect("MapObjSpecular");
    CMapObj::s_unifiedShaders[9] = CShaderEffectManager::GetEffect("MapObjMetal");
    CMapObj::s_unifiedShaders[10] = CShaderEffectManager::GetEffect("MapObjEnv");
    CMapObj::s_unifiedShaders[11] = CShaderEffectManager::GetEffect("MapObjOpaque");
    CMapObj::s_unifiedShaders[12] = CShaderEffectManager::GetEffect("MapObjEnvMetal");
    CMapObj::s_unifiedShaders[13] = 0;
    CMapObj::s_unifiedShaders[0] = CShaderEffectManager::GetEffect("MapObjUDiffuse");
    CMapObj::s_unifiedShaders[1] = CShaderEffectManager::GetEffect("MapObjUSpecular");
    CMapObj::s_unifiedShaders[2] = CShaderEffectManager::GetEffect("MapObjUMetal");
    CMapObj::s_unifiedShaders[3] = CShaderEffectManager::GetEffect("MapObjUEnv");
    CMapObj::s_unifiedShaders[4] = CShaderEffectManager::GetEffect("MapObjUOpaque");
    CMapObj::s_unifiedShaders[5] = CShaderEffectManager::GetEffect("MapObjUEnvMetal");
    CMapObj::s_unifiedShaders[6] = CShaderEffectManager::GetEffect("MapObjUComposite");
    //CMapObj::bPoolsDirty = 1;
    //v0 = SMemAlloc(4, ".\\MapObj.cpp", 88, 0);
    //if (v0) {
    //    *v0 = ObjectAllocAddHeap(36, 128, "MAPOBJOCC", 1);
    //    CMapObj::occluderHeap = v0;
    //} else {
    //    CMapObj::occluderHeap = 0;
    //}
}

// OFFSET: 0x7AD020
void CMapObj::PrepareUpdate() {
    ++CWorldScene::s_portalStamp;
    CWorldScene::s_curMapObjDef = 0;
    CWorldScene::s_stampedMapObjDef = 0;
    //bn_TSGrowableArray_C3Vector_SetCount(&stru_D1BEE8, 0);
    //dword_CFBEC8 = 0;
    switch (CMapObj::s_renderMode) {
    case 0:
        //CMapObj::s_renderGroupExteriorFunc = CMapObj::RenderGroupCollisionFaces;
        //CMapObj::s_renderGroupInteriorFunc = CMapObj::RenderGroupCollisionFaces;
        break;
    case 1:
        //CMapObj::s_renderGroupExteriorFunc = CMapObj::RenderGroupDetailFaces;
        //CMapObj::s_renderGroupInteriorFunc = CMapObj::RenderGroupDetailFaces;
        break;
    case 2:
        //CMapObj::s_renderGroupExteriorFunc = CMapObj::RenderGroupRenderFaces;
        //CMapObj::s_renderGroupInteriorFunc = CMapObj::RenderGroupRenderFaces;
        break;
    case 3:
        //CMapObj::s_renderGroupExteriorFunc = CMapObj::RenderGroupTransFaces;
        //CMapObj::s_renderGroupInteriorFunc = CMapObj::RenderGroupTransFaces;
        break;
    case 4:
        CMapObj::s_renderGroupExteriorFunc = CMapObj::RenderGroupCollidable;
        CMapObj::s_renderGroupInteriorFunc = CMapObj::RenderGroupCollidable;
        break;
    default:
        CMapObj::s_renderGroupExteriorFunc = CMapObj::ExteriorRender;
        CMapObj::s_renderGroupInteriorFunc = CMapObj::InteriorRender;
        break;
    }

    if ((CWorld::s_enables & CWorld::Enables::Enable_WMOTextures) == 0) {
        // CMapObj::s_renderGroupInteriorFunc = CMapObj::RenderGroupLightmapTex;
    }
    if ((CWorld::s_enables & CWorld::Enables::Enable_WMOLighting) == 0) {
        // CMapObj::s_renderGroupInteriorFunc = CMapObj::RenderGroupColorTex;
    }
    //m_next = CMapObj::mapObjHash.m_fulllist.m_terminator.m_next;
    //if ((CMapObj::mapObjHash.m_fulllist.m_terminator.m_next & 1) != 0 || !CMapObj::mapObjHash.m_fulllist.m_terminator.m_next) {
    //    m_next = 0;
    //}
    //while ((m_next & 1) == 0 && m_next) {
    //    v2 = *(&m_next->unk_04 + CMapObj::mapObjHash.m_fulllist.m_linkoffset);
    //    if ((v2 & 1) == 0 && v2)
    //        v3 = *(&m_next->unk_04 + CMapObj::mapObjHash.m_fulllist.m_linkoffset);
    //    else
    //        v3 = 0;
    //    bn_CMapObj_UpdateMaterials(m_next);
    //    v4 = m_next->mapObjGroupList.m_terminator.m_next;
    //    if ((v4 & 1) != 0 || !v4)
    //        v4 = 0;
    //    while ((v4 & 1) == 0 && v4) {
    //        v5 = v4->timer + FrameTime::s_tickTimeSec;
    //        v6 = *(&v4->vertsBlock + m_next->mapObjGroupList.m_linkoffset);
    //        v4->timer = v5;
    //        if (v5 > 5.0)
    //            bn_CMapObjGroup_FreeVB();
    //        v4 = v6;
    //    }
    //    if (!m_next->refCount) {
    //        v7 = m_next->flushTimer + FrameTime::s_tickTimeSec;
    //        m_next->flushTimer = v7;
    //        if (v7 > 10.0) {
    //            maybe_UnlinkBothLists(m_next);
    //            CMap::FreeMapObj(m_next);
    //        }
    //    }
    //    m_next = v3;
    //}
    //if (CMap::s_isStreamingMode)
    //    CMapObj::ProcessAsyncLoadQueue();
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
        //NOP("CMapObj::Create(): mapObj->Read(\"%s\") failed", fileName);
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
    //savedregs = material;
    mapObj->Load();
    mapObj->CreateMaterials();
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

// OFFSET: 0x7ABF50
void CMapObj::RenderGroup(int32_t groupIndex, C44Matrix& matrix, STORM_EXPLICIT_LIST(CFrustum, sceneLink)* frustumList) {
    auto group = this->GetGroup(groupIndex, false);
    //NOP();
    //if ((group->unkLoadedFlag & 2) == 0)
    //    this->AttenTransVerts(group);
    //maybe_CMapObj__SetupGroupShaderConstants(&s_mapLight->unk14);
    CShaderEffect::UpdateProjMatrix();
    uint32_t v7 = 0;
    for (auto frustum = frustumList->Head(); frustum;) {
        auto next = frustumList->Next(frustum);

        CWorldScene::FrustumSet(frustum);
        CWorldScene::FrustumXform(matrix);
        if (group->colorVertexList)
            CMapObj::s_renderGroupInteriorFunc(this, group, v7);
        else
            CMapObj::s_renderGroupExteriorFunc(this, group, v7);
        ++v7;

        frustum = next;
    }
    //if ((CWorld::enables & 0x40000000) != 0)
    //    bn_CMapObj_RenderNormals(Group);
    //if ((CWorld::enables & Enable_WMOPortals) != 0)
    //    (bn_CMapObj_RenderPortals)(Group);
}

// OFFSET: 0x7BF740
void CMapObj::CreateRefs(CMapObjGroup* mapObjGroup, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup) {
    for (uint32_t i = 0; i < mapObjGroup->doodadRefListCount; i++) {
        uint16_t doodadIndex = mapObjGroup->doodadRefList[i];
        uint32_t doodadSet = this->GetDoodadSet(doodadIndex);
        uint16_t setIndex = 0;

        if (doodadSet && doodadSet != mapObjDef->doodadSet) {
            uint32_t slot = 0;
            while (slot < 3 && doodadSet != mapObjDef->doodadSetOverrides[slot])
                slot++;

            if (slot >= 3)
                continue;
            if (doodadSet == 0)
                continue;

            setIndex = doodadSet;
        }

        SMODoodadDef* doodadDef = &this->doodadDefList[doodadIndex];
        char* doodadName = &this->doodadNameList[doodadDef->flags & 0xFFFFFF];

        CMapDoodadDef* mapDoodadDef = CMap::CreateDoodadDef(doodadIndex, doodadDef, doodadName, mapObjDef->m_hashval + 1, &mapObjDef->mat, setIndex);

        CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapDoodadDef);
        link->ref = mapObjDefGroup;

        if (setIndex)
            mapObjDefGroup->doodadDefLinkList.LinkToHead(link);
        else
            mapObjDefGroup->doodadDefLinkList.LinkToTail(link);

        if ((mapObjDefGroup->flags & 2) != 0 && (mapDoodadDef->flags & 4) == 0) {
            mapDoodadDef->flags |= 2;
        } else {
            mapDoodadDef->diffuseLightScale = 1.0f;
            mapDoodadDef->flags = (mapDoodadDef->flags & ~2u) | 4;
        }
    }

    mapObjDefGroup->flags |= 8;
}

// OFFSET: 0x7D7710
void CMapObj::CreateMaterial(uint8_t texture) {
    SMOMaterial* material = &this->materialList[texture];
    if (!material->runTimeData_2) {
        char* textureName1 = &this->textureNameList[material->texture1];
        char* textureName2 = &this->textureNameList[material->texture2];
        if (!*textureName1)
            textureName1 = "createcrappygreentexture.blp";
        if (!CShaderEffect::s_enableShaders)
            *textureName2 = 0;

        switch (material->shader) {
        case 0:
        case 1:
        case 2:
        case 4:
            textureName2 = nullptr;
            break;
        case 3:
        case 5:
        case 6:
            if (!*textureName2) {
                material->shader = 4;
                textureName2 = nullptr;
            }
        }

        material->runTimeData_2 = CMap::LoadTexture(textureName1);
        if (textureName2)
            material->runTimeData_3 = CMap::LoadTexture(textureName2);
        else
            material->runTimeData_3 = nullptr;
    }
}

// OFFSET: 0x7D72D0
void CMapObj::CreateMaterials() {
    for (int32_t i = 0; i < this->materialsCount; i++) {
        this->materialList[i].runTimeData_2 = nullptr;
        this->materialList[i].runTimeData_3 = nullptr;
    }
}

// OFFSET: 0x7AE840
bool CMapObj::TestBounds(C3Vector& start, C3Vector& end) {
    return this->isGroupLoaded && CWorldMath::VectorIntersectAABox2(this->bbox, start, end);
}

// OFFSET: 0x7AE7E0
bool CMapObj::TestBounds(CAaBox& box) {
    return this->isGroupLoaded && this->bbox.Intersects(&box);
}

// OFFSET: 0x7AE880
bool CMapObj::TestGroupBounds(C3Vector& start, C3Vector& end, uint32_t groupNum) {
    return this->isGroupLoaded && (this->mapObjGroupArray[groupNum]->unkLoadedFlag & 1) != 0 && CWorldMath::VectorIntersectAABox2(this->groupInfo[groupNum].boundingBox, start, end) != 0;
}

// OFFSET: 0x7AE880
bool CMapObj::TestGroupBounds(CAaBox& box, uint32_t groupNum, bool a4) {
    return (!a4 || this->isGroupLoaded && (this->mapObjGroupArray[groupNum]->unkLoadedFlag & 1) != 0) && this->groupInfo[groupNum].boundingBox.Intersects(&box);
}

// OFFSET: 0x7AE970
bool CMapObj::GroupBoundingBoxIntersectsSphere(C3Vector& pos, uint32_t groupNum, float radius) {
    if (!this->isGroupLoaded)
        return false;

    if ((this->mapObjGroupArray[groupNum]->unkLoadedFlag & 1) == 0)
        return false;

    const CAaBox *bounds = &this->groupInfo[groupNum].boundingBox;

    return pos.x + radius >= bounds->b.x && pos.x - radius <= bounds->t.x
        && pos.y + radius >= bounds->b.y && pos.y - radius <= bounds->t.y
        && pos.z + radius >= bounds->b.z && pos.z - radius <= bounds->t.z;
}

// OFFSET: 0x7AEF00
bool CMapObj::GetTris(CAaBox& box, uint32_t a3, uint32_t a4, CMapObjDef* mapObjDef) {
    bool result = false;
    uint16_t wmoIgnoreFlags = this->CreateWmoIgnoreFlags(a3);

    for (uint32_t i = 0; i < (uint32_t)this->groupInfoCount; i++) {
        SMOGroupInfo& info = this->groupInfo[i];

        if (box.t.x >= info.boundingBox.b.x && box.t.y >= info.boundingBox.b.y && box.t.z >= info.boundingBox.b.z && box.b.x <= info.boundingBox.t.x && box.b.y <= info.boundingBox.t.y && box.b.z <= info.boundingBox.t.z) {
            if (this->isGroupLoaded) {
                CMapObjGroup* group = this->mapObjGroupArray[i];

                if ((group->unkLoadedFlag & 1) != 0 && (group->flags & 0x80) == 0) {
                    result |= group->GetTris(box, a3, wmoIgnoreFlags, a4, mapObjDef);
                }
            }
        }
    }

    return result;
}

// OFFSET: 0x7AF0F0
bool CMapObj::GetTris(CFrustum* frustum, uint32_t flags, uint32_t a4, CMapObjDef* mapObjDef) {
    bool result = false;

    uint16_t wmoIgnoreFlags = this->CreateWmoIgnoreFlags(flags);

    CAaBox bounds = CAaBox::Bounding(frustum->corners, 8);

    for (uint32_t i = 0; i < (uint32_t)this->groupInfoCount; i++) {
        SMOGroupInfo& info = this->groupInfo[i];

        if (bounds.t.x < info.boundingBox.b.x || bounds.t.y < info.boundingBox.b.y || bounds.t.z < info.boundingBox.b.z)
            continue;

        if (bounds.b.x > info.boundingBox.t.x || bounds.b.y > info.boundingBox.t.y || bounds.b.z > info.boundingBox.t.z)
            continue;

        if (!this->isGroupLoaded)
            continue;

        CMapObjGroup* group = this->mapObjGroupArray[i];

        if ((group->unkLoadedFlag & 1) == 0 || group->flags < 0)
            continue;

        result |= group->GetTris(frustum, flags, wmoIgnoreFlags, a4, mapObjDef);
    }

    return result;
}

// OFFSET: 0x7AF520
bool CMapObj::VectorIntersectPortal(uint32_t groupIndex, C3Segment& seg, float* t, int32_t* outGroups) {
    if (!this->isGroupLoaded) {
        return false;
    }

    CMapObjGroup* group = this->mapObjGroupArray[groupIndex];

    if (!(group->unkLoadedFlag & 1)) {
        return false;
    }

    C3Vector d;
    d.x = seg.t.x - seg.b.x;
    d.y = seg.t.y - seg.b.y;
    d.z = seg.t.z - seg.b.z;

    float segLen = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
    float ooSegLen = 1.0f / segLen;

    CRay ray;
    ray.origin = seg.b;
    ray.dir = { d.x * ooSegLen, d.y * ooSegLen, d.z * ooSegLen };

    float bestT = segLen * *t;
    bool found = false;

    uint32_t portalCount = group->portalCount;

    for (uint32_t i = 0; i < portalCount; i++) {
        SMOPortalRef* ref = &this->portalRefList[group->portalStart + i];
        SMOPortal* portal = &this->portalList[ref->portalIndex];

        if (!this->isGroupLoaded) {
            continue;
        }

        if (!(this->mapObjGroupArray[ref->groupIndex]->unkLoadedFlag & 1)) {
            continue;
        }

        C3Vector hitPoint = { 0.0f, 0.0f, 0.0f };
        float hitT;

        if (!NTempest::Intersect(ray, portal->plane, &hitT, &hitPoint, 0.1f)) {
            continue;
        }

        if (hitT < 0.0f || hitT > bestT) {
            continue;
        }

        if (!NTempest::Intersect(hitPoint, &this->portalVertexList[portal->startVertex], portal->count, portal->plane.n.MajorAxis())) {
            continue;
        }

        found = true;
        bestT = hitT;

        float side = portal->plane.n.z * seg.b.z + portal->plane.n.y * seg.b.y + seg.b.x * portal->plane.n.x + portal->plane.d;

        if ((side >= 0.0f) == (ref->side > 0)) {
            outGroups[0] = groupIndex;
            outGroups[1] = ref->groupIndex;
        } else {
            outGroups[0] = ref->groupIndex;
            outGroups[1] = groupIndex;
        }
    }

    if (found) {
        *t = bestT * ooSegLen;
    }

    return found;
}

// OFFSET: 0x7AF280
bool CMapObj::VectorIntersectPortal(C3Segment& seg, float* t, int* outGroups, int useSphereTest) {
    C3Vector d;
    d.x = seg.t.x - seg.b.x;
    d.y = seg.t.y - seg.b.y;
    d.z = seg.t.z - seg.b.z;

    float segLen = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
    float ooSegLen = 1.0f / segLen;

    CRay ray;
    ray.origin = seg.b;
    ray.dir = { d.x * ooSegLen, d.y * ooSegLen, d.z * ooSegLen };

    bool found = false;
    float bestT = *t * segLen; // caller's fraction -> ray parameter

    for (int i = 0; i < this->groupInfoCount; i++) {
        bool reached;

        if (useSphereTest) {
            reached = this->GroupBoundingBoxIntersectsSphere(seg.b, i, 0.01f);
        } else {
            reached = this->TestGroupBounds(seg.b, seg.t, i);
        }

        if (!reached)
            continue;

        if (!this->isGroupLoaded)
            continue;

        CMapObjGroup* group = this->mapObjGroupArray[i];

        if (!(group->unkLoadedFlag & 1))
            continue;

        if (group->portalCount <= 0)
            continue;

        for (int j = 0; j < group->portalCount; j++) {
            SMOPortalRef* ref = &this->portalRefList[group->portalStart + j];

            C3Vector hitPoint = { 0.0f, 0.0f, 0.0f };
            float hitT;
            SMOPortal* portal = &this->portalList[ref->portalIndex];

            if (!NTempest::Intersect(ray, portal->plane, &hitT, &hitPoint, 0.1f))
                continue;

            if (hitT < 0.0f)
                continue;

            if (hitT > bestT)
                continue;

            if (!NTempest::Intersect(hitPoint, &this->portalVertexList[portal->startVertex], portal->count, portal->plane.n.MajorAxis()))
                continue;

            bestT = hitT;
            found = true;

            float side = portal->plane.n.y * seg.b.y + portal->plane.n.z * seg.b.z + portal->plane.n.x * seg.b.x + portal->plane.d;

            if ((side >= 0.0f) == (ref->side > 0)) {
                outGroups[0] = i;
                outGroups[1] = ref->groupIndex;
            } else {
                outGroups[0] = ref->groupIndex;
                outGroups[1] = i;
            }
        }
    }

    if (found)
        *t = bestT * ooSegLen;

    return found;
}

// OFFSET: 0x7A70D0
float CMapObj::CalcPortalFarthestDistance(SMOPortal* portal) {
    const float nx = CWorldScene::s_camPlaneLocal.n.x;
    const float ny = CWorldScene::s_camPlaneLocal.n.y;
    const float nz = CWorldScene::s_camPlaneLocal.n.z;
    const float d = CWorldScene::s_camPlaneLocal.d;

    const C3Vector* v = &this->portalVertexList[portal->startVertex];

    float result = 0.0f;

    for (uint32_t i = 0; i < portal->count; ++i) {
        float dist = v[i].x * nx + v[i].y * ny + v[i].z * nz + d;

        if (dist > result)
            result = dist;
    }

    return result;
}

// OFFSET: 0x7AE920
bool CMapObj::TestGroupBounds(C3Vector& point, uint32_t groupNum) {
    return this->isGroupLoaded && (this->mapObjGroupArray[groupNum]->unkLoadedFlag & 1) != 0 && this->groupInfo[groupNum].boundingBox.ContainsPoint(point);
}

// OFFSET: 0x7AF200
bool CMapObj::Intersect(C3Vector& start, C3Vector& end, float* distance, uint32_t flags, uint32_t ignoreFlags, uint32_t groupNum, int32_t* hitIndex) {
    if (!this->isGroupLoaded)
        return false;

    CMapObjGroup* group = this->mapObjGroupArray[groupNum];

    if ((group->unkLoadedFlag & 1) == 0)
        return false;

    C3Segment seg;
    seg.b = start;
    seg.t = end;

    return group->Intersect(seg, distance, flags, ignoreFlags, hitIndex);
}

// OFFSET: 0x7AB1E0
void CMapObj::RenderGroupCollidable(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3) {
    if (a3)
        return;

    g_theGxDevicePtr->RsPush();
    int32_t polyFillOriginal = g_theGxDevicePtr->MasterEnable(GxMasterEnable_PolygonFill);
    g_theGxDevicePtr->RsSet(GxRs_Fog, 0);
    CMapObj::SetRenderModeLight();
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, nullptr);
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, 1);
    g_theGxDevicePtr->RsSet(GxRs_BlendingMode, 0);
    GxRsSetAlphaRef();
    GxRsSet(GxRs_MatDiffuse, 0x80CCCCCC);
    GxRsSet(GxRs_DepthWrite, 1);
    CMapObj::RenderGroupCollidableFaces(mapObjGroup);
    GxMasterEnableSet(GxMasterEnable_PolygonFill, 0);
    GxRsSet(GxRs_BlendingMode, 2);
    GxRsSetAlphaRef();
    GxRsSet(GxRs_MatDiffuse, 0x80111111);
    GxRsSet(GxRs_DepthWrite, 0);
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

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 3000);
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
                indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 3000);
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

// OFFSET: 0x7AC6A0
void CMapObj::ExteriorRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3) {
    if ((mapObjGroup->parent->header->flags & 0x2) != 0) {
        CMapObj::UnifiedRender(mapObj, mapObjGroup, a3);
        return;
    }

    mapObjGroup->timer = 0.0;
    mapObjGroup->AllocVB();
    mapObjGroup->SetIndexVB();
    mapObjGroup->SetVertexVB();
    g_theGxDevicePtr->RsPush();
    s_fogMode = -1;
    CMapObj::s_lightingMode = -1;
    s_lastSidnColor = { 0xFF, 0xFF, 0xFF, 0xFF };
    //dword_CFBEA8 = -1;
    CGxTex* gxTex = nullptr;
    //if (CMap::s_isStreamingMode)
    //    GxTex = CTexture::GetGxTex(CWorldScene::s_defaultTexture, 1, 0);
    SMOBatch* batch = mapObjGroup->batchList;
    for (int32_t i = 0; i < mapObjGroup->extBatchCount; i++) {
        if (!a3)
            batch->flags &= 0xF;
        if ((batch->flags & 0xF0) != 0 /*|| mapObj->CullBatch(batch)*/) {
            batch++;
            continue;
        }

        batch->flags |= 0xF0u;

        auto material = &mapObj->materialList[batch->texture];
        auto texture1 = TextureGetGxTex(material->runTimeData_2, 0, nullptr);
        if (!texture1) {
            if (!gxTex) {
                batch++;
                continue;
            }
            texture1 = gxTex;
        }
        CGxTex* texture2 = nullptr;
        if (material->runTimeData_3) {
            texture2 = TextureGetGxTex(material->runTimeData_3, 0, nullptr);
            if (!texture2) {
                texture2 = gxTex;
            }
        }

        auto shader = material->shader;
        //if (!shader && !material->blendMode && !maybe_IsSceneObjectEnabled(material->runTimeData_2))
        //    shader = 4;
        
        //    SetShaderFogFromDayNight(~material->flags & 2);
        mapObjGroup->SetLighting((material->flags & 1) == 0 ? 1 : 0);
        //    if ((v6->flags & 0x48) != 0) {
        //        if (dword_CFBEA8) {
        //            dword_CFBEA8 = 0;
        //            bn_CShadowCache_SetShadowMapGenericInterior(0);
        //            ShadowValue = CShadowCache::GetShadowValue();
//LABEL_26:   
        //            dword_D43010 = ShadowValue;
        //        }
        //    } else if (dword_CFBEA8 != 1) {
        //        dword_CFBEA8 = 1;
        //        bn_CShadowCache_SetShadowMapGenericInterior(1);
        //        ShadowValue = CShadowCache::GetShadowValue() != 0;
        //        goto LABEL_26;
        //    }
        GxRsSet(GxRs_Culling, (material->flags & 4) == 0);
        CImVector color = { 0x00, 0x00, 0x00, 0x00 };
        if ((material->flags & 0x10) != 0)
            color = material->frameSidnColor;

        // TODO
        CImVector dword_D1BEFC = { 0x00, 0x00, 0x00, 0x00 };

        uint32_t sum = dword_D1BEFC.value + color.value;
        uint32_t carry = (color.value ^ dword_D1BEFC.value ^ sum) & 0x01010100;
        uint32_t sat = (sum - carry) | (carry - (carry >> 8));
        color.value = ((sat >> 1) & 0x007F7F7F) | (sat & 0xFF000000);

        CMapObj::SetEmissiveColor(color);
        GxRsSet(GxRs_BlendingMode, material->blendMode);
        CShaderEffect::SetAlphaRefDefault();
        
        GxTexSetWrap(texture1, (material->flags & 0x40) == 0 ? GxTex_Wrap : GxTex_Clamp, (material->flags & 0x80) == 0 ? GxTex_Wrap : GxTex_Clamp);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, texture1);
        g_theGxDevicePtr->RsSet(GxRs_Texture1, texture2);
        CMapObj::s_unifiedShaders[shader + 7]->SetCurrent();
        CMapObj::SelectWorldShaders();

        CGxBatch v26;
        v26.m_count = batch->indexCount;
        v26.m_start = batch->indexStart;
        v26.m_minIndex = batch->vertexStart;
        v26.m_maxIndex = batch->vertexEnd;
        v26.m_primType = GxPrim_Triangles;
        g_theGxDevicePtr->Draw(&v26, 1);

        batch++;
    }
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x7AC9F0
void CMapObj::InteriorRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3) {
    if ((mapObjGroup->parent->header->flags & 0x2) != 0) {
        CMapObj::UnifiedRender(mapObj, mapObjGroup, a3);
        return;
    }

    mapObjGroup->timer = 0.0;
    mapObjGroup->AllocVB();
    mapObjGroup->SetIndexVB();
    mapObjGroup->SetVertexVB();
    g_theGxDevicePtr->RsPush();
    s_fogMode = -1;
    CMapObj::s_lightingMode = -1;
    s_lastSidnColor = { 0xFF, 0xFF, 0xFF, 0xFF };
    // dword_CFBEA8 = -1;
    // v46 = 2 - (s_curGroupIsInterior != 0);
    CGxTex* gxTex = nullptr;
    // if (CMap::s_isStreamingMode)
    //     GxTex = TextureGetGxTex(CWorldScene::s_defaultTexture, 1, 0);
    for (int32_t i = 0; i < mapObjGroup->batchListCount; i++) {
        SMOBatch* batch = &mapObjGroup->batchList[i];

        if (!a3)
            batch->flags &= 0xFu;
        if ((batch->flags & 0xF0) != 0 /*|| mapObj->CullBatch(batchList)*/) {
            continue;
        }

        batch->flags |= 0xF0u;
        SMOMaterial* material = &mapObj->materialList[batch->texture];
        CGxTex* v11 = TextureGetGxTex(material->runTimeData_2, 0, 0);
        if (!v11) {
            if (!gxTex)
                continue;
            v11 = gxTex;
        }

        CGxTex* v49 = nullptr;
        if (material->runTimeData_3) {
            v49 = TextureGetGxTex(material->runTimeData_3, 0, 0);
            if (!v49)
                v49 = gxTex;
        }
        
        uint32_t v47 = material->shader;
        //if (!v47 && !material->blendMode && !maybe_IsSceneObjectEnabled(a1))
        //    v47 = 4;

        GxRsSet(GxRs_Culling, ((material->flags & 4) == 0));

        CImVector color = { 0x00, 0x00, 0x00, 0x00 };
        if ((material->flags & 0x10) != 0)
             color = material->frameSidnColor;

        // TODO
        CImVector dword_D1BEFC = { 0x00, 0x00, 0x00, 0x00 };

        uint32_t sum = dword_D1BEFC.value + color.value;
        uint32_t carry = (color.value ^ dword_D1BEFC.value ^ sum) & 0x01010100;
        uint32_t sat = (sum - carry) | (carry - (carry >> 8));
        color.value = ((sat >> 1) & 0x007F7F7F) | (sat & 0xFF000000);

        CMapObj::SetEmissiveColor(color);
        GxTexSetWrap(v11, (material->flags & 0x40) == 0 ? GxTex_Wrap : GxTex_Clamp, (material->flags & 0x80) == 0 ? GxTex_Wrap : GxTex_Clamp);
        GxRsSet(GxRs_Texture0, v11);
        GxRsSet(GxRs_Texture1, v49);
        CMapObj::s_unifiedShaders[v47 + 7]->SetCurrent();
        //transparencyBatchesCount = a2->transparencyBatchesCount;
        //if (v40 >= transparencyBatchesCount) {
        //    v27 = v40 < transparencyBatchesCount + a2->intBatchCount;
        //    SetShaderFogFromDayNight((v9->flags & 2) == 0 ? v38 : 0);
        //    if (v27) {
        //        if (dword_CFBEAC) {
        //            dword_CFBEAC = 0;
        //            DayNight::GetActiveDayNight();
        //            if (CShaderEffect::s_enableShaders) {
        //                if ((dword_D1C3AC & 1) == 0) {
        //                    dword_D1C3AC |= 1u;
        //                    flt_D1C39C = 0.0;
        //                    flt_D1C3A0 = 0.0;
        //                    flt_D1C3A4 = 0.0;
        //                    flt_D1C3A8 = 0.5;
        //                }
        //                g_theGxDevicePtr->ShaderConstantsSet(g_theGxDevicePtr, GxSh_Vertex, 11, &flt_D1C39C, 1);
        //            } else {
        //                GxRsSet_int32_t(GxRs_Lighting, 0);
        //            }
        //        }
        //    } else {
        //        CMapObjGroup::SetLighting(a2, ((v9->flags & 0x20) != 0) + 1);
        //    }
        //    if (dword_CFBEA8 != 1) {
        //        dword_CFBEA8 = 1;
        //        bn_CShadowCache_SetShadowMapGenericInterior(1);
        //        dword_D43010 = CShadowCache::GetShadowValue() != 0;
        //    }
        //GxRsSet(GxRs_BlendingMode, material->blendMode);
        //CShaderEffect::SetAlphaRefDefault();
        //CMapObj::SelectWorldShaders();
        //CGxBatch v26;
        //v26.m_count = batch->indexCount;
        //v26.m_start = batch->indexStart;
        //v26.m_minIndex = batch->vertexStart;
        //v26.m_maxIndex = batch->vertexEnd;
        //v26.m_primType = GxPrim_Triangles;
        //g_theGxDevicePtr->Draw(&v26, 1);
        //} else {
        //    if (dword_CFBEA8) {
        //        dword_CFBEA8 = 0;
        //        bn_CShadowCache_SetShadowMapGenericInterior(0);
        //        dword_D43010 = CShadowCache::GetShadowValue();
        //    }
        mapObjGroup->SetLighting(((material->flags & 0x20) != 0) + 1);
        //    SetShaderFogFromDayNight(~v9->flags & 2);
        GxRsSet(GxRs_BlendingMode, 9);
        CShaderEffect::SetAlphaRefDefault();
        CMapObj::SelectWorldShaders();
        CGxBatch v27;
        v27.m_count = batch->indexCount;
        v27.m_start = batch->indexStart;
        v27.m_minIndex = batch->vertexStart;
        v27.m_maxIndex = batch->vertexEnd;
        v27.m_primType = GxPrim_Triangles;
        g_theGxDevicePtr->Draw(&v27, 1);
        //    if (dword_CFBEAC) {
        //        dword_CFBEAC = 0;
        //        DayNight::GetActiveDayNight();
        //        if (CShaderEffect::s_enableShaders) {
        //            if ((dword_D1C3AC & 1) == 0) {
        //                dword_D1C3AC |= 1u;
        //                flt_D1C39C = 0.0;
        //                flt_D1C3A0 = 0.0;
        //                flt_D1C3A4 = 0.0;
        //                flt_D1C3A8 = 0.5;
        //            }
        //            g_theGxDevicePtr->ShaderConstantsSet(g_theGxDevicePtr, GxSh_Vertex, 11, &flt_D1C39C, 1);
        //        } else {
        //            GxRsSet_int32_t(GxRs_Lighting, 0);
        //        }
        //    }
        //    SetShaderFogFromDayNight((v9->flags & 2) == 0 ? v38 : 0);
        //    if (dword_CFBEA8 != 1) {
        //        dword_CFBEA8 = 1;
        //        bn_CShadowCache_SetShadowMapGenericInterior(1);
        //        dword_D43010 = CShadowCache::GetShadowValue() != 0;
        //    }
        GxRsSet(GxRs_BlendingMode, 7);
        CShaderEffect::SetAlphaRefDefault();
        CMapObj::SelectWorldShaders();
        CGxBatch v26;
        v26.m_count = batch->indexCount;
        v26.m_start = batch->indexStart;
        v26.m_minIndex = batch->vertexStart;
        v26.m_maxIndex = batch->vertexEnd;
        v26.m_primType = GxPrim_Triangles;
        g_theGxDevicePtr->Draw(&v26, 1);
        //}
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x7A8440
void SetShaderFogFromDayNight(int32_t mode) {
    if (s_fogMode == mode)
        return;
    s_fogMode = mode;

    if (!mode) {
        CShaderEffect::SetFogEnabled(0);
        return;
    }

    DayNight::DNInfo* info = DayNight::GetInfo();
    DNFogInfo* fog = (mode & 2) ? &info->m_fog : &info->m_fogInterior;
    CImVector color = (mode & 4) ? CImVector { 0x00, 0x00, 0x00, 0xFF } : fog->color;

    CShaderEffect::SetFogParams(fog->start, fog->end, fog->m_density, color);
    CShaderEffect::SetFogEnabled(1);
}

// OFFSET: 0x7A9380
void CMapObj::UnifiedRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3) {
    mapObjGroup->timer = 0.0;
    mapObjGroup->AllocVB();
    mapObjGroup->SetIndexVB();
    mapObjGroup->SetVertexVB();
    g_theGxDevicePtr->RsPush();
    s_fogMode = -1;
    CMapObj::s_lightingMode = -1;
    s_lastSidnColor = { 0xFF, 0xFF, 0xFF, 0xFF };
    //dword_CFBEA8 = -1;
    if (!CShaderEffect::s_enableShaders) {
        g_theGxDevicePtr->RsSet(GxRs_ColorMaterial, 2);
    }
    auto v67 = 2 - (CWorldScene::s_curGroupIsInterior != 0);
    CGxTex* gxTex = nullptr;
    //if (CMap::s_isStreamingMode)
    //    GxTex = TextureGetGxTex(CWorldScene::s_defaultTexture, 1, 0);
    auto batchList = mapObjGroup->batchList;
    for (int32_t i = 0; i < mapObjGroup->batchListCount; i++) {
        if (!a3)
            batchList->flags &= 0xFu;
        if ((batchList->flags & 0xF0) != 0 /*|| mapObj->CullBatch(batchList)*/) {
            batchList++;
            continue;
        }

        batchList->flags |= 0xF0u;

        auto material = &mapObj->materialList[batchList->texture];
        auto texture1 = TextureGetGxTex(material->runTimeData_2, 0, nullptr);
        if (!texture1) {
            if (!gxTex) {
                batchList++;
                continue;
            }
            texture1 = gxTex;
        }
        CGxTex* texture2 = nullptr;
        if (material->runTimeData_3) {
            texture2 = TextureGetGxTex(material->runTimeData_3, 0, nullptr);
            if (!texture2) {
                texture2 = gxTex;
            }
        }

        auto shader = material->shader;
        // if (!shader && !material->blendMode && !maybe_IsSceneObjectEnabled(material->runTimeData_2))
        //     shader = 4;

        uint32_t v10 = (material->flags & 4) == 0 ? 1 : 0;
        GxRsSet(GxRs_Culling, v10);
        CImVector color = { 0x00, 0x00, 0x00, 0x00 };
        if ((material->flags & 0x10) != 0)
            color = material->frameSidnColor;

        // TODO
        CImVector dword_D1BEFC = { 0x00, 0x00, 0x00, 0x00 };

        uint32_t sum = dword_D1BEFC.value + color.value;
        uint32_t carry = (color.value ^ dword_D1BEFC.value ^ sum) & 0x01010100;
        uint32_t sat = (sum - carry) | (carry - (carry >> 8));
        color.value = ((sat >> 1) & 0x007F7F7F) | (sat & 0xFF000000);

        CMapObj::SetEmissiveColor(color);
        GxTexSetWrap(texture1, (material->flags & 0x40) == 0 ? GxTex_Wrap : GxTex_Clamp, (material->flags & 0x80) == 0 ? GxTex_Wrap : GxTex_Clamp);
        GxRsSet(GxRs_Texture0, texture1);
        GxRsSet(GxRs_Texture1, texture2);
        CMapObj::s_unifiedShaders[shader]->SetCurrent();
        CGxBatch batch;
        if (i >= mapObjGroup->transparencyBatchesCount) {
            if ((mapObjGroup->flags & 0x48) != 0) {
                mapObjGroup->SetLighting((material->flags & 1) == 0 ? 1 : 0);
                //if (dword_CFBEA8) {
                //    dword_CFBEA8 = 0;
                //    bn_CShadowCache_SetShadowMapGenericInterior(0);
                //    dword_D43010 = CShadowCache::GetShadowValue();
                //}
                if (s_fogMode != 2) {
                    s_fogMode = 2;
                    auto dayNight = DayNight::GetInfo();
                    CShaderEffect::SetFogParams(dayNight->m_fog.start, dayNight->m_fog.end, dayNight->m_fog.m_density, dayNight->m_fog.color);
                    CShaderEffect::SetFogEnabled(1);
                }
            } else {
                if ((material->flags & 0x20) != 0)
                    mapObjGroup->SetLighting(2);
                else
                    mapObjGroup->SetLighting(3);
                //if (dword_CFBEA8 != 1) {
                //    dword_CFBEA8 = 1;
                //    bn_CShadowCache_SetShadowMapGenericInterior(1);
                //    dword_D43010 = CShadowCache::GetShadowValue() != 0;
                //}
                SetShaderFogFromDayNight(v67);
            }
            GxRsSet(GxRs_BlendingMode, material->blendMode);
            CShaderEffect::SetAlphaRefDefault();
            CMapObj::SelectWorldShaders();
            batch.m_count = batchList->indexCount;
            batch.m_minIndex = batchList->vertexStart;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = batchList->indexStart;
            batch.m_maxIndex = batchList->vertexEnd;
            g_theGxDevicePtr->Draw(&batch, 1);
        } else {
            SetShaderFogFromDayNight((material->flags & 2) == 0 ? v67 : 0);
            //if (dword_CFBEA8) {
            //    dword_CFBEA8 = 0;
            //    bn_CShadowCache_SetShadowMapGenericInterior(0);
            //    dword_D43010 = CShadowCache::GetShadowValue();
            //}
            if (CShaderEffect::s_enableShaders) {
                uint32_t lightingMode = 0;
                if ((material->flags & 1) == 0)
                    lightingMode = ((material->flags & 0x20) != 0 ? 1 : 0) + 1;
                mapObjGroup->SetLighting(lightingMode);
                SetShaderFogFromDayNight(~material->flags & 2);
                GxRsSet(GxRs_BlendingMode, 9);
                CShaderEffect::SetAlphaRefDefault();
                CMapObj::SelectWorldShaders();

                batch.m_count = batchList->indexCount;
                batch.m_minIndex = batchList->vertexStart;
                batch.m_primType = GxPrim_Triangles;
                batch.m_start = batchList->indexStart;
                batch.m_maxIndex = batchList->vertexEnd;
                g_theGxDevicePtr->Draw(&batch, 1);

                mapObjGroup->SetLighting(3);
                SetShaderFogFromDayNight((material->flags & 2) == 0 ? v67 : 0);
                //if (dword_CFBEA8 != 1) {
                //    dword_CFBEA8 = 1;
                //    bn_CShadowCache_SetShadowMapGenericInterior(1);
                //    dword_D43010 = CShadowCache::GetShadowValue() != 0;
                //}
                GxRsSet(GxRs_BlendingMode, 7);
                CShaderEffect::SetAlphaRefDefault();
                CMapObj::SelectWorldShaders();

                batch.m_count = batchList->indexCount;
                batch.m_minIndex = batchList->vertexStart;
                batch.m_primType = GxPrim_Triangles;
                batch.m_start = batchList->indexStart;
                batch.m_maxIndex = batchList->vertexEnd;
                g_theGxDevicePtr->Draw(&batch, 1);
            } else {
                GxRsSet(GxRs_ColorMaterial, 0);
                //mapObjGroup->SetTransparencyVB();
                uint32_t lightingMode = 0;
                if ((material->flags & 1) == 0)
                    lightingMode = ((material->flags & 0x20) != 0 ? 1 : 0) + 1;
                mapObjGroup->SetLighting(lightingMode);
                SetShaderFogFromDayNight((material->flags & 2) != 0 ? 0 : 6);
                GxRsSet(GxRs_BlendingMode, 9);
                CShaderEffect::SetAlphaRefDefault();

                batch.m_count = batchList->indexCount;
                batch.m_minIndex = batchList->vertexStart;
                batch.m_primType = GxPrim_Triangles;
                batch.m_start = batchList->indexStart;
                batch.m_maxIndex = batchList->vertexEnd;
                g_theGxDevicePtr->Draw(&batch, 1);

                mapObjGroup->SetLighting(3);
                uint32_t v28 = 0;
                if ((material->flags & 2) == 0)
                    v28 = v67 | 4;
                SetShaderFogFromDayNight(v28);
                GxRsSet(GxRs_BlendingMode, 7);
                CShaderEffect::SetAlphaRefDefault();

                batch.m_count = batchList->indexCount;
                batch.m_minIndex = batchList->vertexStart;
                batch.m_primType = GxPrim_Triangles;
                batch.m_start = batchList->indexStart;
                batch.m_maxIndex = batchList->vertexEnd;
                g_theGxDevicePtr->Draw(&batch, 1);

                mapObjGroup->SetVertexVB();
                if (CMapObj::s_lightingMode) {
                    CMapObj::s_lightingMode = 0;
                    //DayNight::GetActiveDayNight();
                    if (CShaderEffect::s_enableShaders) {
                        static C4Vector lightVector = C4Vector(0.0f, 0.0f, 0.0f, 0.5f);
                        //if ((dword_D1C3AC & 1) == 0) {
                        //    dword_D1C3AC |= 1u;
                        //    flt_D1C39C = 0.0;
                        //    flt_D1C3A0 = 0.0;
                        //    flt_D1C3A4 = 0.0;
                        //    flt_D1C3A8 = 0.5;
                        //}
                        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 11, &lightVector, 1);
                    } else {
                        GxRsSet(GxRs_Lighting, 0);
                    }
                }
                //SetShaderFogFromDayNight((v7->flags & 2) == 0 ? v66 : 0);
                GxRsSet(GxRs_BlendingMode, 10);
                CShaderEffect::SetAlphaRefDefault();

                batch.m_count = batchList->indexCount;
                batch.m_minIndex = batchList->vertexStart;
                batch.m_primType = GxPrim_Triangles;
                batch.m_start = batchList->indexStart;
                batch.m_maxIndex = batchList->vertexEnd;
                g_theGxDevicePtr->Draw(&batch, 1);
                GxRsSet(GxRs_ColorMaterial, 2);
            }
        }
        batchList++;
    }
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x7A6B60
void CMapObj::InvokeGroupRenderCallback(CMapObj* mapObj, uint32_t groupNum) {
    if (CMapObj::gRenderCallback && mapObj->GetGroup(groupNum, false)) {
        CMapObj::gRenderCallback(groupNum, CMapObj::gRenderUserParam);
    }
}

// OFFSET: 0x7A6B40
void CMapObj::SetGroupRenderCallback(RENDER_CALLBACK callback, void* param) {
    CMapObj::gRenderCallback = callback;
    CMapObj::gRenderUserParam = param;
}

// OFFSET: 0x7A8800
void CMapObj::SetRenderModeLight() {
    CGxLight light;
    light.m_flags |= 0x1;

    float d = 1.0f / sqrtf(3.0f);
    light.m_dir = { d, d, d };
    light.m_ambientColor = { 0.33f, 0.33f, 0.33f };
    light.m_dirColor = { 0.75f, 0.75f, 0.75f };
    light.m_specularColor = { 0.0f, 0.0f, 0.0f };
    light.m_constantAttenuation = 0.0f;
    light.m_linearAttenuation = 0.0f;
    light.m_quadraticAttenuation = 0.0f;

    C3Vector origin = { 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->LightSet(0, light, origin);
    g_theGxDevicePtr->LightEnable(0, 1);

    for (int i = 1; i < 4; i++)
        g_theGxDevicePtr->LightEnable(i, 0);

    GxRsSet(GxRs_Lighting, 1);
}

// OFFSET: 0x7A8940
void CMapObj::SetEmissiveColor(CImVector color) {
    color.a = 0;
    if (color == s_lastSidnColor)
        return;
    s_lastSidnColor = color;

    CImVector grey = { 0x7F, 0x7F, 0x7F, 0xFF };
    if (CShaderEffect::s_enableShaders) {
        C4Vector v11;
        v11.x = CMap::s_mapLight->m_light.m_specColor.x;
        v11.y = CMap::s_mapLight->m_light.m_specColor.y;
        v11.z = CMap::s_mapLight->m_light.m_specColor.z;
        v11.w = 14.0;

        C4Vector vecs[2];
        vecs[0] = C4Vector(grey);
        vecs[1] = C4Vector(color);

        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 28, vecs, 2);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 13, &v11, 1);
    } else {
    GxRsSet(GxRs_MatDiffuse, grey.value);
    GxRsSet(GxRs_MatEmissive, color.value);
    }
}

// OFFSET: 0x7A84D0
void CMapObj::SelectWorldShaders() {
    if (!CShaderEffect::s_enableShaders) {
        return;
    }

    int32_t shadow = CShaderEffect::s_shadowValue;

    if (shadow > 2) {
        shadow = 2;
    }

    int32_t vertexPermute = (CMapObj::s_lightingMode != 0) + 2 * (CWorldScene::s_fogPermute + 15 * shadow);

    int32_t pixelPermute = CShaderEffect::SelectShadowShader();

    CShaderEffect::SetShaders(vertexPermute, pixelPermute);
}
