#include "world/CWorldParam.hpp"
#include "console/CVar.hpp"

CVar* CWorldParam::s_cvFarClipOverride;
CVar* CWorldParam::s_cvLod;
CVar* CWorldParam::s_cvMapShadows;
CVar* CWorldParam::s_cvMaxLights;
CVar* CWorldParam::s_cvShadowLevel;
CVar* CWorldParam::s_cvTexLodBias;
CVar* CWorldParam::s_cvFarClip;
CVar* CWorldParam::s_cvNearClip;
CVar* CWorldParam::s_cvSpecular;
CVar* CWorldParam::s_cvMapObjLightLOD;
CVar* CWorldParam::s_cvParticleDensity;
CVar* CWorldParam::s_cvWaterLOD;
CVar* CWorldParam::s_cvBaseMip;
CVar* CWorldParam::s_cvHorizonFarclipScale;
CVar* CWorldParam::s_cvHorizonNearclipScale;
CVar* CWorldParam::s_cvShowfootprints;
CVar* CWorldParam::s_cvBspcache;
CVar* CWorldParam::s_cvFootstepBias;
CVar* CWorldParam::s_cvOcclusion;
CVar* CWorldParam::s_cvWorldPoolUsage;
CVar* CWorldParam::s_cvTerrainAlphaBitDepth;
CVar* CWorldParam::s_cvGroundEffectDensity;
CVar* CWorldParam::s_cvGroundEffectDist;
CVar* CWorldParam::s_cvObjectFade;
CVar* CWorldParam::s_cvObjectFadeZFill;
CVar* CWorldParam::s_cvEnvironmentDetail;
CVar* CWorldParam::s_cvHwPCF;
CVar* CWorldParam::s_cvExtShadowQuality;
CVar* CWorldParam::s_cvProjectedTextures;
CVar* CWorldParam::s_cvGxTextureCacheSize;

// OFFSET: 0x78E400
void CWorldParam::Initialize() {
    CWorldParam::s_cvFarClipOverride = CVar::Register("farClipOverride", "Override old world graphic settings", 1, "0", CWorldParam::FarClipOverrideCallback, 1, 0, 0, 0);
    CWorldParam::s_cvLod = CVar::Register("lod", "Video option: Toggle Lod", 1, "0", CWorldParam::LodCallback, 1, 0, 0, 0);
    CWorldParam::s_cvMapShadows = CVar::Register("mapShadows", "Video option: Toggle map shadows", 1, "1", CWorldParam::MapShadowsCallback, 1, 0, 0, 0);
    CWorldParam::s_cvMaxLights = CVar::Register("MaxLights", "Max number of hardware lights", 1, "4", CWorldParam::MaxLightsCallback, 1, 0, 0, 0);
    CWorldParam::s_cvShadowLevel = CVar::Register("shadowLevel", "Terrain shadow map mip level", 1, "1", CWorldParam::ShadowLevelCallback, 1, 0, 0, 0);
    CWorldParam::s_cvTexLodBias = CVar::Register("texLodBias", "Texture LOD Bias", 1, "0.0", CWorldParam::TexLodBiasCallback, 1, 0, 0, 0);
    CWorldParam::s_cvFarClip = CVar::Register("farclip", "Far clip plane distance", 1, "350", CWorldParam::FarClipCallback, 1, 0, 0, 0);
    CWorldParam::s_cvNearClip = CVar::Register("nearclip", "Near clip plane distance", 1, "0.2", CWorldParam::NearClipCallback, 1, 0, 0, 0);
    CWorldParam::s_cvSpecular = CVar::Register("specular", "Specularity", 1, "0", CWorldParam::SpecularCallback, 1, 0, 0, 0);
    CWorldParam::s_cvMapObjLightLOD = CVar::Register("mapObjLightLOD", "Map object light LOD", 1, "0", CWorldParam::MapObjLightLODCallback, 1, 0, 0, 0);
    CWorldParam::s_cvParticleDensity = CVar::Register("particleDensity", "Video option: Particle density", 1, "1.0", CWorldParam::ParticleDensityCallback, 1, 0, 0, 0);
    CWorldParam::s_cvWaterLOD = CVar::Register("waterLOD", "Water geometry LOD", 1, "0", CWorldParam::WaterLODCallback, 1, 0, 0, 0);
    CWorldParam::s_cvBaseMip = CVar::Register("baseMip", "base mipmap level", 1, "0", CWorldParam::BaseMipCallback, 1, 0, 0, 0);
    CWorldParam::s_cvHorizonFarclipScale = CVar::Register("horizonFarclipScale", "Far clip plane scale for horizon", 1, "4.0", CWorldParam::HorizonFarClipScaleCallback, 1, 0, 0, 0);
    CWorldParam::s_cvHorizonNearclipScale = CVar::Register("horizonNearclipScale", "Near clip plane scale for horizon", 1, "0.7", CWorldParam::HorizonNearClipScaleCallback, 1, 0, 0, 0);
    CWorldParam::s_cvShowfootprints = CVar::Register("showfootprints", "toggles rendering of footprints", 1, "1", CWorldParam::ShowFootprintsCallback, 1, 0, 0, 0);
    CWorldParam::s_cvBspcache = CVar::Register("bspcache", "BSP node caching", 1, "1", CWorldParam::BspCacheCallback, 1, 0, 0, 0);
    CWorldParam::s_cvFootstepBias = CVar::Register("footstepBias", "Unit footstep depth bias", 1, "0.125", CWorldParam::FootstepBiasCallback, 1, 0, 0, 0);
    CWorldParam::s_cvOcclusion = CVar::Register("occlusion", "Use hardware occlusion test", 1, "1", CWorldParam::HardwareOcclusionTestCallback, 1, 0, 0, 0);
    CWorldParam::s_cvWorldPoolUsage = CVar::Register("worldPoolUsage", "CGxPool usage static/dynamic", 1, "Dynamic", CWorldParam::WorldPoolUsageCallback, 1, 0, 0, 0);
    CWorldParam::s_cvTerrainAlphaBitDepth = CVar::Register("terrainAlphaBitDepth", "Terrain alpha map bit depth", 1, "8", CWorldParam::TerrainAlphaBitDepthCallback, 1, 0, 0, 0);
    CWorldParam::s_cvGroundEffectDensity = CVar::Register("groundEffectDensity", "Ground effect density", 1, "16", CWorldParam::GroundEffectDensityCallback, 1, 0, 0, 0);
    CWorldParam::s_cvGroundEffectDist = CVar::Register("groundEffectDist", "Ground effect dist", 1, "70.0", CWorldParam::GroundEffectDistCallback, 1, 0, 0, 0);
    CWorldParam::s_cvObjectFade = CVar::Register("objectFade", "Fade objects into view", 1, "1", CWorldParam::ObjectFadeCallback, 1, 0, 0, 0);
    CWorldParam::s_cvObjectFadeZFill = CVar::Register("objectFadeZFill", "Fade objects using ZFill pass ", 1, "0", CWorldParam::ObjectFadeZFillCallback, 1, 0, 0, 0);
    CWorldParam::s_cvEnvironmentDetail = CVar::Register("environmentDetail", "Environment detail", 1, "1.0", CWorldParam::EnvironmentDetailCallback, 1, 0, 0, 0);
    CWorldParam::s_cvHwPCF = CVar::Register("hwPCF", "Hardware PCF Filtering", 1, "1", CWorldParam::HWPCFCallback, 1, 0, 0, 0);
    CWorldParam::s_cvExtShadowQuality = CVar::Register("extShadowQuality", "Quality of exterior shadows (0-5)", 1, "0", CWorldParam::ExtShadowQualityCallback, 1, 0, 0, 0);
    CWorldParam::s_cvProjectedTextures = CVar::Register("projectedTextures", "Projected Textures", 1, "0", CWorldParam::ProjectedTexturesCallback, 1, 0, 0, 0);
    CWorldParam::s_cvGxTextureCacheSize = CVar::Register("gxTextureCacheSize", "GX Texture Cache Size", 1, "0", CWorldParam::GxTextureCacheSizeCallback, 1, 0, 0, 0);
    CVar::Register("spellEffectLevel", "Video Option: Spell Effects", 1, "9", 0, 1, 0, 0, 0);
    // AddConsoleDeviceDefaultCallback(CWorldParam::SetDefaults);
    // if (ConsoleDeviceHardwareChanged()) {
    //     for (i = 0; i < 3; ++i)
    //         CWorldParam::SetDefaults(i);
    // }
    // v1 = CVar::Lookup("videoOptionsVersion");
    // if (!v1 || v1->m_intValue < 2) {
    //     CWorldParam::SetDefaultParticleDensity();
    //     CWorldParam::SetDefaultProjectedTextures();
    // }
}

bool CWorldParam::FarClipOverrideCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::LodCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::MapShadowsCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::MaxLightsCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::ShadowLevelCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::TexLodBiasCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::FarClipCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::NearClipCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::SpecularCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::MapObjLightLODCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::ParticleDensityCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::WaterLODCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::BaseMipCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::HorizonFarClipScaleCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::HorizonNearClipScaleCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::ShowFootprintsCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::BspCacheCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::FootstepBiasCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::HardwareOcclusionTestCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::WorldPoolUsageCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::TerrainAlphaBitDepthCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::GroundEffectDensityCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::GroundEffectDistCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::ObjectFadeCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::ObjectFadeZFillCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::EnvironmentDetailCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::HWPCFCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::ExtShadowQualityCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::ProjectedTexturesCallback(CVar*, const char*, const char*, void*) {
    return true;
}

bool CWorldParam::GxTextureCacheSizeCallback(CVar*, const char*, const char*, void*) {
    return true;
}
