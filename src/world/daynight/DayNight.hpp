#ifndef WORLD_DAY_NIGHT_HPP
#define WORLD_DAY_NIGHT_HPP

#include <cstdint>
#include "storm/Array.hpp"
#include "tempest/Vector.hpp"
#include "world/daynight/DNOverrideSky.hpp"

class LightRec;
class CM2Scene;
class LightFloatBandRec;
class LightParamsRec;
class DNLightBands;
class LightIntBandRec;

namespace DayNight {

    class DNInfo;

    struct AreaLightOverride {
        uint32_t m_id;         // +0x00
        LightRec* m_light;     // +0x04
        float m_blend;         // +0x08
        uint32_t m_state;      // +0x0C  1 = fading in, 2 = fading out
        uint32_t m_lastTimeMs; // +0x10
        float m_rateMs;        // +0x14
    };

    extern DNInfo g_dnInfo;

    extern uint32_t g_mapId;
    extern TSGrowableArray<LightRec*> g_areaLights;

    static TSGrowableArray<AreaLightOverride> s_areaLightOverrides;
    static int32_t s_useZoneLights = 1;
    static int32_t s_useAreaLights = 1;
    static TSHashTable<DNOverrideSky, HASHKEY_STRI> s_overrideSky;
    static CM2Scene* s_skyScene = nullptr;
    static uint32_t s_skySceneTimeMs = 0;
    static int32_t s_farClipFogMode = 0;
    static float s_fogRateScale = 5.5f;
    static int32_t s_compareByDistanceOnly = 0;

    static uint32_t s_lightFlags = 0;
    static float s_glowBlend = 0.0f;
    static CImVector s_glowColor = { 0, 0, 0, 0 };

    static int32_t s_fogOverrideActive = 0;
    static float s_overrideFogEnd = 0.0f;
    static float s_overrideFogStartMul = 0.0f;
    static float s_overrideFogRate = 0.0f;
    static CImVector s_overrideFogColor = { 0, 0, 0, 0 };

    static float s_glowEndTime = 0.0f;
    static float s_glowStartTime = 0.0f;
    static float s_glowFalloff = 1.0f;

    static C2Vector s_curve0Table[4] = {
        { 0.25, 1.0 },
        { 0.291667, 0.0 },
        { 0.854167, 0.0 },
        { 0.895833, 1.0 },
    };

    static C2Vector s_curve1Table[2] = {
        { 0.0833333, 0.25 },
        { 0.5, 1.0 },
    };

    void InterpColor(CImVector* dst, int32_t t, const CImVector* src);
    int32_t FindBandSlot(const int32_t* times, int32_t count, int32_t now, float* outT, int32_t* outLo, int32_t* outHi);
    void InterpIntBand(const LightIntBandRec* rec, int32_t now, CImVector* out);
    float InterpFloatBand(const LightFloatBandRec* rec, int32_t now, int32_t bandIndex);
    void LoadLightIntBand(CImVector* out, int32_t bandTime, const LightParamsRec* params, int32_t bandIndex);
    float CalcIndividualColorFromDBRec(int32_t bandTime, const LightParamsRec* params, int32_t bandIndex);
    void LoadLightingBands(int32_t bandTime, DNLightBands* bands, LightParamsRec* params);
    void BlendColors(DNLightBands* dst, const DNLightBands* src, float t);
    LightParamsRec* GetLightParams(const LightRec* light, int32_t slot);
    void SetLightColors();
    DNOverrideSky* GetOverrideSky(const char* path, uint32_t flags);
    float InterpTable(const C2Vector* table, uint32_t size, float key);
    void LoadMap(int32_t zoneID);
    void LightLoad(TSGrowableArray<LightRec*>* lightArray, uint32_t a2);
    void ColorLerpBytes(CImVector* out, const CImVector* a, const CImVector* b, float t);
    void BlendOutputs(DNLightBands* dst, const DNLightBands* src, float t);
    void BlendAreaLightColors(DNLightBands* dst, const LightRec* light, int32_t clearSlot, int32_t stormSlot);
    void BlendAreaLight(DNLightBands* dst, float maxFade, const LightRec* light, int32_t clearSlot, int32_t stormSlot); 
    void BlendZoneLight(DNLightBands* dst, float maxFade, const LightRec* light, float distPct, int32_t clearSlot, int32_t stormSlot);
    void SetColors();
    void UpdateFog();
    void Update(int32_t reset, const C3Vector* cameraPos);
    void RenderSky();
    void DrawSky(DNOverrideSky* sky, float weight);
    DNInfo* GetInfo();
    void SetPlanets();
    CImVector DarkenColor(CImVector colour, float scale);

} // namespace DayNight

#endif
