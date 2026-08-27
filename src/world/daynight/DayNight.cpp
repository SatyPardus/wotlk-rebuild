#include "world/daynight/DayNight.hpp"
#include "world/daynight/DNInfo.hpp"
#include "world/daynight/DNStars.hpp"
#include "world/daynight/DNSky.hpp"
#include "gx/Transform.hpp"
#include "gx/RenderState.hpp"
#include "gx/Draw.hpp"
#include "storm/Error.hpp"
#include "db/Db.hpp"
#include "world/daynight/DNPlanets.hpp"
#include "console/DebugScreen.hpp"
#include <model/Model2.hpp>
#include <common/time/Time.hpp>
#include "world/daynight/LightQueue.hpp"
#include <util/Color.hpp>
#include <gx/Device.hpp>
#include <world/map/CMap.hpp>


namespace DayNight {

DNInfo g_dnInfo;
static DNStars g_stars;
static DNSky g_sky;
static DNPlanets g_planets;

uint32_t g_mapId;
TSGrowableArray<LightRec*> g_areaLights;

void sub_5FE800(float a1, float* a2, int32_t* a3) {
    if (a1 <= 0.0)
        *a3 = a1 - 1;
    else
        *a3 = a1;
    *a2 = a1 - *a3;
}

// OFFSET: 0x6ACC50
void InterpColor(CImVector* dst, int32_t t, const CImVector* src) {
    if (t == 255) {
        dst->b = src->b;
        dst->g = src->g;
        dst->r = src->r;
        return;
    }

    dst->b = dst->b + ((t * (src->b - dst->b)) >> 8);
    dst->g = dst->g + ((t * (src->g - dst->g)) >> 8);
    dst->r = dst->r + ((t * (src->r - dst->r)) >> 8);
}

// OFFSET: 0x7EAE70
int32_t FindBandSlot(const int32_t* times, int32_t count, int32_t now, float* outT, int32_t* outLo, int32_t* outHi) {
    *outHi = 0;
    *outT = 0.0f;
    *outLo = 0;

    if (count <= 0) {
        return 0;
    }

    while (1) {
        int32_t lo = times[*outLo];
        int32_t hiIndex = (*outLo + 1) % count;
        *outHi = hiIndex;
        int32_t hi = times[hiIndex];

        if (hi <= lo) {
            if (now > hi && now < lo) {
                if (++(*outLo) >= count) {
                    return hi;
                }
                continue;
            }

            hi += 2880;

            if (now < lo) {
                now += 2880;
            }

            int32_t span = hi - lo;
            *outT = (float)(now - lo) / (float)span;
            return span;
        }

        if (now >= lo && now <= hi) {
            int32_t span = hi - lo;
            *outT = (float)(now - lo) / (float)span;
            return span;
        }

        if (++(*outLo) >= count) {
            return hi;
        }
    }
}

// OFFSET: 0x7EB070
void InterpIntBand(const LightIntBandRec* rec, int32_t now, CImVector* out) {
    if (!rec->m_num) {
        out->b = 0;
        out->g = 0;
        out->r = 0;
        out->a = 0xFF;
        return;
    }

    float t;
    int32_t lo;
    int32_t hi;
    FindBandSlot(rec->m_time, rec->m_num, now, &t, &lo, &hi);

    const uint8_t* loColor = (const uint8_t*)&rec->m_data[lo];
    const uint8_t* hiColor = (const uint8_t*)&rec->m_data[hi];

    out->b = (int32_t)((float)(hiColor[0] - loColor[0]) * t + (float)loColor[0]);
    out->g = (int32_t)((float)(hiColor[1] - loColor[1]) * t + (float)loColor[1]);
    out->r = (int32_t)((float)(hiColor[2] - loColor[2]) * t + (float)loColor[2]);
    out->a = 0xFF;
}

// OFFSET: 0x7EAEF0
float InterpFloatBand(const LightFloatBandRec* rec, int32_t now, int32_t bandIndex) {
    if (!rec->m_num) {
        return 0.0f;
    }

    float t;
    int32_t lo;
    int32_t hi;
    FindBandSlot(rec->m_time, rec->m_num, now, &t, &lo, &hi);

    if (bandIndex) {
        return (rec->m_data[hi] - rec->m_data[lo]) * t + rec->m_data[lo];
    }

    float loValue = rec->m_data[lo] * 0.027777778f;
    return loValue + (0.027777778f * rec->m_data[hi] - loValue) * t;
}

// OFFSET: 0x7EBF30
void LoadLightIntBand(CImVector* out, int32_t bandTime, const LightParamsRec* params, int32_t bandIndex) {
    LightIntBandRec* rec = g_lightIntBandDB.GetRecord(bandIndex + 18 * params->m_ID - 17);

    if (!rec) {
        out->b = 0;
        out->g = 0;
        out->r = 0;
        out->a = 0xFF;
        return;
    }

    InterpIntBand(rec, bandTime, out);
}

// OFFSET: 0x7EBF90
float CalcIndividualColorFromDBRec(int32_t bandTime, const LightParamsRec* params, int32_t bandIndex) {
    LightFloatBandRec* rec = g_lightFloatBandDB.GetRecord(bandIndex + 6 * params->m_ID - 5);

    if (!rec) {
        return 0.0f;
    }

    return InterpFloatBand(rec, bandTime, bandIndex);
}

// OFFSET: 0x7EBFF0
void LoadLightingBands(int32_t bandTime, DNLightBands* bands, LightParamsRec* params) {
    LoadLightIntBand(&bands->m_diffuse, bandTime, params, 0);
    LoadLightIntBand(&bands->m_ambient, bandTime, params, 1);
    LoadLightIntBand(&bands->m_sky0, bandTime, params, 2);
    LoadLightIntBand(&bands->m_sky1, bandTime, params, 3);
    LoadLightIntBand(&bands->m_sky2, bandTime, params, 4);
    LoadLightIntBand(&bands->m_sky3, bandTime, params, 5);
    LoadLightIntBand(&bands->m_sky4, bandTime, params, 6);
    LoadLightIntBand(&bands->m_skyFog, bandTime, params, 7);
    LoadLightIntBand(&bands->m_shadowOpacity, bandTime, params, 8);
    LoadLightIntBand(&bands->m_sunColor, bandTime, params, 9);
    LoadLightIntBand(&bands->m_cloudColor0, bandTime, params, 10);
    LoadLightIntBand(&bands->m_cloudColor1, bandTime, params, 11);
    LoadLightIntBand(&bands->m_cloudColor2, bandTime, params, 12);
    LoadLightIntBand(&bands->m_band13, bandTime, params, 13);
    LoadLightIntBand(&bands->m_band14, bandTime, params, 14);
    LoadLightIntBand(&bands->m_band15, bandTime, params, 15);
    LoadLightIntBand(&bands->m_band16, bandTime, params, 16);
    LoadLightIntBand(&bands->m_band17, bandTime, params, 17);

    bands->m_fogEnd = CalcIndividualColorFromDBRec(bandTime, params, 0);

    bands->m_fogStartMul = CalcIndividualColorFromDBRec(bandTime, params, 1);

    if (bands->m_fogStartMul >= -1.0f) {
        if (bands->m_fogStartMul > 1.0f) {
            bands->m_fogStartMul = 1.0f;
        }
    } else {
        bands->m_fogStartMul = -1.0f;
    }

    bands->m_fogRate = 1.0f;

    bands->m_floatBand2 = CalcIndividualColorFromDBRec(bandTime, params, 2);
    bands->m_cloudDensity = CalcIndividualColorFromDBRec(bandTime, params, 3);
    bands->m_floatBand4 = CalcIndividualColorFromDBRec(bandTime, params, 4);
    bands->m_floatBand5 = CalcIndividualColorFromDBRec(bandTime, params, 5);

    bands->m_highlightSky = (float)params->m_highlightSky;
    bands->m_glow = params->m_glow;
    bands->m_waterShallowAlpha = params->m_waterShallowAlpha;
    bands->m_waterDeepAlpha = params->m_waterDeepAlpha;
    bands->m_oceanShallowAlpha = params->m_oceanShallowAlpha;
    bands->m_oceanDeepAlpha = params->m_oceanDeepAlpha;
    bands->m_skyboxId0 = params->m_lightSkyboxID;
    bands->m_skyboxWeight0 = 1.0f;
    bands->m_cloudTypeId = params->m_cloudTypeID;
    bands->m_cloudWeight = 1.0f;
}

// OFFSET: 0x7ECD00
static float CalcFogRate(float fogStart, float fogEnd) {
    float farClip = g_dnInfo.m_farClip;

    if (farClip >= 700.0f) {
        farClip = 700.0f;
    }

    float range = farClip - 200.0f;
    float span = fogEnd - fogStart;

    if (span <= range) {
        return (1.0f - span / range) * s_fogRateScale + 1.5f;
    }

    return 1.5f;
}

// OFFSET: 0x7ECD80
void CalcColors(int32_t bandTime, DNLightBands* bands, LightParamsRec* params) {
    LoadLightingBands(bandTime, bands, params);

    if (bands->m_fogEnd < 10.0f) {
        bands->m_fogEnd = 10.0f;
    }

    if (s_farClipFogMode == 1) {
        if (bands->m_fogEnd >= 27.777779f) {
            bands->m_fogRate = CalcFogRate(bands->m_fogStartMul * bands->m_fogEnd, bands->m_fogEnd);
            bands->m_fogEnd = g_dnInfo.m_farClip;
        }

        if (bands->m_fogStartMul < 0.0f) {
            bands->m_fogStartMul = 0.0f;
        }
    }
}

// OFFSET: 0x7EC220
void BlendColors(DNLightBands* dst, const DNLightBands* src, float t) {
    float clamped = t;

    if (clamped < 0.0f) {
        clamped = 0.0f;
    } else if (clamped > 1.0f) {
        clamped = 1.0f;
    }

    int32_t ct = (int32_t)(clamped * 255.0f);

    if (ct) {
        InterpColor(&dst->m_ambient, ct, &src->m_ambient);
        InterpColor(&dst->m_diffuse, ct, &src->m_diffuse);
        InterpColor(&dst->m_shadowOpacity, ct, &src->m_shadowOpacity);
        InterpColor(&dst->m_sky0, ct, &src->m_sky0);
        InterpColor(&dst->m_sky1, ct, &src->m_sky1);
        InterpColor(&dst->m_sky2, ct, &src->m_sky2);
        InterpColor(&dst->m_sky3, ct, &src->m_sky3);
        InterpColor(&dst->m_sky4, ct, &src->m_sky4);
        InterpColor(&dst->m_skyFog, ct, &src->m_skyFog);
        InterpColor(&dst->m_sunColor, ct, &src->m_sunColor);
        InterpColor(&dst->m_cloudColor0, ct, &src->m_cloudColor0);
        InterpColor(&dst->m_cloudColor1, ct, &src->m_cloudColor1);
        InterpColor(&dst->m_cloudColor2, ct, &src->m_cloudColor2);
        InterpColor(&dst->m_band13, ct, &src->m_band13);
        InterpColor(&dst->m_band14, ct, &src->m_band14);
        InterpColor(&dst->m_band15, ct, &src->m_band15);
        InterpColor(&dst->m_band16, ct, &src->m_band16);
        InterpColor(&dst->m_band17, ct, &src->m_band17);
    }

    dst->m_fogEnd = (src->m_fogEnd - dst->m_fogEnd) * clamped + dst->m_fogEnd;
    dst->m_fogStartMul = (src->m_fogStartMul - dst->m_fogStartMul) * clamped + dst->m_fogStartMul;
    dst->m_fogRate = (src->m_fogRate - dst->m_fogRate) * clamped + dst->m_fogRate;
    dst->m_glow = dst->m_glow + (src->m_glow - dst->m_glow) * clamped;
    dst->m_cloudDensity = dst->m_cloudDensity + (src->m_cloudDensity - dst->m_cloudDensity) * clamped;
    dst->m_waterShallowAlpha = dst->m_waterShallowAlpha + (src->m_waterShallowAlpha - dst->m_waterShallowAlpha) * clamped;
    dst->m_waterDeepAlpha = dst->m_waterDeepAlpha + (src->m_waterDeepAlpha - dst->m_waterDeepAlpha) * clamped;
    dst->m_oceanShallowAlpha = dst->m_oceanShallowAlpha + (src->m_oceanShallowAlpha - dst->m_oceanShallowAlpha) * clamped;
    dst->m_oceanDeepAlpha = clamped * (src->m_oceanDeepAlpha - dst->m_oceanDeepAlpha) + dst->m_oceanDeepAlpha;
}

// OFFSET: 0x7EB180
LightParamsRec* GetLightParams(const LightRec* light, int32_t slot) {
    return g_lightParamsDB.GetRecord(light->m_lightParamsID[slot]);
}

// OFFSET: 0x7EE750
void SetLightColors() {
    const CImVector ambient = g_dnInfo.m_bands.m_ambient;
    const CImVector diffuse = g_dnInfo.m_bands.m_diffuse;

    CImVector mid;
    mid.b = ambient.b + (((diffuse.b - ambient.b) * 128) >> 8);
    mid.g = ambient.g + (((diffuse.g - ambient.g) * 128) >> 8);
    mid.r = ambient.r + (((diffuse.r - ambient.r) * 128) >> 8);
    mid.a = 0;

    uint32_t packed = (uint32_t)mid.b | ((uint32_t)mid.g << 8) | ((uint32_t)mid.r << 16);
    uint32_t sub = packed - 0x00EFEFF0;
    uint32_t mask = ((packed ^ sub) ^ 0xFFFEFEFF) & 0x01010100;
    uint32_t mid1 = (sub - mask) | (mask - (mask >> 8));

    CImVector mid0;
    mid0.b = diffuse.b + (((ambient.b - diffuse.b) * 128) >> 8);
    mid0.g = diffuse.g + (((ambient.g - diffuse.g) * 128) >> 8);
    mid0.r = diffuse.r + (((ambient.r - diffuse.r) * 128) >> 8);
    mid0.a = 0;

    g_dnInfo.m_light0.m_dir = g_dnInfo.m_light1.m_dir;
    g_dnInfo.m_light0.m_diffuse = diffuse;
    g_dnInfo.m_light0.m_ambient = ambient;
    g_dnInfo.m_light0.m_mid0 = mid0;
    g_dnInfo.m_light0.m_mid1 = *(const CImVector*)&mid1;

    g_dnInfo.m_light1.m_diffuse = diffuse;
    g_dnInfo.m_light1.m_ambient = ambient;
    g_dnInfo.m_light1.m_mid0 = mid0;
    g_dnInfo.m_light1.m_mid1 = *(const CImVector*)&mid1;

    g_dnInfo.m_shadowColor.b = (uint8_t)((85 * (ambient.b + 3)) >> 8);
    g_dnInfo.m_shadowColor.g = (uint8_t)((85 * (ambient.g + 3)) >> 8);
    g_dnInfo.m_shadowColor.r = (uint8_t)((85 * (ambient.r + 3)) >> 8);
    g_dnInfo.m_shadowColor.a = g_dnInfo.m_bands.m_shadowOpacity.r;

    //v8 = DayNight::g_dnInfo.m_bands.m_ambient.g;
    //DayNight::g_dnInfo.m_light0.m_mid1 = DayNight::g_dnInfo.m_light1.m_mid1;
    //v2 = DayNight::g_dnInfo.m_bands.m_ambient.r * 0.0039215689;
    //v3 = DayNight::g_dnInfo.m_bands.m_ambient.g * 0.0039215689;
    //v4 = 0.0039215689 * DayNight::g_dnInfo.m_bands.m_ambient.b;
    //v5 = 0.0;
    //v6 = 0.0;
    //v7 = 0.0;
    //RGBtoHSV(&v2, &v5);
    //v6 = v6 * 0.33000001;
    //v7 = v7 * 1.25;
    //HSVtoRGB(&v5, &v2);
    //DayNight::g_dnInfo.m_light1.m_desatR = v2;
    //DayNight::g_dnInfo.m_light1.m_desatG = v3;
    //DayNight::g_dnInfo.m_light1.m_desatB = v4;
    //DayNight::g_dnInfo.m_light1.m_desatA = 1.0;
}

// OFFSET: 0x7F30C0
DNOverrideSky* GetOverrideSky(const char* path, uint32_t flags) {
    if (!path || !*path) {
        return nullptr;
    }

    DNOverrideSky* sky = s_overrideSky.Ptr(path);

    if (sky) {
        return sky;
    }

    sky = s_overrideSky.New(path, 0, 0);

    if (!s_skyScene) {
        s_skyScene = M2CreateScene();
        s_skySceneTimeMs = (uint32_t)OsGetAsyncTimeMs();
    }

    sky->m_model = s_skyScene->CreateModel(path, 0);
    sky->m_flags = flags;

    return sky;
}

float InterpTable(const C2Vector* table, uint32_t size, float key) {
    STORM_ASSERT(size);

    uint32_t i = 0;
    uint32_t j = 0;

    for (i = 0; i < size; ++i) {
        if (key <= table[i].x) {
            break;
        }
    }

    if (i == size) {
        i = 0;
        j = size - 1;
    } else if (i > 0) {
        j = i - 1;
    } else {
        j = size - 1;
    }

    float v5 = table[i].x - table[j].x;
    if (v5 < 0.0) {
        v5 = v5 + 1.0;
    }

    float v6 = key - table[j].x;
    if (v6 < 0.0) {
        v6 = v6 + 1.0;
    }

    float v7 = v6 / v5;

    if (table[i].y < table[j].y) {
        return table[j].y - v7 * (table[j].y - table[i].y);
    } else {
        return table[j].y + v7 * (table[i].y - table[j].y);
    }
}

// OFFSET: 0x7F2790
void LoadMap(int32_t zoneID) {
    g_mapId = zoneID;
    LightLoad(&g_areaLights, zoneID);

    DayNight::g_dnInfo.m_dayProgression = 0.0;
    DayNight::g_dnInfo.m_day = 0.0;
    DayNight::g_dnInfo.m_timeOfDay = 0;
    DayNight::g_dnInfo.m_playerPos.x = 0.0;
    DayNight::g_dnInfo.m_playerPos.y = 0.0;
    DayNight::g_dnInfo.m_playerPos.z = 0.0;
    DayNight::g_dnInfo.m_cameraPos.x = 0.0;
    DayNight::g_dnInfo.m_cameraPos.y = 0.0;
    DayNight::g_dnInfo.m_cameraPos.z = 0.0;
    DayNight::g_dnInfo.m_faceAngle = 0.0;
    DayNight::g_dnInfo.m_lightRefPos.x = 0.0;
    DayNight::g_dnInfo.m_farClip = 0.0;
    DayNight::g_dnInfo.m_timeSec = 0.0;
    DayNight::g_dnInfo.m_lightRefPos.y = 0.0;
    DayNight::g_dnInfo.m_deltaSec = 0.0;
    DayNight::g_dnInfo.m_lightRefPos.z = 0.0;
    DayNight::g_dnInfo.m_weatherIntensity = 0.0;
    DayNight::g_dnInfo.m_skyboxWeight = 0.0;
    DayNight::g_dnInfo.m_cameraDir.x = 0.0;
    DayNight::g_dnInfo.m_cameraDir.y = 0.0;
    DayNight::g_dnInfo.m_cameraDir.z = 0.0;
    DayNight::g_dnInfo.m_flashBlend = 0;
    *DayNight::g_dnInfo.m_flashColorBGRA = 0;
    DayNight::g_dnInfo.m_overrideLightParamsId = -1;
    DayNight::g_dnInfo.m_skybox = nullptr;
    for (int32_t i = 0; i < 3; i++) {
        DayNight::g_dnInfo.m_blendSkyboxWeight[i] = 0.0f;
        DayNight::g_dnInfo.m_blendSkybox[i] = nullptr;
        DayNight::g_dnInfo.m_blendSkyboxFlags[i] = 0;
    }
    DayNight::g_dnInfo.m_interiorFogBlend = 0.0;
    DayNight::g_dnInfo.m_interiorFogId = 0;
    DayNight::g_dnInfo.m_interiorFogIds.m_count = 0;

    g_sky.GenSphere(1.0f);

    DayNight::g_planets.sun.Initialize("Textures\\sunCenter.blp");
    DayNight::g_planets.sun.m_baseScale = 1.0f;
    DayNight::g_planets.sun.m_period = 1.0f;

    DayNight::g_planets.moon.Initialize("Textures\\moon.blp");
    DayNight::g_planets.moon.m_baseScale = 1.75f;
    DayNight::g_planets.moon.m_period = 1.0f;

    DayNight::g_planets.moon2.Initialize("Textures\\moon02.blp");
    DayNight::g_planets.moon2.m_baseScale = 1.0f;
    DayNight::g_planets.moon2.m_period = 1.7f;

    g_stars.Initialize();
    g_dnInfo.m_showSky = 1;
}

// OFFSET: 0x7ECB30
void LightLoad(TSGrowableArray<LightRec*>* lightArray, uint32_t a2) {
    uint32_t lightCount = 1;

    LightRec* lightRec;
    for (uint32_t i = 0; i < g_lightDB.GetNumRecords(); i++) {
        lightRec = g_lightDB.GetRecordByIndex(i);
        if (lightRec->m_continentID == a2 && (lightRec->m_gameCoords[0] != 0.0f || lightRec->m_gameCoords[1] != 0.0f || lightRec->m_gameCoords[2] != 0.0f))
            lightCount++;
    }

    lightArray->SetCount(lightCount);

    bool haveDefault = false;
    uint32_t next = 1;

    for (int32_t i = 0; i < g_lightDB.GetNumRecords(); i++) {
        lightRec = g_lightDB.GetRecordByIndex(i);

        if (lightRec->m_continentID != a2) {
            continue;
        }

        if (lightRec->m_gameCoords[0] == 0.0f && lightRec->m_gameCoords[1] == 0.0f && lightRec->m_gameCoords[2] == 0.0f) {
            haveDefault = true;

            lightArray->m_data[0] = lightRec;
        } else {
            lightArray->m_data[next++] = lightRec;
        }
    }

    if (!haveDefault) {
        lightArray->m_data[0] = g_lightDB.GetRecord(1);
    }
}

// OFFSET: 0x7ED2D0
void ColorLerpBytes(CImVector* out, const CImVector* a, const CImVector* b, float t) {
    out->b = (uint8_t)(int32_t)(b->b < a->b ? a->b - (a->b - b->b) * t : a->b + (b->b - a->b) * t);
    out->g = (uint8_t)(int32_t)(b->g < a->g ? a->g - (a->g - b->g) * t : a->g + (b->g - a->g) * t);
    out->r = (uint8_t)(int32_t)(b->r < a->r ? a->r - (a->r - b->r) * t : a->r + (b->r - a->r) * t);
    out->a = 0xFF;
}

static float LerpToward(float dst, float src, float t) {
    return src < dst ? dst - (dst - src) * t : dst + (src - dst) * t;
}

// OFFSET: 0x7ED4C0
void BlendOutputs(DNLightBands* dst, const DNLightBands* src, float t) {
    CImVector* dstColors = &dst->m_ambient;
    const CImVector* srcColors = &src->m_ambient;

    for (uint32_t i = 0; i < 18; ++i) {
        ColorLerpBytes(&dstColors[i], &dstColors[i], &srcColors[i], t);
    }

    dst->m_fogEnd = LerpToward(dst->m_fogEnd, src->m_fogEnd, t);
    dst->m_fogStartMul = LerpToward(dst->m_fogStartMul, src->m_fogStartMul, t);
    dst->m_fogRate = LerpToward(dst->m_fogRate, src->m_fogRate, t);
    dst->m_highlightSky = LerpToward(dst->m_highlightSky, src->m_highlightSky, t);
    dst->m_cloudDensity = LerpToward(dst->m_cloudDensity, src->m_cloudDensity, t);
    dst->m_glow = LerpToward(dst->m_glow, src->m_glow, t);
    dst->m_waterShallowAlpha = LerpToward(dst->m_waterShallowAlpha, src->m_waterShallowAlpha, t);
    dst->m_waterDeepAlpha = LerpToward(dst->m_waterDeepAlpha, src->m_waterDeepAlpha, t);
    dst->m_oceanShallowAlpha = LerpToward(dst->m_oceanShallowAlpha, src->m_oceanShallowAlpha, t);
    dst->m_oceanDeepAlpha = LerpToward(dst->m_oceanDeepAlpha, src->m_oceanDeepAlpha, t);

    const uint32_t skyboxId = src->m_skyboxId0;

    if (skyboxId && t > 0.0f) {
        uint32_t* ids = &dst->m_skyboxId0;
        float* weights = &dst->m_skyboxWeight0;
        uint32_t slot = 0;

        while (ids[slot * 2] != skyboxId) {
            if (!ids[slot * 2]) {
                weights[slot * 2] = t;
                ids[slot * 2] = skyboxId;
                dst->m_cloudTypeId = src->m_cloudTypeId;
                dst->m_cloudWeight = t;
                return;
            }

            if (++slot >= 3) {
                dst->m_cloudTypeId = src->m_cloudTypeId;
                dst->m_cloudWeight = t;
                return;
            }
        }

        weights[slot * 2] += t;

        if (weights[slot * 2] > 1.0f) {
            weights[slot * 2] = 1.0f;
        }
    }

    dst->m_cloudTypeId = src->m_cloudTypeId;
    dst->m_cloudWeight = t;
}

// OFFSET: 0x7EE510
void BlendAreaLightColors(DNLightBands* dst, const LightRec* light, int32_t clearSlot, int32_t stormSlot) {
    const int32_t bandTime = (int32_t)floorf(g_dnInfo.m_dayProgression * 2880.0f);

    LightParamsRec* params = GetLightParams(light, clearSlot);
    CalcColors(bandTime, dst, params);

    if (g_dnInfo.m_stormBlend > 0.0f) {
        DNLightBands stormBands;

        LightParamsRec* stormParams = GetLightParams(light, stormSlot);
        CalcColors(bandTime, &stormBands, stormParams);

        BlendColors(dst, &stormBands, g_dnInfo.m_stormBlend);
    }
}

// OFFSET: 0x7EE5D0
void BlendAreaLight(DNLightBands* dst, float maxFade, const LightRec* light, int32_t clearSlot, int32_t stormSlot) {
    DNLightBands bands;
    BlendAreaLightColors(&bands, light, clearSlot, stormSlot);

    const float dx = g_dnInfo.m_lightRefPos.x - light->m_gameCoords[0];
    const float dy = g_dnInfo.m_lightRefPos.y - light->m_gameCoords[1];
    const float dz = g_dnInfo.m_lightRefPos.z - light->m_gameCoords[2];
    const float dist = sqrt(dx * dx + dy * dy + dz * dz);

    float fade = 1.0f;

    if (light->m_gameFalloffStart < dist) {
        fade = 1.0f - (dist - light->m_gameFalloffStart) / (light->m_gameFalloffEnd - light->m_gameFalloffStart);

        if (fade < 0.0f) {
            fade = 0.0f;
        }
    }

    BlendOutputs(dst, &bands, maxFade <= fade ? maxFade : fade);
}

// OFFSET: 0x7EE6B0
void BlendZoneLight(DNLightBands* dst, float maxFade, const LightRec* light, float distPct, int32_t clearSlot, int32_t stormSlot) {
    DNLightBands bands;
    BlendAreaLightColors(&bands, light, clearSlot, stormSlot);

    const float remaining = (100.0f - distPct) * 0.0099999998f;

    float fade;

    if (remaining <= 0.0f) {
        fade = 1.0f;
    } else {
        fade = 1.0f - remaining;

        if (fade < 0.0f) {
            fade = 0.0f;
        }
    }

    BlendOutputs(dst, &bands, maxFade <= fade ? maxFade : fade);
}

// OFFSET : 0x7F1360
void DoAreaLights(DNLightBands* light, int32_t underwater) {
    const int32_t clearSlot = underwater != 0;
    const int32_t stormSlot = clearSlot + 2;

    LightQueue queue(32, 32);

    if (g_dnInfo.m_nearLightCount) {
        if (s_useZoneLights) {
            for (uint32_t i = 0; i < g_dnInfo.m_nearLightCount; ++i) {
                BlendZoneLight(light, 1.0f, g_dnInfo.m_nearLight[i], g_dnInfo.m_nearLightBlend[i], clearSlot, stormSlot);
            }
        }

        // DNInfo::ClearZoneLights, 0x7ECCB0
        for (uint32_t i = 0; i < 5; ++i) {
            g_dnInfo.m_nearLight[i] = nullptr;
            g_dnInfo.m_nearLightBlend[i] = 0.0f;
        }

        g_dnInfo.m_nearLightCount = 0;
    }

    if (s_useAreaLights) {
        for (uint32_t i = 1; i < g_areaLights.Count(); ++i) {
            LightRec** entry = &g_areaLights.m_data[i];

            if (s_useZoneLights && (*entry)->m_gameFalloffEnd < 3.0f) {
                continue;
            }

            const float dx = g_dnInfo.m_lightRefPos.x - (*entry)->m_gameCoords[0];
            const float dy = g_dnInfo.m_lightRefPos.y - (*entry)->m_gameCoords[1];
            const float dz = g_dnInfo.m_lightRefPos.z - (*entry)->m_gameCoords[2];
            const float distSq = dx * dx + dy * dy + dz * dz;

            if ((*entry)->m_gameFalloffEnd * (*entry)->m_gameFalloffEnd > distSq) {
                queue.Insert(distSq, entry);
            }
        }
    }

    while (queue.m_count > 1) {
        LightQE entry;
        queue.Pop(&entry);

        BlendAreaLight(light, 1.0f, *entry.m_light, clearSlot, stormSlot);
    }

    uint32_t count = s_areaLightOverrides.Count();
    uint32_t i = 0;

    while (i < count) {
        AreaLightOverride* over = &s_areaLightOverrides[i];

        const uint32_t now = (uint32_t)OsGetAsyncTimeMs();
        const uint32_t elapsed = now - over->m_lastTimeMs;
        over->m_lastTimeMs = now;

        if (over->m_state == 1) {
            over->m_blend = (float)elapsed / over->m_rateMs + over->m_blend;

            if (over->m_blend > 1.0f) {
                over->m_blend = 1.0f;
            }
        } else if (over->m_state == 2) {
            over->m_blend = over->m_blend - (float)elapsed / over->m_rateMs;

            if (over->m_blend < 0.0f) {
                over->m_blend = 0.0f;
            }
        }

        if (over->m_blend > 0.0f) {
            const LightRec* rec = over->m_light;

            const float dx = g_dnInfo.m_lightRefPos.x - rec->m_gameCoords[0];
            const float dy = g_dnInfo.m_lightRefPos.y - rec->m_gameCoords[1];
            const float dz = g_dnInfo.m_lightRefPos.z - rec->m_gameCoords[2];

            if (rec->m_gameFalloffEnd * rec->m_gameFalloffEnd > dx * dx + dy * dy + dz * dz) {
                BlendAreaLight(light, over->m_blend, rec, clearSlot, stormSlot);
            }
        }

        if (over->m_state == 2 && over->m_blend == 0.0f) {
            // CDataAllocator<LightRec>::PutData, pool at 0x00D38D7C
            delete over->m_light;

            --count;
            s_areaLightOverrides[i] = s_areaLightOverrides[count];
            s_areaLightOverrides.SetCount(count);

            continue;
        }

        ++i;
    }
}

// OFFSET: 0x7F3230
void SetColors() {
    //v33 = 0.0;
    //v0 = COERCE_FLOAT(maybe_GetCameraUnderwaterDepth(&v33));
    //m_maxDarkenDepth = v0;
    //if (v0 == 0.0 || SLODWORD(v0) < g_LiquidTypeDB.minIndex || SLODWORD(v0) > g_LiquidTypeDB.maxIndex)
    //    v1 = 0;
    //else
    //    v1 = g_LiquidTypeDB.Rows[LODWORD(v0) - g_LiquidTypeDB.minIndex];
    //v2 = 0;
    //if (v1)
    //    LOBYTE(v2) = LODWORD(v1->m_fogDarkenIntensity) != 0;
    const int32_t liquidType = 0;
    const bool liquidHasLight = false;

    g_dnInfo.m_skybox = nullptr;
    g_dnInfo.m_skyboxWeight = 0.0f;
    g_dnInfo.m_blendSkybox[0] = nullptr;
    g_dnInfo.m_blendSkybox[1] = nullptr;
    g_dnInfo.m_blendSkybox[2] = nullptr;
    g_dnInfo.m_blendSkyboxWeight[0] = 0.0f;
    g_dnInfo.m_blendSkyboxWeight[1] = 0.0f;
    g_dnInfo.m_blendSkyboxWeight[2] = 0.0f;
    g_dnInfo.m_blendSkyboxFlags[0] = 0;
    g_dnInfo.m_blendSkyboxFlags[1] = 0;
    g_dnInfo.m_blendSkyboxFlags[2] = 0;

    if (g_areaLights.Count()) {
        if (!liquidType || !liquidHasLight) {
            DNLightBands bands;

            const int32_t bandTime = (int32_t)floorf(g_dnInfo.m_dayProgression * 2880.0f);

            LightRec* defaultLight = g_areaLights[0];
            LightParamsRec* params = GetLightParams(defaultLight, liquidType != 0);
            CalcColors(bandTime, &bands, params);

            if (g_dnInfo.m_stormBlend > 0.0f) {
                DNLightBands stormBands;

                LightParamsRec* stormParams = GetLightParams(defaultLight, (liquidType != 0) + 2);
                CalcColors(bandTime, &stormBands, stormParams);

                BlendColors(&bands, &stormBands, g_dnInfo.m_stormBlend);
            }

            DoAreaLights(&bands, liquidType);

            LightParamsRec* overrideParams = nullptr;

            if (g_dnInfo.m_overrideLightParamsId != -1) {
                overrideParams = GetLightParams(defaultLight, g_dnInfo.m_overrideLightParamsId);
            }

            if (!overrideParams) {
                g_dnInfo.m_bands.Copy(&bands);
            } else {
                CalcColors(bandTime, &g_dnInfo.m_bands, overrideParams);

                g_dnInfo.m_bands.m_glow = bands.m_glow;
                g_dnInfo.m_bands.m_waterShallowAlpha = bands.m_waterShallowAlpha;
                g_dnInfo.m_bands.m_waterDeepAlpha = bands.m_waterDeepAlpha;
                g_dnInfo.m_bands.m_oceanShallowAlpha = bands.m_oceanShallowAlpha;
                g_dnInfo.m_bands.m_oceanDeepAlpha = bands.m_oceanDeepAlpha;
                g_dnInfo.m_bands.m_cloudTypeId = bands.m_cloudTypeId;
                g_dnInfo.m_bands.m_cloudWeight = bands.m_cloudWeight;
                g_dnInfo.m_bands.m_skyboxId0 = bands.m_skyboxId0;
                g_dnInfo.m_bands.m_skyboxWeight0 = bands.m_skyboxWeight0;
                g_dnInfo.m_bands.m_skyboxId1 = bands.m_skyboxId1;
                g_dnInfo.m_bands.m_skyboxWeight1 = bands.m_skyboxWeight1;
                g_dnInfo.m_bands.m_skyboxId2 = bands.m_skyboxId2;
                g_dnInfo.m_bands.m_skyboxWeight2 = bands.m_skyboxWeight2;

                LightSkyboxRec* skyboxRec = g_lightSkyboxDB.GetRecord(overrideParams->m_lightSkyboxID);

                if (skyboxRec) {
                    g_dnInfo.m_skybox = GetOverrideSky(skyboxRec->m_name, skyboxRec->m_flags);
                    g_dnInfo.m_skyboxWeight = 1.0f;
                }
            }
        } else {
            //if (!v1)
            //    goto LABEL_14;
            //*&m_spellID = DayNight::g_dnInfo.m_dayProgression * 2880.0;
            //v34 = (*&m_spellID - flt_AF4B78);
            //DayNight::CalcColors(v34);
        }
    } else {
        memset(&g_dnInfo.m_bands, 255, sizeof(g_dnInfo.m_bands));
        g_dnInfo.m_bands.m_ambient = { 0x40, 0x40, 0x40, 0xFF };
        g_dnInfo.m_bands.m_fogEnd = 1.0e10f;
        g_dnInfo.m_bands.m_fogStartMul = 0.5f;
        g_dnInfo.m_bands.m_fogRate = 4.0f;
        g_dnInfo.m_bands.m_highlightSky = 0.0f;
        g_dnInfo.m_bands.m_glow = 0.5f;
        g_dnInfo.m_bands.m_floatBand2 = 0.0f;
        g_dnInfo.m_bands.m_cloudDensity = 0.0f;
        g_dnInfo.m_bands.m_floatBand4 = 0.0f;
        g_dnInfo.m_bands.m_floatBand5 = 0.0f;
        g_dnInfo.m_bands.m_waterShallowAlpha = 0.5f;
        g_dnInfo.m_bands.m_waterDeepAlpha = 1.0f;
        g_dnInfo.m_bands.m_oceanShallowAlpha = 0.75f;
        g_dnInfo.m_bands.m_oceanDeepAlpha = 1.0f;
        g_dnInfo.m_bands.m_skyboxId0 = 0;
        g_dnInfo.m_bands.m_skyboxId1 = 0;
        g_dnInfo.m_bands.m_skyboxId2 = 0;
        g_dnInfo.m_bands.m_cloudTypeId = 0;
    }

    SetLightColors();

    //if (v0 != 0.0) {
    //    if (v1) {
    //        v3 = 0.0;
    //        if (*&v1->m_soundBank > 0.0) {
    //            v4 = -*&v1->m_soundBank;
    //            if (v33 <= 0.0)
    //                v5 = v33;
    //            else
    //                v5 = 0.0;
    //            if (v5 <= v4) {
    //                v3 = v4;
    //            } else if (v33 <= 0.0) {
    //                v3 = v33;
    //            }
    //            v13 = *&v1->m_soundID;
    //            m_spellID = v1->m_spellID;
    //            m_maxDarkenDepth = v1->m_maxDarkenDepth;
    //            v14 = v13;
    //            v15 = 1.0 - v3 / v4 - 1.0;
    //            *&v34 = v15;
    //            a1 = v14 * v15 + 1.0;
    //            DayNight::g_dnInfo.m_fog.color = *DayNight::DarkenColor(*&DayNight::g_dnInfo.m_fog.color, a1);
    //            a1a = *&v34 * *&m_spellID + 1.0;
    //            DayNight::g_dnInfo.m_light1.m_ambient = *DayNight::DarkenColor(*&DayNight::g_dnInfo.m_light1.m_ambient, a1a);
    //            a1b = *&v34 * m_maxDarkenDepth + 1.0;
    //            DayNight::g_dnInfo.m_light1.m_diffuse = *DayNight::DarkenColor(*&DayNight::g_dnInfo.m_light1.m_diffuse, a1b);
    //        }
    //    }
    //}

    g_sky.SetColors();

    g_planets.sun.m_color = g_dnInfo.m_bands.m_sunColor;
    g_planets.moon.m_color = g_dnInfo.m_bands.m_sunColor;

    // g_sunGlare.m_color = g_dnInfo.m_bands.m_sunColor;
    // g_moonGlare.m_color = g_dnInfo.m_bands.m_sunColor;

    if (g_dnInfo.m_flashBlend) {
        const CImVector* flash = (const CImVector*)g_dnInfo.m_flashColorBGRA;

        InterpColor(&g_dnInfo.m_fog.color, g_dnInfo.m_flashBlend, flash);
        InterpColor(&g_dnInfo.m_light1.m_ambient, g_dnInfo.m_flashBlend, flash);
        InterpColor(&g_dnInfo.m_light1.m_diffuse, g_dnInfo.m_flashBlend, flash);
        InterpColor(&g_planets.sun.m_color, g_dnInfo.m_flashBlend, flash);
        InterpColor(&g_planets.moon.m_color, g_dnInfo.m_flashBlend, flash);
    }

    if (g_dnInfo.m_stormBlend != 0.0f) {
        const uint8_t alpha = (uint8_t)(int32_t)((1.0f - g_dnInfo.m_stormBlend) * 255.0f);

        g_planets.sun.m_color.a = alpha;
        g_planets.moon.m_color.a = alpha;
        g_planets.moon2.m_color.a = alpha;

        // g_sunGlare.m_color.a = alpha;
        // g_moonGlare.m_color.a = alpha;
    }

    const uint32_t skyboxIds[3] = {
        g_dnInfo.m_bands.m_skyboxId0,
        g_dnInfo.m_bands.m_skyboxId1,
        g_dnInfo.m_bands.m_skyboxId2,
    };

    const float skyboxWeights[3] = {
        g_dnInfo.m_bands.m_skyboxWeight0,
        g_dnInfo.m_bands.m_skyboxWeight1,
        g_dnInfo.m_bands.m_skyboxWeight2,
    };

    uint32_t out = 0;

    for (uint32_t i = 0; i < 3 && out < 3; ++i) {
        if (!skyboxIds[i] || skyboxWeights[i] <= 0.0f) {
            continue;
        }

        LightSkyboxRec* rec = g_lightSkyboxDB.GetRecord(skyboxIds[i]);

        if (!rec) {
            continue;
        }

        DNOverrideSky* sky = GetOverrideSky(rec->m_name, rec->m_flags);
        const uint32_t additive = rec->m_flags & 2;

        if (skyboxWeights[i] > 0.99000001f && out && !additive) {
            out = 0;
        }

        g_dnInfo.m_blendSkybox[out] = sky;
        g_dnInfo.m_blendSkyboxWeight[out] = skyboxWeights[i];
        g_dnInfo.m_blendSkyboxFlags[out] = additive;
        ++out;
    }

    if (out < 3) {
        g_dnInfo.m_blendSkyboxWeight[out] = 0.0f;
    }

    g_dnInfo.m_curve0 = InterpTable(s_curve0Table, 4, g_dnInfo.m_dayProgression);
    g_dnInfo.m_curve1 = InterpTable(s_curve1Table, 2, g_dnInfo.m_dayProgression);
}

// OFFSET: 0x7F16F0
void UpdateFog() {
    float fogEnd = g_dnInfo.m_farClip;
    float fogRate;

    if (s_fogOverrideActive) {
        if (s_overrideFogEnd < fogEnd) {
            fogEnd = s_overrideFogEnd;
        }

        g_dnInfo.m_fog.end = fogEnd;
        g_dnInfo.m_fog.color = s_overrideFogColor;
        g_dnInfo.m_fog.start = fogEnd * s_overrideFogStartMul;
        fogRate = s_overrideFogRate;
    } else {
        if (g_dnInfo.m_bands.m_fogEnd < fogEnd) {
            fogEnd = g_dnInfo.m_bands.m_fogEnd;
        }

        g_dnInfo.m_fog.end = fogEnd;
        g_dnInfo.m_fog.color = g_dnInfo.m_bands.m_skyFog;
        g_dnInfo.m_fog.start = fogEnd * g_dnInfo.m_bands.m_fogStartMul;
        fogRate = g_dnInfo.m_bands.m_fogRate;
    }

    g_dnInfo.m_fog.m_density = fogRate;

    float interiorDistance = 0.0f;
    bool inInterior = false;
    int32_t liquidType = 0;

    // TODO: the WMO interior fog query and the underwater liquid lookup.
    //
    //   SMOFog fogRecords[2] = {};
    //   uint32_t fogId;
    //   bool hasFog;
    //   TSFixedArray<uint32_t>* fogIds;
    //   inInterior = FindViewerInteriorGroupForFog(fogRecords, &fogId, &hasFog,
    //                                              &fogIds, &interiorDistance) == 1;
    //   if (!inInterior) interiorDistance = 0.0f;
    //   liquidType = GetCameraUnderwaterDepth(nullptr);
    //
    //   if (inInterior) {
    //       LiquidTypeRec* liquid = g_liquidTypeDB.GetRecord(liquidType);
    //       int32_t useUnderwaterRecord = 0;
    //       if (liquidType) {
    //           useUnderwaterRecord = 1;
    //           if ((liquid->m_flags & 0x20) && !(fogRecords[0].flags & 0x100))
    //               useUnderwaterRecord = 0;
    //           if ((liquid->m_flags & 0x100) && !(fogRecords[0].flags & 0x10))
    //               useUnderwaterRecord = 0;   // falls through to the mirror below
    //       }
    //       if (!liquidType || useUnderwaterRecord) {
    //           ApplyFogSettings(&fogRecords[useUnderwaterRecord != 0]);
    //           if (liquid && (liquid->m_flags & 0x40)) {
    //               g_dnInfo.m_fog = g_dnInfo.m_fogGroup;
    //           }
    //       } else {
    //           g_dnInfo.m_fogGroup = g_dnInfo.m_fog;
    //       }
    //       if (hasFog) {
    //           g_dnInfo.m_interiorFogId = fogId;
    //           if (fogIds != &g_dnInfo.m_interiorFogIds)
    //               g_dnInfo.m_interiorFogIds.Set(fogIds->m_count, fogIds->m_data);
    //           g_dnInfo.m_interiorFogFlags = fogIds[1].m_alloc;
    //       }
    //   }

    float density = g_dnInfo.m_fog.m_density;

    float blend = interiorDistance * 0.039999999f;

    if (blend >= 0.0f) {
        if (blend >= 1.0f) {
            blend = 1.0f;
        }
    } else {
        blend = 0.0f;
    }

    g_dnInfo.m_interiorFogBlend = blend;
    g_dnInfo.m_interiorFogId = 0;
    g_dnInfo.m_interiorFogIds.m_count = 0;

    if (inInterior) {
        g_dnInfo.m_fogInterior.end = g_dnInfo.m_fog.end + (g_dnInfo.m_fogGroup.end - g_dnInfo.m_fog.end) * blend;
        g_dnInfo.m_fogInterior.start = g_dnInfo.m_fog.start + (g_dnInfo.m_fogGroup.start - g_dnInfo.m_fog.start) * blend;
        g_dnInfo.m_fogInterior.m_density = density + (g_dnInfo.m_fogGroup.m_density - density) * blend;

        CImVector blended = g_dnInfo.m_fog.color;
        const int32_t t = (int32_t)(blend * 255.0f);

        if (t) {
            InterpColor(&blended, t, &g_dnInfo.m_fogGroup.color);
        }

        g_dnInfo.m_fog.end = g_dnInfo.m_fogInterior.end;
        g_dnInfo.m_fogInterior.color = blended;
        g_dnInfo.m_fog.start = g_dnInfo.m_fogInterior.start;
        g_dnInfo.m_fog.m_density = g_dnInfo.m_fogInterior.m_density;
        density = g_dnInfo.m_fogInterior.m_density;
    } else {
        g_dnInfo.m_fogInterior.color = g_dnInfo.m_fog.color;
        g_dnInfo.m_fogInterior.start = g_dnInfo.m_fog.start;
        g_dnInfo.m_fogInterior.end = g_dnInfo.m_fog.end;
        g_dnInfo.m_fogInterior.m_density = density;
    }

    if (s_farClipFogMode == 1 && liquidType) {
        g_dnInfo.m_fog.m_density = density * 2.0f;
        g_dnInfo.m_fogInterior.m_density = density * 2.0f;
    }

    if (s_lightFlags & 2) {
        // ApplyFogColorBlend2();
        // ApplyFogColorBlend2();
    }

    if (s_lightFlags & 1) {
        if (g_dnInfo.m_timeSec <= s_glowEndTime) {
            float fade = 1.0f / exp2f((g_dnInfo.m_timeSec - s_glowStartTime) / s_glowFalloff * 7.2134752f);

            if (fade >= 0.0f) {
                if (fade >= 1.0f) {
                    fade = 1.0f;
                }
            } else {
                fade = 0.0f;
            }

            s_glowBlend = 1.0f - fade;

            // ApplyFogColorBlend(&g_dnInfo.m_fog);
            // ApplyFogColorBlend(&g_dnInfo.m_fogInterior);
        } else {
            s_lightFlags &= ~1u;
        }
    }
}

// OFFSET: 0x7EEA90
void SetDirection() {
    static C2Vector s_dirPolar[4] = { { 0.0f, 2.2165682f }, { 0.25f, 1.9198623f }, { 0.5f, 2.2165682f }, { 0.75f, 1.9198623f } };
    static C2Vector s_dirAzimuth[4] = { { 0.0f, 3.926991f }, { 0.25f, 3.926991f }, { 0.5f, 3.926991f }, { 0.75f, 3.926991f } };

    float polar = InterpTable(s_dirPolar, 4, g_dnInfo.m_dayProgression);
    float azimuth = InterpTable(s_dirAzimuth, 4, g_dnInfo.m_dayProgression);

    float polarTurns = polar * 0.31830987f;
    float azimuthTurns = azimuth * 0.31830987f;
    float fraction;
    int32_t sign;

    sub_5FE800(polarTurns - 0.5f, &fraction, &sign);
    float sinPolar = 1.0f - fraction * ((6.0f - 4.0f * fraction) * fraction);
    if (sign & 1)
        sinPolar = -sinPolar;

    sub_5FE800(polarTurns, &fraction, &sign);
    float cosPolar = 1.0f - fraction * ((6.0f - 4.0f * fraction) * fraction);
    if (sign & 1)
        cosPolar = -cosPolar;

    sub_5FE800(azimuthTurns - 0.5f, &fraction, &sign);
    float sinAzimuth = 1.0f - fraction * ((6.0f - 4.0f * fraction) * fraction);
    if (sign & 1)
        sinAzimuth = -sinAzimuth;

    sub_5FE800(azimuthTurns, &fraction, &sign);
    float cosAzimuth = 1.0f - fraction * ((6.0f - 4.0f * fraction) * fraction);
    if (sign & 1)
        cosAzimuth = -cosAzimuth;

    g_dnInfo.m_light1.m_dir.x = cosAzimuth * sinPolar;
    g_dnInfo.m_light1.m_dir.y = sinPolar * sinAzimuth;
    g_dnInfo.m_light1.m_dir.z = cosPolar;
}

// OFFSET: 0x7F3920
void UpdateLighting() {
    // TODO
    SetColors();
    SetDirection();
    SetPlanets();
}

// OFFSET: 0x7816F0
void Update(int32_t reset, const C3Vector* cameraPos) {
    float screenGlow = 0.0f;

    DNInfo* info = DayNight::GetInfo();

    int32_t farClipFogMode = g_theGxDevicePtr->Caps().m_shaderTargets[0] > 1;

    if (g_mapId < 530) {
        farClipFogMode = 0;
    }

    // SetFarClipFogMode(farClipFogMode);
    // info->ClearZoneLights();

    if (cameraPos) {
        // FindNearestLightParams(cameraPos);
    }

    if (reset) {
        if (cameraPos) {
            info->m_cameraPos = *cameraPos;
        }

        // ResetFogAccumulators(1);
        // ClearLightFlag(1);
    } else {
        screenGlow = 0.0f; // g_sunGlare.m_screenGlow * 0.34999999f;

        if (screenGlow == 0.0f) {
            // ClearLightFlag(1);
        }
    }

    UpdateLighting();
    // g_clouds.Update();
    g_stars.Update();
    UpdateFog();

    // CWorld::SetShadowColor(&info->m_shadowColor);

    const int32_t glowScale = (int32_t)((1.0f - screenGlow) * 255.0f);

    info->m_light1.m_ambient.b = (uint8_t)((glowScale * info->m_light1.m_ambient.b + 255) >> 8);
    info->m_light1.m_ambient.g = (uint8_t)((glowScale * info->m_light1.m_ambient.g + 255) >> 8);
    info->m_light1.m_ambient.r = (uint8_t)((glowScale * info->m_light1.m_ambient.r + 255) >> 8);

    info->m_light1.m_diffuse.b = (uint8_t)((glowScale * info->m_light1.m_diffuse.b + 255) >> 8);
    info->m_light1.m_diffuse.g = (uint8_t)((glowScale * info->m_light1.m_diffuse.g + 255) >> 8);
    info->m_light1.m_diffuse.r = (uint8_t)((glowScale * info->m_light1.m_diffuse.r + 255) >> 8);

    CM2Light* light = &CMap::s_mapLight->m_light;

    light->SetDirection(info->m_light1.m_dir);

    light->m_ambColor.x = info->m_light1.m_ambient.r * 0.0039215689f;
    light->m_ambColor.y = info->m_light1.m_ambient.g * 0.0039215689f;
    light->m_ambColor.z = info->m_light1.m_ambient.b * 0.0039215689f;

    light->m_dirColor.x = info->m_light1.m_diffuse.r * 0.0039215689f;
    light->m_dirColor.y = info->m_light1.m_diffuse.g * 0.0039215689f;
    light->m_dirColor.z = info->m_light1.m_diffuse.b * 0.0039215689f;

    light->m_specColor.x = info->m_bands.m_sunColor.r * 0.0039215689f;
    light->m_specColor.y = info->m_bands.m_sunColor.g * 0.0039215689f;
    light->m_specColor.z = info->m_bands.m_sunColor.b * 0.0039215689f;

    // SetFarClipFogMode(0);
}

void RenderSky() {
    if (!g_dnInfo.m_showSky) {
        return;
    }

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

    // TODO

    GxXformSetViewport(minX, maxX, minY, maxY, 0.99902344f, 1.0f);
    //GxRsSet(GxRs_ScissorTest, 1);

    g_stars.Render();
    DayNight::g_planets.sun.Render();
    DayNight::g_planets.moon.Render();
    DayNight::g_planets.moon2.Render();
    g_sky.Render();

    if (s_skyScene) {
        uint32_t delta = OsGetAsyncTimeMs() - s_skySceneTimeMs;
        s_skyScene->AdvanceTime(delta);
        s_skySceneTimeMs += delta;

        //HourAndMinutes = WowTime::GetHourAndMinutes(&g_clientGameTime);
        //m_blendSkybox = DayNight::g_dnInfo.m_blendSkybox;
        //do
        //    DayNight::SyncSkyAnimWithTime(*m_blendSkybox++, HourAndMinutes);
        //while (m_blendSkybox < DayNight::g_dnInfo.m_blendSkyboxWeight);
        //DayNight::SyncSkyAnimWithTime(DayNight::g_dnInfo.m_skybox, HourAndMinutes);

        float weigth = DayNight::g_dnInfo.m_skyboxWeight;
        auto skybox = DayNight::g_dnInfo.m_skybox;
        if (!DayNight::g_dnInfo.m_skybox || DayNight::g_dnInfo.m_skyboxWeight < 1.0) {
            for (int32_t i = 0; i < 3; i++) {
                DayNight::DrawSky(DayNight::g_dnInfo.m_blendSkybox[i], DayNight::g_dnInfo.m_blendSkyboxWeight[i]);
            }
            weigth = DayNight::g_dnInfo.m_skyboxWeight;
            skybox = DayNight::g_dnInfo.m_skybox;
        }
        if (skybox && weigth > 0.0) {
            DayNight::DrawSky(skybox, weigth);
        }
    }

    //GxRsSet(GxRs_ScissorTest, 0);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

void DrawSky(DNOverrideSky* sky, float weight) {
    if (!sky || !sky->m_model || !sky->m_model->IsDrawable(0, 0) || weight <= 0.0f)
        return;

    sky->m_model->SetAnimating(1);
    sky->m_model->SetVisible(1);
    if (sky->m_model->m_attachParent)
        sky->m_model->f_flags |= 0x20000u;
    else
        sky->m_model->f_flags |= 0x10000u;

    //a1->m_model->unk_0170.a2 = a2;
    C3Vector vec;
    s_skyScene->Animate(vec);
    s_skyScene->Draw(M2PASS_0);
    s_skyScene->Draw(M2PASS_1);
    sky->m_model->SetAnimating(0);
    sky->m_model->SetVisible(0);
    if (sky->m_model->m_attachParent)
        sky->m_model->f_flags &= ~0x20000u;
    else
        sky->m_model->f_flags &= ~0x10000u;
}

// OFFSET: 0x7ECEF0
DNInfo* GetInfo() {
    return &g_dnInfo;
}

// OFFSET: 0x7EECC0
void SetPlanets() {
    static const C2Vector s_sunTable[5] = {
        { 0.22916667, 1.7453293 },
        { 0.49652779, 0.087266468 },
        { 0.5, 0.087266468 },
        { 0.50347221, 0.087266468 },
        { 0.89583331, 1.7453293 },
    };

    static const C2Vector s_sunElevationTable[3] = {
        { 0.22916667, 0.78539819 },
        { 0.5, 0.78539819 },
        { 0.89583331, 0.78539819 },
    };

    static const C2Vector s_sunPathTable[5] = {
        { 0.0, 0.61086524 },
        { 0.0034722222, 0.61086524 },
        { 0.16666667, 1.7453293 },
        { 0.91666669, 1.7453293 },
        { 0.99652779, 0.61086524 },
    };

    static const C2Vector s_moonPathTable[3] = {
        { 0.0, 0.78539819 },
        { 0.16666667, 0.78539819 },
        { 0.91666669, 0.78539819 },
    };

    static const C2Vector s_moon2PathTable[5] = {
        { 0.0, 0.61086524 },
        { 0.0034722222, 0.61086524 },
        { 0.16666667, 1.7453293 },
        { 0.91666669, 1.7453293 },
        { 0.99652779, 0.61086524 },
    };

    static const C2Vector s_moon2PhaseTable[3] = {
        { 0.0, 2.3561945 },
        { 0.16666667, 2.6179938 },
        { 0.91666669, 2.8797934 },
    };

    static const C2Vector s_sunSizeTable[4] = {
        { 0.25, 2.0 },
        { 0.28125, 1.0 },
        { 0.84375, 1.0 },
        { 0.875, 2.0 },
    };

    static const C2Vector s_moonSizeTable[4] = {
        { 0.041666672, 1.0 },
        { 0.16666667, 1.5 },
        { 0.91666669, 1.5 },
        { 0.99930561, 1.0 },
    };

    const float dayProgression = DayNight::g_dnInfo.m_dayProgression;

    float sunAngle = InterpTable(s_sunTable, 5, dayProgression);
    float sunElevation = InterpTable(s_sunElevationTable, 3, dayProgression);
    float sunAngleTurns = sunAngle * 0.31830987f;
    float sunElevationTurns = sunElevation * 0.31830987f;

    float sunAngleFraction = 0.0f;
    int32_t sunAngleSign = 0;
    sub_5FE800(sunAngleTurns - 0.5f, &sunAngleFraction, &sunAngleSign);

    float sunX = 1.0f - sunAngleFraction * ((6.0f - 4.0f * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        sunX = -sunX;

    sub_5FE800(sunAngleTurns, &sunAngleFraction, &sunAngleSign);

    float sunZ = 1.0f - sunAngleFraction * ((6.0f - 4.0f * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        sunZ = -sunZ;

    sub_5FE800(sunElevationTurns - 0.5f, &sunAngleFraction, &sunAngleSign);

    float sunElevationX = 1.0f - sunAngleFraction * ((6.0f - 4.0f * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        sunElevationX = -sunElevationX;

    sub_5FE800(sunElevationTurns, &sunAngleFraction, &sunAngleSign);

    float sunElevationZ = 1.0f - sunAngleFraction * ((6.0f - 4.0f * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        sunElevationZ = -sunElevationZ;

    const float sunY = sunElevationX * sunX;
    sunX = sunElevationZ * sunX;

    const float sunScale = 12.0f / sqrt(sunX * sunX + sunZ * sunZ + sunY * sunY);

    DayNight::g_planets.sun.m_position.x = sunX * sunScale + DayNight::g_dnInfo.m_cameraPos.x;
    DayNight::g_planets.sun.m_position.y = sunY * sunScale + DayNight::g_dnInfo.m_cameraPos.y;
    DayNight::g_planets.sun.m_position.z = sunZ * sunScale + DayNight::g_dnInfo.m_cameraPos.z;

    DayNight::g_planets.sun.m_scale = InterpTable(s_sunSizeTable, 4, dayProgression) * DayNight::g_planets.sun.m_baseScale;

    // DayNight::g_sunGlare.unk_000C = DayNight::g_planets.sun.m_position.x;
    // DayNight::g_sunGlare.unk_0010 = DayNight::g_planets.sun.m_position.y;
    // DayNight::g_sunGlare.m_scale = DayNight::g_planets.sun.m_position.z;

    float moonAngle = InterpTable(s_sunPathTable, 5, dayProgression);
    float moonElevation = InterpTable(s_moonPathTable, 3, dayProgression);
    float moonAngleTurns = moonAngle * 0.31830987;
    float moonElevationTurns = moonElevation * 0.31830987;

    sub_5FE800(moonAngleTurns - 0.5, &sunAngleFraction, &sunAngleSign);

    float moonX = 1.0 - sunAngleFraction * ((6.0 - 4.0 * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        moonX = -moonX;

    sub_5FE800(moonAngleTurns, &sunAngleFraction, &sunAngleSign);

    float moonZ = 1.0 - sunAngleFraction * ((6.0 - 4.0 * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        moonZ = -moonZ;

    sub_5FE800(moonElevationTurns - 0.5, &sunAngleFraction, &sunAngleSign);

    float moonElevationX = 1.0 - sunAngleFraction * ((6.0 - 4.0 * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        moonElevationX = -moonElevationX;

    sub_5FE800(moonElevationTurns, &sunAngleFraction, &sunAngleSign);

    float moonElevationZ = 1.0 - sunAngleFraction * ((6.0 - 4.0 * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        moonElevationZ = -moonElevationZ;

    const float moonY = moonElevationX * moonX;
    moonX = moonElevationZ * moonX;

    const float moonScale = 12.0 / sqrt(moonZ * moonZ + moonY * moonY + moonX * moonX);

    DayNight::g_planets.moon.m_position.x = moonX * moonScale + DayNight::g_dnInfo.m_cameraPos.x;
    DayNight::g_planets.moon.m_position.y = moonY * moonScale + DayNight::g_dnInfo.m_cameraPos.y;
    DayNight::g_planets.moon.m_position.z = moonZ * moonScale + DayNight::g_dnInfo.m_cameraPos.z;

    DayNight::g_planets.moon.m_scale = InterpTable(s_moonSizeTable, 4, dayProgression) * DayNight::g_planets.moon.m_baseScale;

    // DayNight::g_moonGlare.unk_0090 = DayNight::g_planets.moon.m_scale;
    // DayNight::g_moonGlare.unk_000C = DayNight::g_planets.moon.m_position.x;
    // DayNight::g_moonGlare.unk_0094 = DayNight::g_planets.moon.m_scale;
    // DayNight::g_moonGlare.unk_0010 = DayNight::g_planets.moon.m_position.y;
    // DayNight::g_moonGlare.m_scale = DayNight::g_planets.moon.m_position.z;

    const float dayFixed = DayNight::g_dnInfo.m_day * 65536.0;
    const float progressionFixed = dayProgression * 65536.0;

    const uint32_t combinedFixed = (uint32_t)(progressionFixed - 0.5f) + (uint32_t)(dayFixed - 0.5f);

    const float moon2Cycle = floor((DayNight::g_dnInfo.m_day + dayProgression) / DayNight::g_planets.moon2.m_period) * DayNight::g_planets.moon2.m_period * 65536.0;

    uint32_t cycleFixed = (uint32_t)(moon2Cycle - 0.5f);

    if (cycleFixed >= combinedFixed)
        cycleFixed = combinedFixed;

    const float moon2Progress = (combinedFixed - cycleFixed) * 0.000015258789 / DayNight::g_planets.moon2.m_period;

    float moon2Angle = InterpTable(s_moon2PathTable, 5, moon2Progress);
    float moon2Elevation = InterpTable(s_moon2PhaseTable, 3, moon2Progress);
    float moon2AngleTurns = moon2Angle * 0.31830987;
    float moon2ElevationTurns = moon2Elevation * 0.31830987;

    sub_5FE800(moon2AngleTurns - 0.5, &sunAngleFraction, &sunAngleSign);

    float moon2X = 1.0 - sunAngleFraction * ((6.0 - 4.0 * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        moon2X = -moon2X;

    sub_5FE800(moon2AngleTurns, &sunAngleFraction, &sunAngleSign);

    float moon2Z = 1.0 - sunAngleFraction * ((6.0 - 4.0 * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        moon2Z = -moon2Z;

    sub_5FE800(moon2ElevationTurns - 0.5, &sunAngleFraction, &sunAngleSign);

    float moon2ElevationX = 1.0 - sunAngleFraction * ((6.0 - 4.0 * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        moon2ElevationX = -moon2ElevationX;

    sub_5FE800(moon2ElevationTurns, &sunAngleFraction, &sunAngleSign);

    float moon2ElevationZ = 1.0 - sunAngleFraction * ((6.0 - 4.0 * sunAngleFraction) * sunAngleFraction);

    if (sunAngleSign & 1)
        moon2ElevationZ = -moon2ElevationZ;

    const float moon2Y = moon2ElevationX * moon2X;
    moon2X = moon2ElevationZ * moon2X;

    const float moon2Scale = 12.0 / sqrt(moon2Y * moon2Y + moon2X * moon2X + moon2Z * moon2Z);

    DayNight::g_planets.moon2.m_position.x = moon2X * moon2Scale + DayNight::g_dnInfo.m_cameraPos.x;
    DayNight::g_planets.moon2.m_position.y = moon2Y * moon2Scale + DayNight::g_dnInfo.m_cameraPos.y;
    DayNight::g_planets.moon2.m_position.z = moon2Z * moon2Scale + DayNight::g_dnInfo.m_cameraPos.z;
    DayNight::g_planets.moon2.m_scale = InterpTable(s_moonSizeTable, 4, moon2Progress) * DayNight::g_planets.moon2.m_baseScale;

    DebugScreenSet("Time", "%f", DayNight::g_dnInfo.m_dayProgression);
    DebugScreenSet("Sun pos", "%f %f %f", DayNight::g_planets.sun.m_position.x, DayNight::g_planets.sun.m_position.y, DayNight::g_planets.sun.m_position.z);

    float pathStart;

    if (dayProgression < 0.22916667) {
        pathStart = 0.5;
    } else {
        if (dayProgression < 0.5) {
            DayNight::g_dnInfo.m_sunMoonPath = (dayProgression - 0.22916667) * 3.6923077;
            return;
        }

        pathStart = 0.5;
    }

    if (dayProgression < pathStart || dayProgression >= 0.89583331) {
        if (dayProgression < 0.91666669 || dayProgression >= 1.0) {
            if (dayProgression < 0.0 || dayProgression >= 0.16666667) {
                DayNight::g_dnInfo.m_sunMoonPath = 0.0;
            } else {
                DayNight::g_dnInfo.m_sunMoonPath = 1.0 - dayProgression * 6.0;
            }
        } else {
            DayNight::g_dnInfo.m_sunMoonPath = (dayProgression - 0.91666669) * 12.000003;
        }
    } else {
        DayNight::g_dnInfo.m_sunMoonPath = 1.0 - (dayProgression - pathStart) * 2.5263159;
    }
}

// OFFSET: 0x7ED790
CImVector DarkenColor(CImVector colour, float scale) {
    C3Vector rgb;
    rgb.x = colour.r * 0.0039215689f;
    rgb.y = colour.g * 0.0039215689f;
    rgb.z = colour.b * 0.0039215689f;

    C3Vector hsv = { 0.0f, 0.0f, 0.0f };

    RGBtoHSV(&rgb, &hsv);

    hsv[2] = hsv[2] * scale;

    HSVtoRGB(&hsv, &rgb);

    CImVector out;
    out.a = 0xFF;
    out.r = (uint8_t)(int32_t)(rgb.x * 255.0f);
    out.g = (uint8_t)(int32_t)(rgb.y * 255.0f);
    out.b = (uint8_t)(int32_t)(rgb.z * 255.0f);

    return out;
}


} // namespace DayNight
