#include "world/CWorldScene.hpp"
#include "CWorldView.hpp"
#include "cursor/Cursor.hpp"
#include "gameui/CGWorldFrame.hpp"
#include "gameui/camera/CGCamera.hpp"
#include "gx/Device.hpp"
#include "gx/Draw.hpp"
#include "gx/RenderState.hpp"
#include "gx/Shader.hpp"
#include "gx/Transform.hpp"
#include "model/Model2.hpp"
#include "world/CWorld.hpp"
#include "world/daynight/DNInfo.hpp"
#include "world/daynight/DayNight.hpp"
#include "world/map/CMap.hpp"
#include "world/map/CMapChunk.hpp"
#include "world/map/CWorldOcclusion.hpp"
#include <tempest/Intersect.hpp>
#include <tempest/math/CMath.hpp>
#include <console/DebugScreen.hpp>
#include "CWorldMath.hpp"

CM2Scene* CWorldScene::s_m2Scene;
HTEXTURE CWorldScene::s_defaultTexture;
HTEXTURE CWorldScene::s_defaultBlendTexture;

int32_t CWorldScene::frustumIndex;
CFrustum CWorldScene::frustumStack[32];
CFrustum CWorldScene::s_clipFrustum;
CPortalView CWorldScene::frustumPortalView;
CiRect CWorldScene::s_frustumChunkRect;
C3Vector CWorldScene::s_frustumCorners[8];

CSortTable CWorldScene::sortTable;

C3Vector CWorldScene::s_activeWorldView;
C3Vector CWorldScene::camTarget;
C3Vector CWorldScene::camVec;
C4Plane CWorldScene::camPlane;
C4Plane CWorldScene::camPlaneXY;
C44Matrix CWorldScene::viewMatrix;
C44Matrix CWorldScene::projMatrix;
CAaBox CWorldScene::boundingBox;

uint32_t CWorldScene::s_chunksRendered;
uint32_t CWorldScene::s_doodadsRendered;

C3Vector CWorldScene::s_camPosLocal;
C3Vector CWorldScene::s_camTargetLocal;
C4Plane CWorldScene::s_camPlaneLocal;
C44Matrix CWorldScene::s_viewProj;
C44Matrix CWorldScene::s_modelView;
C44Matrix CWorldScene::s_modelViewProj;
C44Matrix CWorldScene::s_mapObjToWorld;
bool CWorldScene::s_cullStateValid;

uint32_t CWorldScene::s_interiorPass;
uint32_t CWorldScene::s_portalStamp;
uint32_t CWorldScene::s_maxPortalDepth = 10;
CMapObjDef* CWorldScene::s_curMapObjDef;
CMapObjDef* CWorldScene::s_stampedMapObjDef;
CMapObjDef* CWorldScene::s_viewerMapObjDef;
CMapObjDef* CWorldScene::s_viewerMovedMapObjDef;
TSGrowableArray<uint16_t> CWorldScene::s_viewerMapObjGroups;
TSGrowableArray<uint16_t> CWorldScene::s_viewerMovedMapObjGroups;

SPortalExt CWorldScene::s_portalExt[2048];
TSGrowableArray<CPortalView> CWorldScene::s_pendingPortalViews;
TSGrowableArray<CRect> CWorldScene::s_coveredRects;
int32_t CWorldScene::s_curGroupIsInterior;

int32_t CWorldScene::s_fogPermute;
int32_t CWorldScene::s_savedFogColor;

bool CWorldScene::s_entityCanLink;

char CWorldScene::s_debugMapName[260];
char CWorldScene::s_debugMapChunk[64];

STORM_EXPLICIT_LIST(CFrustum, sceneLink) CWorldScene::s_frustumFreeList;

void CWorldSceneLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg) {
    lighting->AddAmbient({ 1.0f, 1.0f, 1.0f });
    lighting->AddDiffuse({ 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f });
    lighting->AddSpecular({ 0.0f, 0.0f, 0.0f });
}

// OFFSET: 0x7997D0
void CWorldScene::Initialize() {
    // CBarrier::Initialize();
    // flt_CD877C = 0.0;
    // dword_CD87B0 = 0;
    // dword_CD87A8 = 0;
    // dword_CD8794 = 0;
    // sub_8A1770(1, 1.0, 0, (char*)&zeroValue, (int)&g_LiquidTypeDB.funcTable2, (int)&g_liquidMaterialDB.funcTable2);
    // v1 = SMemAlloc(0x20, ".\\WorldScene.cpp", 841, 0);
    // if (v1) {
    //     v2 = 1;
    //     v3 = v1 + 2;
    //     do {
    //         *(v3 - 2) = 0;
    //         *(v3 - 1) = 0;
    //         *v3 = 0;
    //         v3[1] = 0;
    //         v3 += 4;
    //         --v2;
    //     } while (v2 >= 0);
    //     dword_CD8610 = v1;
    // } else {
    //     dword_CD8610 = 0;
    // }
    // NOP();

    CWorldScene::s_defaultTexture = TextureCreateSolid({ 0x80, 0x80, 0x80, 0xFF });
    CWorldScene::s_defaultBlendTexture = TextureCreateSolid({ 0, 0, 0, 0xFF });
}

// OFFSET: 0x795400
void CWorldScene::Update(C3Vector* camPos, C3Vector* camTarget) {
    // v2 = 0;
    // if (dword_CD8610)
    //     NOP();
    // flt_ADF574 = 3.4028235e38;
    // flt_ADF570 = 3.4028235e38;
    // dword_CD8620 = 0;
    // dword_CD861C = 0;
    // flt_ADF57C = -3.4028235e38;
    // dword_ADF584 = 0;
    // flt_ADF578 = -3.4028235e38;
    // dword_ADF588 = 0;
    CWorldScene::frustumPortalView.verts = nullptr;
    // flt_ADF580 = -1.0;
    CWorldScene::frustumPortalView.vertCount = 0;
    CWorldScene::frustumPortalView.rect.minX = 3.4028235e38;
    CWorldScene::frustumPortalView.rect.minY = 3.4028235e38;
    CWorldScene::frustumPortalView.rect.maxX = -3.4028235e38;
    CWorldScene::frustumPortalView.rect.maxY = -3.4028235e38;
    CWorldScene::frustumPortalView.maxViewDepth = -1.0;
    // bn_TSGrowableArray_CPortalView_SetCount(0);
    // bn_TSGrowableArray_CPortalView_SetCount(0);

    CWorldScene::s_activeWorldView = *camPos;
    CWorldScene::camTarget = *camTarget;

    C3Vector camDir;
    camDir.x = camTarget->x - camPos->x;
    camDir.y = camTarget->y - camPos->y;
    camDir.z = camTarget->z - camPos->z;

    float invLen = 1.0f / sqrtf(camDir.x * camDir.x + camDir.y * camDir.y + camDir.z * camDir.z);
    CWorldScene::camVec.x = camDir.x * invLen;
    CWorldScene::camVec.y = camDir.y * invLen;
    CWorldScene::camVec.z = camDir.z * invLen;

    CWorldScene::camPlane.n = CWorldScene::camVec;
    CWorldScene::camPlane.d = -(camPos->x * CWorldScene::camVec.x +
                                camPos->y * CWorldScene::camVec.y +
                                camPos->z * CWorldScene::camVec.z);

    // float farClip = CWorld::s_farClip - 100.0f;
    // flt_CD87AC = (farClip < 400.0f) ? 400.0f : farClip;

    float flatX = CWorldScene::camVec.x;
    float flatY = CWorldScene::camVec.y;
    float flatLen = flatX * flatX + flatY * flatY;

    if (flatLen <= 0.0001f) {
        CWorldScene::camPlaneXY.n = { 0.0f, 0.0f, 0.0f };
    } else {
        float invFlatLen = 1.0f / sqrtf(flatLen);
        CWorldScene::camPlaneXY.n.x = flatX * invFlatLen;
        CWorldScene::camPlaneXY.n.y = flatY * invFlatLen;
        CWorldScene::camPlaneXY.n.z = 0.0f;
    }

    CWorldScene::camPlaneXY.d = -(CWorldScene::camPlaneXY.n.x * camPos->x +
                                  CWorldScene::camPlaneXY.n.y * camPos->y +
                                  CWorldScene::camPlaneXY.n.z * camPos->z);

    // sub_7906C0(&CWorldScene::sortTable, v30);
    g_theGxDevicePtr->XformView(CWorldScene::viewMatrix);
    g_theGxDevicePtr->XformProjection(CWorldScene::projMatrix);
    // CWorldScene::viewPort.x.l = v22->m_viewport.x.l;
    // CWorldScene::viewPort.y.h = v22->m_viewport.x.h;
    // CWorldScene::viewPort.x.h = v22->m_viewport.y.l;
    // CWorldScene::viewPort.z.l = v22->m_viewport.y.h;
    // CWorldScene::viewPort.y.l = v22->m_viewport.z.l;
    // CWorldScene::viewPort.z.h = v22->m_viewport.z.h;
    GxuXformCalcFrustumCorners(&CWorldScene::viewMatrix, &CWorldScene::projMatrix, CWorldScene::s_frustumCorners);
    for (int32_t i = 0; i < 8; i++) {
        CWorldScene::s_frustumCorners[i] += CWorldScene::s_activeWorldView;
    }
    CWorldScene::s_clipFrustum.CalcPlanesFromCorners(CWorldScene::s_frustumCorners);
    CWorldScene::viewMatrix.Translate(-CWorldScene::s_activeWorldView);
    C44Matrix v23 = CWorldScene::viewMatrix * CWorldScene::projMatrix;
    // stru_ADF5A8.M11 = v23->M11;
    // stru_ADF5A8.M12 = v23->M12;
    // stru_ADF5A8.M13 = v23->M13;
    // M14 = v23->M14;
    // stru_ADF5A8.M14 = v23->M14;
    // stru_ADF5A8.M21 = v23->M21;
    // stru_ADF5A8.M22 = v23->M22;
    // stru_ADF5A8.M23 = v23->M23;
    // stru_ADF5A8.M24 = v23->M24;
    // stru_ADF5A8.M31 = v23->M31;
    // stru_ADF5A8.M32 = v23->M32;
    // stru_ADF5A8.M33 = v23->M33;
    // M34 = v23->M34;
    // stru_ADF5A8.M34 = v23->M34;
    // stru_ADF5A8.M41 = v23->M41;
    // stru_ADF5A8.M42 = v23->M42;
    // stru_ADF5A8.M43 = v23->M43;
    // stru_ADF5A8.M44 = v23->M44;
    // v36.z = stru_ADF5A8.M44;
    //*(float*)&v35 = M14;
    // dword_CD8FC8 = v35;
    // v36.x = stru_ADF5A8.M24;
    // dword_CD8FCC = LODWORD(stru_ADF5A8.M24);
    // v36.y = M34;
    // dword_CD8FD0 = LODWORD(v36.y);
    // dword_CD8FD4 = LODWORD(stru_ADF5A8.M44);
    CWorldScene::boundingBox = CAaBox::Bounding(CWorldScene::s_frustumCorners, 8u);
    CWorldScene::s_frustumChunkRect.minX = (int32_t)floorf(-(CWorldScene::boundingBox.t.y - 17066.666f) * 0.03f);
    CWorldScene::s_frustumChunkRect.minY = (int32_t)floorf(-(CWorldScene::boundingBox.t.x - 17066.666f) * 0.03f);
    CWorldScene::s_frustumChunkRect.maxX = (int32_t)floorf(-(CWorldScene::boundingBox.b.y - 17066.666f) * 0.03f);
    CWorldScene::s_frustumChunkRect.maxY = (int32_t)floorf(-(CWorldScene::boundingBox.b.x - 17066.666f) * 0.03f);
    CWorldScene::frustumIndex = 0;
    CWorldScene::frustumStack[0].CalcPlanesFromCorners(CWorldScene::s_frustumCorners);
    CMapChunk::farCornerIndex = 0;
    if (CWorldScene::camTarget.x > CWorldScene::s_activeWorldView.x)
        CMapChunk::farCornerIndex = 2;
    if (CWorldScene::camTarget.y > CWorldScene::s_activeWorldView.y)
        CMapChunk::farCornerIndex += 1;
    // dword_CD8F3C = 0;
    // if (CWorldScene::camTarg.x < (double)CWorldScene::s_activeWorldView.x) {
    //     v2 = 2;
    //     dword_CD8F3C = 2;
    // }
    // if (CWorldScene::camTarg.y < (double)CWorldScene::s_activeWorldView.y)
    //     dword_CD8F3C = v2 + 1;
    // CWorldScene::camPlane.n = CWorldScene::camVec;
    // CWorldScene::camPlane.d = -(camPos->x * CWorldScene::camVec.x + camPos->y * CWorldScene::camVec.y + CWorldScene::camVec.z * camPos->z);
    // if (v33 <= 0.000099999997) {
    //     stru_ADF460.M44 = 1.0;
    //     stru_ADF460.M33 = 1.0;
    //     stru_ADF460.M22 = 1.0;
    //     stru_ADF460.M11 = 1.0;
    //     stru_ADF460.M43 = 0.0;
    //     stru_ADF460.M42 = 0.0;
    //     stru_ADF460.M41 = 0.0;
    //     stru_ADF460.M34 = 0.0;
    //     stru_ADF460.M32 = 0.0;
    //     stru_ADF460.M31 = 0.0;
    //     stru_ADF460.M24 = 0.0;
    //     stru_ADF460.M23 = 0.0;
    //     stru_ADF460.M21 = 0.0;
    //     stru_ADF460.M14 = 0.0;
    //     stru_ADF460.M13 = 0.0;
    //     stru_ADF460.M12 = 0.0;
    //     AsyncTimeMs = OsGetAsyncTimeMs();
    //     return sub_9A81F0(AsyncTimeMs);
    // } else {
    //     v32.M11 = 1.0;
    //     v32.M12 = 0.0;
    //     v32.M13 = 0.0;
    //     v32.M14 = 0.0;
    //     v32.M21 = 0.0;
    //     v32.M23 = 0.0;
    //     v32.M24 = 0.0;
    //     v32.M31 = 0.0;
    //     v32.M32 = 0.0;
    //     v32.M34 = 0.0;
    //     v32.M41 = 0.0;
    //     v32.M42 = 0.0;
    //     v32.M43 = 0.0;
    //     a3.x = 0.0;
    //     a3.y = 0.0;
    //     v36.x = 0.0;
    //     v36.y = 0.0;
    //     v36.z = 0.0;
    //     v32.M22 = 1.0;
    //     v32.M33 = 1.0;
    //     v32.M44 = 1.0;
    //     a3.z = 1.0;
    //     sub_6BFE60(&v36.x, &v38, &a3.x, &v32);
    //     v36.x = -CWorldScene::s_activeWorldView.x;
    //     v36.y = -CWorldScene::s_activeWorldView.y;
    //     v36.z = -CWorldScene::s_activeWorldView.z;
    //     C44Matrix::Translate(&v32, &v36);
    //     v26 = C44Matrix::Multiply(&v31, &v32, &CWorldScene::projMatrix);
    //     stru_ADF460.M11 = v26->M11;
    //     stru_ADF460.M12 = v26->M12;
    //     stru_ADF460.M13 = v26->M13;
    //     stru_ADF460.M14 = v26->M14;
    //     stru_ADF460.M21 = v26->M21;
    //     stru_ADF460.M22 = v26->M22;
    //     stru_ADF460.M23 = v26->M23;
    //     stru_ADF460.M24 = v26->M24;
    //     stru_ADF460.M31 = v26->M31;
    //     stru_ADF460.M32 = v26->M32;
    //     stru_ADF460.M33 = v26->M33;
    //     stru_ADF460.M34 = v26->M34;
    //     stru_ADF460.M41 = v26->M41;
    //     stru_ADF460.M42 = v26->M42;
    //     stru_ADF460.M43 = v26->M43;
    //     stru_ADF460.M44 = v26->M44;
    //     v27 = OsGetAsyncTimeMs();
    //     return sub_9A81F0(v27);
    // }
}

// OFFSET: 0x790650
void CWorldScene::GetNearestCornerToCamera(CAaBox* box, C3Vector* outCorner) {
    outCorner->x = (CWorldScene::camTarget.x >= CWorldScene::s_activeWorldView.x)
                       ? box->b.x
                       : box->t.x;

    outCorner->y = (CWorldScene::camTarget.y >= CWorldScene::s_activeWorldView.y)
                       ? box->b.y
                       : box->t.y;

    outCorner->z = (CWorldScene::camTarget.z >= CWorldScene::s_activeWorldView.z)
                       ? box->b.z
                       : box->t.z;
}

// OFFSET: 0x7C3E70
void CWorldScene::AddMapChunk(CMapChunk* mapChunk) {
    if (CWorldScene::FrustumCull(&mapChunk->bbox))
        return;

    C3Vector corner;
    CWorldScene::GetNearestCornerToCamera(&mapChunk->bbox, &corner);
    mapChunk->distToCamera = CWorldScene::camPlane.n.z * corner.z + CWorldScene::camPlane.n.y * corner.y + CWorldScene::camPlane.n.x * corner.x + CWorldScene::camPlane.d;
    int32_t vertexIndex = CMapChunk::cornerVertexIndex[CMapChunk::farCornerIndex];
    C3Vector chunkPos = {
        CMapChunk::vertexList[vertexIndex].x + mapChunk->topLeftCoords.x,
        CMapChunk::vertexList[vertexIndex].y + mapChunk->topLeftCoords.y,
        mapChunk->height[vertexIndex] + mapChunk->topLeftCoords.z
    };
    CWorldScene::AddMapChunkToRenderList(mapChunk, &chunkPos);
}

// OFFSET: 0x792AD0
void CWorldScene::AddMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup) {
    if ((mapObjDef->owner->GetGroupFlags(mapObjDefGroup->groupNum) & 0x10008) == 0)
        return;

    if ((mapObjDef->flags & 0x400) != 0) {
        CWorldScene::sortTable.pendingExteriorGroupList.LinkToTail(mapObjDefGroup);
    } else {
        C3Vector nearest;
        CWorldScene::GetNearestCornerToCamera(&mapObjDefGroup->bbox, &nearest);
        mapObjDefGroup->distanceToCamera = CWorldScene::camPlane.n.x * nearest.x
                                             + CWorldScene::camPlane.n.y * nearest.y
                                             + CWorldScene::camPlane.n.z * nearest.z
                                             + CWorldScene::camPlane.d;

        auto planeDist = nearest.y * CWorldScene::camPlaneXY.n.y
               + nearest.z * CWorldScene::camPlaneXY.n.z
               + nearest.x * CWorldScene::camPlaneXY.n.x
               + CWorldScene::camPlaneXY.d;
        int index = (int32_t)floorf(planeDist * 0.03f);
        if (planeDist <= 0.0f || index < 64) {
            CWorldScene::sortTable.table[planeDist <= 0.0f ? 0 : index].exteriorGroupList.LinkToTail(mapObjDefGroup);
        }
    }
}

// OFFSET: 0x792D80
void CWorldScene::AddMapChunkToRenderList(CMapChunk* mapChunk, C3Vector* pos) {
    float planeDist = C3Vector::Dot(*pos, CWorldScene::camPlaneXY.n) + CWorldScene::camPlaneXY.d;
    int index = (int32_t)floorf(planeDist * 0.03f);
    if (planeDist <= 0.0f || index < 64) {
        CWorldScene::sortTable.table[planeDist <= 0.0f ? 0 : index].mapChunkList.LinkToTail(mapChunk);
    }
}

// OFFSET: 0x799310
void CWorldScene::AddMapObjDefGroupToSortTable(uint32_t groupNum, CMapObjDef* mapObjDef) {
    CMapObjDefGroup* mapObjDefGroup = mapObjDef->Groups()[groupNum];
    CMapObjGroup* group = mapObjDef->owner->GetGroup(mapObjDefGroup->groupNum, 0);
    if (!mapObjDefGroup->sortTableLink.Next()) {
        CWorldScene::sortTable.mapObjDefGroup.LinkToTail(mapObjDefGroup);
        if (group && (group->flags & 0x1000) != 0 && (CWorld::s_enables & CWorld::Enables::Enable_1000000) != 0) {
            //CWorldScene::sortTable.interiorLiquidGroupList.LinkToTail(mapObjDefGroup);
        }
        C3Vector outCorner;
        CWorldScene::GetNearestCornerToCamera(&mapObjDefGroup->bbox, &outCorner);
        mapObjDefGroup->distanceToCamera = CWorldScene::camPlane.n.z * outCorner.z + CWorldScene::camPlane.n.y * outCorner.y + CWorldScene::camPlane.n.x * outCorner.x + CWorldScene::camPlane.d;
        //    if ((group->flags & 0x40) != 0)
        //        dword_CD8778 = 1;
        mapObjDefGroup->flags &= ~0x8000u;
    }
    //if (dword_CFBEB8)
    //    mapObjDefGroup->flags |= 0x8000u;
    CFrustum* frustum = CWorldScene::AllocFrustum();
    CWorldScene::FrustumPush(frustum, &CWorldScene::frustumStack[CWorldScene::frustumIndex]);
    mapObjDefGroup->frustumList.LinkToTail(frustum);
}

// OFFSET: 0x792E60
void CWorldScene::AddEntityToSortTable(CMapEntity* entity) {
    //if ((entity->unk_07C & 0x4000) != 0 && !entity->model.unk23) {
    //    CWorldScene::sortTable.culledEntityList.LinkToTail(entity);
    //    return;
    //}

    C3Vector outCorner;
    CWorldScene::GetNearestCornerToCamera(&entity->bbox, &outCorner);
    entity->m_distanceToCamera = CWorldScene::camPlane.n.z * outCorner.z + CWorldScene::camPlane.n.y * outCorner.y + CWorldScene::camPlane.n.x * outCorner.x + CWorldScene::camPlane.d;
    //BYTE1(entity->unk_024) = 2;
    if ((entity->unk_07C & 0x1) != 0) {
        CWorldScene::sortTable.culledEntityList.LinkToTail(entity);
        return;
    }

    auto planeDist = outCorner.y * CWorldScene::camPlaneXY.n.y + outCorner.z * CWorldScene::camPlaneXY.n.z + outCorner.x * CWorldScene::camPlaneXY.n.x + CWorldScene::camPlaneXY.d;
    int index = (int32_t)floorf(planeDist * 0.03f);
    if (planeDist <= 0.0f || index < 64) {
        CWorldScene::sortTable.table[planeDist <= 0.0f ? 0 : index].entityList.LinkToHead(entity);
    }
}

// OFFSET: 0x7983B0
CFrustum* CWorldScene::AllocFrustum() {
    CFrustum* frustum = CWorldScene::s_frustumFreeList.Head();
    if (!frustum) {
        auto m = STORM_ALLOC(sizeof(CFrustum));
        frustum = new (m) CFrustum();
        frustum->sceneLink.Unlink();

        CWorldScene::s_frustumFreeList.LinkToTail(frustum);
    }

    frustum->sceneLink.Unlink();
    return frustum;
}

// OFFSET: 0x78FB20
bool CWorldScene::FrustumCull(CAaBox* box) {
    return CWorldScene::frustumStack[CWorldScene::frustumIndex].Cull(box) == WorldCull_outside;
}

// OFFSET: 0x791120
bool CWorldScene::FrustumCull(CAaSphere* sphere) {
    return CWorldScene::frustumStack[CWorldScene::frustumIndex].Cull(sphere) == WorldCull_outside;
}

// OFFSET: 0x790AF0
void CWorldScene::FrustumSet(CRect* rect) {
    C3Vector corners[8] = {};

    for (int i = 0; i < 2; i++) {
        const C3Vector& c0 = s_frustumCorners[i * 4 + 0];
        const C3Vector& c1 = s_frustumCorners[i * 4 + 1];
        const C3Vector& c2 = s_frustumCorners[i * 4 + 2];
        const C3Vector& c3 = s_frustumCorners[i * 4 + 3];

        C3Vector edgeTop = c2 - c1;
        C3Vector edgeBot = c3 - c0;

        C3Vector topLeft = c1 + edgeTop * rect->minX;
        C3Vector topRight = c1 + edgeTop * rect->maxX;
        C3Vector botLeft = c0 + edgeBot * rect->minX;
        C3Vector botRight = c0 + edgeBot * rect->maxX;

        C3Vector diagLeft = topLeft - botLeft;
        C3Vector diagRight = topRight - botRight;

        corners[i * 4 + 0] = botLeft + diagLeft * rect->minY;
        corners[i * 4 + 1] = botLeft + diagLeft * rect->maxY;
        corners[i * 4 + 2] = botRight + diagRight * rect->maxY;
        corners[i * 4 + 3] = botRight + diagRight * rect->minY;
    }

    CWorldScene::frustumStack[CWorldScene::frustumIndex].CalcPlanesFromCorners(corners);
}

// OFFSET: 0x791100
void CWorldScene::FrustumSet(CFrustum* frustum) {
    CWorldScene::FrustumPush(&CWorldScene::frustumStack[CWorldScene::frustumIndex], frustum);
}

void CWorldScene::FrustumSet(C3Vector* corners, CRect* rect) {
    C3Vector sub[8] = {};

    for (int quad = 0; quad < 2; ++quad)
    {
        const C3Vector* in = corners + quad * 4;
        C3Vector* out = sub + quad * 4;

        const C3Vector dBottom(in[3].x - in[0].x, in[3].y - in[0].y, in[3].z - in[0].z);
        const C3Vector dTop(in[2].x - in[1].x, in[2].y - in[1].y, in[2].z - in[1].z);

        const C3Vector loBottom(in[0].x + dBottom.x * rect->minX,
                                in[0].y + dBottom.y * rect->minX,
                                in[0].z + dBottom.z * rect->minX);
        const C3Vector loTop(in[1].x + dTop.x * rect->minX,
                             in[1].y + dTop.y * rect->minX,
                             in[1].z + dTop.z * rect->minX);
        const C3Vector hiBottom(in[0].x + dBottom.x * rect->maxX,
                                in[0].y + dBottom.y * rect->maxX,
                                in[0].z + dBottom.z * rect->maxX);
        const C3Vector hiTop(in[1].x + dTop.x * rect->maxX,
                             in[1].y + dTop.y * rect->maxX,
                             in[1].z + dTop.z * rect->maxX);

        const C3Vector dLo(loTop.x - loBottom.x, loTop.y - loBottom.y, loTop.z - loBottom.z);
        const C3Vector dHi(hiTop.x - hiBottom.x, hiTop.y - hiBottom.y, hiTop.z - hiBottom.z);

        out[0].x = loBottom.x + dLo.x * rect->minY;
        out[0].y = loBottom.y + dLo.y * rect->minY;
        out[0].z = loBottom.z + dLo.z * rect->minY;

        out[1].x = loBottom.x + dLo.x * rect->maxY;
        out[1].y = loBottom.y + dLo.y * rect->maxY;
        out[1].z = loBottom.z + dLo.z * rect->maxY;

        out[2].x = hiBottom.x + dHi.x * rect->maxY;
        out[2].y = hiBottom.y + dHi.y * rect->maxY;
        out[2].z = hiBottom.z + dHi.z * rect->maxY;

        out[3].x = hiBottom.x + dHi.x * rect->minY;
        out[3].y = hiBottom.y + dHi.y * rect->minY;
        out[3].z = hiBottom.z + dHi.z * rect->minY;
    }

    CWorldScene::frustumStack[CWorldScene::frustumIndex].CalcPlanesFromCorners(sub);
}

// OFFSET: 0x790020
void CWorldScene::FrustumPush(CFrustum* a1, CFrustum* a2) {
    if (a1 != a2) {
        a1->planes[0].n.x = a2->planes[0].n.x;
        a1->planes[0].n.y = a2->planes[0].n.y;
        a1->planes[0].n.z = a2->planes[0].n.z;
        a1->planes[0].d = a2->planes[0].d;
        a1->planes[1] = a2->planes[1];
        a1->planes[2] = a2->planes[2];
        a1->planes[3] = a2->planes[3];
        a1->planes[4] = a2->planes[4];
        a1->planes[5] = a2->planes[5];
        a1->corners[0].x = a2->corners[0].x;
        a1->corners[0].y = a2->corners[0].y;
        a1->corners[0].z = a2->corners[0].z;
        a1->corners[1] = a2->corners[1];
        a1->corners[2] = a2->corners[2];
        a1->corners[3] = a2->corners[3];
        a1->corners[4] = a2->corners[4];
        a1->corners[5] = a2->corners[5];
        a1->corners[6] = a2->corners[6];
        a1->corners[7] = a2->corners[7];
        a1->lookPos = a2->lookPos;
        a1->lookAt = a2->lookAt;
        a1->lookUp = a2->lookUp;
        a1->fovy = a2->fovy;
        a1->aspect = a2->aspect;
        a1->minz = a2->minz;
        a1->maxz = a2->maxz;
    }
}

// OFFSET: 0x791950
void CWorldScene::FrustumPush() {
    ++CWorldScene::frustumIndex;
    CWorldScene::FrustumPush(
        &CWorldScene::frustumStack[CWorldScene::frustumIndex],
        &CWorldScene::frustumStack[CWorldScene::frustumIndex - 1]);
}

// OFFSET: 0x78FB50
void CWorldScene::FrustumPop() {
    CWorldScene::frustumIndex--;
}

// OFFSET: 0x78FB00
void CWorldScene::FrustumXform(C44Matrix& mat) {
    CWorldScene::frustumStack[CWorldScene::frustumIndex].Transform(mat);
}

// OFFSET: 0x7D6690
bool CWorldScene::InsideFrustumRect(CiRect* rect) {
    return rect->minX <= CWorldScene::s_frustumChunkRect.maxX && rect->minY <= CWorldScene::s_frustumChunkRect.maxY && rect->maxX >= CWorldScene::s_frustumChunkRect.minX && rect->maxY >= CWorldScene::s_frustumChunkRect.minY;
}

// OFFSET: 0x79A790
void CWorldScene::CullSortTable(CRect* a1) {
    // CWorldScene::CreateOcclusionVolumes(&CWorldScene::s_activeWorldView.x, stru_CDB108, 0);
    CWorldScene::FrustumPush();
    CWorldScene::FrustumSet(a1);
    for (int32_t i = 0; i < 64; i++) {
        CSortEntry* entry = &CWorldScene::sortTable.table[i];

        CWorldScene::CullChunks(entry, i);
        CWorldScene::CullMapObjDefGroups(entry, a1, i);
        // CWorldScene::CullLiquid(entry);
        // sub_793060(entry);
        float v4 = (float)i * 33.333332f;
        uint8_t v3 = CWorldView::GetFadeLevelForDistance(v4);
        CWorldScene::CullDoodads(entry, v3);
        CWorldScene::CullEntitys(entry);
    }
    // CWorldScene::CullHorizon(a1);
    CWorldScene::FrustumPop();
}

// OFFSET: 0x7987A0
void CWorldScene::CullDoodads(CSortEntry* entry, uint8_t fadeLevel) {
    
}

// OFFSET: 0x791CB0
void CWorldScene::AddDoodadDefModelToModelScene(CMapDoodadDef* a1) {
    a1->doodadDefLink.Unlink();

    if ((a1->flags & MAPOBJ_FLAG_DISABLED) == 0 && (CWorld::s_enables & CWorld::Enables::Enable_Doodads) != 0) {
        //a1->unk_030 = a1->sphere.n.y * CWorldScene::camPlane.n.y + a1->sphere.n.z * CWorldScene::camPlane.n.z + a1->sphere.n.x * CWorldScene::camPlane.n.x + CWorldScene::camPlane.d - a1->sphere.d;
        //v15 = 1.0;
        //if ((CWorld::enables & Enable_Occluders) != 0 && (unk_C & 0x800) == 0) {
        //    v6 = a1->sphere.n.z - CWorldScene::s_activeWorldView.z;
        //    v7 = a1->sphere.n.y - CWorldScene::s_activeWorldView.y;
        //    v8 = v7 * v7 + v6 * v6;
        //    v9 = a1->sphere.n.x - CWorldScene::s_activeWorldView.x;
        //    v10 = v9 * v9 + v8;
        //    if (v10 > CWorldView::s_fadeDistMax[a1->fadeLevel])
        //        return;
        //    if (v10 > flt_ADF3DC[a1->fadeLevel] && (!dword_CF08F8 ? (v11 = sqrt(v10)) : (v16 = v10, v11 = fsqrt(v16)),
        //                                            v12 = 1.0 - (v11 - flt_ADF3C8[a1->fadeLevel]) / flt_ADF38C[a1->fadeLevel],
        //                                            v15 = v12,
        //                                            v12 <= 0.99000001)) {
        //        if (v12 <= 0.0099999998)
        //            return;
        //    } else {
        //        v15 = 1.0;
        //    }
        //}
        a1->model->SetAnimating(1);
        a1->model->SetVisible(1);
        //if (ukn19)
        //    model->m_bitFlags |= 0x20000u;
        //else
        //    model->m_bitFlags |= 0x10000u;
        //a1->model->ukn74.a2 = v15;
    }
}

// OFFSET: 0x799980
void CWorldScene::CullDoodadsExterior(STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink)* linkList, uint8_t fadeLevel) {
    for (auto mapChunk = linkList->Head(); mapChunk;) {
        auto next = linkList->Next(mapChunk);

        CMapDoodadDef* mapDoodadDef = reinterpret_cast<CMapDoodadDef*>(mapChunk->owner);
        if (mapDoodadDef->fadeLevel < fadeLevel) {
            if ((CMap::header.flags & 0x8) != 0)
                return;
        } else {
            if (mapDoodadDef->model && (mapDoodadDef->flags & MAPOBJ_FLAG_PREPARED) != 0) {
                // if (mapDoodadDef->unk_0B0 == dword_CD87B0) {
                //     mapChunk = next;
                //     continue;
                // }
                // mapDoodadDef->unk_0B0 = dword_CD87B0;
                mapDoodadDef->unk_025 = 1;

                bool visible = false;

                if (!CWorldScene::FrustumCull(&mapDoodadDef->sphere)) {
                    mapDoodadDef->unk_025 = 0;
                    visible = true; //(CWorldOcclusion::QueryBuffer_0(&mapDoodadDef->sphere, 16) < 2);
                }

                if (visible) {
                    CWorldScene::AddDoodadDefModelToModelScene(mapDoodadDef);
                    ++CWorldScene::s_doodadsRendered;
                } else {
                    bool animate = true; //(mapDoodadDef->unk_07C & 0x400) != 0;

                    if (!animate) {
                        float dx = mapDoodadDef->sphere.c.x - CWorldScene::s_activeWorldView.x;
                        float dy = mapDoodadDef->sphere.c.y - CWorldScene::s_activeWorldView.y;
                        float dz = mapDoodadDef->sphere.c.z - CWorldScene::s_activeWorldView.z;
                        animate = (dx * dx + dy * dy + dz * dz < 100.0f);
                    }

                    mapDoodadDef->model->SetAnimating(animate ? 1 : 0);
                }
            } else {
                //v15.min.x = v4;
                //v15.min.y = v4;
                //v15.min.z = v4;
                //v15.max.x = v4;
                //v15.max.y = v4;
                //v15.max.z = v4;
                //m_shared = model->m_shared;
                //x_low = LODWORD(m_shared->m_boundingBox.min.x);
                //m_shared = (CM2Shared*)((char*)m_shared + 340);
                //v14[0] = x_low;
                //v14[1] = (int)m_shared->m_cache;
                //v14[2] = m_shared->m_flags;
                //v14[3] = m_shared->m_asyncObject;
                //v14[4] = (int)m_shared->m_callbackList;
                //v14[5] = m_shared->ukn6;
                //CWorldMath::TransformAABox(&owner->mat, (float*)v14, &v15);
                //sub_7946D0(dword_ADF4A0, (int)&v15, 10.0);
                //v4 = 0.0;
            }
        }

        mapChunk = next;
    }
}

// OFFSET: 0x79A160
void CWorldScene::CullMapObjDefGroups(CSortEntry* entry, CRect* a2, uint32_t a3) {
    for (auto mapObjDefGroup = entry->exteriorGroupList.Head(); mapObjDefGroup;) {
        auto next = entry->exteriorGroupList.Next(mapObjDefGroup);

        mapObjDefGroup->sortEntryLink.Unlink();

        auto parent = mapObjDefGroup->parentLinkList.Head();
        auto mapObjDef = static_cast<CMapObjDef*>(parent->ref);

        if (!CWorldScene::FrustumCull(&mapObjDefGroup->bbox)
            && !CWorldOcclusion::QueryVolumes(&mapObjDefGroup->sphere)
            && !CWorldOcclusion::QueryBuffer(&mapObjDefGroup->bbox, 1)) {
            CWorldScene::CullMapObjDefGroupFromExterior(mapObjDef, mapObjDefGroup, a2, 0);
            //CWorldScene::AddDoodadDefs(&m_next->unk_78, a3);
        }

        mapObjDefGroup = next;
    }
}

// OFFSET: 0x7B3A10
void CWorldScene::CullMapObjDefGroupFromExterior(CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup, CRect* a3, uint32_t a4) {
    CMapObj::SetGroupRenderCallback(reinterpret_cast<RENDER_CALLBACK>(CWorldScene::AddMapObjDefGroupToSortTable), mapObjDef);
    s_curMapObjDef = mapObjDef;
    CWorldScene::FrustumPush();
    CWorldScene::FrustumSet(CWorldScene::s_frustumCorners, a3);
    auto groupFlags = mapObjDef->owner->GetGroupFlags(mapObjDefGroup->groupNum);
    if (!CWorldScene::FrustumCull(&mapObjDefGroup->bbox) && (a4 || !CWorldOcclusion::QueryBuffer(&mapObjDefGroup->bbox, 1))) {
        if ((groupFlags & 0x10000) != 0) {
            CMapObj::InvokeGroupRenderCallback(mapObjDef->owner, mapObjDefGroup->groupNum);
            CWorldScene::FrustumPop();
            return;
        }
        if ((groupFlags & 8) != 0) {
            CRect rect;
            rect.minY = a3->minY * 2.0f - 1.0f;
            rect.minX = a3->minX * 2.0f - 1.0f;
            rect.maxY = a3->maxY * 2.0f - 1.0f;
            rect.maxX = a3->maxX * 2.0 - 1.0f;
            CWorldScene::RenderThruPortalsExterior(mapObjDef->owner, mapObjDef->mat, mapObjDef->invMat, CWorldScene::s_activeWorldView, CWorldScene::camTarget, rect, mapObjDefGroup->groupNum);
        }
    }
    CWorldScene::FrustumPop();
}

// OFFSET: 0x793060
void CWorldScene::CullEntitys(CSortEntry* entry) {
    CWorldScene::s_entityCanLink = 0;
    for (auto entity = entry->entityList.Head(); entity;) {
        auto next = entry->entityList.Next(entity);

        entity->sortEntryLink.Unlink();

        if (CWorldScene::FrustumCull(&entity->sphere) || CWorldOcclusion::QueryVolumes(&entity->sphere) || CWorldOcclusion::QueryBuffer(&entity->sphere, 0)) {
            CWorldScene::sortTable.culledEntityList.LinkToTail(entity);
        } else {
            //entity->model->unk0024 = 0;
            if (entity->model) {
                entity->model->SetAnimating(1);
                entity->model->SetVisible(1);
                if (entity->model->m_attachParent) {
                    entity->model->m_flag20000 = 1;
                } else {
                    entity->model->m_flag10000 = 1;
                }

                //entity->model->m_lightingCallback = CMapStaticEntity::ModelLightingCallback;
                entity->model->m_lightingCallback = CWorldSceneLightingCallback;
                entity->model->m_lightingArg = entity;
            }
            if (entity->m_func && !entity->m_func(entity->m_funcParam, 5, entity->m_funcParam64, entity->m_funcParam32)) {
                entity = next;
                continue;
            }
            CWorldScene::sortTable.visibleEntityList.LinkToTail(entity);
        }

        entity = next;
    }
    CWorldScene::s_entityCanLink = 1;
}

// OFFSET: 0x799D40
void CWorldScene::CullChunks(CSortEntry* entry, int32_t index) {
    bool v19 = true;

    if ((CWorld::s_enables & CWorld::Enables::Enable_Culling) == 0 || index >= 63)
        v19 = false;

    for (auto mapChunk = entry->mapChunkList.Head(); mapChunk;) {
        auto next = entry->mapChunkList.Next(mapChunk);
        mapChunk->sortListLink.Unlink();

        if (CWorldScene::frustumStack[CWorldScene::frustumIndex].Cull(&mapChunk->bbox) == WorldCull_outside) {
            mapChunk = next;
            continue;
        }

        if (CWorldOcclusion::QueryVolumes(&mapChunk->sphere) || CWorldOcclusion::QueryBuffer(&mapChunk->bbox, 0)) {
            mapChunk = next;
            continue;
        }

        uint8_t fadeLevel = CWorldView::GetFadeLevelForDistance(mapChunk->distToCamera);
        CWorldScene::CullDoodadsExterior(&mapChunk->doodadDefLinkList, fadeLevel);
        if (CWorldScene::frustumStack[CWorldScene::frustumIndex].Cull(&mapChunk->bbox2) == WorldCull_outside || CWorldOcclusion::QueryBuffer(&mapChunk->bbox2, 0)) {
            mapChunk = next;
            continue;
        }

        if (mapChunk->header->holes != 0xFFFF) {
            ++CWorldScene::s_chunksRendered;
            mapChunk->RenderPrep();
            if (mapChunk->renderChunk) {
                uint8_t layersCount = 0;
                if ((mapChunk->renderChunk->unkFlags & 8) != 0)
                    layersCount = mapChunk->renderChunk->layersCount;

                int32_t v10 = 0;
                if (layersCount) {
                    if ((mapChunk->renderChunk->unk_0A & 1) != 0)
                        v10 = (mapChunk->renderChunk->unk_0A & 4 | 2) >> 1;
                    else
                        v10 = (mapChunk->renderChunk->unk_0A >> 1) & 2;
                }

                CWorldScene::sortTable.renderChunkLists[4 * layersCount + v10].LinkToTail(mapChunk->renderChunk);
            }
        }

        if (v19) {
            //    if (m_next->header->holes) {
            //        v12 = index;
            // LABEL_36
            //         HashTable::AddEntry(&CWorldScene::sortTable.table[v12].unkList8, (char*)m_next);
            //         continue;
            //     }
            //     sub_790520(m_next, dword_AEEE3C[dword_CD8F3C], &v15);
            //     v13 = 0;
            //     v14 = sub_790620(&v15);
            //     if (v14 <= 0.0 || (v17 = v14 * 0.029999999, v18 = (int)(v17 - halfConst), v13 = v18, v18 < 64)) {
            //         v12 = v13;
            //         goto LABEL_36;
            //     }
        }
        mapChunk = next;
    }
}

// OFFSET: 0x79A870
void CWorldScene::Render(const C3Vector& cameraPos, float time) {
    // if (!dword_CD87A4)
    //     sub_792BD0();
    // sub_790920();
    GxRsPush();
    GxXformPush(GxXform_World);

    // dword_CD8774 = 0;
    CWorldScene::s_chunksRendered = 0;
    CWorldScene::s_doodadsRendered = 0;
    // dword_CD8624 = 0;
    CWorldScene::frustumStack[CWorldScene::frustumIndex].CalcPlanesFromCorners(CWorldScene::s_frustumCorners);
    // flt_CD8784 = World::s_farClip - 33.333332;
    // dword_CD8778 = ((int)stru_CD9048.unkList1.m_terminator.m_next & 1) == 0 && stru_CD9048.unkList1.m_terminator.m_next;
    // ActiveCamera = CGWorldFrame::GetActiveCamera();
    // flt_CD877C = CWorld::farFog / cos(((double(__thiscall*)(CGCamera*))ActiveCamera->Fov)(ActiveCamera) * 0.5) - CWorld::farFog;
    // sub_782F20();
    CMapRenderChunk::UpdatePools();
    // sub_7AE060();
    // sub_7B2A80();
    // memset(&byte_CD87B8, 0, 0x180u);
    // memset32(flt_CD8938, 0xC9742400, 0x180u);
    CWorldOcclusion::ClearVolumes();
    // CWorldScene::AddWorldOccluders();
    if (CWorldScene::s_viewerMapObjDef) {
        //++dword_CD87B0;
        if (CWorldScene::s_viewerMovedMapObjDef) {
            CWorldScene::RenderMapObjWithCallback(CWorldScene::s_viewerMovedMapObjDef, &CWorldScene::s_viewerMovedMapObjGroups);
            //stru_ADF570.rect.minX = 3.4028235e38;
            //stru_ADF570.rect.minY = 3.4028235e38;
            //stru_ADF570.verts = 0;
            //stru_ADF570.rect.maxX = -3.4028235e38;
            //stru_ADF570.vertCount = 0;
            //stru_ADF570.rect.maxY = -3.4028235e38;
            CWorldScene::frustumPortalView.verts = 0;
            CWorldScene::frustumPortalView.vertCount = 0;
            //stru_ADF570.maxViewDepth = -1.0;
            CWorldScene::frustumPortalView.rect.minX = 3.4028235e38;
            CWorldScene::frustumPortalView.rect.minY = 3.4028235e38;
            CWorldScene::frustumPortalView.rect.maxX = -3.4028235e38;
            CWorldScene::frustumPortalView.rect.maxY = -3.4028235e38;
            CWorldScene::frustumPortalView.maxViewDepth = -1.0;
            //bn_TSGrowableArray_CPortalView_SetCount(&stru_CDD0E8.m_alloc, 0);
            //bn_TSGrowableArray_CPortalView_SetCount(&stru_CDD0F8.m_alloc, 0);
        }
        CWorldScene::RenderMapObjWithCallback(CWorldScene::s_viewerMapObjDef, &CWorldScene::s_viewerMapObjGroups);
        if (CWorldScene::frustumPortalView.maxViewDepth < 0.0) {
            //CSortTable::Clear(&CWorldScene::sortTable);
        } else {
            //flt_CD8780 = CWorldScene::frustumPortalView.maxViewDepth + 33.333332;
            CWorldScene::CullSortTable(&CWorldScene::frustumPortalView.rect);
        }
        //v16.minY = 0.0;
        //v16.minX = 0.0;
        //v16.maxY = 1.0;
        //v16.maxX = 1.0;
        //CWorldScene::CullThroughPortal(&v16);
    } else {
    //    ++dword_CD87B0;
    //    stru_ADF570.rect.minY = 0.0;
    //    stru_ADF570.rect.minX = 0.0;
    //    stru_ADF570.maxViewDepth = 0.0;
    //    v16.minY = 0.0;
    //    stru_ADF570.rect.maxY = 1.0;
    //    v16.minX = 0.0;
    //    stru_ADF570.rect.maxX = 1.0;
    //    v16.maxY = 1.0;
    //    v16.maxX = 1.0;
    CWorldScene::frustumPortalView.rect.minY = 0.0;
    CWorldScene::frustumPortalView.maxViewDepth = 0.0;
    //    stru_ADF570.vertCount = 0;
    CWorldScene::frustumPortalView.rect.minX = 0.0;
    //    flt_CD8780 = -10000.0;
    CWorldScene::frustumPortalView.rect.maxY = 1.0;
    CWorldScene::frustumPortalView.rect.maxX = 1.0;
    CWorldScene::frustumPortalView.vertCount = 0;
    CWorldScene::CullSortTable(&CWorldScene::frustumPortalView.rect);
    }
    //CWorldScene::CullMapObjDefGroup();
    //maybe_CWorldScene__UpdateSortedModels();
    CWorldScene::sortTable.pendingExteriorGroupList.UnlinkAll();
    CImVector color = { 0x00, 0x00, 0x00, 0xFF };
    // ActiveDayNight = DayNight::GetActiveDayNight();
    // if (sub_683100(8)) {
    //    if (flt_ADF580 >= 0.0) {
    //        if (dword_CD8794 || !ActiveDayNight[115] && sub_7ECE00())
    //            color = ActiveDayNight[35];
    //        else
    //            color = 0;
    //    } else {
    //        color = ActiveDayNight[40];
    //    }
    //}
    GxSceneClear(3, color);
    // sub_9A80C0(&off_B2EB68);
    // if (dword_CD87A8) {
    //    v5 = *(float*)(dword_CD87A8 + 112);
    //    v6 = *(float*)(dword_CD87A8 + 116);
    //    v17 = *(float*)(dword_CD87A8 + 108);
    //    v18 = v5;
    //    v19 = v6 + 2.0;
    //    sub_7BB670(&v17);
    //} else {
    //    sub_7BB670(&CWorldScene::s_activeWorldView.x);
    //}

    if (CWorldScene::s_m2Scene) {
        CWorldScene::s_m2Scene->m_flags |= 1u;
        CWorldScene::s_m2Scene->AdvanceTime(static_cast<uint32_t>(time * 1000.0f));
        CWorldScene::s_m2Scene->Animate(cameraPos);
        CWorldScene::s_m2Scene->m_flags &= ~1u;
    }

    // sub_6FDA20();
    // CShadowQuery::Update();

    CShaderEffect::UpdateProjMatrix();
    // CWorldSceneRender::ApplyFogState();
    CWorldScene::RenderChunks();
    CWorldScene::RenderMapObjDefGroups();
    //CWorldScene::RenderHorizon();

    //####TODO TESTING
    DayNight::RenderSky();
    //##########

    // CWorldScene::UpdateLighting();
    // sub_795F80();
    // if (flt_ADF580 >= 0.0) {
    //     if (dword_CDD0EC) {
    //         sub_7968D0();
    //         sub_796C10(&unk_CDD0E8, 1);
    //     }
    //     if (dword_CDD0FC)
    //         sub_796C10(&unk_CDD0F8, 0);
    //     if (dword_CD861C) {
    //         sub_7F31C0(0, dword_CD861C, 0, *((float*)ActiveDayNight + 39));
    //         sub_7F31C0(1, 0, 0, 0.0);
    //     }
    //     if (!dword_CD8794)
    //         DayNight::CDayNightObject::RenderSky((int)&flt_ADF570);
    // }
    // sub_793980();
    // sub_8A2F00();
    // sub_793D20();
    // if ((CWorld::enables & 0x1000000) != 0 && dword_CD8610)
    //     sub_8A2240((char*)dword_CD8610, (int)&CWorldScene::s_activeWorldView, 0);
    // v10 = 0.0;
    // v11 = 0.0;
    // v12 = 0.0;
    // v13 = 0.0;
    // v14 = 0.0;
    // v15 = 0.0;
    // if (dword_CD7544 && (unsigned __int8)sub_784A30(&v10)) {
    //     v10 = v10 + CWorldScene::s_activeWorldView.x;
    //     v11 = v11 + CWorldScene::s_activeWorldView.y;
    //     v12 = v12 + CWorldScene::s_activeWorldView.z;
    //     v13 = CWorldScene::s_activeWorldView.x + v13;
    //     v14 = CWorldScene::s_activeWorldView.y + v14;
    //     v15 = CWorldScene::s_activeWorldView.z + v15;
    // }

    GxXformPop(GxXform_World);
    GxRsPop();

    CursorResetCursor();

    if (CWorld::GetEnables() & CWorld::Enables::Enable_Collisions) {
        CWorldScene::RenderCollisionDebug();
        CMap::debugVertexArray.SetCount(0);
        CMap::debugIndexArray.SetCount(0);
    }
}

// OFFSET: 0x798DA0
void CWorldScene::RenderChunks() {
    GxRsPush();

    g_theGxDevicePtr->RsGet(GxRs_FogColor, CWorldScene::s_savedFogColor);

    if (CMap::enableTerrainShaderVertex) {
        C44Matrix worldTranslate;
        C3Vector vec = { -CWorldScene::s_activeWorldView.x, -CWorldScene::s_activeWorldView.y, -CWorldScene::s_activeWorldView.z };
        worldTranslate.Translate(vec);

        C44Matrix view;
        g_theGxDevicePtr->XformView(view);

        // CMapRenderChunk::InitializeVertexShaderConstants(worldTranslate, view);
    } else {
        DayNight::DNInfo* activeDayNight = DayNight::GetInfo();

        GxRsSet(GxRs_FogStart, activeDayNight->m_fog.start);
        GxRsSet(GxRs_FogEnd, activeDayNight->m_fog.end);
        GxRsSet(GxRs_MatDiffuse, 0xFF7F7F7F);

        if (CMap::enableSpecularTerrain) {
            GxRsSet(GxRs_MatSpecular, 0xFFFFFFFF);
            GxRsSet(GxRs_MatSpecularExp, 20.0f);
        }

        for (int32_t i = 0; i < 5; i++) {
            GxRsSet((EGxRenderState)(GxRs_TextureCoord0 + i), i);
            GxRsSet((EGxRenderState)(GxRs_TexGen0 + i), 2);
            GxRsSet((EGxRenderState)(GxRs_TextureShader0 + i), 1);
        }
    }

    if (CMap::gTerrainPixelShadersValid) {
        DayNight::DNInfo* activeDayNight = DayNight::GetInfo();

        if (g_theGxDevicePtr->Caps().int134) {
            GxRsSet(GxRs_FogColor, activeDayNight->m_fog.color.value);
        } else {
            C4Vector fogColor = C4Vector(
                activeDayNight->m_fog.color.r * 0.0039215689f,
                activeDayNight->m_fog.color.g * 0.0039215689f,
                activeDayNight->m_fog.color.b * 0.0039215689f,
                activeDayNight->m_fog.color.a * 0.0039215689f);

            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 2, &fogColor, 1);
        }

        if (g_theGxDevicePtr->Caps().int138) {
            GxRsSet(GxRs_Fog, 1);
        }

        // CShadowCache::SetShadowMapTerrain();
    } else {
        DayNight::DNInfo* activeDayNight = DayNight::GetInfo();

        GxRsSet(GxRs_FogColor, activeDayNight->m_fog.color.value);
        GxRsSet(GxRs_Fog, 1);
        GxRsSet(GxRs_ColorOp0, 1);
        GxRsSet(GxRs_AlphaOp0, 0);
        GxRsSet(GxRs_ColorOp1, 0);
        GxRsSet(GxRs_AlphaOp1, 0);
    }

    CWorldScene::RenderChunksSinglePass();
    CWorldScene::RenderChunksSolid();
    //CWorldScene::RenderChunksZoneDebug();

    for (int32_t i = 0; i < 5; i++) {
        g_theGxDevicePtr->m_xforms[GxXform_Tex0 + i].Identity();
    }

    GxRsPop();
}

// OFFSET: 0x7964A0
void CWorldScene::RenderMapObjDefGroups() {
    g_theGxDevicePtr->RsPush();
    CWorldScene::FrustumPush();
    //bn_CShadowCache_SetShadowMapGenericGlobal();
    for (auto mapObjDefGroup = CWorldScene::sortTable.mapObjDefGroup.Head(); mapObjDefGroup;) {
        auto next = CWorldScene::sortTable.mapObjDefGroup.Next(mapObjDefGroup);

        mapObjDefGroup->sortTableLink.Unlink();
        auto v8 = mapObjDefGroup->parentLinkList.Head();
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(v8->ref);

        if ((CWorld::s_enables & CWorld::Enables::Enable_WMO) != 0) {
            C44Matrix mat = mapObjDef->mat;
            C44Matrix camTranslate;
            C3Vector vec = { -CWorldScene::s_activeWorldView.x, -CWorldScene::s_activeWorldView.y, -CWorldScene::s_activeWorldView.z };
            camTranslate.Translate(vec);
            mat *= camTranslate;
            CWorldScene::SetWorldProjection(mat);
            //unk_68 = mapObjDefGroup->unk_68;
            //if (unk_68 && *(unk_68 + 16))
            //    (*(**(unk_68 + 16) + 8))(*(unk_68 + 16), (mapObjDefGroup->flags >> 15) & 1);
            CM2Lighting lighting = CM2Lighting(mapObjDefGroup->sphere);
            s_m2Scene->SelectLights(&lighting);
            mapObjDefGroup->SelectLights(&lighting);
            CWorldScene::SetupLighting(&lighting, &CWorldScene::s_activeWorldView);
            //ActiveDayNight = DayNight::GetActiveDayNight();
            //if (*&ref->unk_148 && ref->unk_148 == dword_CD7770 && ref->unk_14C == dword_CD7774)
            //    sub_7A8430(ActiveDayNight->unk107);
            s_curGroupIsInterior = (mapObjDefGroup->flags >> 15) & 1;
            mapObjDef->owner->RenderGroup(mapObjDefGroup->groupNum, mapObjDef->invMat, &mapObjDefGroup->frustumList);
        }
        for (auto frustum = mapObjDefGroup->frustumList.Head(); frustum;) {
            auto nextFrustum = mapObjDefGroup->frustumList.Next(frustum);

            frustum->sceneLink.Unlink();
            CWorldScene::s_frustumFreeList.LinkToTail(frustum);

            frustum = nextFrustum;
        }

        mapObjDefGroup = next;
    }

    CWorldScene::FrustumPop();
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x7A8320
void CWorldScene::SetWorldProjection(C44Matrix& mat) {
    if (CShaderEffect::s_enableShaders) {
        C44Matrix view;
        g_theGxDevicePtr->XformView(view);

        C44Matrix worldView = (mat * view).Transpose();

        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 31, reinterpret_cast<C4Vector*>(&worldView), 4);
    }

    g_theGxDevicePtr->XformSet(GxXform_World, mat);
}

void CWorldScene::RenderChunksSinglePass() {
    for (int32_t pass = 0; pass < 4; pass++) {
        switch (pass) {
        case 0:
            CMapRenderChunk::SetShaders(0, 0);
            break;
        case 1:
            CMapRenderChunk::SetShaders(0, 1);
            break;
        case 2:
            CMapRenderChunk::SetShaders(1, 0);
            break;
        case 3:
            CMapRenderChunk::SetShaders(1, 1);
            break;
        }

        for (int32_t layer = 0; layer < 4; layer++) {
            if (CMapRenderChunk::s_currentShaderX[layer])
                GxRsSet(GxRs_PixelShader, CMapRenderChunk::s_currentShaderX[layer]);

            int32_t layerIndex = 4 + pass + (layer * 4);
            for (auto renderChunk = CWorldScene::sortTable.renderChunkLists[layerIndex].Head(); renderChunk;) {
                auto next = CWorldScene::sortTable.renderChunkLists[layerIndex].Next(renderChunk);
                renderChunk->RenderSetup(1);
                //if ((CWorld::s_enables & CWorld::Enables::Enable_Terrain) != 0) {
                //     if (*v36) {
                //         if (CMap::enableTerrainShaderVertex)
                //             sub_7D2D70((int)v2);
                //         else
                //             sub_7D28B0(v2);
                //     } else {
                CMapRenderChunk::s_renderLayersFunc(renderChunk);
                //    }
                //}

                renderChunk->renderChunkLink.Unlink();

                if ((CWorld::s_enables & 0x40000000) != 0) {
                    // v6 = *(int*)((char*)&v2->renderChunkLink.m_prevLink + v32.m_linkoffset);
                    // v7 = (TSLink*)((char*)v2 + v32.m_linkoffset);
                    // if (v6) {
                    //     v8 = (unsigned int)v7->m_next;
                    //     if ((v8 & 1) == 0 && v8)
                    //         v9 = (TSLink**)((char*)&v7->m_prevlink + v8 - *(_DWORD*)(v6 + 4));
                    //     else
                    //         v9 = (_DWORD*)(v8 & 0xFFFFFFFE);
                    //     *v9 = v6;
                    //     v7->m_prevlink->m_next = v7->m_next;
                    //     v7->m_prevlink = 0;
                    //     v7->m_next = 0;
                    // }
                    // v10 = v32.m_terminator.m_prevlink;
                    // v1 = v34;
                    // v7->m_prevlink = v32.m_terminator.m_prevlink;
                    // v7->m_next = v10->m_next;
                    // v10->m_next = v2;
                    // v2 = v33;
                    // v32.m_terminator.m_prevlink = v7;
                } else {
                    CMap::s_mapRenderChunkUpdateList.LinkToTail(renderChunk);
                }
                renderChunk = next;
            }
        }
    }

    GxRsSet(GxRs_VertexShader, (CGxShader*)nullptr);
    GxRsSet(GxRs_PixelShader, (CGxShader*)nullptr);

    // TODO 0x40000000 render list
}

// OFFSET: 0x793B10
void CWorldScene::RenderChunksSolid() {
    CMapRenderChunk::SetShaders(0, 0);
    if (CMapRenderChunk::s_currentShaderX[0])
        GxRsSet(GxRs_PixelShader, CMapRenderChunk::s_currentShaderX[0]);
    for (auto renderChunk = CWorldScene::sortTable.renderChunkLists[0].Head(); renderChunk;) {
        auto next = CWorldScene::sortTable.renderChunkLists[0].Next(renderChunk);
        renderChunk->RenderSetup(1);
        //    if ((CWorld::enables & 2) != 0) {
        //        if (CMapRenderChunk::s_currentShaderX) {
        //            if (CMap::enableTerrainShaderVertex)
        //                CMapRenderChunk::RenderSolidVertexPixelShader(m_next);
        //            else
        //                CMapRenderChunk::RenderSolidPixelShader((int)m_next);
        //        } else {
        renderChunk->RenderSolid();
        //        }
        //    }

        renderChunk->renderChunkLink.Unlink();
        CMap::s_mapRenderChunkUpdateList.LinkToTail(renderChunk);
        renderChunk = next;
    }
}

// OFFSET: 0x7A6E00
void CWorldScene::SetupMapObjDefCull(CMapObj* mapObj, C44Matrix& a2, C44Matrix& a3, C3Vector& a4, C3Vector& a5) {
    C44Matrix mat;
    C3Vector vec = { -a4.x, -a4.y, -a4.z };
    mat.Translate(vec);
    mat = a2 * mat;
    g_theGxDevicePtr->XformSet(GxXform_World, mat);

    CWorldScene::s_camPosLocal = a3.TransformPoint(a4);
    CWorldScene::s_camTargetLocal = a3.TransformPoint(a5);

    C3Vector vec2 = {
        CWorldScene::s_camTargetLocal.x - CWorldScene::s_camPosLocal.x,
        CWorldScene::s_camTargetLocal.y - CWorldScene::s_camPosLocal.y,
        CWorldScene::s_camTargetLocal.z - CWorldScene::s_camPosLocal.z
    };
    float sq = vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z;
    if (sq > 0.000099999997) {
        float v13 = 1.0f / sqrt(sq);
        vec2.x *= v13;
        vec2.y *= v13;
        vec2.z *= v13;
    }

    CWorldScene::s_camPlaneLocal.n = vec2;
    CWorldScene::s_camPlaneLocal.d = -(s_camPosLocal.x * vec2.x + s_camPosLocal.y * vec2.y + s_camPosLocal.z * vec2.z);

    GxXformViewProj(CWorldScene::s_viewProj);

    C44Matrix viewMat;
    g_theGxDevicePtr->XformView(viewMat);
    CWorldScene::s_modelView = mat * viewMat;
    CWorldScene::s_modelViewProj = mat * CWorldScene::s_viewProj;
    CWorldScene::s_mapObjToWorld = a2;
    if (mapObj->unk_1E4 == 0xFFFF) {
        for (int32_t i = 0; i < mapObj->groupInfoCount; i++) {
            if ((mapObj->groupInfo[i].flags & 0x10008) == 0) {
                mapObj->unk_1E4++;
            }
        }
    }
    CWorldScene::s_cullStateValid = true;
}

// OFFSET: 0x7AD350
void CWorldScene::RenderThruPortalsExterior(CMapObj* mapObj, C44Matrix& mat, C44Matrix& invMat, C3Vector& worldPos, C3Vector& camTarget, CRect& a6, int32_t a7) {
    CWorldScene::SetupMapObjDefCull(mapObj, mat, invMat, worldPos, camTarget);
    s_interiorPass = 0;
    if (s_curMapObjDef != s_stampedMapObjDef) {
        ++s_portalStamp;
        s_stampedMapObjDef = s_curMapObjDef;
    }
    CWorldScene::RenderThruPortals(mapObj, a7, 0xFFFF, a6, 0, 0);
}

// OFFSET: 0x7AD1F0
void CWorldScene::RenderInterior(CMapObj* mapObj, C44Matrix& mat, C44Matrix& invMat, C3Vector& worldPos, C3Vector& camTarget, TSGrowableArray<uint16_t>* groups) {
    CWorldScene::SetupMapObjDefCull(mapObj, mat, invMat, worldPos, camTarget);
    s_interiorPass = 1;
    //dword_CD8620 = 0;
    if (s_curMapObjDef != s_stampedMapObjDef) {
        ++s_portalStamp;
        s_stampedMapObjDef = s_curMapObjDef;
    }

    CWorldScene::FrustumPush();
    CRect rect;
    rect.minY = -1.0f;
    rect.minX = -1.0f;
    rect.maxY = 1.0f;
    rect.maxX = 1.0f;
    for (int32_t i = 0; i < groups->Count(); i++) {
        CMapObjGroup* group = mapObj->GetGroup(groups->Ptr()[i], false);
        if (group) {
            //if ((group->flags & 0x48) == 0)
            //    dword_CD8620 = 1;
            CWorldScene::RenderThruPortals(mapObj, groups->Ptr()[i], 0xFFFF, rect, 0, 1);
        }
    }
    CWorldScene::FrustumPop();

    CAaBox box;
    for (int32_t i = 0; i < mapObj->groupInfoCount; i++) {
        SMOGroupInfo groupInfo = mapObj->groupInfo[i];
        if ((groupInfo.flags & 0x10000) != 0) {
            CWorldMath::TransformAABox(mat, groupInfo.boundingBox, box);
            if (!CWorldScene::FrustumCull(&box)) {
                if (mapObj->GetGroup(i, false)) {
                    if (CMapObj::gRenderCallback)
                        CMapObj::gRenderCallback(i, CMapObj::gRenderUserParam);
                }
            }
        }
    }
}

// OFFSET: 0x7AC060
void CWorldScene::RenderThruPortals(CMapObj* mapObj, uint32_t groupNum, uint32_t fromGroup, CRect& ndcRect, uint32_t depth, int32_t interior) {
    if (depth > s_maxPortalDepth)
        return;

    CMapObjGroup* group = mapObj->GetGroup(groupNum, false);
    if (!group)
        return;

    if ((group->flags & 0x10000) != 0)
        return;

    if (interior && (group->flags & 0x48) != 0) {
        interior = 0;
    }
    s_curGroupIsInterior = interior;
    //if (s_interiorPass && (v7->flags & 0x40000) != 0)
    //    CWorldScene::s_interiorSkybox = p_objectIndex->skybox;

    if (CMapObj::gRenderCallback)
        CMapObj::gRenderCallback(groupNum, CMapObj::gRenderUserParam);

    if (!group->portalCount)
        return;

    //if ((dword_D1C3D0 & 1) == 0) {
    //    dword_D1C3D0 |= 1u;
    //    s_pendingPortalViews.m_alloc = 0;
    //    s_pendingPortalViews.m_count = 0;
    //    s_pendingPortalViews.m_data = 0;
    //    s_pendingPortalViews.m_chunk = 0;
    //    atexit(maybe_StaticDtor_at_9DBE20);
    //}
    //if ((dword_D1C3D0 & 2) == 0) {
    //    dword_D1C3D0 |= 2u;
    //    s_coveredRects.m_alloc = 0;
    //    s_coveredRects.m_count = 0;
    //    s_coveredRects.m_data = 0;
    //    s_coveredRects.m_chunk = 0;
    //    atexit(maybe_StaticDtor_CRect);
    //}
    if (!s_interiorPass && !depth) {
        s_pendingPortalViews.SetCount(0);
        s_coveredRects.SetCount(0);
    }

    SMOPortalRef* portalRef = &mapObj->portalRefList[group->portalStart];

    for (int32_t i = 0; i < group->portalCount; i++) {
        if (portalRef->groupIndex == 0xFFFF || portalRef->groupIndex == fromGroup) {
            portalRef++;
            continue;
        }

        SMOPortal* portal = &mapObj->portalList[portalRef->portalIndex];

        uint32_t groupFlags = mapObj->GetGroupFlags(portalRef->groupIndex);
        SPortalExt* portalExt = &s_portalExt[portalRef->portalIndex];
        if (portalExt->stamp != s_portalStamp) {
            portalExt->stamp = s_portalStamp;
            portalExt->flags = 0;
            if ((groupFlags & 8) == 0 && (group->flags & 8) == 0)
                portalExt->flags = 16;
            CWorldScene::TransformPortal(mapObj, portal, portalExt);
        }

        auto v16 = portal->plane.n.y * s_camPosLocal.y + portal->plane.n.z * s_camPosLocal.z + portal->plane.n.x * s_camPosLocal.x + portal->plane.d;
        if (portalRef->side < 0)
            v16 = -v16;

        if (v16 < 0.0) {
            if (!s_interiorPass && !depth) {
                CRect rect;
                rect.minY = 0.0;
                rect.minX = 0.0;
                rect.maxY = 1.0;
                rect.maxX = 1.0;
                s_coveredRects.Add(1, &rect);
            }
        
            portalRef++;
            continue;
        }

        if ((portalExt->flags & 2) == 0 && (portalExt->flags & 1) != 0
            || ndcRect.maxX < portalExt->rect.minX
            || ndcRect.minX > portalExt->rect.maxX
            || ndcRect.maxY < portalExt->rect.minY
            || ndcRect.minY > portalExt->rect.maxY) {
            portalRef++;
            continue;
        }

        CRect clip = portalExt->rect;
        if (clip.minX < ndcRect.minX)
            clip.minX = ndcRect.minX;
        if (clip.maxX > ndcRect.maxX)
            clip.maxX = ndcRect.maxX;
        if (clip.minY < ndcRect.minY)
            clip.minY = ndcRect.minY;

        if (CMath::fequalz(clip.minX, clip.maxX, 0.001) || CMath::fequalz(clip.minY, clip.maxY, 0.001)) {
            portalRef++;
            continue;
        }

        if (!s_interiorPass) {
            if ((groupFlags & 0x10008) != 0) {
                portalRef++;
                continue;
            }
            if (!depth && s_cullStateValid && (groupFlags & 0x140) == 0)
                AddInteriorPortalView(mapObj, portal, portalRef, portalExt, &s_pendingPortalViews);
        } else if ((groupFlags & 0x50148) != 0) {
            AddExteriorPortalView(mapObj, portal, portalRef, portalExt, groupFlags & 0x10008);
            if ((groupFlags & 0x10008) != 0) {
                portalRef++;
                continue;
            }
        }

        CRect v40 = { 2.0f, 2.0f, 2.0f, 2.0f };
        CRect v39 = { 1.0f, 1.0f, 1.0f, 1.0f };
        CRect v27 = clip + v39;
        v27 /= v40;
        CWorldScene::FrustumPush();
        CWorldScene::FrustumSet(CWorldScene::s_frustumCorners, &v27);
        CWorldScene::RenderThruPortals(mapObj, portalRef->groupIndex, groupNum, clip, depth + 1, interior);
        CWorldScene::FrustumPop();

        portalRef++;
    }

    if (depth || !s_pendingPortalViews.Count())
        return;

    for (int32_t i = 0; i < s_pendingPortalViews.Count(); i++) {
        CPortalView* portalView = &s_pendingPortalViews[i];

        uint32_t j = 0;
        uint32_t n = s_coveredRects.Count();

        while (j < n && !s_coveredRects[j].Intersects(portalView->rect))
            ++j;

         if (j >= n)
            CWorldScene::PushPortalView(portalView);
    }
    s_pendingPortalViews.SetCount(0);
    s_coveredRects.SetCount(0);
}

// OFFSET: 0x795D20
void CWorldScene::PushPortalView(CPortalView* portalView) {
    //bn_TSGrowableArray_CPortalView_Add(&stru_CDD0F8.m_alloc, 1, a1);
}

// OFFSET: 0x7A9090
void CWorldScene::TransformPortal(CMapObj* mapObj, SMOPortal* portal, SPortalExt* portalExt) {
    CWorldScene::ClassifyPortalPlane(mapObj, portal, portalExt);

    C3Vector* clippedVerts = nullptr;
    uint32_t clippedCount = 0;

    if (!(portalExt->flags & 2)) {
        C3Vector offset = { 0.0f, 0.0f, 0.0f };
        portalExt->flags |= CWorldScene::TransformAndClipVerts(mapObj, (portalExt->flags >> 4) & 1, &mapObj->portalVertexList[portal->startVertex], portal->count, offset, clippedVerts, clippedCount);
    }

    if ((portalExt->flags & 2) != 0) {
        portalExt->rect.minX = -1.0;
        portalExt->rect.maxX = 1.0;
        portalExt->rect.maxY = 1.0;
        portalExt->rect.minY = -1.0;
    } else if ((portalExt->flags & 1) != 0) {
        portalExt->rect.minX = 3.4028235e38;
        portalExt->rect.maxX = -3.4028235e38;
        portalExt->rect.maxY = -3.4028235e38;
        portalExt->rect.minY = 3.4028235e38;
    } else {
        CWorldScene::CalcScreenRectFromVerts(portalExt->rect, clippedVerts, clippedCount);
    }
}

// OFFSET: 0x7A7210
void CWorldScene::ClassifyPortalPlane(CMapObj* mapObj, SMOPortal* portal, SPortalExt* portalExt) {
    auto sq = portal->plane.n.y * s_camPosLocal.y + portal->plane.n.z * s_camPosLocal.z + portal->plane.n.x * s_camPosLocal.x + portal->plane.d;
    if (sq > -0.0099999998 && sq < 0.0099999998) {
        if (NTempest::Intersect(s_camPosLocal, &mapObj->portalVertexList[portal->startVertex], portal->count, portal->plane.n.MajorAxis()))
            portalExt->flags |= 2u;
    }
}

// OFFSET: 0x7A85E0
uint32_t CWorldScene::TransformAndClipVerts(CMapObj* mapObj, uint32_t a2, C3Vector* verts, uint32_t count, C3Vector& offset, C3Vector*& clippedVerts, uint32_t& clippedCount) {
    static C3Vector s_worldVerts[12]; // MAX_CLIP_VERTS = 12
    static bool s_worldVertsInit = false;
    if (!s_worldVertsInit) {
        s_worldVertsInit = true;
        for (uint32_t i = 0; i < 12; i++) {
            s_worldVerts[i] = C3Vector(0.0f, 0.0f, 0.0f);
        }
    }

    if (count > 12) {
        count = 12;
    }

    for (uint32_t i = 0; i < count; i++) {
        C3Vector v = {
            verts[i].x + offset.x,
            verts[i].y + offset.y,
            verts[i].z + offset.z
        };
        s_worldVerts[i] = s_mapObjToWorld.TransformPoint(v);
    }

    if (CWorldOcclusion::GetClipVolumeCount() && !a2 && CWorldOcclusion::QueryVolumes(s_worldVerts, count)) {
        clippedVerts = nullptr;
        clippedCount = 0;
        return 1;
    }

    CWorldScene::ClipVerts(s_worldVerts, count, &clippedVerts, &clippedCount);

    if (clippedCount < 3) {
        clippedVerts = nullptr;
        clippedCount = 0;
        return 1;
    }

    for (uint32_t i = 0; i < clippedCount; i++) {
        C4Vector p = {
            clippedVerts[i].x - CWorldScene::s_activeWorldView.x,
            clippedVerts[i].y - CWorldScene::s_activeWorldView.y,
            clippedVerts[i].z - CWorldScene::s_activeWorldView.z,
            1.0f
        };

        p = s_viewProj.TransformPoint(p);

        float w = 0.0001f;
        if (p.w >= 0.0001f) {
            w = p.w;
        } else {
            p.w = 0.0001f;
        }

        float invW = 1.0f / w;

        clippedVerts[i].x = p.x * invW;
        clippedVerts[i].y = p.y * invW;
        clippedVerts[i].z = p.z;
    }

    return 0;
}

// OFFSET: 0x7A72A0
void CWorldScene::ClipVerts(C3Vector* verts, uint32_t count, C3Vector** outVerts, uint32_t* outCount) {
    static C3Vector s_buffers[2][16];
    static bool s_init = false;
    if (!s_init) {
        s_init = true;
        for (uint32_t b = 0; b < 2; b++) {
            for (uint32_t i = 0; i < 16; i++) {
                s_buffers[b][i] = C3Vector(0.0f, 0.0f, 0.0f);
            }
        }
    }

    static int32_t s_code[16];
    static float s_dist[16];

    uint32_t bufCount[2] = { count, 0 };
    for (uint32_t i = 0; i < count; i++) {
        s_buffers[0][i] = verts[i];
    }

    uint32_t src = 0;
    uint32_t dst = 1;

    for (uint32_t plane = 0; plane < 5; plane++) {

        const C4Plane& p = CWorldScene::s_clipFrustum.planes[plane];

        src = plane & 1;
        dst = (plane - 1) & 1;

        const C3Vector* in = s_buffers[src];
        const uint32_t inCnt = bufCount[src];

        for (uint32_t i = 0; i < inCnt; i++) {
            float d = in[i].x * p.n.x + in[i].y * p.n.y + in[i].z * p.n.z + p.d;
            s_dist[i] = d;
            if (d > 0.0001f) {
                s_code[i] = 1;
            } else if (d >= -0.0001f) {
                s_code[i] = 0;
            } else {
                s_code[i] = 2;
            }
        }

        s_code[inCnt] = s_code[0];
        s_dist[inCnt] = s_dist[0];

        if (inCnt == 0) {
            break;
        }

        C3Vector* out = s_buffers[dst];
        uint32_t outCnt = 0;

        for (uint32_t i = 0; i < inCnt; i++) {

            int32_t code = s_code[i];

            if (code == 0) {
                out[outCnt++] = in[i];
                continue;
            }

            if (code == 1) {
                out[outCnt++] = in[i];
            }

            int32_t next = s_code[i + 1];
            if (next != 0 && next != code) {
                uint32_t j = (i + 1) % inCnt;
                float t = s_dist[i] / (s_dist[i] - s_dist[j]);

                out[outCnt].x = (in[j].x - in[i].x) * t + in[i].x;
                out[outCnt].y = (in[j].y - in[i].y) * t + in[i].y;
                out[outCnt].z = (in[j].z - in[i].z) * t + in[i].z;
                outCnt++;
            }
        }

        bufCount[dst] = outCnt;
        *outCount = outCnt;

        if (outCnt == 0) {
            break;
        }
    }

    if (bufCount[dst] == 0) {
        *outVerts = nullptr;
        *outCount = 0;
        return;
    }

    *outVerts = s_buffers[dst];
}

// OFFSET: 0x7A6B90
void CWorldScene::CalcScreenRectFromVerts(CRect& rect, C3Vector* clippedVerts, uint32_t clippedCount) {
    rect.minX = 3.4028235e38;
    rect.maxX = -3.4028235e38;
    rect.maxY = -3.4028235e38;
    rect.minY = 3.4028235e38;

    for (uint32_t i = 0; i < clippedCount; i++) {
        if (rect.minX > clippedVerts[i].x)
            rect.minX = clippedVerts[i].x;
        if (rect.maxX < clippedVerts[i].x)
            rect.maxX = clippedVerts[i].x;
        if (rect.minY > clippedVerts[i].y)
            rect.minY = clippedVerts[i].y;
        if (rect.maxY < clippedVerts[i].y)
            rect.maxY = clippedVerts[i].y;
    }
}

// OFFSET: 0x795D40
void CWorldScene::LocateViewer3() {
    CWorldScene::s_viewerMapObjDef = 0;
    CWorldScene::s_viewerMovedMapObjDef = 0;
    CWorldScene::s_viewerMapObjGroups.m_count = 0;
    CWorldScene::s_viewerMovedMapObjGroups.m_count = 0;
    s_debugMapName[0] = 0;
    s_debugMapChunk[0] = 0;
    if ((CWorld::s_enables & CWorld::Enables::Enable_WMO) != 0) {
        C3Vector end = CWorldScene::s_activeWorldView;
        end.z -= 1760.0f;
        float v17 = 1.0f;
        CMapChunk* v16 = nullptr;
        CMapObjDef* mapObjDefs[2] = { nullptr, nullptr };
        uint32_t mapObjGroups[4];

        bool v0 = CMap::VectorIntersectTerrain(&CWorldScene::s_activeWorldView, &end, &v17, 0x100u, &v16);
        if (CMap::LocateViewerMapObjs(CWorldScene::s_activeWorldView, end, v17, mapObjDefs, mapObjGroups)) {
            CWorldScene::s_viewerMapObjDef = mapObjDefs[0];
            if (mapObjDefs[0]) {
                if (mapObjDefs[0]->owner->m_wmoName)
                    SStrCopy(s_debugMapName, mapObjDefs[0]->owner->m_wmoName, 260);
                char* groupName = mapObjDefs[0]->owner->GetGroupName(mapObjGroups[0]);
                if (groupName)
                    SStrCopy(s_debugMapChunk, groupName, 64);
                CWorldScene::AddViewerGroup(&CWorldScene::s_viewerMapObjGroups, mapObjGroups[0]);
                if (mapObjGroups[1] != 0xFFFF)
                    CWorldScene::AddViewerGroup(&CWorldScene::s_viewerMapObjGroups, mapObjGroups[1]);
                //            GroupFlags = 0;
                //            if (v8 != 0xFFFF) {
                //                GroupFlags = CMapObj::GetGroupFlags(dword_CD87A4->owner, v8);
                //                v4 = v9;
                //            }
                //            if (v4 != 0xFFFF)
                //                GroupFlags |= CMapObj::GetGroupFlags(dword_CD87A4->owner, v4);
                //            if ((GroupFlags & 0x40140) != 0) {
                //                v7.minY = 0.0;
                //                v7.minX = 0.0;
                //                v7.maxY = 1.0;
                //                v7.maxX = 1.0;
                //                CPortalView::CPortalView(&v6, &v7, 0.0);
                //                maybe_CWorldScene__MergeViewerEntry(&stru_ADF570, &v6);
                //                maybe_CWorldScene__PushPortalView(&v6);
                //            }
            }
            CWorldScene::s_viewerMovedMapObjDef = mapObjDefs[1];
            if (CWorldScene::s_viewerMovedMapObjDef) {
                CWorldScene::AddViewerGroup(&CWorldScene::s_viewerMovedMapObjGroups, mapObjGroups[2]);
                if (mapObjGroups[3] != 0xFFFF)
                    CWorldScene::AddViewerGroup(&CWorldScene::s_viewerMovedMapObjGroups, mapObjGroups[3]);
            }
        } else if (v0) {
            SStrCopy(s_debugMapName, CMap::mapName, 260);
            SStrPrintf(s_debugMapChunk, 0x40u, "%i, %i", v16->cOffset.x / 16, v16->cOffset.y / 16);
        }

        // DEBUG
        DebugScreenSet("Location1", s_debugMapName);
        DebugScreenSet("Location2", s_debugMapChunk);
    }
}

// OFFSET: 0x7D5610
void CWorldScene::RenderCollisionDebug() {
    if (!CMap::debugVertexArray.Count())
        return;

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_PolygonOffset, 1.0f);
    C3Vector vec = { -CWorldScene::s_activeWorldView.x,
                     -CWorldScene::s_activeWorldView.y,
                     -CWorldScene::s_activeWorldView.z };
    C44Matrix mat;
    mat.Translate(vec);
    g_theGxDevicePtr->XformPush(GxXform_World, mat);
    GxRsSet(GxRs_BlendingMode, 2);
    GxRsSetAlphaRef();
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_Culling, 0);
    CGxVertexPC* vertexData = CMap::debugVertexArray.Ptr();
    GxPrimVertexPtr(CMap::debugVertexArray.Count(), &vertexData->p, sizeof(CGxVertexPC), nullptr, 0, &vertexData->c, sizeof(CGxVertexPC), nullptr, 0, nullptr, 0);
    GxPrimIndexPtr(CMap::debugIndexArray.Count(), CMap::debugIndexArray.Ptr());
    CGxBatch batch;
    batch.m_count = CMap::debugIndexArray.Count();
    batch.m_maxIndex = CMap::debugVertexArray.Count() - 1;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_minIndex = 0;
    g_theGxDevicePtr->Draw(&batch, true);
    g_theGxDevicePtr->XformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x792FC0
void CWorldScene::AddViewerGroup(TSGrowableArray<uint16_t>* group, uint16_t val) {
    for (int32_t i = 0; i < group->Count(); i++) {
        if (group->Ptr()[i] == val) {
            return;
        }
    }

    group->Add(1, &val);
}

// OFFSET: 0x7B3B20
void CWorldScene::RenderMapObjWithCallback(CMapObjDef* mapObjDef, TSGrowableArray<uint16_t>* groups) {
    CMapObj::SetGroupRenderCallback(reinterpret_cast<RENDER_CALLBACK>(CWorldScene::AddMapObjDefGroupToSortTable), mapObjDef);
    CWorldScene::s_curMapObjDef = mapObjDef;
    CWorldScene::RenderInterior(mapObjDef->owner, mapObjDef->mat, mapObjDef->invMat, CWorldScene::s_activeWorldView, CWorldScene::camTarget, groups);
}

// OFFSET: 0x7A8F20
void CWorldScene::AddExteriorPortalView(CMapObj* mapObj, SMOPortal* portal, SMOPortalRef* ref, SPortalExt* ext, uint32_t destIsExterior) {
    if ((ext->flags & 4) != 0)
        return;

    ext->flags |= 4;

    C3Vector nudge = portal->plane.n * 0.01f;
    if (ref->side > 0)
        nudge = -nudge;

    C3Vector* verts = 0;
    uint32_t n = 0;
    ext->flags |= TransformAndClipVerts(mapObj, (ext->flags >> 4) & 1, &mapObj->portalVertexList[portal->startVertex], portal->count, nudge, verts, n);

    if (n <= 2)
        return;

    CPortalView v17;
    v17.rect.minX = 3.4028235e38;
    v17.rect.minY = 3.4028235e38;
    v17.rect.maxX = -3.4028235e38;
    v17.rect.maxY = -3.4028235e38;
    v17.maxViewDepth = -1.0;
    v17.verts = 0;
    v17.vertCount = 0;
    CWorldScene::CalcScreenRectFromVerts(v17.rect, verts, n);
    v17.rect.minX = (v17.rect.minX + 1.0) * 0.5;
    v17.rect.maxX = (v17.rect.maxX + 1.0) * 0.5;
    v17.rect.minY = (v17.rect.minY + 1.0) * 0.5;
    v17.rect.maxY = 0.5 * (v17.rect.maxY + 1.0);
    v17.maxViewDepth = mapObj->CalcPortalFarthestDistance(portal);
    //maybe_CWorldScene__PushPortalView(&v17);
    //maybe_CWorldScene__MergeIntoWorldRect(&v17.rect.minY);
    if (destIsExterior)
        CWorldScene::MergeIntoFrustumRect(&v17);
}

// OFFSET: 0x7A9200
void CWorldScene::AddInteriorPortalView(CMapObj* mapObj, SMOPortal* portal, SMOPortalRef* ref, SPortalExt* ext, TSGrowableArray<CPortalView>* a5) {
    if ((ext->flags & 0xC) != 0)
        return;

    C3Vector nudge = portal->plane.n * 0.01f;
    if (ref->side > 0)
        nudge = -nudge;

    C3Vector* verts = 0;
    uint32_t n = 0;
    ext->flags |= TransformAndClipVerts(mapObj, (ext->flags >> 4) & 1, &mapObj->portalVertexList[portal->startVertex], portal->count, nudge, verts, n);

    if (n > 2) {
        //CPortalView v18;
        //v18.rect.minX = 3.4028235e38;
        //v18.rect.minY = 3.4028235e38;
        //v18.verts = 0;
        //v18.rect.maxX = -3.4028235e38;
        //v18.vertCount = 0;
        //v18.rect.maxY = -3.4028235e38;
        //v18.maxViewDepth = -1.0;
        //CWorldScene::CalcScreenRectFromVerts(v18.rect, verts, n);
        //v17 = (stru_D1BEE8.m_data + 12 * stru_D1BEE8.m_count);
        //v18.vertCount = n;
        //v18.rect.minX = (v18.rect.minX + 1.0) * 0.5;
        //v18.verts = v17;
        //v18.rect.maxX = (v18.rect.maxX + 1.0) * 0.5;
        //v18.rect.minY = (v18.rect.minY + 1.0) * 0.5;
        //v18.rect.maxY = 0.5 * (v18.rect.maxY + 1.0);
        //bn_TSGrowableArray_C3Vector_SetCount(&stru_D1BEE8, v16 + stru_D1BEE8.m_count);
        //memcpy(v17, verts, 12 * n);
        //a5->Add(1, &v18);
    }
    ext->flags |= 8;
}

void CWorldScene::MergeIntoFrustumRect(CPortalView* portalView) {
    CWorldScene::frustumPortalView.Merge(portalView);
}

// OFFSET: 0x7A9160
void CWorldScene::SetupLighting(CM2Lighting* lighting, C3Vector* view) {
    if (CShaderEffect::s_enableShaders) {
        CWorldScene::s_fogPermute = lighting->m_lightCount;
        if (CWorldScene::s_fogPermute) {
            CShaderEffect::LocalLights lights;
            //sub_7A8A60(lights);
            CShaderEffect::ComputeLocalLights(&lights, CWorldScene::s_fogPermute, lighting->m_lights, view);
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 17, reinterpret_cast<C4Vector*>(&lights), 11);
        }
        //dword_D1BEFC = 0;
    } else {
        lighting->SetupGxLights(view);
        //dword_D1BEFC = 0;
    }
}
