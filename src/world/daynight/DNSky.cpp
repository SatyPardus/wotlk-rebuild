#include "world/daynight/DNSky.hpp"
#include "gx/Device.hpp"
#include "gx/Transform.hpp"
#include "gx/RenderState.hpp"
#include "gx/Draw.hpp"
#include <tempest/Math.hpp>
#include <tempest/Matrix.hpp>
#include "world/daynight/DayNight.hpp"
#include "world/daynight/DNInfo.hpp"

namespace DayNight {

float DNSky::m_stripSizes[SKY_NUMBANDS] = { 0.0f, 0.17f, 0.2f, 0.23f, 0.23999999f, 0.25f, 1.0f };
float DNSky::m_fadeAngle[SKY_NUMBANDS];
float DNSky::m_darkAngle[SKY_NUMBANDS];

static const C2Vector s_highlightCurve[6] = {
    { 0.125f, 0.0f },
    { 0.270833f, 1.0f },
    { 0.291667f, 0.0f },
    { 0.854167f, 0.0f },
    { 0.895833f, 1.0f },
    { 0.999306f, 0.0f },
};

static const C2Vector s_ringCurve[6] = {
    { 0.125f, 1.0f },
    { 0.375f, 0.0f },
    { 0.500f, -0.5f },
    { 0.625f, -0.7f },
    { 0.750f, -0.5f },
    { 0.875f, 0.0f },
};

// OFFSET: 0x9ACB00
void DNSky::Render() {
    GxXformPush(GxXform_World);
    g_theGxDevicePtr->m_xforms[GxXform_World].Identity();

    C44Matrix worldScale;
    worldScale.Scale(6.6666665f);
    GxXformSet(GxXform_World, worldScale);
    GxRsPush();
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_BlendingMode, GxBlend_Add);
    GxRsSetAlphaRef();
    GxPrimLockVertexPtrs(
        this->m_nVerts, this->m_geoVerts.Ptr(), sizeof(C3Vector),
        nullptr, 0,
        this->m_clrVerts.Ptr(), sizeof(CImVector),
        nullptr, 0,
        nullptr, 0,
        nullptr, 0);
    GxDrawLockedElements(GxPrim_TriangleStrip, this->m_nIndices, this->m_indices.Ptr());
    GxPrimUnlockVertexPtrs();
    GxXformPop(GxXform_World);
    GxRsPop();
}

// OFFSET: 0x7F2470
void DNSky::GenSphere(float sphRadius) {
    const uint16_t totalSlices = 24;
    const uint16_t totalIndices = (totalSlices + 1) * 2;

    this->m_sphThetaTess = totalSlices;
    this->m_geoVerts.SetCount(SKY_NUMBANDS * totalSlices); // 168
    this->m_clrVerts.SetCount(SKY_NUMBANDS * totalSlices); // 168
    this->m_indices.SetCount((SKY_NUMBANDS - 1) * totalIndices); // 300

    uint16_t lastIndex = 0;

    uint16_t lastRowIndex = 0;
    uint16_t thisRowIndex = 0;
    uint16_t prevRowIndex = 0;

    float prevPhi = 0.0f;

    for (int32_t i = 0; i < SKY_NUMBANDS; ++i) {
        float phi = DNSky::m_stripSizes[i] * CMath::PI;

        float cosPhi = CMath::cos(phi);
        float sinPhi = CMath::sin(phi);

        thisRowIndex = lastRowIndex;

        for (uint16_t j = 0; j < totalSlices; ++j) {
            auto& vertex = this->m_geoVerts[lastRowIndex++];

            float theta = static_cast<float>(j) / static_cast<float>(totalSlices) * CMath::TWO_PI;
            vertex.x = CMath::sin(theta) * sinPhi * sphRadius;
            vertex.y = CMath::cos(theta) * sinPhi * sphRadius;
            vertex.z = cosPhi * sphRadius - 0.70710678f;

            if (CMath::fequal(phi, 0.0f) || CMath::fequal(phi, CMath::PI)) {
                break;
            }
        }

        if (i > 0) {
            for (uint16_t k = 0; k < totalIndices / 2; ++k) {
                uint16_t idx1 = CMath::fequal(prevPhi, 0.0f) ? 0 : (k % totalSlices);
                uint16_t idx2 = CMath::fequal(phi, CMath::PI) ? 0 : (k % totalSlices);
                this->m_indices[lastIndex++] = idx1 + prevRowIndex;
                this->m_indices[lastIndex++] = idx2 + thisRowIndex;
            }
        }

        prevPhi = phi;
        prevRowIndex = thisRowIndex;
    }

    this->m_nVerts = lastRowIndex;
    this->m_nIndices = lastIndex; // Should be always equal to 300
}

// OFFSET: 0x7F0530
void DNSky::SetColors() {
    CImVector* out = this->m_clrVerts.Ptr();

    const float h = InterpTable(s_highlightCurve, 6, g_dnInfo.m_dayProgression) * g_dnInfo.m_bands.m_highlightSky;

    if (s_lightFlags & 1) {
        CImVector* band = &g_dnInfo.m_bands.m_sky0;

        for (int32_t i = 0; i < 6; ++i) {
            const uint8_t t = (uint8_t)(int32_t)((1.0f - s_glowBlend) * 255.0f);
            CImVector glow = s_glowColor;

            if (t) {
                InterpColor(&band[i], t, &glow);
            }
        }
    }

    CImVector ring[6] = {};

    for (int32_t i = 0; i < 5; ++i) {
        ColorLerpBytes(&ring[i + 1], &(&g_dnInfo.m_bands.m_sky1)[i], &g_dnInfo.m_bands.m_sky1, h);
    }

    *out++ = DarkenColor(g_dnInfo.m_bands.m_sky0, 1.0f);

    const float step = -1.0f / (float)this->m_sphThetaTess;

    for (int32_t ringIdx = 1; ringIdx <= 4; ++ringIdx) {
        float u = g_dnInfo.m_faceAngle * 0.15915494f + 0.25f;

        if (u > 1.0f) {
            u -= 1.0f;
        }

        for (int32_t i = 0; i < this->m_sphThetaTess; ++i) {
            if (u < 0.0f) {
                u += 1.0f;
            }

            const float v = InterpTable(s_ringCurve, 6, u);

            CImVector colour;

            if (v < 0.0f) {
                CImVector darkened;
                ColorLerpBytes(&darkened, &ring[ringIdx], &g_dnInfo.m_bands.m_sky0, h * 0.69999999f);
                ColorLerpBytes(&colour, &ring[ringIdx], &darkened, -v * h);
            } else {
                ColorLerpBytes(&colour, &(&g_dnInfo.m_bands.m_sky0)[ringIdx], &ring[ringIdx], (1.0f - v) * h);
            }

            if (g_dnInfo.m_flashBlend) {
                CImVector flash = *(const CImVector*)g_dnInfo.m_flashColorBGRA;
                InterpColor(&colour, g_dnInfo.m_flashBlend, &flash);
            }

            *out++ = colour;
            u += step;
        }
    }

    CImVector horizon = g_dnInfo.m_bands.m_skyFog;

    if (g_dnInfo.m_flashBlend) {
        CImVector flash = *(const CImVector*)g_dnInfo.m_flashColorBGRA;
        InterpColor(&horizon, g_dnInfo.m_flashBlend, &flash);
    }

    for (int32_t i = 0; i < this->m_sphThetaTess; ++i) {
        *out++ = horizon;
    }

    CImVector nadir = g_dnInfo.m_bands.m_skyFog;

    if (g_dnInfo.m_flashBlend) {
        CImVector flash = *(const CImVector*)g_dnInfo.m_flashColorBGRA;
        InterpColor(&nadir, g_dnInfo.m_flashBlend, &flash);
    }

    *out = nadir;
}

} // namespace DayNight
