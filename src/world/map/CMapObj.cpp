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

TSHashTable<CMapObj, HASHKEY_STRI> CMapObj::mapObjHashtable;
uint32_t CMapObj::s_renderMode = 5;
RENDER_FUNC CMapObj::s_renderGroupExteriorFunc;
RENDER_FUNC CMapObj::s_renderGroupInteriorFunc;
RENDER_CALLBACK CMapObj::gRenderCallback;
void* CMapObj::gRenderUserParam;
CImVector CMapObj::s_lastSidnColor;

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

// OFFSET: 0x7AEB10
SMOGroupInfo* CMapObj::GetGroupInfo(int32_t index) {
    if (this->isGroupLoaded)
        return &this->groupInfo[index];
    return nullptr;
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

// OFFSET: 0x7AD020
void CMapObj::PrepareUpdate() {
    //++dword_D1C424;
    //dword_D1C420 = 0;
    //dword_D1C41C = 0;
    //bn_TSGrowableArray_C3Vector_SetCount(&dword_D1BEE8, 0);
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
    //savedregs = v10;
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

// OFFSET: 0x7D7710
void CMapObj::CreateMaterial(uint8_t texture) {
    SMOMaterial* material = &this->materialList[texture];
    if (!material->runTimeData_2) {
        char* textureName1 = &this->textureNameList[material->texture1];
        char* textureName2 = &this->textureNameList[material->texture2];
        if (!*textureName1)
            textureName1 = "createcrappygreentexture.blp";
        if (!CShaderEffect::s_enableShaders)
            textureName2 = nullptr;

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

// OFFSET: 0x7AE880
bool CMapObj::TestGroupBounds(C3Vector& start, C3Vector& end, uint32_t groupNum) {
    return this->isGroupLoaded && (this->mapObjGroupArray[groupNum]->unkLoadedFlag & 1) != 0 && CWorldMath::VectorIntersectAABox2(this->groupInfo[groupNum].boundingBox, start, end) != 0;
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
    //dword_CFBEB0 = -1;
    //dword_CFBEAC = -1;
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
        
        //    SetShaderFogFromDayNight(~v10->flags & 2);
        //    maybe_SetWorldLightingMode(v6, (v10->flags & 1) == 0);
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
        //    bn_CShaderEffect_SetAlphaRefDefault();
        
        GxTexSetWrap(texture1, (material->flags & 0x40) == 0 ? GxTex_Wrap : GxTex_Clamp, (material->flags & 0x80) == 0 ? GxTex_Wrap : GxTex_Clamp);
        //    v20 = CMapObjRender::s_unifiedShaders[shader + 7];
        g_theGxDevicePtr->RsSet(GxRs_Texture0, texture1);
        g_theGxDevicePtr->RsSet(GxRs_Texture1, texture2);
        //    bn_CShaderEffect_SetCurrent(v20);
        //    maybe_SelectWorldShaders();

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
    // dword_CFBEB0 = -1;
    // dword_CFBEAC = -1;
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
        SMOMaterial* v10 = &mapObj->materialList[batch->texture];
        CGxTex* v11 = TextureGetGxTex(v10->runTimeData_2, 0, 0);
        if (!v11) {
            if (!gxTex)
                continue;
            v11 = gxTex;
        }

        CGxTex* v49 = nullptr;
        if (v10->runTimeData_3) {
            v49 = TextureGetGxTex(v10->runTimeData_3, 0, 0);
            if (!v49)
                v49 = gxTex;
        }
        
        uint32_t v47 = v10->shader;
        //if (!v47 && !v10->blendMode && !maybe_IsSceneObjectEnabled(a1))
        //    v47 = 4;

        GxRsSet(GxRs_Culling, ((v10->flags & 4) == 0));

         CImVector color = { 0x00, 0x00, 0x00, 0x00 };
        if ((v10->flags & 0x10) != 0)
             color = v10->frameSidnColor;

        // TODO
        CImVector dword_D1BEFC = { 0x00, 0x00, 0x00, 0x00 };

        uint32_t sum = dword_D1BEFC.value + color.value;
        uint32_t carry = (color.value ^ dword_D1BEFC.value ^ sum) & 0x01010100;
        uint32_t sat = (sum - carry) | (carry - (carry >> 8));
        color.value = ((sat >> 1) & 0x007F7F7F) | (sat & 0xFF000000);

        CMapObj::SetEmissiveColor(color);
        GxTexSetWrap(v11, (v10->flags & 0x40) == 0 ? GxTex_Wrap : GxTex_Clamp, (v10->flags & 0x80) == 0 ? GxTex_Wrap : GxTex_Clamp);
        //v17 = CMapObjRender::s_unifiedShaders[v47 + 7];
        GxRsSet(GxRs_Texture0, v11);
        GxRsSet(GxRs_Texture1, v49);
        //bn_CShaderEffect_SetCurrent(v16);
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
        GxRsSet(GxRs_BlendingMode, v10->blendMode);
        //    bn_CShaderEffect_SetAlphaRefDefault();
        //    maybe_SelectWorldShaders();
        CGxBatch v26;
        v26.m_count = batch->indexCount;
        v26.m_start = batch->indexStart;
        v26.m_minIndex = batch->vertexStart;
        v26.m_maxIndex = batch->vertexEnd;
        v26.m_primType = GxPrim_Triangles;
        g_theGxDevicePtr->Draw(&v26, 1);
        //} else {
        //    if (dword_CFBEA8) {
        //        dword_CFBEA8 = 0;
        //        bn_CShadowCache_SetShadowMapGenericInterior(0);
        //        dword_D43010 = CShadowCache::GetShadowValue();
        //    }
        //    CMapObjGroup::SetLighting(a2, ((v9->flags & 0x20) != 0) + 1);
        //    SetShaderFogFromDayNight(~v9->flags & 2);
        //    if (g_theGxDevicePtr->m_context) {
        //        v18 = g_theGxDevicePtr->m_appRenderStates.m_data + 6;
        //        if (v18->m_value.m_data.i[0] != 9) {
        //            CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
        //            v18->m_value.m_data.i[0] = 9;
        //        }
        //    }
        //    bn_CShaderEffect_SetAlphaRefDefault();
        //    maybe_SelectWorldShaders();
        //    v19 = v42;
        //    v20 = v42->indexStart;
        //    v21 = v42->vertexStart;
        //    v35.m_count = v42->indexCount;
        //    vertexEnd = v42->vertexEnd;
        //    v35.m_start = v20;
        //    v35.m_minIndex = v21;
        //    v35.m_maxIndex = vertexEnd;
        //    v35.m_primType = GxPrim_Triangles;
        //    g_theGxDevicePtr->Draw(g_theGxDevicePtr, &v35, 1);
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
        //    if (g_theGxDevicePtr->m_context) {
        //        v23 = g_theGxDevicePtr->m_appRenderStates.m_data + 6;
        //        if (v23->m_value.m_data.i[0] != 7) {
        //            CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
        //            v23->m_value.m_data.i[0] = 7;
        //        }
        //    }
        //    bn_CShaderEffect_SetAlphaRefDefault();
        //    maybe_SelectWorldShaders();
        //    v24 = v19->indexStart;
        //    v25 = v19->vertexStart;
        //    v33.m_count = v19->indexCount;
        //    v26 = v19->vertexEnd;
        //    v33.m_start = v24;
        //    v33.m_minIndex = v25;
        //    v33.m_maxIndex = v26;
        //    v33.m_primType = GxPrim_Triangles;
        //    g_theGxDevicePtr->Draw(g_theGxDevicePtr, &v33, 1);
        //}
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x7A9380
void CMapObj::UnifiedRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3) {
    mapObjGroup->timer = 0.0;
    mapObjGroup->AllocVB();
    mapObjGroup->SetIndexVB();
    mapObjGroup->SetVertexVB();
    g_theGxDevicePtr->RsPush();
    //dword_CFBEB0 = -1;
    //dword_CFBEAC = -1;
    s_lastSidnColor = { 0xFF, 0xFF, 0xFF, 0xFF };
    //dword_CFBEA8 = -1;
    if (!CShaderEffect::s_enableShaders) {
        g_theGxDevicePtr->RsSet(GxRs_ColorMaterial, 2);
    }
    //v67 = 2 - (dword_CFBEB8 != 0);
    CGxTex* GxTex = nullptr;
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
        //########## TESTING
        g_theGxDevicePtr->RsSet(GxRs_Fog, 0);
        g_theGxDevicePtr->RsSet(GxRs_ColorMaterial, 2);
        CMapObj::SetRenderModeLight();
        // SetRenderModeLight();
        // m_data = v7->m_appRenderStates.m_data;
        // v10 = CGxDevice::s_alphaRef[m_data[6].m_value.m_data.i[0]];
        // p_m_data = &v7->m_appRenderStates.m_data;
        // if (m_data[7].m_value.m_data.i[0] != v10) {
        //     CGxDevice::IRsDirty(v7, GxRs_AlphaRef);
        //     (*p_m_data)[7].m_value.m_data.i[0] = v10;
        //     v7 = g_theGxDevicePtr;
        // }
        g_theGxDevicePtr->RsSet(GxRs_MatDiffuse, 0x80CCCCCC);
        g_theGxDevicePtr->RsSet(GxRs_DepthWrite, 1);
        SMOMaterial* material = &mapObj->materialList[batchList->texture];
        g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(material->runTimeData_2, 0, 0));
        if (material->runTimeData_3)
            g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(material->runTimeData_3, 0, 0));
        CGxBatch v26;
        v26.m_count = batchList->indexCount;
        v26.m_start = batchList->indexStart;
        v26.m_minIndex = batchList->vertexStart;
        v26.m_maxIndex = batchList->vertexEnd;
        v26.m_primType = GxPrim_Triangles;
        g_theGxDevicePtr->Draw(&v26, 1);
        //##########################


        //v7 = &v62->materialList[batchList->texture];
        //v8 = v7->runTimeData[2];
        //v66 = CTexture::GetGxTex(v8, 0, 0);
        //if (!v66) {
        //    if (!GxTex)
        //        goto LABEL_88;
        //    v66 = GxTex;
        //}
        //v9 = v7->runTimeData[3];
        //v64 = 0;
        //if (!v9)
        //    goto LABEL_19;
        //v64 = CTexture::GetGxTex(v9, 0, 0);
        //if (v64)
        //    goto LABEL_19;
        //if (GxTex) {
        //    v64 = GxTex;
//LABEL_19:
        //    shader = v7->shader;
        //    if (!shader && !v7->blendMode && !maybe_IsSceneObjectEnabled(v8))
        //        shader = 4;
        //    v10 = (v7->flags & 4) == 0;
        //    if (g_theGxDevicePtr->m_context) {
        //        v4 = g_theGxDevicePtr->m_appRenderStates.m_data[17].m_value.m_data.i[0] == v10;
        //        v70 = g_theGxDevicePtr->m_appRenderStates.m_data + 17;
        //        if (!v4) {
        //            CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_Culling);
        //            v70->m_value.m_data.i[0] = v10;
        //        }
        //    }
        //    if ((v7->flags & 0x10) != 0) {
        //        p_frameSidnColor = &v7->frameSidnColor;
        //    } else {
        //        v63 = 0;
        //        p_frameSidnColor = &v63;
        //    }
        //    v12 = dword_D1BEFC + *p_frameSidnColor;
        //    v13 = (*p_frameSidnColor ^ dword_D1BEFC ^ v12) & 0x1010100;
        //    v14 = v12 - v13;
        //    v70 = (v14 | (v13 - (v13 >> 8)));
        //    BYTE1(v70) >>= 1;
        //    LOBYTE(v70) = v70 >> 1;
        //    BYTE2(v70) = ((v14 | (v13 - (v13 >> 8))) >> 16) >> 1;
        //    v44.color = v70;
        //    maybe_SetShaderAmbientAndFog(v44);
        //    GxTexSetWrap(v66, (v7->flags & 0x40) == 0, (v7->flags & 0x80) == 0);
        //    v15 = CMapObjRender::s_unifiedShaders[shader];
        //    CGxDevice::RsSet(g_theGxDevicePtr, GxRs_Texture0, v66);
        //    CGxDevice::RsSet(g_theGxDevicePtr, GxRs_Texture1, v64);
        //    bn_CShaderEffect_SetCurrent(v15);
        //    v16 = mapObjGroup;
        //    if (v65 >= mapObjGroup->transparencyBatchesCount) {
        //        if ((mapObjGroup->flags & 0x48) != 0) {
        //            maybe_SetWorldLightingMode(mapObjGroup, (v7->flags & 1) == 0);
        //            if (dword_CFBEA8) {
        //                dword_CFBEA8 = 0;
        //                bn_CShadowCache_SetShadowMapGenericInterior(0);
        //                dword_D43010 = CShadowCache::GetShadowValue();
        //            }
        //            if (dword_CFBEB0 != 2) {
        //                dword_CFBEB0 = 2;
        //                ActiveDayNight = DayNight::GetActiveDayNight();
        //                color = ActiveDayNight->fogInfo.color;
        //                bn_CShaderEffect_SetFogParams(
        //                    ActiveDayNight->fogInfo.start,
        //                    ActiveDayNight->fogInfo.end,
        //                    *&ActiveDayNight->unk38,
        //                    &color);
        //                bn_CShaderEffect_SetFogEnabled(1);
        //            }
        //        } else {
        //            if ((v7->flags & 0x20) != 0)
        //                maybe_SetWorldLightingMode(mapObjGroup, 2);
        //            else
        //                maybe_SetWorldLightingMode(mapObjGroup, 3);
        //            if (dword_CFBEA8 != 1) {
        //                dword_CFBEA8 = 1;
        //                bn_CShadowCache_SetShadowMapGenericInterior(1);
        //                dword_D43010 = CShadowCache::GetShadowValue() != 0;
        //            }
        //            SetShaderFogFromDayNight(v67);
        //        }
        //        blendMode = v7->blendMode;
        //        if (g_theGxDevicePtr->m_context) {
        //            v40 = g_theGxDevicePtr->m_appRenderStates.m_data + 6;
        //            if (v40->m_value.m_data.i[0] != blendMode) {
        //                CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
        //                v40->m_value.m_data.i[0] = blendMode;
        //            }
        //        }
        //        bn_CShaderEffect_SetAlphaRefDefault();
        //        maybe_SelectWorldShaders();
        //        indexStart = batchList->indexStart;
        //        vertexEnd = batchList->vertexEnd;
        //        v46[2] = batchList->indexCount;
        //        vertexStart = batchList->vertexStart;
        //        v46[0] = 3;
        //        v46[1] = indexStart;
        //        v48 = vertexEnd;
        //        g_theGxDevicePtr->Draw(g_theGxDevicePtr, v46, 1);
        //    } else {
        //        SetShaderFogFromDayNight((v7->flags & 2) == 0 ? v67 : 0);
        //        if (dword_CFBEA8) {
        //            dword_CFBEA8 = 0;
        //            bn_CShadowCache_SetShadowMapGenericInterior(0);
        //            dword_D43010 = CShadowCache::GetShadowValue();
        //        }
        //        if (CShaderEffect::s_enableShaders) {
        //            if ((v7->flags & 1) != 0)
        //                v17 = 0;
        //            else
        //                v17 = ((v7->flags & 0x20) != 0) + 1;
        //            maybe_SetWorldLightingMode(mapObjGroup, v17);
        //            SetShaderFogFromDayNight(~v7->flags & 2);
        //            if (g_theGxDevicePtr->m_context) {
        //                v4 = g_theGxDevicePtr->m_appRenderStates.m_data[6].m_value.m_data.i[0] == 9;
        //                shader = &g_theGxDevicePtr->m_appRenderStates.m_data[6];
        //                if (!v4) {
        //                    CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
        //                    *shader = 9;
        //                }
        //            }
        //            bn_CShaderEffect_SetAlphaRefDefault();
        //            maybe_SelectWorldShaders();
        //            v18 = batchList->indexStart;
        //            v19 = batchList->vertexStart;
        //            v58[2] = batchList->indexCount;
        //            v20 = batchList->vertexEnd;
        //            v58[1] = v18;
        //            v59 = v19;
        //            v60 = v20;
        //            v58[0] = 3;
        //            g_theGxDevicePtr->Draw(g_theGxDevicePtr, v58, 1);
        //            maybe_SetWorldLightingMode(mapObjGroup, 3);
        //            SetShaderFogFromDayNight((v7->flags & 2) == 0 ? v67 : 0);
        //            if (dword_CFBEA8 != 1) {
        //                dword_CFBEA8 = 1;
        //                bn_CShadowCache_SetShadowMapGenericInterior(1);
        //                dword_D43010 = CShadowCache::GetShadowValue() != 0;
        //            }
        //            if (g_theGxDevicePtr->m_context) {
        //                v21 = g_theGxDevicePtr->m_appRenderStates.m_data + 6;
        //                if (v21->m_value.m_data.i[0] != 7) {
        //                    CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
        //                    v21->m_value.m_data.i[0] = 7;
        //                }
        //            }
        //            bn_CShaderEffect_SetAlphaRefDefault();
        //            maybe_SelectWorldShaders();
        //            v22 = batchList->indexStart;
        //            v23 = batchList->vertexStart;
        //            v52[2] = batchList->indexCount;
        //            v54 = batchList->vertexEnd;
        //            v52[0] = 3;
        //            v52[1] = v22;
        //            v53 = v23;
        //            g_theGxDevicePtr->Draw(g_theGxDevicePtr, v52, 1);
        //        } else {
        //            if (g_theGxDevicePtr->m_context) {
        //                v4 = g_theGxDevicePtr->m_appRenderStates.m_data[85].m_value.m_data.i[0] == 0;
        //                shader = &g_theGxDevicePtr->m_appRenderStates.m_data[85];
        //                if (!v4) {
        //                    CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_ColorMaterial);
        //                    *shader = 0;
        //                }
        //            }
        //            CMapObjGroup::SetTransparencyVB(mapObjGroup);
        //            if ((v7->flags & 1) != 0)
        //                v24 = 0;
        //            else
        //                v24 = ((v7->flags & 0x20) != 0) + 1;
        //            maybe_SetWorldLightingMode(mapObjGroup, v24);
        //            SetShaderFogFromDayNight((v7->flags & 2) != 0 ? 0 : 6);
        //            if (g_theGxDevicePtr->m_context) {
        //                v4 = g_theGxDevicePtr->m_appRenderStates.m_data[6].m_value.m_data.i[0] == 9;
        //                shader = &g_theGxDevicePtr->m_appRenderStates.m_data[6];
        //                if (!v4) {
        //                    CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
        //                    *shader = 9;
        //                }
        //            }
        //            bn_CShaderEffect_SetAlphaRefDefault();
        //            v25 = batchList->indexStart;
        //            v26 = batchList->vertexStart;
        //            v55[2] = batchList->indexCount;
        //            v27 = batchList->vertexEnd;
        //            v55[1] = v25;
        //            v56 = v26;
        //            v57 = v27;
        //            v55[0] = 3;
        //            g_theGxDevicePtr->Draw(g_theGxDevicePtr, v55, 1);
        //            maybe_SetWorldLightingMode(mapObjGroup, 3);
        //            if ((v7->flags & 2) != 0)
        //                v28 = 0;
        //            else
        //                v28 = v67 | 4;
        //            SetShaderFogFromDayNight(v28);
        //            if (g_theGxDevicePtr->m_context) {
        //                v29 = g_theGxDevicePtr->m_appRenderStates.m_data + 6;
        //                if (v29->m_value.m_data.i[0] != 7) {
        //                    CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
        //                    v29->m_value.m_data.i[0] = 7;
        //                }
        //                v16 = mapObjGroup;
        //            }
        //            bn_CShaderEffect_SetAlphaRefDefault();
        //            v30 = batchList->indexStart;
        //            v31 = batchList->vertexStart;
        //            v49[2] = batchList->indexCount;
        //            v32 = batchList->vertexEnd;
        //            v49[1] = v30;
        //            v50 = v31;
        //            v51 = v32;
        //            v49[0] = 3;
        //            g_theGxDevicePtr->Draw(g_theGxDevicePtr, v49, 1);
        //            CMapObjGroup::SetVertexVB(v16);
        //            if (dword_CFBEAC) {
        //                dword_CFBEAC = 0;
        //                DayNight::GetActiveDayNight();
        //                if (CShaderEffect::s_enableShaders) {
        //                    if ((dword_D1C3AC & 1) == 0) {
        //                        dword_D1C3AC |= 1u;
        //                        flt_D1C39C = 0.0;
        //                        flt_D1C3A0 = 0.0;
        //                        flt_D1C3A4 = 0.0;
        //                        flt_D1C3A8 = 0.5;
        //                    }
        //                    g_theGxDevicePtr->ShaderConstantsSet(g_theGxDevicePtr, GxSh_Vertex, 11, &flt_D1C39C, 1);
        //                } else {
        //                    GxRsSet_int32_t(GxRs_Lighting, 0);
        //                }
        //            }
        //            SetShaderFogFromDayNight((v7->flags & 2) == 0 ? v67 : 0);
        //            if (g_theGxDevicePtr->m_context) {
        //                v33 = g_theGxDevicePtr->m_appRenderStates.m_data + 6;
        //                if (v33->m_value.m_data.i[0] != 10) {
        //                    CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
        //                    v33->m_value.m_data.i[0] = 10;
        //                }
        //            }
        //            bn_CShaderEffect_SetAlphaRefDefault();
        //            v34 = batchList->indexStart;
        //            v35 = batchList->vertexStart;
        //            v45.m_count = batchList->indexCount;
        //            v36 = batchList->vertexEnd;
        //            v45.m_start = v34;
        //            v45.m_minIndex = v35;
        //            v45.m_maxIndex = v36;
        //            v45.m_primType = GxPrim_Triangles;
        //            g_theGxDevicePtr->Draw(g_theGxDevicePtr, &v45, 1);
        //            if (g_theGxDevicePtr->m_context) {
        //                v37 = g_theGxDevicePtr->m_appRenderStates.m_data + GxRs_ColorMaterial;
        //                if (v37->m_value.m_data.i[0] != 2) {
        //                    CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_ColorMaterial);
        //                    v37->m_value.m_data.i[0] = 2;
        //                }
        //            }
        //        }
        //    }
        //}
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
    //if (CShaderEffect::s_enableShaders) {
    //    x = s_mapLight->unk14.m_specularColor.x;
    //    y = s_mapLight->unk14.m_specularColor.y;
    //    v11.w = 14.0;
    //    z = s_mapLight->unk14.m_specularColor.z;
    //    v12 = x;
    //    v11.x = x;
    //    v13 = y;
    //    v11.y = y;
    //    v14 = z;
    //    v11.z = z;
    //    maybe_C4Vector__C4Vector(&v9, &v15);
    //    maybe_C4Vector__C4Vector(&v10, &a1);
    //    g_theGxDevicePtr->ShaderConstantsSet(g_theGxDevicePtr, GxSh_Vertex, 28, &v9, 2);
    //    g_theGxDevicePtr->ShaderConstantsSet(g_theGxDevicePtr, GxSh_Vertex, 13, &v11, 1);
    //} else {
    GxRsSet(GxRs_MatDiffuse, grey.value);
    GxRsSet(GxRs_MatEmissive, color.value);
    //}
}
