#ifndef WORLD_DAY_NIGHT_LIGHT_BANDS_HPP
#define WORLD_DAY_NIGHT_LIGHT_BANDS_HPP

#include <cstdint>
#include <storm/Hash.hpp>
#include "tempest/Vector.hpp"

class CM2Model;

namespace DayNight {

class DNLightBands {
    public:
    CImVector m_ambient;
    CImVector m_diffuse;
    CImVector m_shadowOpacity;
    CImVector m_sky0;
    CImVector m_sky1;
    CImVector m_sky2;
    CImVector m_sky3;
    CImVector m_sky4;
    CImVector m_skyFog;
    CImVector m_sunColor;
    CImVector m_cloudColor0;
    CImVector m_cloudColor1;
    CImVector m_cloudColor2;
    CImVector m_band13;
    CImVector m_band14;
    CImVector m_band15;
    CImVector m_band16;
    CImVector m_band17;
    float m_fogEnd;
    float m_fogStartMul;
    float m_fogRate;
    float m_highlightSky;
    float m_glow;
    float m_floatBand2;
    float m_cloudDensity;
    float m_floatBand4;
    float m_floatBand5;
    float m_waterShallowAlpha;
    float m_waterDeepAlpha;
    float m_oceanShallowAlpha;
    float m_oceanDeepAlpha;
    uint32_t m_skyboxId0;
    float m_skyboxWeight0;
    uint32_t m_skyboxId1;
    float m_skyboxWeight1;
    uint32_t m_skyboxId2;
    float m_skyboxWeight2;
    uint32_t m_cloudTypeId;
    float m_cloudWeight;

    DNLightBands();
    void Copy(const DNLightBands* src);
};

} // namespace DayNight

#endif
