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


void CWorld::Initialize() {
    CWorld::s_enables |=
          Enables::Enable_1
        | Enables::Enable_2
        | Enables::Enable_10
        | Enables::Enable_Culling
        | Enables::Enable_Shadow
        | Enables::Enable_100
        | Enables::Enable_200
        | Enables::Enable_800
        | Enables::Enable_4000
        | Enables::Enable_DetailDoodads
        | Enables::Enable_1000000
        | Enables::Enable_Particulates
        | Enables::Enable_LowDetail;

    // TODO

    if (GxCaps().m_shaderTargets[GxSh_Pixel] > GxShPS_none) {
        CWorld::s_enables |= Enables::Enable_PixelShader;
    }

    if (GxCaps().m_shaderTargets[GxSh_Vertex] > GxShVS_none) {
        CWorld::s_enables2 |= Enables2::Enable_VertexShader;
    }

    // TODO

    uint32_t m2Flags = M2GetCacheFlags();
    CShaderEffect::InitShaderSystem(
        (m2Flags & 0x8) != 0,
        (CWorld::s_enables2 & Enables2::Enable_HwPcf) != 0
    );

    CWorldScene::Initialize();
    CMap::Initialize();

    // TODO
}

void CWorld::LoadMap(const char* mapName, const C3Vector& position, int32_t zoneID) {
    // TODO: calculate far clip
    CWorld::s_farClip = 1583.3334f;
    //World::s_farClip = sub_780770(CWorldParam::cvar_farClip->m_numberValue, mapid);
    CWorld::s_nearClip = 0.2f;
    CWorld::prevFarClip = CWorld::s_farClip;
    //if (IsStreamingAndTrial())
    //    sub_420AA0(mapid);
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

void CWorld::PrepareAreaOfInterest(const C3Vector& position) {
    CWorld::s_currentWorldPos = position;

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

    CWorld::s_groupAreaOfInterest.b = { position.x - 150.0f, position.y - 150.0f, position.z - 150.0f };
    CWorld::s_groupAreaOfInterest.t = { position.x + 150.0f, position.y + 150.0f, position.z + 150.0f };
    CWorld::s_objectAreaOfInterest.b = { position.x - CWorld::s_farClip, position.y - CWorld::s_farClip, position.z - CWorld::s_farClip };
    CWorld::s_objectAreaOfInterest.t = { position.x + CWorld::s_farClip, position.y + CWorld::s_farClip, position.z + CWorld::s_farClip };
    int32_t v10 = 1 - (int32_t)(streamDist * -0.030000001);
    float v25 = -(position.y - 17066.666) * 0.029999999;
    float v26 = -(position.x - 17066.666) * 0.029999999;
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

void CWorld::SetLoadProgressCallback(CALLBACK_FUNC callback, void* param) {
    CWorld::s_loadProgressCallback = callback;
    CWorld::s_loadProgressParam = param;
}
