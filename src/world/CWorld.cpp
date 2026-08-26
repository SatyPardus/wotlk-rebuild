#include "world/CWorld.hpp"
#include "world/CWorldScene.hpp"
#include "world/map/CMap.hpp"
#include "world/daynight/DayNight.hpp"

#include "gx/Device.hpp"
#include "gx/Shader.hpp"
#include "gx/Transform.hpp"

#include "model/Model2.hpp"

#include "util/SFile.hpp"
#include <gameui/CGWorldFrame.hpp>
#include <gameui/camera/CGCamera.hpp>
#include "world/MapWeather.hpp"

uint32_t CWorld::s_enables;
uint32_t CWorld::s_enables2;
C3Vector CWorld::s_currentWorldPos;
CAaBox CWorld::s_groupAreaOfInterest;
CAaBox CWorld::s_objectAreaOfInterest;
CiRect CWorld::s_chunkRectHigh;
CiRect CWorld::s_chunkRectLow;
float CWorld::s_farClip;
float CWorld::s_nearClip;
float CWorld::prevFarClip;
CWorld::CALLBACK_FUNC CWorld::s_loadProgressCallback;
void* CWorld::s_loadProgressParam;
int32_t CWorld::terrainAlphaBitDepth;
Weather* CWorld::s_weather;

void CWorld::Initialize() {
    CWorld::s_enables |=
          Enables::Enable_Doodads
        | Enables::Enable_Terrain
        | Enables::Enable_10
        | Enables::Enable_Culling
        | Enables::Enable_Shadow
        | Enables::Enable_WMO
        | Enables::Enable_WMOLighting
        | Enables::Enable_WMOTextures
        | Enables::Enable_Occluders
        | Enables::Enable_DetailDoodads
        | Enables::Enable_1000000
        | Enables::Enable_Particulates
        | Enables::Enable_LowDetail
        // TODO DEBUGGING
        | Enables::Enable_Collisions;

    //flt_CD769C = 0.0;
    //CWorld::frameCnt = 0;
    //dword_CD768C = 0;
    //dword_CD7694 = 0;

    if (GxCaps().m_shaderTargets[GxSh_Pixel] > GxShPS_none) {
        CWorld::s_enables |= Enables::Enable_PixelShader;
    }

    if (GxCaps().m_shaderTargets[GxSh_Vertex] > GxShVS_none) {
        CWorld::s_enables2 |= Enables2::Enable_VertexShader;
    }

    //dword_CD7754 |= 7u;
    //dword_CD765C = *(_DWORD*)(dword_CD85C4 + 48);
    //CWorld::shadowMipLevel = CWorldParam::cvar_shadowLevel->m_intValue;
    //CWorld::farFog = CWorldParam::cvar_farClip->m_numberValue;
    //dword_CD7664 = 4;
    //CWorld::prepareAll = 0i64;
    //CWorld::bShowSimpleDoodads = 0;
    //CWorld::bLoadSimpleDoodads = 0;
    //World::texVect[0].x = 0.0;
    //World::texVect[0].y = 0.0;
    CWorldScene::s_m2Scene = M2CreateScene();
    //World::texVect[0].z = 0.0;
    //dword_CD7658 = 1;
    //CWorld::shadowColor = -1;
    //World::texVect[0].w = 1.0;
    //CWorld::detailDoodadAlphaRef = 128;
    //World::texVect[1].w = 1.0;
    //dword_CD7670 = 0;
    //World::texVect[2].w = 1.0;
    //World::texVect[3].w = 1.0;
    //World::texVect[4].w = 1.0;
    //World::texVect[5].w = 1.0;
    //World::texVect[6].w = 1.0;
    //World::texVect[7].w = 1.0;
    //flt_ADF1A4 = 1.0;
    //flt_ADF190 = 1.0;
    //flt_ADF17C = 1.0;
    //flt_ADF168 = 1.0;
    //World::texVect[1].x = 0.0;
    //World::texVect[1].y = 0.0;
    //World::texVect[1].z = 0.0;
    //World::texVect[2].x = 0.0;
    //World::texVect[2].y = 0.0;
    //World::texVect[2].z = 0.0;
    //World::texVect[3].x = 0.0;
    //World::texVect[3].y = 0.0;
    //World::texVect[3].z = 0.0;
    //World::texVect[4].x = 0.0;
    //World::texVect[4].y = 0.0;
    //World::texVect[4].z = 0.0;
    //World::texVect[5].x = 0.0;
    //World::texVect[5].y = 0.0;
    //World::texVect[5].z = 0.0;
    //World::texVect[6].x = 0.0;
    //World::texVect[6].y = 0.0;
    //World::texVect[6].z = 0.0;
    //World::texVect[7].x = 0.0;
    //World::texVect[7].y = 0.0;
    //World::texVect[7].z = 0.0;
    //flt_ADF1A0 = 0.0;
    //flt_ADF19C = 0.0;
    //flt_ADF198 = 0.0;
    //flt_ADF194 = 0.0;
    //flt_ADF18C = 0.0;
    //flt_ADF188 = 0.0;
    //flt_ADF184 = 0.0;
    //flt_ADF180 = 0.0;
    //flt_ADF178 = 0.0;
    //flt_ADF174 = 0.0;
    //flt_ADF170 = 0.0;
    //flt_ADF16C = 0.0;
    //World::groundEffectDistValueSqr = CWorld::detailDoodadDist * CWorld::detailDoodadDist;
    //CWorld::shadowMipLevel = CWorldParam::cvar_shadowLevel->m_intValue;

    uint32_t m2Flags = M2GetCacheFlags();
    CShaderEffect::InitShaderSystem(
        (m2Flags & 0x8) != 0,
        (CWorld::s_enables2 & Enables2::Enable_HwPcf) != 0
    );

    CShaderEffectManager::AddEffectFile("MapObj.wfx");
    CShaderEffectManager::AddEffectFile("MapObjU.wfx");
    CShaderEffectManager::AddEffectFile("Model2.wfx");
    CShaderEffectManager::AddEffectFile("Particle.wfx");
    CShaderEffectManager::AddEffectFile("ShadowMap.wfx");
    //CShadowQuery::Initialize();

    CWorldScene::Initialize();
    CMap::Initialize();

    //CWorld::chunkRectHi.minY = 0;
    //CWorld::chunkRectHi.minX = 0;
    //CWorld::chunkRectHi.maxY = 0;
    //CWorld::s_chunkRectLow.minY = 0;
    //CWorld::s_chunkRectLow.minX = 0;
    //CWorld::s_chunkRectLow.maxY = 0;
    //CMap::gbPrevChunkRect.minY = 0;
    //CMap::gbPrevChunkRect.minX = 0;
    //CMap::gbPrevChunkRect.maxY = 0;
    //CWorld::groupAoi.min.x = 0.0;
    //CWorld::groupAoi.min.y = 0.0;
    //CWorld::groupAoi.min.z = 0.0;
    //CWorld::groupAoi.max.x = 0.0;
    //CWorld::groupAoi.max.y = 0.0;
    //CWorld::groupAoi.max.z = 0.0;
    //CWorld::objectAoi.min.x = 0.0;
    //CWorld::objectAoi.min.y = 0.0;
    //CWorld::objectAoi.min.z = 0.0;
    //dword_ADEEC4 = 3;
    //dword_ADEEC8 = 2;
    //CWorld::chunkRectHi.maxX = 0;
    //CWorld::s_chunkRectLow.maxX = 0;
    //CMap::gbPrevChunkRect.maxX = 0;
    //CWorld::objectAoi.max.x = 0.0;
    //CWorld::objectAoi.max.y = 0.0;
    //CWorld::objectAoi.max.z = 0.0;
    //v2 = SMemAlloc(64064, ".\\World.cpp", 470, 0);
    //if (v2)
    //    CWorld::particulate = (void*)Particulate::Particulate(
    //        (int)v2,
    //        0.027777778,
    //        30.0,
    //        (int)"Textures\\WaterPoop02.blp");
    //else
    //    CWorld::particulate = 0;
    CWorld::s_weather = new (STORM_ALLOC(sizeof(Weather))) Weather();
    //CM2Scene::SetProjectTextureCallback(s_m2Scene, (DWORD)World::ProjectTextureCallback, 0);
    //CM2Scene::SetProjectPositionCallback(s_m2Scene, (int)World::ProjectPositionCallback, 0);
    //ConsoleCommandRegister("showDetailDoodads", (int)sub_77F5B0, 1, 0);
    //ConsoleCommandRegister("maxLOD", (int)sub_77F600, 1, 0);
    //ConsoleCommandRegister("showCull", (int)sub_77F650, 1, 0);
    //ConsoleCommandRegister("setShadow", (int)CWorld::ConsoleCommand_SetShadow, 1, 0);
    //ConsoleCommandRegister("waterRipples", (int)sub_77F690, 1, 0);
    //ConsoleCommandRegister("waterParticulates", (int)sub_77F6B0, 1, 0);
    //ConsoleCommandRegister("showShadow", (int)sub_77F7E0, 1, 0);
    //ConsoleCommandRegister("showLowDetail", (int)sub_77F820, 1, 0);
    //ConsoleCommandRegister("showSimpleDoodads", (int)CWorld::ConsoleCommand_ShowSimpleDoodads, 1, 0);
    //ConsoleCommandRegister("detailDoodadAlpha", (int)sub_77F700, 1, 0);
    //ConsoleCommandRegister("characterAmbient", (int)sub_77F750, 1, 0);
    //sub_77ED40();
}

void CWorld::LoadMap(const char* mapName, C3Vector* position, int32_t zoneID) {
    // TODO: calculate far clip
    CWorld::s_farClip = 1583.3334f;
    //World::s_farClip = sub_780770(CWorldParam::cvar_farClip->m_numberValue, mapid);
    CWorld::s_nearClip = 0.2f;
    CWorld::prevFarClip = CWorld::s_farClip;
    //if (IsStreamingAndTrial())
    //    sub_420AA0(mapid);

    //###DEBUG
    CGWorldFrame::s_currentWorldFrame->m_camera->m_position = *position;
    //###

    CWorld::PrepareAreaOfInterest(position);
    //CMap::gbPrevChunkRect = CWorld::gbChunkRect;
    CMap::Load(mapName, zoneID);
    //v3 = 1;
    //if ((dword_CD7750 & 1) == 0 || (CWorld::enables & 0x10000000) == 0)
    //    v3 = 0;
    //sub_8A1720(v3);
    //sub_8A1730((unsigned __int8)byte_CE04A0);
    //sub_8A1F50();
}

// OFFSET: 0x7831A0
void CWorld::Update(C3Vector* camPos, C3Vector* camTarget, C3Vector* position) {
    //sub_77F900();
    //v3 = profIdx;
    bool v4 = true;
    //profTimes[profIdx] = flt_CD76A0;
    //profIdx = v3 + 1;
    //if (v3 == 29)
    //    profIdx = 0;
    //CMap::gbPrevChunkRect = CWorld::s_chunkRectLow;
    CWorld::PrepareAreaOfInterest(position);
    //maxX = CMap::gbPrevChunkRect.maxX;
    //if (CMap::gbPrevChunkRect.maxX >= CWorld::s_chunkRectLow.maxX)
    //    maxX = CWorld::s_chunkRectLow.maxX;
    //maxY = CMap::gbPrevChunkRect.maxY;
    //if (CMap::gbPrevChunkRect.maxY >= CWorld::s_chunkRectLow.maxY)
    //    maxY = CWorld::s_chunkRectLow.maxY;
    //minX = CMap::gbPrevChunkRect.minX;
    //if (CMap::gbPrevChunkRect.minX <= CWorld::s_chunkRectLow.minX)
    //    minX = CWorld::s_chunkRectLow.minX;
    //minY = CMap::gbPrevChunkRect.minY;
    //if (CMap::gbPrevChunkRect.minY <= CWorld::s_chunkRectLow.minY)
    //    minY = CWorld::s_chunkRectLow.minY;
    //if (minY < maxY && minX < maxX) {
    //    HIDWORD(CWorld::prepareAll) = 0;
    //} else {
    //    HIDWORD(CWorld::prepareAll) = 1;
    //    dword_D4314C = 1;
    //}
    //++CWorld::frameCnt;
    //if (s_FrameCntCallback)
    //    s_FrameCntCallback();
    //v9 = flt_CD76A0;
    //p_y = &World::texVect[0].y;
    //for (i = 0; i < 8; ++i) {
    //    v12 = stru_ADEE78[i].x * v9 + *(p_y - 1);
    //    *(p_y - 1) = v12;
    //    v13 = stru_ADEE78[i].y * v9 + *p_y;
    //    *p_y = v13;
    //    if (v12 >= 64.0)
    //        *(p_y - 1) = 0.0;
    //    if (v13 >= 64.0)
    //        *p_y = 0.0;
    //    p_y += 4;
    //}
    CWorldScene::Update(camPos, camTarget);
    //v14 = 0;
    //if (World::s_farClip - World::s_prevFarClip > 10.0) {
    //    v4 = 0;
    //    v14 = 1;
    //}
    //World::s_prevFarClip = World::s_farClip;
    //if (v14) {
    //    World::s_loadProgressCallback = (int(__cdecl*)(_DWORD, _DWORD))LoadingScreenWorldCallback;
    //    World::s_loadProgressParam = 0;
    //    LoadingScreenEnable(s_mapId, 1);
    //    CMap::bPreload = 1;
    //}
    CMap::PrepareUpdate(v4);
    //if (v14) {
    //    CMap::bPreload = 0;
    //    World::s_loadProgressCallback = 0;
    //    World::s_loadProgressParam = 0;
    //    AsyncFile::ProgressCallback(0, 0);
    //    LoadingScreenDisable();
    //}
    CWorldScene::LocateViewer3();
    //sub_7816F0(HIDWORD(CWorld::prepareAll) | CWorld::prepareAll, &camPos->x);
    //CWorld::farFog = DayNight::GetActiveDayNight()->fogInfo.end;
    //if (!CGxDevice::MasterEnable(g_theGxDevicePtr, 1))
    //    CWorld::farFog = 100000.0;
    //LODWORD(CWorld::prepareAll) = 0;
    //if ((CWorld::enables & 0x2000000) != 0 && dword_CD8794)
    //    sub_79BF40(CWorld::particulate);
    //MapWeather::Update((int)dword_CD7544);
    //ActivePlayer = ClntObjMgrGetActivePlayer();
    //v16 = (CGPlayer_C*)ClntObjMgrObjectPtr(ActivePlayer, TYPEMASK_PLAYER);
    //if (v16) {
    //    if (!dword_CD7740) {
    //        sub_782560(*(_DWORD*)&v16->gap0[184], &v22);
    //        if (v22 >= g_AreaTableDB.minIndex && v22 <= g_AreaTableDB.maxIndex) {
    //            v17 = g_AreaTableDB.Rows[v22 - g_AreaTableDB.minIndex];
    //            if (v17) {
    //                if ((*(_DWORD*)(v17 + 16) & 0x2000) == 0) {
    //                    v18 = *(_DWORD*)(v17 + 8);
    //                    if (v18) {
    //                        if (v18 < g_AreaTableDB.minIndex || v18 > g_AreaTableDB.maxIndex)
    //                            goto LABEL_49;
    //                        v17 = g_AreaTableDB.Rows[v18 - g_AreaTableDB.minIndex];
    //                    }
    //                }
    //                if (v17) {
    //                    v19 = *(float*)(v17 + 72) + *(float*)(v17 + 72) + 1.0;
    //                    v20 = v19 - flt_ADEEBC;
    //                    v21 = flt_CD76A0;
    //                    if (fabs(v20) > 0.0099999998 && v21 < 1.0)
    //                        v19 = flt_ADEEBC + v21 * v20;
    //                    flt_ADEEBC = v19;
    //                }
    //            }
    //        }
    //    }
    //}
//LABEL_49:
    //s_m2Scene->ukn82 = dword_CD8794;
}

void CWorld::PrepareAreaOfInterest(C3Vector* position) {
    CWorld::s_currentWorldPos = *position;

    CGCamera* activeCamera = CGWorldFrame::GetActiveCamera();
    float fov = activeCamera->FOV();
    float farZ = activeCamera->FarZ();
    float nearZ = activeCamera->NearZ();
    float aspect = activeCamera->Aspect();

    C44Matrix projMatrix;
    GxuXformCreateProjection_Exact(fov * 0.60000002, aspect, nearZ, farZ, projMatrix);
    C44Matrix viewMatrix;
    g_theGxDevicePtr->XformView(viewMatrix);

    C3Vector corners[8];
    GxuXformCalcFrustumCorners(&viewMatrix, &projMatrix, corners);

    float farCornerDist = sqrt(corners[7].z * corners[7].z + corners[7].y * corners[7].y + corners[7].x * corners[7].x);

    float streamDist;
    if (farCornerDist > CWorld::s_farClip) {
        streamDist = std::min(farCornerDist, CWorld::s_farClip * 2.0f);
    } else {
        streamDist = CWorld::s_farClip * 1.25f;
    }

    CWorld::s_groupAreaOfInterest.b = { position->x - 150.0f, position->y - 150.0f, position->z - 150.0f };
    CWorld::s_groupAreaOfInterest.t = { position->x + 150.0f, position->y + 150.0f, position->z + 150.0f };
    CWorld::s_objectAreaOfInterest.b = { position->x - CWorld::s_farClip, position->y - CWorld::s_farClip, position->z - CWorld::s_farClip };
    CWorld::s_objectAreaOfInterest.t = { position->x + CWorld::s_farClip, position->y + CWorld::s_farClip, position->z + CWorld::s_farClip };
    int32_t v10 = 1 - (int32_t)(streamDist * -0.030000001);
    float v25 = -(position->y - 17066.666) * 0.029999999;
    float v26 = -(position->x - 17066.666) * 0.029999999;
    int32_t v11 = (int32_t)(v26 - 0.5);
    int32_t v25i = (int32_t)(v25 - 0.5);
    //if (IsStreamingAndTrial())
    //    sub_420A50(v25i, v11);
    int32_t v14 = (v25i - v10) & ~1;
    CWorld::s_chunkRectHigh.maxX = ((v25i + v10) & ~1u) + 1;
    int32_t v15 = ((v25i + v10) & ~1u) + 3;
    int32_t v16 = (v11 - v10) & ~1u;
    int32_t yMax = ((v10 + v11) & ~1u) + 1;
    CWorld::s_chunkRectLow.minX = v14 - 2;
    CWorld::s_chunkRectLow.minY = v16 - 2;
    CWorld::s_chunkRectLow.maxY = ((v10 + v11) & ~1u) + 3;
    int32_t v18 = v15 - v25i;
    CWorld::s_chunkRectHigh.minX = v14;
    CWorld::s_chunkRectHigh.minY = v16;
    CWorld::s_chunkRectHigh.maxY = yMax;
    CWorld::s_chunkRectLow.maxX = v15;
    if (v15 - v25i < 8) {
        CWorld::s_chunkRectLow.minY -= 8 - v18;
        CWorld::s_chunkRectLow.minY &= ~1u;
        yMax = CWorld::s_chunkRectHigh.maxY;
        v15 = ((v25i + 8) & ~1u) + 1;
        CWorld::s_chunkRectLow.minX = (CWorld::s_chunkRectLow.minX - (8 - v18)) & ~1u;
        CWorld::s_chunkRectLow.maxX = v15;
        CWorld::s_chunkRectLow.maxY = ((8 - v18 + CWorld::s_chunkRectLow.maxY) & ~1u) + 1;
    }
    if (v14 < 0)
        CWorld::s_chunkRectHigh.minX = 0;
    if (CWorld::s_chunkRectHigh.maxX >= 1024)
        CWorld::s_chunkRectHigh.maxX = 1023;
    if (v16 < 0)
        CWorld::s_chunkRectHigh.minY = 0;
    if (yMax >= 1024)
        CWorld::s_chunkRectHigh.maxY = 1023;
    if (CWorld::s_chunkRectLow.minX < 0)
        CWorld::s_chunkRectLow.minX = 0;
    if (v15 >= 1024)
        CWorld::s_chunkRectLow.maxX = 1023;
    if (CWorld::s_chunkRectLow.minY < 0)
        CWorld::s_chunkRectLow.minY = 0;
    if (CWorld::s_chunkRectLow.maxY >= 1024)
        CWorld::s_chunkRectLow.maxY = 1023;
}

void CWorld::Render(const C3Vector& cameraPos, float time) {
    CWorldScene::Render(cameraPos, time);
    // TODO: BotDetectionRoutine();
}

uint32_t CWorld::GetEnables() {
    return CWorld::s_enables;
}

// OFFSET: 0x77EC90
void CWorld::SetLoadProgressCallback(CALLBACK_FUNC callback, void* param) {
    CWorld::s_loadProgressCallback = callback;
    CWorld::s_loadProgressParam = param;
}
