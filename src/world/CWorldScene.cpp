#include "world/CWorldScene.hpp"
#include "world/CWorld.hpp"
#include "world/map/CMap.hpp"
#include "world/daynight/DayNight.hpp"

#include "gx/Device.hpp"
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

// OFFSET: 0x79A870
void CWorldScene::Render(const C3Vector& cameraPos, float time) {
    //if (!dword_CD87A4)
    //    sub_792BD0();
    //sub_790920();
    GxRsPush();
    GxXformPush(GxXform_World);
    CRect rect;
    CGWorldFrame::s_currentWorldFrame->GetRect(&rect);
    CGWorldFrame::GetActiveCamera()->SetGxProjectionAndView(rect);

    //dword_CD8774 = 0;
    //dword_CD8770 = 0;
    //dword_CD872C = 0;
    //dword_CD8624 = 0;
    //CFrustum::CalcPlanesFromCorners(&flt_CDB168[63 * dword_CD8798], &stru_CDB108[0].x);
    //flt_CD8784 = World::s_farClip - 33.333332;
    //dword_CD8778 = ((int)stru_CD9048.unkList1.m_terminator.m_next & 1) == 0 && stru_CD9048.unkList1.m_terminator.m_next;
    //ActiveCamera = CGWorldFrame::GetActiveCamera();
    //flt_CD877C = CWorld::farFog / cos(((double(__thiscall*)(CGCamera*))ActiveCamera->Fov)(ActiveCamera) * 0.5) - CWorld::farFog;
    //sub_782F20();
    //sub_7BA600();
    //sub_7AE060();
    //sub_7B2A80();
    //memset(&byte_CD87B8, 0, 0x180u);
    //memset32(flt_CD8938, 0xC9742400, 0x180u);
    //CWorldOcclusion::ClearVolumes();
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
    //    flt_ADF58C = 0.0;
    //    flt_ADF59C = 0.0;
    //    dword_ADF588 = 0;
    //    flt_ADF590 = 0.0;
    //    flt_CD8780 = -10000.0;
    //    flt_ADF594 = 1.0;
    //    flt_ADF598 = 1.0;
    //    dword_ADF5A4 = 0;
    //    sub_79A790(&flt_ADF58C);
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
        for (size_t i = 0; i < 10; ++i) {
            if (!g_models[i])
                continue;
            g_models[i]->SetAnimating(1);
            g_models[i]->SetVisible(1);
        }

        CWorldScene::s_m2Scene->m_flags |= 1u;
        CWorldScene::s_m2Scene->AdvanceTime(static_cast<uint32_t>(time * 1000.0f));
        CWorldScene::s_m2Scene->Animate(cameraPos);
        CWorldScene::s_m2Scene->m_flags &= ~1u;
    }

    // sub_6FDA20();
    // CShadowQuery::Update();

    CShaderEffect::UpdateProjMatrix();
    //sub_781610();
    //sub_798DA0();

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
