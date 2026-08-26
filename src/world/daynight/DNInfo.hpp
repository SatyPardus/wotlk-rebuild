#ifndef WORLD_DAY_NIGHT_INFO_HPP
#define WORLD_DAY_NIGHT_INFO_HPP

#include <cstdint>
#include <tempest/Vector.hpp>
#include "storm/array/TSFixedArray.hpp"
#include "world/daynight/DNLightBands.hpp"

class CM2Scene;
class CM2Model;
class DNOverrideSky;
class LightRec;

struct DNFogInfo {
    CImVector color;
    float start;
    float end;
    float m_density;
};

struct DNKeyframe {
    float time;
    float value;
};

struct DNLightState {
    C3Vector m_dir;
    CImVector m_diffuse;
    CImVector m_ambient;
    CImVector m_mid0;
    CImVector m_mid1;
    float m_desatR;
    float m_desatG;
    float m_desatB;
    float m_desatA;
};

namespace DayNight {

class DNInfo {
    public:

    uint32_t m_timeOfDay;
    float m_dayProgression;
    float m_day;
    C3Vector m_playerPos;
    C3Vector m_cameraPos;
    C3Vector m_lightRefPos;
    C3Vector m_cameraDir;
    float m_faceAngle;
    float m_farClip;
    float m_timeSec;
    float m_deltaSec;
    float m_weatherIntensity;
    uint8_t m_flashBlend;
    uint8_t m_flashColorBGRA[4];
    uint8_t m_pad055[3];
    uint32_t m_overrideLightParamsId;
    DNOverrideSky* m_skybox;
    float m_skyboxWeight;
    DNOverrideSky* m_blendSkybox[3];
    float m_blendSkyboxWeight[3];
    uint32_t m_blendSkyboxFlags[3];
    float m_stormBlend;
    DNFogInfo m_fog;
    float m_interiorFogBlend;
    DNFogInfo m_fogInterior;
    DNFogInfo m_fogGroup;
    uint32_t m_interiorFogId;
    TSFixedArray<uint32_t> m_interiorFogIds;
    uint32_t m_interiorFogFlags;
    DNLightBands m_bands;
    DNLightState m_light0;
    DNLightState m_light1;
    CImVector m_shadowColor;
    uint32_t m_showSky;
    float m_cloudSunU;
    float m_cloudSunV;
    float m_sunMoonPath;
    float m_curve0;
    float m_curve1;
    uint32_t m_nearLightCount;
    uint32_t m_unk1E8;
    LightRec* m_nearLight[5];
    float m_nearLightBlend[5];
};

} // namespace DayNight

#endif
