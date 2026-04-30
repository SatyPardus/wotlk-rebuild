#include "world/CWorldScene.hpp"
#include "world/CWorld.hpp"
#include "world/map/CMap.hpp"
#include "world/map/CMapChunk.hpp"
#include "world/daynight/DayNight.hpp"
#include "world/daynight/DNInfo.hpp"
#include "world/map/CWorldOcclusion.hpp"

#include "gx/Device.hpp"
#include "gx/Draw.hpp"
#include "gx/Shader.hpp"
#include "gx/RenderState.hpp"
#include "gx/Transform.hpp"

#include "model/Model2.hpp"

#include "gameui/camera/CGCamera.hpp"
#include "gameui/CGWorldFrame.hpp"

#include "cursor/Cursor.hpp"


CM2Scene* CWorldScene::s_m2Scene;
HTEXTURE CWorldScene::s_defaultTexture;
HTEXTURE CWorldScene::s_defaultBlendTexture;

int32_t CWorldScene::frustumIndex;
CFrustum CWorldScene::frustumStack[32];
CRect CWorldScene::frustumRect;
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

CM2Model* g_models[10] = {};

void CWorldSceneLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg) {
    lighting->AddAmbient({ 1.0f, 1.0f, 1.0f });
    lighting->AddDiffuse({ 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f });
    lighting->AddSpecular({ 0.0f, 0.0f, 0.0f });
}

// OFFSET: 0x7997D0
void CWorldScene::Initialize() {
    // sub_799730();
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

    // DEBUG CODE! NOT REAL!
    CWorldScene::s_m2Scene = M2CreateScene();
    g_models[0] = CWorldScene::s_m2Scene->CreateModel(R"(World\LORDAERON\Arathi\PassiveDoodads\Trees\ArathiStump01.m2)", 0);
    g_models[0]->SetWorldTransform(C3Vector(0.0f, 1.0f, 0.0f), 180.0f, 0.1f);

    g_models[1] = CWorldScene::s_m2Scene->CreateModel(R"(World\NoDXT\Detail\ApkBus01.m2)", 0);
    g_models[1]->SetWorldTransform(C3Vector(0.0f, 1.5f, 0.0f), 180.0f, 1.0f);

    g_models[2] = CWorldScene::s_m2Scene->CreateModel(R"(Creature\BloodElfGuard\BloodElfMale_Guard.m2)", 0);
    g_models[2]->SetWorldTransform(C3Vector(0.0f), 180.0f, 1.0f);

    g_models[3] = CWorldScene::s_m2Scene->CreateModel(R"(World\AZEROTH\ELWYNN\PASSIVEDOODADS\Trees\ElwynnTree01\ElwynnPine01.m2)", 0);
    g_models[3]->SetWorldTransform(C3Vector(0.0f, -1.0f, 0.0f), 180.0f, 1.0f);

    for (size_t i = 0; i < 10; ++i) {
        if (!g_models[i])
            continue;

        g_models[i]->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
        g_models[i]->SetLightingCallback(&CWorldSceneLightingCallback, nullptr);
    }
}

// OFFSET: 0x795400
void CWorldScene::Update(C3Vector* camPos, C3Vector* camTarget) {
    //v2 = 0;
    //if (dword_CD8610)
    //    NOP();
    //flt_ADF574 = 3.4028235e38;
    //flt_ADF570 = 3.4028235e38;
    //dword_CD8620 = 0;
    //dword_CD861C = 0;
    //flt_ADF57C = -3.4028235e38;
    //dword_ADF584 = 0;
    //flt_ADF578 = -3.4028235e38;
    //dword_ADF588 = 0;
    //dword_ADF5A0 = 0;
    //flt_ADF580 = -1.0;
    //dword_ADF5A4 = 0;
    CWorldScene::frustumRect.minX = 3.4028235e38;
    CWorldScene::frustumRect.minY = 3.4028235e38;
    CWorldScene::frustumRect.maxX = -3.4028235e38;
    CWorldScene::frustumRect.maxY = -3.4028235e38;
    //flt_ADF59C = -1.0;
    //sub_794190(0);
    //sub_794190(0);

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

    //float farClip = CWorld::s_farClip - 100.0f;
    //flt_CD87AC = (farClip < 400.0f) ? 400.0f : farClip;

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

    //sub_7906C0(&CWorldScene::sortTable, v30);
    g_theGxDevicePtr->XformView(CWorldScene::viewMatrix);
    g_theGxDevicePtr->XformProjection(CWorldScene::projMatrix);
    //CWorldScene::viewPort.x.l = v22->m_viewport.x.l;
    //CWorldScene::viewPort.y.h = v22->m_viewport.x.h;
    //CWorldScene::viewPort.x.h = v22->m_viewport.y.l;
    //CWorldScene::viewPort.z.l = v22->m_viewport.y.h;
    //CWorldScene::viewPort.y.l = v22->m_viewport.z.l;
    //CWorldScene::viewPort.z.h = v22->m_viewport.z.h;
    GxuXformCalcFrustumCorners(&CWorldScene::viewMatrix, &CWorldScene::projMatrix, CWorldScene::s_frustumCorners);
    for (int32_t i = 0; i < 8; i++) {
        CWorldScene::s_frustumCorners[i] += CWorldScene::s_activeWorldView;
    }
    //CFrustum::CalcPlanesFromCorners(&stru_CDD108, CWorldScene::s_frustumCorners);
    CWorldScene::viewMatrix.Translate(-CWorldScene::s_activeWorldView);
    C44Matrix v23 = CWorldScene::viewMatrix * CWorldScene::projMatrix;
    //stru_ADF5A8.M11 = v23->M11;
    //stru_ADF5A8.M12 = v23->M12;
    //stru_ADF5A8.M13 = v23->M13;
    //M14 = v23->M14;
    //stru_ADF5A8.M14 = v23->M14;
    //stru_ADF5A8.M21 = v23->M21;
    //stru_ADF5A8.M22 = v23->M22;
    //stru_ADF5A8.M23 = v23->M23;
    //stru_ADF5A8.M24 = v23->M24;
    //stru_ADF5A8.M31 = v23->M31;
    //stru_ADF5A8.M32 = v23->M32;
    //stru_ADF5A8.M33 = v23->M33;
    //M34 = v23->M34;
    //stru_ADF5A8.M34 = v23->M34;
    //stru_ADF5A8.M41 = v23->M41;
    //stru_ADF5A8.M42 = v23->M42;
    //stru_ADF5A8.M43 = v23->M43;
    //stru_ADF5A8.M44 = v23->M44;
    //v36.z = stru_ADF5A8.M44;
    //*(float*)&v35 = M14;
    //dword_CD8FC8 = v35;
    //v36.x = stru_ADF5A8.M24;
    //dword_CD8FCC = LODWORD(stru_ADF5A8.M24);
    //v36.y = M34;
    //dword_CD8FD0 = LODWORD(v36.y);
    //dword_CD8FD4 = LODWORD(stru_ADF5A8.M44);
    CWorldScene::boundingBox = CAaBox::Bounding(CWorldScene::s_frustumCorners, 8u);
    CWorldScene::s_frustumChunkRect.minX = (int)((-(CWorldScene::boundingBox.t.y - 17066.666f) * 0.03f) - 0.5f);
    CWorldScene::s_frustumChunkRect.minY = (int)((-(CWorldScene::boundingBox.t.x - 17066.666f) * 0.03f) - 0.5f);
    CWorldScene::s_frustumChunkRect.maxX = (int)((-(CWorldScene::boundingBox.b.y - 17066.666f) * 0.03f) - 0.5f);
    CWorldScene::s_frustumChunkRect.maxY = (int)((-(CWorldScene::boundingBox.b.x - 17066.666f) * 0.03f) - 0.5f);
    CWorldScene::frustumIndex = 0;
    CWorldScene::frustumStack[0].CalcPlanesFromCorners(CWorldScene::s_frustumCorners);
    CMapChunk::farCornerIndex = 0;
    if (CWorldScene::camTarget.x > CWorldScene::s_activeWorldView.x)
        CMapChunk::farCornerIndex = 2;
    if (CWorldScene::camTarget.y > CWorldScene::s_activeWorldView.y)
        CMapChunk::farCornerIndex += 1;
    //dword_CD8F3C = 0;
    //if (CWorldScene::camTarg.x < (double)CWorldScene::s_activeWorldView.x) {
    //    v2 = 2;
    //    dword_CD8F3C = 2;
    //}
    //if (CWorldScene::camTarg.y < (double)CWorldScene::s_activeWorldView.y)
    //    dword_CD8F3C = v2 + 1;
    //CWorldScene::camPlane.n = CWorldScene::camVec;
    //CWorldScene::camPlane.d = -(camPos->x * CWorldScene::camVec.x + camPos->y * CWorldScene::camVec.y + CWorldScene::camVec.z * camPos->z);
    //if (v33 <= 0.000099999997) {
    //    stru_ADF460.M44 = 1.0;
    //    stru_ADF460.M33 = 1.0;
    //    stru_ADF460.M22 = 1.0;
    //    stru_ADF460.M11 = 1.0;
    //    stru_ADF460.M43 = 0.0;
    //    stru_ADF460.M42 = 0.0;
    //    stru_ADF460.M41 = 0.0;
    //    stru_ADF460.M34 = 0.0;
    //    stru_ADF460.M32 = 0.0;
    //    stru_ADF460.M31 = 0.0;
    //    stru_ADF460.M24 = 0.0;
    //    stru_ADF460.M23 = 0.0;
    //    stru_ADF460.M21 = 0.0;
    //    stru_ADF460.M14 = 0.0;
    //    stru_ADF460.M13 = 0.0;
    //    stru_ADF460.M12 = 0.0;
    //    AsyncTimeMs = OsGetAsyncTimeMs();
    //    return sub_9A81F0(AsyncTimeMs);
    //} else {
    //    v32.M11 = 1.0;
    //    v32.M12 = 0.0;
    //    v32.M13 = 0.0;
    //    v32.M14 = 0.0;
    //    v32.M21 = 0.0;
    //    v32.M23 = 0.0;
    //    v32.M24 = 0.0;
    //    v32.M31 = 0.0;
    //    v32.M32 = 0.0;
    //    v32.M34 = 0.0;
    //    v32.M41 = 0.0;
    //    v32.M42 = 0.0;
    //    v32.M43 = 0.0;
    //    a3.x = 0.0;
    //    a3.y = 0.0;
    //    v36.x = 0.0;
    //    v36.y = 0.0;
    //    v36.z = 0.0;
    //    v32.M22 = 1.0;
    //    v32.M33 = 1.0;
    //    v32.M44 = 1.0;
    //    a3.z = 1.0;
    //    sub_6BFE60(&v36.x, &v38, &a3.x, &v32);
    //    v36.x = -CWorldScene::s_activeWorldView.x;
    //    v36.y = -CWorldScene::s_activeWorldView.y;
    //    v36.z = -CWorldScene::s_activeWorldView.z;
    //    C44Matrix::Translate(&v32, &v36);
    //    v26 = C44Matrix::Multiply(&v31, &v32, &CWorldScene::projMatrix);
    //    stru_ADF460.M11 = v26->M11;
    //    stru_ADF460.M12 = v26->M12;
    //    stru_ADF460.M13 = v26->M13;
    //    stru_ADF460.M14 = v26->M14;
    //    stru_ADF460.M21 = v26->M21;
    //    stru_ADF460.M22 = v26->M22;
    //    stru_ADF460.M23 = v26->M23;
    //    stru_ADF460.M24 = v26->M24;
    //    stru_ADF460.M31 = v26->M31;
    //    stru_ADF460.M32 = v26->M32;
    //    stru_ADF460.M33 = v26->M33;
    //    stru_ADF460.M34 = v26->M34;
    //    stru_ADF460.M41 = v26->M41;
    //    stru_ADF460.M42 = v26->M42;
    //    stru_ADF460.M43 = v26->M43;
    //    stru_ADF460.M44 = v26->M44;
    //    v27 = OsGetAsyncTimeMs();
    //    return sub_9A81F0(v27);
    //}
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

// OFFSET: 0x792D80
void CWorldScene::AddMapChunkToRenderList(CMapChunk* mapChunk, C3Vector* pos) {
    float planeDist = C3Vector::Dot(*pos, CWorldScene::camPlaneXY.n) + CWorldScene::camPlaneXY.d;
    int index = (int)(planeDist * 0.03f - 0.5f);
    if (planeDist <= 0.0f || index < 64) {
        CWorldScene::sortTable.table[planeDist <= 0.0f ? 0 : index].mapChunkList.LinkToTail(mapChunk);
    }
}

// OFFSET: 0x78FB20
bool CWorldScene::FrustumCull(CAaBox* box) {
    return CWorldScene::frustumStack[CWorldScene::frustumIndex].Cull(box) == WorldCull_outside;
}

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

// OFFSET: 0x7D6690
bool CWorldScene::InsideFrustumRect(CiRect* rect) {
    return rect->minX <= CWorldScene::s_frustumChunkRect.maxX
        && rect->minY <= CWorldScene::s_frustumChunkRect.maxY
        && rect->maxX >= CWorldScene::s_frustumChunkRect.minX
        && rect->maxY >= CWorldScene::s_frustumChunkRect.minY;
}

// OFFSET: 0x79A790
void CWorldScene::CullSortTable(CRect* a1) {
    //CWorldScene::CreateOcclusionVolumes(&CWorldScene::s_activeWorldView.x, stru_CDB108, 0);
    ++CWorldScene::frustumIndex;
    CWorldScene::frustumStack[CWorldScene::frustumIndex].FrustumPush(&CWorldScene::frustumStack[CWorldScene::frustumIndex - 1]);
    CWorldScene::FrustumSet(a1);
    for (int32_t i = 0; i < 64; i++) {
        CSortEntry* entry = &CWorldScene::sortTable.table[i];

        CWorldScene::CullChunks(entry, i);
        //CWorldScene::CullMapObjDefGroups(entry, a1, v1);
        //CWorldScene::CullLiquid(entry);
        //sub_793060(entry);
        //v4 = (double)v1 * 33.333332;
        //v3 = sub_78FB60(v4);
        //CWorldScene::CullDoodads(entry, v3);
        //sub_793760(entry);
    }
    //CWorldScene::CullHorizon(a1);
    --CWorldScene::frustumIndex;
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

        //v7 = sub_78FB60(m_next->distToCamera);
        //sub_799980(&m_next->doodadDefLinkList.m_linkoffset, v7);
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
    //if (!dword_CD87A4)
    //    sub_792BD0();
    //sub_790920();
    GxRsPush();
    GxXformPush(GxXform_World);

    //dword_CD8774 = 0;
    CWorldScene::s_chunksRendered = 0;
    //dword_CD872C = 0;
    //dword_CD8624 = 0;
    //CFrustum::CalcPlanesFromCorners(&flt_CDB168[63 * dword_CD8798], &stru_CDB108[0].x);
    //flt_CD8784 = World::s_farClip - 33.333332;
    //dword_CD8778 = ((int)stru_CD9048.unkList1.m_terminator.m_next & 1) == 0 && stru_CD9048.unkList1.m_terminator.m_next;
    //ActiveCamera = CGWorldFrame::GetActiveCamera();
    //flt_CD877C = CWorld::farFog / cos(((double(__thiscall*)(CGCamera*))ActiveCamera->Fov)(ActiveCamera) * 0.5) - CWorld::farFog;
    //sub_782F20();
    CMapRenderChunk::UpdatePools();
    //sub_7AE060();
    //sub_7B2A80();
    //memset(&byte_CD87B8, 0, 0x180u);
    //memset32(flt_CD8938, 0xC9742400, 0x180u);
    CWorldOcclusion::ClearVolumes();
    //CWorldScene::AddWorldOccluders();
    //if (dword_CD87A4) {
    //    ++dword_CD87B0;
    //    if (dword_CD87A0) {
    //        sub_7B3B20((char*)dword_CD87A0, (int)&unk_CDB0E4);
    //        flt_ADF574 = 3.4028235e38;
    //        flt_ADF570 = 3.4028235e38;
    //        dword_ADF584 = 0;
    //        flt_ADF57C = -3.4028235e38;
    //        dword_ADF588 = 0;
    //        flt_ADF578 = -3.4028235e38;
    //        dword_ADF5A0 = 0;
    //        dword_ADF5A4 = 0;
    //        flt_ADF580 = -1.0;
    //        flt_ADF590 = 3.4028235e38;
    //        flt_ADF58C = 3.4028235e38;
    //        flt_ADF598 = -3.4028235e38;
    //        flt_ADF594 = -3.4028235e38;
    //        flt_ADF59C = -1.0;
    //        sub_794190(0);
    //        sub_794190(0);
    //    }
    //    sub_7B3B20((char*)dword_CD87A4, (int)&unk_CDB0D4);
    //    if (flt_ADF59C < 0.0) {
    //        sub_794250(&stru_CD9048);
    //    } else {
    //        flt_CD8780 = flt_ADF59C + 33.333332;
    //        sub_79A790(&flt_ADF58C);
    //    }
    //    v16 = 0.0;
    //    v17 = 0.0;
    //    v18 = 1.0;
    //    v19 = 1.0;
    //    sub_799F80(&v16);
    //} else {
    //    ++dword_CD87B0;
    //    flt_ADF570 = 0.0;
    //    flt_ADF574 = 0.0;
    //    flt_ADF580 = 0.0;
    //    v16 = 0.0;
    //    flt_ADF578 = 1.0;
    //    v17 = 0.0;
    //    flt_ADF57C = 1.0;
    //    v18 = 1.0;
    //    v19 = 1.0;
    CWorldScene::frustumRect.minY = 0.0;
    //    flt_ADF59C = 0.0;
    //    dword_ADF588 = 0;
    CWorldScene::frustumRect.minX = 0.0;
    //    flt_CD8780 = -10000.0;
    CWorldScene::frustumRect.maxY = 1.0;
    CWorldScene::frustumRect.maxX = 1.0;
    //    dword_ADF5A4 = 0;
    CWorldScene::CullSortTable(&CWorldScene::frustumRect);
    //}
    //sub_79A260();
    //sub_793450();
    //sub_7CECD0(&stru_CD9048.unkList1.m_linkoffset);
    //ActiveDayNight = DayNight::GetActiveDayNight();
    //if (sub_683100(8)) {
    //    if (flt_ADF580 >= 0.0) {
    //        if (dword_CD8794 || !ActiveDayNight[115] && sub_7ECE00())
    //            v4 = ActiveDayNight[35];
    //        else
    //            v4 = 0;
    //    } else {
    //        v4 = ActiveDayNight[40];
    //    }
    //} else {
    //    v4 = -16777216;
    //}
    //GxSceneClear(3, v4);
    //sub_9A80C0(&off_B2EB68);
    //if (dword_CD87A8) {
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
        /*for (size_t i = 0; i < 10; ++i) {
            if (!g_models[i])
                continue;
            g_models[i]->SetAnimating(1);
            g_models[i]->SetVisible(1);
        }*/

        /*uint32_t count = 0;
        for (auto mapDoodadDef = CMap::doodadDefList.Head(); mapDoodadDef;) {
            auto next = CMap::doodadDefList.Next(mapDoodadDef);

            if (mapDoodadDef->model) {
                mapDoodadDef->model->SetAnimating(1);
                mapDoodadDef->model->SetVisible(1);
                count++;
            }

            mapDoodadDef = next;
        }*/

        CWorldScene::s_m2Scene->m_flags |= 1u;
        CWorldScene::s_m2Scene->AdvanceTime(static_cast<uint32_t>(time * 1000.0f));
        CWorldScene::s_m2Scene->Animate(cameraPos);
        CWorldScene::s_m2Scene->m_flags &= ~1u;
    }

    // sub_6FDA20();
    // CShadowQuery::Update();

    CShaderEffect::UpdateProjMatrix();
    //sub_781610();
    CWorldScene::RenderChunks();

    DayNight::Update();
    DayNight::RenderSky();

    //CWorldScene::UpdateLighting();
    //sub_795F80();
    //if (flt_ADF580 >= 0.0) {
    //    if (dword_CDD0EC) {
    //        sub_7968D0();
    //        sub_796C10(&unk_CDD0E8, 1);
    //    }
    //    if (dword_CDD0FC)
    //        sub_796C10(&unk_CDD0F8, 0);
    //    if (dword_CD861C) {
    //        sub_7F31C0(0, dword_CD861C, 0, *((float*)ActiveDayNight + 39));
    //        sub_7F31C0(1, 0, 0, 0.0);
    //    }
    //    if (!dword_CD8794)
    //        DayNight::CDayNightObject::RenderSky((int)&flt_ADF570);
    //}
    //sub_793980();
    //sub_8A2F00();
    //sub_793D20();
    //if ((CWorld::enables & 0x1000000) != 0 && dword_CD8610)
    //    sub_8A2240((char*)dword_CD8610, (int)&CWorldScene::s_activeWorldView, 0);
    //v10 = 0.0;
    //v11 = 0.0;
    //v12 = 0.0;
    //v13 = 0.0;
    //v14 = 0.0;
    //v15 = 0.0;
    //if (dword_CD7544 && (unsigned __int8)sub_784A30(&v10)) {
    //    v10 = v10 + CWorldScene::s_activeWorldView.x;
    //    v11 = v11 + CWorldScene::s_activeWorldView.y;
    //    v12 = v12 + CWorldScene::s_activeWorldView.z;
    //    v13 = CWorldScene::s_activeWorldView.x + v13;
    //    v14 = CWorldScene::s_activeWorldView.y + v14;
    //    v15 = CWorldScene::s_activeWorldView.z + v15;
    //}

    GxXformPop(GxXform_World);
    GxRsPop();

    CursorResetCursor();

    if (CWorld::GetEnables() & 0x200000) {
        // TODO
    }
}

// OFFSET: 0x798DA0
void CWorldScene::RenderChunks() {
    GxRsPush();
    //dword_D2509C = g_theGxDevicePtr->m_appRenderStates.m_data[GxRs_FogColor].m_value.m_data.i[GxRs_PolygonOffset];
    //if (CMap::enableTerrainShaderVertex) {
    //    v42.M11 = 1.0;
    //    v42.M22 = 1.0;
    //    v42.M33 = 1.0;
    //    v42.M44 = 1.0;
    //    v42.M12 = 0.0;
    //    v42.M13 = 0.0;
    //    v42.M14 = 0.0;
    //    v42.M21 = 0.0;
    //    v42.M23 = 0.0;
    //    v42.M24 = 0.0;
    //    v42.M31 = 0.0;
    //    v42.M32 = 0.0;
    //    v42.M34 = 0.0;
    //    v42.M41 = 0.0;
    //    v42.M42 = 0.0;
    //    v42.M43 = 0.0;
    //    v44.y = -CWorldScene::s_activeWorldView.x;
    //    v44.z = -CWorldScene::s_activeWorldView.y;
    //    v44.w = -CWorldScene::s_activeWorldView.z;
    //    C44Matrix::Translate(&v42, (const C3Vector*)&v44.y);
    //    v43.M11 = 1.0;
    //    v43.M12 = 0.0;
    //    v43.M13 = 0.0;
    //    v43.M14 = 0.0;
    //    v43.M21 = 0.0;
    //    v43.M23 = 0.0;
    //    v43.M24 = 0.0;
    //    v43.M31 = 0.0;
    //    v43.M32 = 0.0;
    //    v43.M34 = 0.0;
    //    v43.M41 = 0.0;
    //    v43.M42 = 0.0;
    //    v43.M43 = 0.0;
    //    v43.M22 = 1.0;
    //    v43.M33 = 1.0;
    //    v43.M44 = 1.0;
    //    C44Matrix::Copy(&v43, &g_theGxDevicePtr->m_xforms[10].m_mtx[g_theGxDevicePtr->m_xforms[10].m_level]);
    //    sub_7CFBE0(&v42, &v43);
    //} else {
    //    ActiveDayNight = DayNight::GetActiveDayNight();
    //    v1 = g_theGxDevicePtr;
    //    v2 = g_theGxDevicePtr->m_context == 0;
    //    v3 = ActiveDayNight;
    //    start = ActiveDayNight->fogInfo.start;
    //    v45 = ActiveDayNight->fogInfo.start;
    //    if (!v2) {
    //        m_data = g_theGxDevicePtr->m_appRenderStates.m_data;
    //        v6 = start == m_data[8].m_value.m_data.f[0];
    //        f = m_data[8].m_value.m_data.f;
    //        if (!v6) {
    //            CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_FogStart);
    //            *f = v45;
    //            v1 = g_theGxDevicePtr;
    //        }
    //    }
    //    v2 = v1->m_context == 0;
    //    end = v3->fogInfo.end;
    //    v45 = v3->fogInfo.end;
    //    if (!v2) {
    //        v9 = v1->m_appRenderStates.m_data;
    //        v10 = end == v9[9].m_value.m_data.f[0];
    //        v11 = v9[9].m_value.m_data.f;
    //        if (!v10) {
    //            CGxDevice::IRsDirty(v1, GxRs_FogEnd);
    //            *v11 = v45;
    //            v1 = g_theGxDevicePtr;
    //        }
    //    }
    //    if (v1->m_context) {
    //        v12 = v1->m_appRenderStates.m_data;
    //        v13 = v12[GxRs_MatDiffuse].m_value.m_data.i[GxRs_PolygonOffset];
    //        v14 = v12 + 1;
    //        if (v13 != 0xFF7F7F7F) {
    //            CGxDevice::IRsDirty(v1, GxRs_MatDiffuse);
    //            v14->m_value.m_data.i[0] = 0xFF7F7F7F;
    //            v1 = g_theGxDevicePtr;
    //        }
    //    }
    //    if (CMap::enableSpecularTerrain) {
    //        if (v1->m_context) {
    //            v15 = v1->m_appRenderStates.m_data;
    //            v16 = v15[3].m_value.m_data.i[0];
    //            v17 = v15 + 3;
    //            if (v16 != -1) {
    //                CGxDevice::IRsDirty(v1, GxRs_MatSpecular);
    //                v17->m_value.m_data.i[0] = 0xFFFFFFFF;
    //            }
    //        }
    //        sub_763C70(GxRs_MatSpecularExp, 20.0);
    //        v1 = g_theGxDevicePtr;
    //    }
    for (int32_t i = 0; i < 5; i++) {
        GxRsSet((EGxRenderState)(GxRs_TextureCoord0 + i), i);
        GxRsSet((EGxRenderState)(GxRs_TexGen0 + i), 2);
        GxRsSet((EGxRenderState)(GxRs_TextureShader0 + i), 1);
    }
    //if (CMap::gTerrainPixelShadersValid) {
    //    v23 = DayNight::GetActiveDayNight();
    //    if (CGxDevice::Caps((char*)g_theGxDevicePtr)->int134) {
    //        v24 = (char*)g_theGxDevicePtr;
    //        color = v23->fogInfo.color;
    //        if (!g_theGxDevicePtr->m_context)
    //            goto LABEL_35;
    //        v26 = g_theGxDevicePtr->m_appRenderStates.m_data + 10;
    //        if (v26->m_value.m_data.i[0] == color)
    //            goto LABEL_35;
    //        CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_FogColor);
    //        v26->m_value.m_data.i[0] = (int32_t)color;
    //    } else {
    //        sub_984C90(&v44, &v23->fogInfo);
    //        ((void(__thiscall*)(CGxDevice*, int, int, C4Vector*, int))g_theGxDevicePtr->ShaderConstantsSet)(
    //            g_theGxDevicePtr,
    //            4,
    //            2,
    //            &v44,
    //            1);
    //    }
    //    v24 = (char*)g_theGxDevicePtr;
//LABEL_35:
    //    if (CGxDevice::Caps(v24)->int138) {
    //        if (g_theGxDevicePtr->m_context) {
    //            v27 = g_theGxDevicePtr->m_appRenderStates.m_data + 12;
    //            if (v27->m_value.m_data.i[0] != 1) {
    //                CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_Fog);
    //                v27->m_value.m_data.i[0] = 1;
    //            }
    //        }
    //    }
    //    sub_874660();
    //    goto LABEL_58;
    //}
    DayNight::DNInfo* activeDayNight = DayNight::GetInfo();
    GxRsSet(GxRs_FogColor, activeDayNight->fogInfo.color.value);
    GxRsSet(GxRs_Fog, 1);
    GxRsSet(GxRs_ColorOp0, 1);
    GxRsSet(GxRs_AlphaOp0, 0);
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 0);
//LABEL_58:
    CWorldScene::RenderChunksSinglePass();
    CWorldScene::RenderChunksSolid();
    //CWorldScene::RenderChunksZoneDebug();
    //for (j = 0; j < 5; ++j) {
    //    m_level = g_theGxDevicePtr->m_xforms[j].m_level;
    //    v39 = &g_theGxDevicePtr->m_xforms[j];
    //    if ((v39->m_flags[m_level] & 1) == 0) {
    //        p_M11 = &v39->m_mtx[m_level];
    //        p_M11->M44 = 1.0;
    //        p_M11->M33 = 1.0;
    //        p_M11->M22 = 1.0;
    //        p_M11->M11 = 1.0;
    //        p_M11->M43 = 0.0;
    //        p_M11->M42 = 0.0;
    //        p_M11->M41 = 0.0;
    //        p_M11->M34 = 0.0;
    //        p_M11->M32 = 0.0;
    //        p_M11->M31 = 0.0;
    //        p_M11->M24 = 0.0;
    //        p_M11->M23 = 0.0;
    //        p_M11->M21 = 0.0;
    //        p_M11->M14 = 0.0;
    //        p_M11->M13 = 0.0;
    //        p_M11->M12 = 0.0;
    //        v41 = v39->m_level;
    //        v39->m_dirty = 1;
    //        v39->m_flags[v41] = 1;
    //    }
    //}
    GxRsPop();
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
                //if ((CWorld::enables & Enable_2) != 0) {
                //    if (*v36) {
                //        if (CMap::enableTerrainShaderVertex)
                //            sub_7D2D70((int)v2);
                //        else
                //            sub_7D28B0(v2);
                //    } else {
                CMapRenderChunk::s_renderLayersFunc(renderChunk);
                //    }
                //}

                renderChunk->renderChunkLink.Unlink();

                if ((CWorld::s_enables & 0x40000000) != 0) {
                    //v6 = *(int*)((char*)&v2->renderChunkLink.m_prevLink + v32.m_linkoffset);
                    //v7 = (TSLink*)((char*)v2 + v32.m_linkoffset);
                    //if (v6) {
                    //    v8 = (unsigned int)v7->m_next;
                    //    if ((v8 & 1) == 0 && v8)
                    //        v9 = (TSLink**)((char*)&v7->m_prevlink + v8 - *(_DWORD*)(v6 + 4));
                    //    else
                    //        v9 = (_DWORD*)(v8 & 0xFFFFFFFE);
                    //    *v9 = v6;
                    //    v7->m_prevlink->m_next = v7->m_next;
                    //    v7->m_prevlink = 0;
                    //    v7->m_next = 0;
                    //}
                    //v10 = v32.m_terminator.m_prevlink;
                    //v1 = v34;
                    //v7->m_prevlink = v32.m_terminator.m_prevlink;
                    //v7->m_next = v10->m_next;
                    //v10->m_next = v2;
                    //v2 = v33;
                    //v32.m_terminator.m_prevlink = v7;
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
