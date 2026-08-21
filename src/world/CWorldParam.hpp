#ifndef WORLD_C_WORLD_PARAM_HPP
#define WORLD_C_WORLD_PARAM_HPP

#include <cstdint>
#include <tempest/Box.hpp>
#include <tempest/Rect.hpp>
#include <tempest/Vector.hpp>

class CVar;

class CWorldParam {
    public:
    // Static variables
    static CVar* s_cvFarClipOverride;
    static CVar* s_cvLod;
    static CVar* s_cvMapShadows;
    static CVar* s_cvMaxLights;
    static CVar* s_cvShadowLevel;
    static CVar* s_cvTexLodBias;
    static CVar* s_cvFarClip;
    static CVar* s_cvNearClip;
    static CVar* s_cvSpecular;
    static CVar* s_cvMapObjLightLOD;
    static CVar* s_cvParticleDensity;
    static CVar* s_cvWaterLOD;
    static CVar* s_cvBaseMip;
    static CVar* s_cvHorizonFarclipScale;
    static CVar* s_cvHorizonNearclipScale;
    static CVar* s_cvShowfootprints;
    static CVar* s_cvBspcache;
    static CVar* s_cvFootstepBias;
    static CVar* s_cvOcclusion;
    static CVar* s_cvWorldPoolUsage;
    static CVar* s_cvTerrainAlphaBitDepth;
    static CVar* s_cvGroundEffectDensity;
    static CVar* s_cvGroundEffectDist;
    static CVar* s_cvObjectFade;
    static CVar* s_cvObjectFadeZFill;
    static CVar* s_cvEnvironmentDetail;
    static CVar* s_cvHwPCF;
    static CVar* s_cvExtShadowQuality;
    static CVar* s_cvProjectedTextures;
    static CVar* s_cvGxTextureCacheSize;

    // Static functions
    static bool FarClipOverrideCallback(CVar*, const char*, const char*, void*);
    static bool LodCallback(CVar*, const char*, const char*, void*);
    static bool MapShadowsCallback(CVar*, const char*, const char*, void*);
    static bool MaxLightsCallback(CVar*, const char*, const char*, void*);
    static bool ShadowLevelCallback(CVar*, const char*, const char*, void*);
    static bool TexLodBiasCallback(CVar*, const char*, const char*, void*);
    static bool FarClipCallback(CVar*, const char*, const char*, void*);
    static bool NearClipCallback(CVar*, const char*, const char*, void*);
    static bool SpecularCallback(CVar*, const char*, const char*, void*);
    static bool MapObjLightLODCallback(CVar*, const char*, const char*, void*);
    static bool ParticleDensityCallback(CVar*, const char*, const char*, void*);
    static bool WaterLODCallback(CVar*, const char*, const char*, void*);
    static bool BaseMipCallback(CVar*, const char*, const char*, void*);
    static bool HorizonFarClipScaleCallback(CVar*, const char*, const char*, void*);
    static bool HorizonNearClipScaleCallback(CVar*, const char*, const char*, void*);
    static bool ShowFootprintsCallback(CVar*, const char*, const char*, void*);
    static bool BspCacheCallback(CVar*, const char*, const char*, void*);
    static bool FootstepBiasCallback(CVar*, const char*, const char*, void*);
    static bool HardwareOcclusionTestCallback(CVar*, const char*, const char*, void*);
    static bool WorldPoolUsageCallback(CVar*, const char*, const char*, void*);
    static bool TerrainAlphaBitDepthCallback(CVar*, const char*, const char*, void*);
    static bool GroundEffectDensityCallback(CVar*, const char*, const char*, void*);
    static bool GroundEffectDistCallback(CVar*, const char*, const char*, void*);
    static bool ObjectFadeCallback(CVar*, const char*, const char*, void*);
    static bool ObjectFadeZFillCallback(CVar*, const char*, const char*, void*);
    static bool EnvironmentDetailCallback(CVar*, const char*, const char*, void*);
    static bool HWPCFCallback(CVar*, const char*, const char*, void*);
    static bool ExtShadowQualityCallback(CVar*, const char*, const char*, void*);
    static bool ProjectedTexturesCallback(CVar*, const char*, const char*, void*);
    static bool GxTextureCacheSizeCallback(CVar*, const char*, const char*, void*);
    static void Initialize();
};

#endif
