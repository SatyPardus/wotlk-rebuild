#include "world/daynight/DNLightBands.hpp"

namespace DayNight {

    // OFFSET: 0x7EE360
    DNLightBands::DNLightBands() {
        this->m_ambient = { 0, 0, 0, 0 };
        this->m_diffuse = { 0, 0, 0, 0 };
        this->m_shadowOpacity = { 0, 0, 0, 0 };
        this->m_sky0 = { 0, 0, 0, 0 };
        this->m_sky1 = { 0, 0, 0, 0 };
        this->m_sky2 = { 0, 0, 0, 0 };
        this->m_sky3 = { 0, 0, 0, 0 };
        this->m_sky4 = { 0, 0, 0, 0 };
        this->m_skyFog = { 0, 0, 0, 0 };
        this->m_sunColor = { 0, 0, 0, 0 };
        this->m_cloudColor0 = { 0, 0, 0, 0 };
        this->m_cloudColor1 = { 0, 0, 0, 0 };
        this->m_cloudColor2 = { 0, 0, 0, 0 };
        this->m_band13 = { 0, 0, 0, 0 };
        this->m_band14 = { 0, 0, 0, 0 };
        this->m_band15 = { 0, 0, 0, 0 };
        this->m_band16 = { 0, 0, 0, 0 };
        this->m_band17 = { 0, 0, 0, 0 };
        this->m_fogEnd = 0.0f;
        this->m_fogStartMul = 0.0f;
        this->m_fogRate = 0.0f;
        this->m_highlightSky = 0.0f;
        this->m_glow = 0.0f;
        this->m_floatBand2 = 0.0f;
        this->m_cloudDensity = 0.0f;
        this->m_floatBand4 = 0.0f;
        this->m_floatBand5 = 0.0f;
        this->m_waterShallowAlpha = 0.0f;
        this->m_waterDeepAlpha = 0.0f;
        this->m_oceanShallowAlpha = 0.0f;
        this->m_oceanDeepAlpha = 0.0f;
        this->m_skyboxId0 = 0;
        this->m_skyboxWeight0 = 0.0f;
        this->m_skyboxId1 = 0;
        this->m_skyboxWeight1 = 0.0f;
        this->m_skyboxId2 = 0;
        this->m_skyboxWeight2 = 0.0f;
        this->m_cloudTypeId = 0;
        this->m_cloudWeight = 0.0f;
    }

    // OFFSET: 0x7ED910
    void DNLightBands::Copy(const DNLightBands* src) {
        *this = *src;
    }

}
