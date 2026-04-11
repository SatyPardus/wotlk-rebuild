#include "world/CWorldScene.hpp"
#include "world/CWorld.hpp"
#include "world/map/CMap.hpp"
#include "world/map/CMapChunk.hpp"
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
    CMapRenderChunk::UpdatePools();
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
    //    dword_D2509C = *(_DWORD*)(g_theGxDevicePtr->ukn1[2620] + 240);
    //    if (CMap::enableTerrainShaderVertex) {
    //        v42.M11 = 1.0;
    //        v42.M22 = 1.0;
    //        v42.M33 = 1.0;
    //        v42.M44 = 1.0;
    //        v42.M12 = 0.0;
    //        v42.M13 = 0.0;
    //        v42.M14 = 0.0;
    //        v42.M21 = 0.0;
    //        v42.M23 = 0.0;
    //        v42.M24 = 0.0;
    //        v42.M31 = 0.0;
    //        v42.M32 = 0.0;
    //        v42.M34 = 0.0;
    //        v42.M41 = 0.0;
    //        v42.M42 = 0.0;
    //        v42.M43 = 0.0;
    //        a3.x = -CWorldScene::s_activeWorldView.x;
    //        a3.y = -CWorldScene::s_activeWorldView.y;
    //        a3.z = -CWorldScene::s_activeWorldView.z;
    //        C44Matrix::Translate(&v42, &a3);
    //        v43.M11 = 1.0;
    //        v43.M12 = 0.0;
    //        v43.M13 = 0.0;
    //        v43.M14 = 0.0;
    //        v43.M21 = 0.0;
    //        v43.M23 = 0.0;
    //        v43.M24 = 0.0;
    //        v43.M31 = 0.0;
    //        v43.M32 = 0.0;
    //        v43.M34 = 0.0;
    //        v43.M41 = 0.0;
    //        v43.M42 = 0.0;
    //        v43.M43 = 0.0;
    //        v43.M22 = 1.0;
    //        v43.M33 = 1.0;
    //        v43.M44 = 1.0;
    //        C44Matrix::Copy(&v43, (C44Matrix*)&g_theGxDevicePtr->ukn1[16 * g_theGxDevicePtr->ukn1[1725] + 1727]);
    //        sub_7CFBE0(&v42, &v43);
    //    } else {
    //        ActiveDayNight = DayNight::GetActiveDayNight();
    //        v1 = g_theGxDevicePtr;
    //        v2 = g_theGxDevicePtr->ukn1[981] == 0;
    //        v3 = ActiveDayNight;
    //        v4 = *((float*)ActiveDayNight + 36);
    //        v46 = *((float*)ActiveDayNight + 36);
    //        if (!v2) {
    //            v5 = g_theGxDevicePtr->ukn1[2620];
    //            v6 = v4 == *(float*)(v5 + 192);
    //            v7 = (float*)(v5 + 192);
    //            if (!v6) {
    //                CGxDevice::IRsDirty(g_theGxDevicePtr, 8);
    //                *v7 = v46;
    //                v1 = g_theGxDevicePtr;
    //            }
    //        }
    //        v2 = v1->ukn1[981] == 0;
    //        v8 = *((float*)v3 + 37);
    //        v46 = *((float*)v3 + 37);
    //        if (!v2) {
    //            v9 = v1->ukn1[2620];
    //            v10 = v8 == *(float*)(v9 + 216);
    //            v11 = (float*)(v9 + 216);
    //            if (!v10) {
    //                CGxDevice::IRsDirty(v1, 9);
    //                *v11 = v46;
    //                v1 = g_theGxDevicePtr;
    //            }
    //        }
    //        if (v1->ukn1[981]) {
    //            v12 = v1->ukn1[2620];
    //            v13 = *(_DWORD*)(v12 + 24);
    //            v14 = (_DWORD*)(v12 + 24);
    //            if (v13 != -8421505) {
    //                CGxDevice::IRsDirty(v1, 1);
    //                *v14 = -8421505;
    //                v1 = g_theGxDevicePtr;
    //            }
    //        }
    //        if (byte_CE049D) {
    //            if (v1->ukn1[981]) {
    //                v15 = v1->ukn1[2620];
    //                v16 = *(_DWORD*)(v15 + 72);
    //                v17 = (_DWORD*)(v15 + 72);
    //                if (v16 != -1) {
    //                    CGxDevice::IRsDirty(v1, 3);
    //                    *v17 = -1;
    //                }
    //            }
    //            sub_763C70(4, 20.0);
    //            v1 = g_theGxDevicePtr;
    //        }
    //        v18 = 0;
    //        for (i = 1272; i < 1392; i += 24) {
    //            if (v1->ukn1[981]) {
    //                v20 = (_DWORD*)(v1->ukn1[2620] + i + 384);
    //                if (*v20 != v18) {
    //                    CGxDevice::IRsDirty(v1, v18 + 69);
    //                    *v20 = v18;
    //                    v1 = g_theGxDevicePtr;
    //                }
    //                if (v1->ukn1[981]) {
    //                    v21 = (_DWORD*)(i + v1->ukn1[2620]);
    //                    if (*v21 != 2) {
    //                        CGxDevice::IRsDirty(v1, v18 + 53);
    //                        *v21 = 2;
    //                        v1 = g_theGxDevicePtr;
    //                    }
    //                    if (v1->ukn1[981]) {
    //                        v22 = (_DWORD*)(v1->ukn1[2620] + i + 192);
    //                        if (*v22 != 1) {
    //                            CGxDevice::IRsDirty(v1, v18 + 61);
    //                            *v22 = 1;
    //                            v1 = g_theGxDevicePtr;
    //                        }
    //                    }
    //                }
    //            }
    //            ++v18;
    //        }
    //    }
    //    if (byte_CE049E) {
    //        v23 = DayNight::GetActiveDayNight();
    //        if (CGxDevice::Caps((char*)g_theGxDevicePtr)->int134) {
    //            v24 = (char*)g_theGxDevicePtr;
    //            v25 = v23[35];
    //            if (!g_theGxDevicePtr->ukn1[981])
    //                goto LABEL_35;
    //            v26 = (int*)(g_theGxDevicePtr->ukn1[2620] + 240);
    //            if (*v26 == v25)
    //                goto LABEL_35;
    //            CGxDevice::IRsDirty(g_theGxDevicePtr, 10);
    //            *v26 = v25;
    //        } else {
    //            sub_984C90(v23 + 35);
    //            ((void(__thiscall*)(CGxDevice*, int, int, char*, int))g_theGxDevicePtr->ukn70)(
    //                g_theGxDevicePtr,
    //                4,
    //                2,
    //                v44,
    //                1);
    //        }
    //        v24 = (char*)g_theGxDevicePtr;
    // LABEL_35:
    //        if (CGxDevice::Caps(v24)->int138) {
    //            if (g_theGxDevicePtr->ukn1[981]) {
    //                v27 = (_DWORD*)(g_theGxDevicePtr->ukn1[2620] + 288);
    //                if (*v27 != 1) {
    //                    CGxDevice::IRsDirty(g_theGxDevicePtr, 12);
    //                    *v27 = 1;
    //                }
    //            }
    //        }
    //        sub_874660();
    //        goto LABEL_58;
    //    }
    //    v28 = DayNight::GetActiveDayNight();
    //    v29 = g_theGxDevicePtr;
    //    v30 = v28[35];
    //    if (g_theGxDevicePtr->ukn1[981]) {
    //        v31 = (int*)(g_theGxDevicePtr->ukn1[2620] + 240);
    //        if (*v31 != v30) {
    //            CGxDevice::IRsDirty(g_theGxDevicePtr, 10);
    //            *v31 = v30;
    //            v29 = g_theGxDevicePtr;
    //        }
    //        if (v29->ukn1[981]) {
    //            v32 = (_DWORD*)(v29->ukn1[2620] + 288);
    //            if (*v32 != 1) {
    //                CGxDevice::IRsDirty(v29, 12);
    //                *v32 = 1;
    //                v29 = g_theGxDevicePtr;
    //            }
    //            if (v29->ukn1[981]) {
    //                v33 = (_DWORD*)(v29->ukn1[2620] + 888);
    //                if (*v33 != 1) {
    //                    CGxDevice::IRsDirty(v29, 37);
    //                    *v33 = 1;
    //                    v29 = g_theGxDevicePtr;
    //                }
    //                if (v29->ukn1[981]) {
    //                    v34 = (_DWORD*)(v29->ukn1[2620] + 1080);
    //                    if (*v34) {
    //                        CGxDevice::IRsDirty(v29, 45);
    //                        *v34 = 0;
    //                        v29 = g_theGxDevicePtr;
    //                    }
    //                    if (v29->ukn1[981]) {
    //                        v35 = (_DWORD*)(v29->ukn1[2620] + 912);
    //                        if (*v35) {
    //                            CGxDevice::IRsDirty(v29, 38);
    //                            *v35 = 0;
    //                            v29 = g_theGxDevicePtr;
    //                        }
    //                        if (v29->ukn1[981]) {
    //                            v36 = (_DWORD*)(v29->ukn1[2620] + 1104);
    //                            if (*v36) {
    //                                CGxDevice::IRsDirty(v29, 46);
    //                                *v36 = 0;
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
    // LABEL_58:
    //    CWorldScene::RenderChunksSinglePass();
    CWorldScene::RenderChunksSolid();
    //    CWorldScene::RenderChunksZoneDebug();
    //    for (j = 0; j < 350; j += 70) {
    //        v38 = g_theGxDevicePtr->ukn1[j + 1025];
    //        v39 = &g_theGxDevicePtr->ukn1[j + 1025];
    //        if ((v39[v38 + 66] & 1) == 0) {
    //            v40 = (float*)&v39[16 * v38 + 2];
    //            v40[15] = 1.0;
    //            v40[10] = 1.0;
    //            v40[5] = 1.0;
    //            *v40 = 1.0;
    //            v40[14] = 0.0;
    //            v40[13] = 0.0;
    //            v40[12] = 0.0;
    //            v40[11] = 0.0;
    //            v40[9] = 0.0;
    //            v40[8] = 0.0;
    //            v40[7] = 0.0;
    //            v40[6] = 0.0;
    //            v40[4] = 0.0;
    //            v40[3] = 0.0;
    //            v40[2] = 0.0;
    //            v40[1] = 0.0;
    //            v41 = *v39;
    //            *((_BYTE*)v39 + 4) = 1;
    //            v39[v41 + 66] = 1;
    //        }
    //    }
    GxRsPop();
}

// OFFSET: 0x793B10
void CWorldScene::RenderChunksSolid() {
    // ### FAKE WORLD RENDER #######
    for (auto link = CMap::mapAreaList.Head(); link; link = CMap::mapAreaList.Next(link)) {
        CMapArea* area = (CMapArea*)link->owner;
        for (int32_t i = 0; i < 16 * 16; i++) {
            CMapChunk* chunk = area->mapChunks[i];
            if (!chunk)
                continue;

            chunk->RenderPrep();
            if (chunk->renderChunk) {
                chunk->renderChunk->RenderSetup(1);
            }
        }
    }
    // #############################


    //CMapRenderChunk::SetShaders(0, 0);
    //if (CMapRenderChunk::s_currentShaderX)
    //    CGxDevice::RsSet(g_theGxDevicePtr, GxRs_PixelShader, CMapRenderChunk::s_currentShaderX);
    //m_next = (CMapRenderChunk*)CWorldScene::sortTable.renderChunkList[0].m_terminator.m_next;
    //if (((int)CWorldScene::sortTable.renderChunkList[0].m_terminator.m_next & 1) != 0 || !CWorldScene::sortTable.renderChunkList[0].m_terminator.m_next) {
    //    m_next = 0;
    //}
    //while (((unsigned __int8)m_next & 1) == 0 && m_next) {
    //    v9 = *(CMapRenderChunk**)((char*)&m_next->renderChunkLink.m_next + CWorldScene::sortTable.renderChunkList[0].m_linkoffset);
    //    CMapRenderChunk::RenderSetup(m_next, 1);
    //    if ((CWorld::enables & 2) != 0) {
    //        if (CMapRenderChunk::s_currentShaderX) {
    //            if (CMap::enableTerrainShaderVertex)
    //                CMapRenderChunk::RenderSolidVertexPixelShader(m_next);
    //            else
    //                CMapRenderChunk::RenderSolidPixelShader((int)m_next);
    //        } else {
    //            CMapRenderChunk::RenderSolid((int)m_next);
    //        }
    //    }
    //    m_prevLink = m_next->renderChunkLink.m_prevLink;
    //    if (m_next->renderChunkLink.m_prevLink) {
    //        v2 = m_next->renderChunkLink.m_next;
    //        if (((unsigned __int8)v2 & 1) == 0 && v2)
    //            v3 = (TSLink_CMapRenderChunk**)((char*)&v2->renderChunkLink.m_prevLink + (char*)m_next - (char*)m_prevLink->m_next);
    //        else
    //            v3 = (_DWORD*)((unsigned int)v2 & 0xFFFFFFFE);
    //        *v3 = m_prevLink;
    //        m_next->renderChunkLink.m_prevLink->m_next = m_next->renderChunkLink.m_next;
    //        m_next->renderChunkLink.m_prevLink = 0;
    //        m_next->renderChunkLink.m_next = 0;
    //    }
    //    v4 = *(int*)((char*)&m_next->renderChunkLink.m_prevLink + CMap::s_mapRenderChunkUpdateList.m_linkoffset);
    //    v5 = (TSLink*)((char*)m_next + CMap::s_mapRenderChunkUpdateList.m_linkoffset);
    //    if (v4) {
    //        v6 = (unsigned int)v5->m_next;
    //        if ((v6 & 1) == 0 && v6)
    //            v7 = (TSLink**)((char*)&v5->m_prevlink + v6 - *(_DWORD*)(v4 + 4));
    //        else
    //            v7 = (_DWORD*)(v6 & 0xFFFFFFFE);
    //        *v7 = v4;
    //        v5->m_prevlink->m_next = v5->m_next;
    //        v5->m_prevlink = 0;
    //        v5->m_next = 0;
    //    }
    //    v8 = CMap::s_mapRenderChunkUpdateList.m_terminator.m_prevlink;
    //    v5->m_prevlink = CMap::s_mapRenderChunkUpdateList.m_terminator.m_prevlink;
    //    v5->m_next = v8->m_next;
    //    v8->m_next = m_next;
    //    m_next = v9;
    //    CMap::s_mapRenderChunkUpdateList.m_terminator.m_prevlink = v5;
    //}
}
