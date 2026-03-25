#include "world/LoadingScreen.hpp"
#include <common/time/Time.hpp>
#include <event/Event.hpp>
#include <util/CStatus.hpp>
#include <util/Unimplemented.hpp>
#include <db/StaticDb.hpp>

void LoadingScreenInitialize() {
    g_theGxDevicePtr->ShaderCreate(&s_vertexShader, GxSh_Vertex, "Shaders\\Vertex", "UI", 2);
    g_theGxDevicePtr->ShaderCreate(&s_pixelShader, GxSh_Pixel, "Shaders\\Pixel", "UI", 1);
    // sub_40B0B0(&g_TaxiPathNodeDB, stru_AD4BEC.maxIndex + 1);
    // sub_407C50();
}

// OFFSET: 0x40AB70
void LoadingScreenEnable(int mapId, bool a2) {
    // CGGameUI::SetAspect();
    if (s_gameTip) {
        char* fonts[] = {
            "Fonts\\FRIZQT__.TTF",
            "Fonts\\2002.TTF",
            "Fonts\\FRIZQT__.TTF",
            "Fonts\\FRIZQT__.TTF",
            "Fonts\\ZYKai_T.TTF",
            "Fonts\\blei00d.TTF",
            "Fonts\\FRIZQT__.TTF",
            "Fonts\\FRIZQT__.TTF",
            "Fonts\\NIM_____.TTF"
        };
        auto v8 = NDCToDDCHeight(0.017999999);
        s_textFont = TextBlockGenerateFont(fonts[0 /* TODO s_locale*/], 1, v8);
        auto fontPtr = TextBlockGetFontPtr(s_textFont);
        v8 = 515.0 / (CoordinateGetAspectCompensation() * 1024.0);
        v8 = NDCToDDCWidth(v8);
        float v12 = DDCToNDCWidth(v8);
        float v4 = 0.0;
        auto AspectCompensation = CoordinateGetAspectCompensation();
         if (AspectCompensation < 1.0)
             v4 = (1.0 - AspectCompensation) * 0.5;
         C3Vector pos = C3Vector(0.5 - v12 * 0.5, v4 + 0.1, 1.0);
        auto v5 = alloca(SStrLen(s_gameTip) + 1);
        auto v6 = SStrLen(s_gameTip);
        char* buf = new char[v6 + 1];
        memcpy(buf, s_gameTip, v6 + 1);
        // for (i = &v9[(_DWORD)(SStrLen(v9) - 1)]; *i == 13 || *i == 10; --i)
        //     *i = 0;
        CImVector textColor = { 0xC8, 0xC8, 0xC8, 0xD7 };
        GxuFontCreateString(fontPtr, buf, 0.017999999, pos, v12, 1.0, 0.0049999999, s_tipStr, GxVJ_Bottom, GxHJ_Left, 0, textColor, 0.0, 1.0);
        C2Vector offset = C2Vector(0.001, -0.001);
        CImVector color = { 0x00, 0x00, 0x00, 0xFF };
        GxuFontAddShadow(s_tipStr, color, offset);
        s_batch = GxuFontCreateBatch(0, 0);
        GxuFontAddToBatch(s_batch, s_tipStr);
    }
    s_simpleMapID = mapId;
    InitializeProgressBar(a2);
}

// OFFSET: 0x409550
void LoadingScreenDisable() {
    if (s_loadingScreenLayer)
        HandleClose(s_loadingScreenLayer);
    s_loadingScreenLayer = 0;
     for (int32_t i = 0; i < 2; ++i) {
         if (s_textures[i])
             HandleClose(s_textures[i]);
         s_textures[i] = 0;
     }
    if (s_simpleBackgroundTexture)
        HandleClose(s_simpleBackgroundTexture);
    s_simpleBackgroundTexture = 0;
    LoadingScreenDisableEvents();
    ClearDynamicData();
    s_gameTip = 0;
     if (s_batch)
         GxuFontDestroyBatch(s_batch);
     s_batch = 0;
     if (s_tipStr)
         GxuFontDestroyString(s_tipStr);
     s_tipStr = 0;
     if (s_textFont)
         HandleClose(s_textFont);
     s_textFont = 0;
    // SI2::StopAllMusic();
    // SI2::StopZoneAmbience();
    // if (SFile::IsStreamingMode()) {
    //     sub_53E930();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    //     sub_53E810();
    // }
}

// OFFSET: 0x407E30
void LoadingScreenSetTip(const char* tip) {
    s_gameTip = tip;
}

// OFFSET: 0x40A920
void UpdateProgressBar(bool force) {
    // TODO
    auto currentTime = OsGetAsyncTimeMs();
    auto context = EventGetCurrentContext();
    // EventInputProcess(CurrentContext);

    if (force || currentTime - s_lastUpdateTime >= 250) {
        s_lastUpdateTime = currentTime;
        // SE2::HeartBeat();
        // NOP();
        ProgressBarSendKeepAlive(currentTime);
        UpdateProgressValue();
        LoadingScreenPaint(nullptr, nullptr, nullptr, 0);
        // sub_682A00();
    }
}

void UpdateProgressValue() {
    float v0 = 0.0;
    float v1 = 1.0;
    if (s_xmlProgress != 0.0) {
        v0 = s_xmlProgress;
        if (s_loadingWorld) {
            v1 = 0.5;
            v0 = 0.5 * s_xmlProgress;
        }
    }
    float v2 = v0 + s_asyncProgress * 0.0 + v1 * s_worldProgress;
    s_progress = v2;
    if (v2 < 0.0)
        s_progress = 0.0;
    if (v2 >= 1.0) {
        s_progress = 1.0;
    }
    //if (IsStreamingAndTrial() && byte_B2FED9) {
    //    v3 = sub_4215D0();
    //    if (sub_4215A0(v3))
    //        s_progress = s_progress * 0.30000001 + 0.69999999;
    //    else
    //        s_progress = flt_B2FEDC * 0.69999999;
    //}
}

// OFFSET: 0x408520
void ProgressBarSendKeepAlive(int time) {
    // TODO
    if (time - s_nextPingTime >= 0) {
        //    v2 = off_9E0E24;
        //    v3 = 0;
        //    v4 = 0;
        //    v5[0] = 0;
        //    v5[1] = 0;
        //    v6 = -1;
        //    CDataStore::PutInt32(&v2, CMSG_KEEP_ALIVE);
        //    v6 = 0;
        //    ClientServices::Send2(&v2);
        s_nextPingTime = time + 30000;
        //    v2 = off_9E0E24;
        //    if (v5[0] != -1)
        //        CDataStore::InternalDestroy(&v3, &v4, v5);
    }
}

// OFFSET: 0x40A270
void LoadingScreenPaint(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    // TODO
    if (g_theGxDevicePtr /* && CGxDevice::CapsHasContext(-1) && g_theGxDevicePtr && CGxDevice::CapsIsWindowVisible(-1) */ && (s_progress <= 0.99000001 || IsStillLoading())) {
        if (!s_simpleBackgroundTexture && s_simpleMapID != -1 && !LoadSimpleBackgroundTexture())
            s_simpleMapID = -1;
        C3Vector saveMin;
        C3Vector saveMax;
        GxXformViewport(saveMin.x, saveMax.x, saveMin.y, saveMax.y, saveMin.z, saveMax.z);
        float minX = 0.0;
        float minY = 0.0;
        float maxX = 1.0;
        float maxY = 1.0;
        float minZ = 1.0;
        float maxZ = 1.0;
        GxXformSetViewport(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
        CImVector clearColor = { 0x00, 0x00, 0x00, 0xFF };
        GxSceneClear(1, clearColor);

        auto aspectCompensation = CoordinateGetAspectCompensation();

        if (s_usingWideScreen) {
            minZ = 1.6 / 1.3333334;
        }

        minZ = aspectCompensation / minZ;
        if (minZ <= maxZ) {
            if (minZ >= maxZ) {

            } else {
                maxY = minZ;
                minY = (maxZ - minZ) * 0.5;
            }
        } else {
            maxX = maxZ / minZ;
            minX = (maxZ - maxX) * 0.5;
        }
        maxY += minY;
        maxX += minX;
        GxXformSetViewport(minX, maxX, minY, maxY, 0.0, maxZ);
        C44Matrix viewMatrix = C44Matrix(
            C4Vector(1.0, 0.0, 0.0, 0.0),
            C4Vector(0.0, 1.0, 0.0, 0.0),
            C4Vector(0.0, 0.0, 1.0, 0.0),
            C4Vector(0.0, 0.0, 0.0, 1.0));
        C44Matrix matrix = C44Matrix(
            C4Vector(1.0, 0.0, 0.0, 0.0),
            C4Vector(1.0, 0.0, 0.0, 0.0),
            C4Vector(1.0, 0.0, 0.0, 0.0),
            C4Vector(1.0, 0.0, 0.0, 0.0));
        GxuXformCreateOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 500.0, matrix);
        GxXformSetView(viewMatrix);
        GxXformSetProjection(matrix);
        g_theGxDevicePtr->RsPush();
        C44Matrix viewProjMat = C44Matrix(
            C4Vector(1.0, 0.0, 0.0, 0.0),
            C4Vector(0.0, 1.0, 0.0, 0.0),
            C4Vector(0.0, 0.0, 1.0, 0.0),
            C4Vector(0.0, 0.0, 0.0, 1.0));
        if (s_vertexShader->Valid() && s_pixelShader->Valid()) {
            g_theGxDevicePtr->RsSet(GxRs_VertexShader, s_vertexShader);
            g_theGxDevicePtr->RsSet(GxRs_PixelShader, s_pixelShader);
            GxXformViewProjNativeTranspose(viewProjMat);
            GxShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<float*>(&viewProjMat), 4);
        } else {
            //         sub_408BF0(11, 0);
        }
        //     if (*(_DWORD*)(g_theGxDevicePtr + 3928)) {
        //         v10 = (_DWORD*)(*(_DWORD*)(g_theGxDevicePtr + 10484) + 288);
        //         if (*v10) {
        g_theGxDevicePtr->IRsDirty(GxRs_Fog);
        //             *v10 = 0;
        //         }
        //     }
        if (!PaintBackgroundImage()) {
            PaintSimpleBackground();
            if (s_vertexShader->Valid() && s_pixelShader->Valid()) {
                GxShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<float*>(&viewProjMat), 4);
            }
            PaintLoadingBar(s_textureInfo, 2, s_textures);
        }
        g_theGxDevicePtr->RsPop();
        GxXformSetViewport(saveMin.x, saveMax.x, saveMin.y, saveMax.y, saveMin.z, saveMax.z);
    }
}

// OFFSET: 0x40AD50
void LoadingScreenEnableShip(int32_t a1, int32_t a2, int32_t a3) {
    // TODO
    s_simpleMapID = a3;
    // ActivePlayer = ClntObjMgrGetActivePlayer();
    // v4 = ClntObjMgrObjectPtr(ActivePlayer, TYPEMASK_PLAYER);
    // if (v4) {
    //     v5 = ((__int64(__thiscall*)(CGUnit_C*))v4->ObjectBase.GetTransportGUID)(v4);
    //     v6 = ClntObjMgrObjectPtr(v5, TYPEMASK_GAMEOBJECT);
    //     if (v6) {
    //         if (v6->ObjectBase.ObjectData->OBJECT_FIELD_ENTRY == a1) {
    //             v7 = CGameObjectDef::GetPropNum(SBYTE1(v6->UnitData->UNIT_FIELD_CREATEDBY.guid_high), 35);
    //             v8 = CGGameObject_C::GetPropertyValue(v7);
    //             v9 = sub_408CD0(&g_TaxiPathNodeDB, v8, a2);
    //             if (!InitializeSpline(v9, v8) || !LoadDynamicBackground()) {
    //                 ClearDynamicData();
    //                 InitializeProgressBar(1);
    //             }
    //             s_simpleMapID = -1;
    //         }
    //     }
    // }
    InitializeProgressBar(1);
}

// OFFSET: 0x4094E0
void ClearDynamicData() {
    // TODO
    // if (s_dynamicElementsTexture)
    //     HandleClose((int)s_dynamicElementsTexture);
    // s_dynamicElementsTexture = 0;
    // for (i = 0; i < 12; ++i) {
    //     if (s_dynamicBackgroundTextures[i])
    //         HandleClose(s_dynamicBackgroundTextures[i]);
    //     s_dynamicBackgroundTextures[i] = 0;
    // }
    // if (dword_B302C8)
    //     SMemFree(dword_B302C8, (int)".?AUDYNAMICELEMENTVERT@@", -2, 0);
    // dword_B302C0 = 0;
    // dword_B302C4 = 0;
    // dword_B302C8 = 0;
}

// OFFSET: 0x40A990
void InitializeProgressBar(bool worldLoading) {
    // TODO
    if (s_loadingScreenLayer) {
        //auto flags = ScrnLayerGetFlags(s_loadingScreenLayer);
        //ScrnLayerSetFlags(s_loadingScreenLayer, flags & 0xFFFFFFFE);
    }
    // flt_B2FEB8 = 0.0;
    // flt_B2FEB4 = 0.0;
    // flt_B2FEB0 = 0.0;
    s_loadingWorld = worldLoading;
    if (!s_loadingScreenLayer) {
        CStatus status;
        for (int32_t i = 0; i < 2; i++) {
            if (!s_textures[i]) {
                CGxTexFlags v0 = CGxTexFlags(GxTex_Nearest, 0, 0, 0, 0, 0, 1);
                s_textures[i] = TextureCreate(s_textureInfo[i].path, v0, &status, 0);
            }
        }
        const RECTF rect = { 0.0, 0.0, 1.0f, 1.0f };
        ScrnLayerCreate(&rect, 9.0, 6, 0, LoadingScreenPaint, &s_loadingScreenLayer);
        LoadingScreenEnableEvents();
        // CStatus::Destroy(status);
    }

    s_nextPingTime = OsGetAsyncTimeMs() + 30000;
    s_lastUpdateTime = 0;

    UpdateProgressBar(0);
}

// OFFSET: 0x407B00
void LoadingScreenEnableEvents() {
    EventRegisterEx(EVENT_ID_CHAR, EatEvent, nullptr, 8.0);
    EventRegisterEx(EVENT_ID_IME, EatEvent, nullptr, 8.0);
    EventRegisterEx(EVENT_ID_KEYDOWN, EatEvent, nullptr, 8.0);
    EventRegisterEx(EVENT_ID_KEYDOWN_REPEATING, EatEvent, nullptr, 8.0);
    EventRegisterEx(EVENT_ID_MOUSEDOWN, EatEvent, nullptr, 8.0);
    EventRegisterEx(EVENT_ID_MOUSEMOVE, EatEvent, nullptr, 8.0);
    EventRegisterEx(EVENT_ID_SIZE, SizeEvent, nullptr, 8.0);
    s_sizeEventPosted = false;
}

// OFFSET: 0x407BD0
void LoadingScreenDisableEvents() {
    // EventUnregister(EVENT_ID_CHAR, EatEvent);
    // EventUnregister(EVENT_ID_IME, EatEvent);
    // EventUnregister(EVENT_ID_KEYDOWN, EatEvent);
    // EventUnregister(EVENT_ID_KEYDOWN_REPEATING, EatEvent);
    // EventUnregister(EVENT_ID_MOUSEDOWN, EatEvent);
    // EventUnregister(EVENT_ID_MOUSEMOVE, EatEvent);
    // EventUnregister(EVENT_ID_SIZE, SizeEvent);
    if (s_sizeEventPosted) {
        // CurrentContext = EventGetCurrentContext();
        // EventQueuePost(CurrentContext, 35, (int)&s_sizeEvent, 24);
    }
}

int32_t EatEvent(const void* a1, void* a2) {
    return 0;
}

// OFFSET: 0x407AB0
int32_t SizeEvent(const void* a1, void* a2) {
    // off_AB6470 = *(int (***)())a1;
    // dword_AB6474 = *(_DWORD*)(a1 + 4);
    // dword_AB6478 = *(_DWORD*)(a1 + 8);
    // dword_AB647C = *(_DWORD*)(a1 + 12);
    // dword_AB6480 = *(_DWORD*)(a1 + 16);
    // dword_AB6484 = *(_DWORD*)(a1 + 20);
    s_sizeEventPosted = true;
    return 0;
}

// OFFSET: 0x40A160
bool LoadDynamicBackground() {
    // TODO
    CStatus status;
    if (!s_dynamicElementsTexture) {
        CGxTexFlags v0 = CGxTexFlags(GxTex_Nearest, 0, 0, 0, 0, 0, 1);
        s_dynamicElementsTexture = (CTexture*)TextureCreate("Interface\\Glues\\LoadingScreens\\DynamicElements", v0, &status, 1);
    }
    if (s_dynamicElementsTexture) {
        char buffer[STORM_MAX_PATH] = { 0 };
        for (int i = 0; i < 12; i++) {
            SStrPrintf(buffer, STORM_MAX_PATH, "Interface\\WorldMap\\World\\World%d", i + 1);
            CGxTexFlags v0 = CGxTexFlags(GxTex_Nearest, 0, 0, 0, 0, 0, 1);
            auto texture = (CTexture*)TextureCreate(buffer, v0, &status, 1);
            if (!texture) {
                // CStatus::Destroy(status);
                return false;
            }

            s_dynamicBackgroundTextures[i] = texture;
        }

        // CStatus::Destroy(status);
        return true;
    }

    // CStatus::Destroy(status);
    return false;
}

// OFFSET: 0x409010
bool PaintBackgroundImage() {
    // v6 = (int)this;
    // if (!dword_B302C4)
    //     return 0;
    // v2 = dword_B2FF00;
    // v3 = (float*)dword_B30140;
    // v6 = -1;
    // for (i = 0; i < 12; ++i) {
    //     GxPrimLockVertexPtrs(4, v2, 12, 0, 0, &v6, 0, 0, 0, v3, 8, 0, 0);
    //     GxTex = CTexture::GetGxTex((CTexture*)s_dynamicBackgroundTextures[i], 1, 0);
    //     CGxDevice::RsSet(g_theGxDevicePtr, 21, GxTex);
    //     sub_682340(4, 4, &unk_AB63C0);
    //     NOP();
    //     v2 += 12;
    //     v3 += 8;
    // }
    // PaintDynamicLoadingBar();
    // return 1;
    return false;
}

// OFFSET: 0x409ED0
bool LoadSimpleBackgroundTexture() {
    CStatus status;
    if (s_simpleMapID >= g_mapDB.m_minID && s_simpleMapID <= g_mapDB.m_maxID) {
        auto mapRow = g_mapDB.GetRecord(s_simpleMapID);
        if (mapRow && mapRow->m_loadingScreenID >= g_loadingScreensDB.m_minID && mapRow->m_loadingScreenID <= g_loadingScreensDB.m_maxID) {
            auto loadingScreenRow = g_loadingScreensDB.GetRecord(mapRow->m_loadingScreenID);
            if (loadingScreenRow) {
                s_usingWideScreen = false;
                s_simpleBackgroundTexture = nullptr;

                if (SFile::FileExists(loadingScreenRow->m_fileName)) {
                    if(true) {//if (!IsStreamingAndTrial() || sub_4217A0((int)v2->m_fileName, 6) == 1) {
                        CGxTexFlags v0 = CGxTexFlags(GxTex_Nearest, 0, 0, 0, 0, 0, 1);
                        s_simpleBackgroundTexture = TextureCreate(loadingScreenRow->m_fileName, v0, &status, 1);
                    } else {
                        //sub_421800((int)v2->m_fileName, 6, 0);
                    }
                }
            }
        }
    }
    /*
    if (s_simpleMapID >= g_MapDB.minIndex && s_simpleMapID <= g_MapDB.maxIndex) {
        v0 = g_MapDB.Rows[s_simpleMapID - g_MapDB.minIndex];
        if (v0) {
            v1 = *(_DWORD*)(v0 + 36);
            if (v1 >= g_LoadingScreensDB.minIndex && v1 <= g_LoadingScreensDB.maxIndex) {
                v2 = (LoadingScreensRec*)g_LoadingScreensDB.Rows[v1 - g_LoadingScreensDB.minIndex];
                if (v2) {
                    s_usingWideScreen = 0;
                    s_simpleBackgroundTexture = 0;
                    if (flt_AB63B4 + 0.001 < GetAspectRatio() && v2->m_hasWideScreen) {
                        v3 = alloca(strlen(v2->m_fileName) + 32);
                        v4 = strchr(v2->m_fileName, 46);
                        v5 = v2->m_fileName;
                        v6 = &v16;
                        do {
                            v7 = *v5;
                            *v6++ = *v5++;
                        } while (v7);
                        if (v4)
                            v17[v4 - v2->m_fileName - 1] = 0;
                        strcpy(&v17[&v17[strlen(&v16)] - v17 - 1], "Wide");
                        if (v4) {
                            v8 = &v17[&v17[strlen(&v16)] - v17 - 1] - v4;
                            do {
                                v9 = *v4;
                                v4[v8] = *v4;
                                ++v4;
                            } while (v9);
                        }
                        if (SFile::FileExists((int)&v16)) {
                            if (!IsStreamingAndTrial() || sub_4217A0((int)&v16, 6) == 1) {
                                v10 = (int*)CGxTexFlags::CGxTexFlags(&v20, 1, 0, 0, 0, 0, 0, 1u, 0, 0, 0);
                                s_simpleBackgroundTexture = (CTexture*)TextureCreate(&v16, *v10, (int)v18, 1);
                                if (!s_simpleBackgroundTexture) {
LABEL_23:
                                    if (SFile::FileExists((int)v2->m_fileName)) {
                                        if (!IsStreamingAndTrial() || sub_4217A0((int)v2->m_fileName, 6) == 1) {
                                            v12 = (int*)CGxTexFlags::CGxTexFlags(&v20, 1, 0, 0, 0, 0, 0, 1u, 0, 0, 0);
                                            s_simpleBackgroundTexture = (CTexture*)TextureCreate(
                                                v2->m_fileName,
                                                *v12,
                                                (int)v18,
                                                1);
                                        } else {
                                            sub_421800((int)v2->m_fileName, 6, 0);
                                        }
                                    }
                                    goto LABEL_28;
                                }
                                s_usingWideScreen = 1;
                            } else {
                                sub_421800((int)&v16, 6, 0);
                            }
                        }
                    }
                    v11 = s_simpleBackgroundTexture == 0;
                    if (s_simpleBackgroundTexture)
                        goto LABEL_30;
                    goto LABEL_23;
                }
            }
        }
    }
LABEL_28:*/

    if (!s_simpleBackgroundTexture) {
        CGxTexFlags v0 = CGxTexFlags(GxTex_Nearest, 0, 0, 0, 0, 0, 1);
        s_simpleBackgroundTexture = TextureCreate("Interface\\Glues\\loading", v0, &status, 1);
    }
    // LABEL_30:
    //  CStatus::Destroy(v18);
    return s_simpleBackgroundTexture != nullptr;
}

// OFFSET: 0x4085A0
bool PaintSimpleBackground() {
    if (!s_simpleBackgroundTexture)
        return false;
    if (!s_positionsGuard) {
        s_positionsGuard = true;
        s_positions[0] = C3Vector(0.0, 0.0, 0.0);
        s_positions[1] = C3Vector(1.0, 0.0, 0.0);
        s_positions[2] = C3Vector(0.0, 1.0, 0.0);
        s_positions[3] = C3Vector(1.0, 1.0, 0.0);
    }
    CImVector color = { 0xFF, 0xFF, 0xFF, 0xFF };
    GxPrimLockVertexPtrs(4, s_positions, 12, nullptr, 0, &color, 0, nullptr, 0, s_texCoord, 8, nullptr, 0);
    auto gxTex = TextureGetGxTex(s_simpleBackgroundTexture, 1, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_indices);
    GxPrimUnlockVertexPtrs();
    GxuFontRenderBatch(s_batch);
    return true;
}

// OFFSET: 0x4090C0
void PaintLoadingBar(const TextureInfo* textureInfo, size_t infoCount, const HTEXTURE* textures) {
    //v3 = 0.0;
    //if ((dword_B30730 & 1) == 0) {
    //    dword_B30730 |= 1u;
    //    flt_B30724 = 0.0;
    //    flt_B30728 = 0.0;
    //    flt_B3072C = 1.0;
    //}
    //v4 = 0;
    if (infoCount <= 0)
        return;

    for (size_t i = 0; i < 1; i++) {
        if (!textures[i])
            continue;

        //v6 = g_theGxDevicePtr;
        //if (g_theGxDevicePtr->ukn1[981]) {
        //    v7 = (_DWORD*)(g_theGxDevicePtr->ukn1[2620] + 144);
        //    if (*v7 != 2) {
        //        CGxDevice::IRsDirty(g_theGxDevicePtr, 6);
        //        p_xMax = v31;
        //        *v7 = 2;
        //        v6 = g_theGxDevicePtr;
        //    }
        //    if (v6->ukn1[981]) {
        //        v8 = v6->ukn1[2620];
        //        v9 = dword_AD8B7C[*(_DWORD*)(v8 + 144)];
        //        v10 = &v6->ukn1[2620];
        //        if (*(_DWORD*)(v8 + 168) != v9) {
        //            CGxDevice::IRsDirty(v6, 7);
        //            p_xMax = v31;
        //            *(_DWORD*)(*v10 + 168) = v9;
        //        }
        //    }
        //}
        C3Vector positions[4];
        float v12 = textureInfo[i].rect.maxY * 0.5;
        float v13 = textureInfo[i].rect.minY - v12;
        positions[0].x = v13;
        positions[2].x = v13;
        float v14 = 0.5 * textureInfo[i].rect.maxX;
        positions[0].y = textureInfo[i].rect.minX - v14;
        positions[1].y = positions[0].y;
        positions[1].x = v12 + textureInfo[i].rect.minY;
        positions[3].x = positions[1].x;
        positions[2].y = v14 + textureInfo[i].rect.minX;
        positions[3].y = positions[2].y;
        if (textureInfo[i].unk) {
            positions[1].x = v13 + textureInfo[i].rect.maxY * s_progress;
            positions[3].x = positions[1].x;
        }
        positions[0].z = 0.0;
        positions[1].z = 0.0;
        positions[2].z = 0.0;
        positions[3].z = 0.0;
        CImVector color = { 0xFF, 0xFF, 0xFF, 0xFF };
        GxPrimLockVertexPtrs(4, positions, 12, nullptr, 0, &color, 0, nullptr, 0, s_texCoord, 8, nullptr, 0);
        auto gxTex = TextureGetGxTex(textures[i], 1, nullptr);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
        GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_indices);
        GxPrimUnlockVertexPtrs();
    }
}

// OFFSET: 0x408E10
void PaintDynamicLoadingBar() {
    // if (dword_B302C4) {
    //     v0 = (dword_B302C4 - 8) / 4;
    //     v1 = 0;
    //     v17 = 0;
    //     if (v0 > 0) {
    //         v16 = v0 - 1;
    //         v2 = (char*)dword_B302C8 + 236;
    //         v3 = s_progress;
    //         do {
    //             v4 = (double)(int)v17;
    //             *(v2 - 6) = dword_AB63FC;
    //             *v2 = dword_AB63FC;
    //             v2[6] = dword_AB63FC;
    //             v2[12] = dword_AB63FC;
    //             if (v4 / (double)(v0 - 1) > v3)
    //                 break;
    //             ++v1;
    //             v2 += 24;
    //             v17 = (_BYTE*)v1;
    //         } while (v1 < v0);
    //     }
    //     v5 = 2 * (3 * v1 + 6);
    //     v16 = 4 * v1 + 8;
    //     v6 = alloca(4 * (3 * v1 + 6));
    //     v7 = v15;
    //     v17 = v15;
    //     if (v1 > 0) {
    //         v8 = 8;
    //         do {
    //             v7[1] = v8 + 1;
    //             v7[3] = v8 + 1;
    //             *v7 = v8;
    //             v7[2] = v8 + 2;
    //             v7[4] = v8 + 3;
    //             v7[5] = v8 + 2;
    //             v7 += 6;
    //             v8 += 4;
    //             --v1;
    //         } while (v1);
    //     }
    //     *v7 = 0;
    //     v7[1] = 1;
    //     v7[2] = 2;
    //     v7[3] = 1;
    //     v7[4] = 3;
    //     v7[5] = 2;
    //     v7[6] = 4;
    //     v7[7] = 5;
    //     v7[8] = 6;
    //     v7[9] = 5;
    //     v7[10] = 7;
    //     v7[11] = 6;
    //     GxTex = CTexture::GetGxTex(s_dynamicElementsTexture, 1, 0);
    //     CGxDevice::RsSet(g_theGxDevicePtr, 21, GxTex);
    //     v10 = g_theGxDevicePtr;
    //     if (g_theGxDevicePtr->ukn1[981]) {
    //         v11 = (_DWORD*)(g_theGxDevicePtr->ukn1[2620] + 144);
    //         if (*v11 != 2) {
    //             CGxDevice::IRsDirty(g_theGxDevicePtr, 6);
    //             *v11 = 2;
    //             v10 = g_theGxDevicePtr;
    //         }
    //         if (v10->ukn1[981]) {
    //             v12 = v10->ukn1[2620];
    //             v13 = dword_AD8B7C[*(_DWORD*)(v12 + 144)];
    //             v14 = &v10->ukn1[2620];
    //             if (*(_DWORD*)(v12 + 168) != v13) {
    //                 CGxDevice::IRsDirty(v10, 7);
    //                 *(_DWORD*)(*v14 + 168) = v13;
    //             }
    //         }
    //     }
    //     GxPrimLockVertexPtrs(
    //         v16,
    //         dword_B302C8,
    //         24,
    //         0,
    //         0,
    //         (int*)dword_B302C8 + 5,
    //         24,
    //         0,
    //         0,
    //         (float*)dword_B302C8 + 3,
    //         24,
    //         0,
    //         0);
    //     sub_682340(3, v5, (int)v17);
    //     NOP();
    // }
}

// OFFSET: 0x409800
bool IsStillLoading() {
    //ActivePlayer = ClntObjMgrGetActivePlayer();
    //v1 = ClntObjMgrObjectPtr(ActivePlayer, TYPEMASK_PLAYER);
    //if (!v1)
    //    return 1;
    //if (!((__int64(__thiscall*)(CGUnit_C*))v1->ObjectBase.GetTransportGUID)(v1))
    //    return 1;
    //v2 = ((__int64(__thiscall*)(CGUnit_C*))v1->ObjectBase.GetTransportGUID)(v1);
    //v3 = ClntObjMgrObjectPtr(v2, TYPEMASK_GAMEOBJECT);
    //if (v3) {
    //    v5 = (*(int(__thiscall**)(DWORD))(*(_DWORD*)v3->data0D4[51] + 112))(v3->data0D4[51]);
    //    if (!v5 || !sub_77FCD0(v5))
    //        return 1;
    //}
    //LoadingScreenDisable();
    //return 0;
    return true;
}

// OFFSET: 0x40AEF0
void LoadingScreenAsyncCallback(float progress, void* param) {
    if (progress > s_asyncProgress) {
        s_asyncProgress = progress;
        UpdateProgressBar(progress == 1);
    }
}

// OFFSET: 0x40AF90
void LoadingScreenXMLCallback(float progress, void* param) {
    if (progress > s_xmlProgress) {
        s_xmlProgress = progress;
        UpdateProgressBar(progress == 1);
    }
}

// OFFSET: 0x40AF40
void LoadingScreenWorldCallback(float progress, void* param) {
    if (progress > s_worldProgress) {
        s_worldProgress = progress;
        UpdateProgressBar(progress == 1);
    }
}
