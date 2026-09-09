#include "gx/shader/CShaderEffect.hpp"
#include "gx/Device.hpp"
#include "gx/Gx.hpp"
#include "gx/RenderState.hpp"
#include "gx/Shader.hpp"
#include "gx/Transform.hpp"
#include "model/CM2Lighting.hpp"
#include <algorithm>
#include <cstring>
#include "model/CM2Light.hpp"
#include "tempest/Vector.hpp"

CShaderEffect* CShaderEffect::s_curEffect;
int32_t CShaderEffect::s_enableShaders;
C4Vector CShaderEffect::s_fogColorAlphaRef;
float CShaderEffect::s_fogMul;
C4Vector CShaderEffect::s_fogParams;
int32_t CShaderEffect::s_lightEnabled;
uint32_t CShaderEffect::s_localLightCount;
CShaderEffect::LocalLights CShaderEffect::s_localLights;
C3Vector CShaderEffect::s_sunAmbient;
C3Vector CShaderEffect::s_sunDiffuse;
C3Vector CShaderEffect::s_sunDir;
int32_t CShaderEffect::s_useAlphaRef;
int32_t CShaderEffect::s_usePcfFiltering;
int32_t CShaderEffect::s_shadowValue = 0;

// OFFSET: 0x872900
void CShaderEffect::ComputeLocalLights(LocalLights* localLights, uint32_t localLightsCount, CM2Light** lights, const C3Vector* origin) {
    uint32_t i = 0;

    if (localLightsCount) {
        C44Matrix xform;

        if (origin) {
            xform = g_theGxDevicePtr->m_xforms[GxXform_World].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_World].m_level];
        }

        do {
            CM2Light* light = lights[i];

            if (light->m_type == 1) {
                localLights->color[i].x = light->m_dirColor.x;
                localLights->color[i].y = light->m_dirColor.y;
                localLights->color[i].z = light->m_dirColor.z;
                localLights->color[i].w = 1.0f;

                if (origin) {
                    C3Vector rel = {
                        light->m_pos.x - origin->x,
                        light->m_pos.y - origin->y,
                        light->m_pos.z - origin->z,
                    };

                    C3Vector pos = xform.TransformPoint(rel);

                    localLights->position[i].x = pos.x;
                    localLights->position[i].y = pos.y;
                    localLights->position[i].z = pos.z;
                } else {
                    localLights->position[i].x = light->m_viewPos.x;
                    localLights->position[i].y = light->m_viewPos.y;
                    localLights->position[i].z = light->m_viewPos.z;
                }

                localLights->position[i].w = 1.0f;

                localLights->attenConstant[i] = light->m_constantAttenuation;
                localLights->attenLinear[i] = light->m_linearAttenuation;
                localLights->attenQuadratic[i] = light->m_quadraticAttenuation;
            } else {
                localLights->color[i].x = 0.0f;
                localLights->color[i].y = 0.0f;
                localLights->color[i].z = 0.0f;
                localLights->color[i].w = 0.0f;
            }

            i++;
        } while (i < localLightsCount);
    }

    while (i < 4) {
        localLights->color[i].x = 0.0f;
        localLights->color[i].y = 0.0f;
        localLights->color[i].z = 0.0f;
        localLights->color[i].w = 0.0f;

        i++;
    }
}

void CShaderEffect::InitShaderSystem(int32_t enableShaders, int32_t usePcf) {
    CShaderEffect::s_enableShaders = enableShaders;
    CShaderEffect::s_usePcfFiltering = enableShaders && usePcf ? 1 : 0;
    CShaderEffect::s_fogMul = 1.0f;

    CShaderEffect::s_useAlphaRef = GxCaps().int130;
}

void CShaderEffect::SetAlphaRef(float alphaRef) {
    CShaderEffect::s_fogColorAlphaRef.w = alphaRef;

    if (CShaderEffect::s_useAlphaRef) {
        GxRsSet(GxRs_AlphaRef, static_cast<int32_t>(alphaRef * 255.0f));
    } else {
        GxShaderConstantsSet(GxSh_Pixel, 2, reinterpret_cast<C4Vector*>(&CShaderEffect::s_fogColorAlphaRef), 1);
    }
}

// OFFSET: 0x872DE0
int32_t CShaderEffect::SelectShadowShader() {
    if (CShaderEffect::s_fogColorAlphaRef.w <= 0.0f || (GxCaps().int130 && !CShaderEffect::s_shadowValue)) {

        return CShaderEffect::s_shadowValue + 4 * CShaderEffect::s_usePcfFiltering;
    }

    return CShaderEffect::s_shadowValue + 4 * (CShaderEffect::s_usePcfFiltering + 2);
}

// OFFSET: 0x873EE0
void CShaderEffect::SetAlphaRefDefault() {
    float alpha = CGxDevice::s_alphaRef[g_theGxDevicePtr->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0]] / 255.0f;
    CShaderEffect::SetAlphaRef(alpha);
 }

void CShaderEffect::SetDiffuse(const C4Vector& diffuse) {
    if (CShaderEffect::s_enableShaders) {
        GxShaderConstantsSet(GxSh_Vertex, 28, reinterpret_cast<const C4Vector*>(&diffuse), 1);
        return;
    }

    // TODO
    // - non-shader code path
}

void CShaderEffect::SetEmissive(const C4Vector& emissive) {
    if (CShaderEffect::s_enableShaders) {
        GxShaderConstantsSet(GxSh_Vertex, 29, reinterpret_cast<const C4Vector*>(&emissive), 1);
        return;
    }

    // TODO non-shader code path
}

void CShaderEffect::SetFogEnabled(int32_t fogEnabled) {
    if (fogEnabled && GxMasterEnable(GxMasterEnable_Fog)) {
        if (CShaderEffect::s_enableShaders && !GxCaps().int138) {
            GxShaderConstantsSet(GxSh_Vertex, 30, reinterpret_cast<C4Vector*>(&CShaderEffect::s_fogParams), 1);
        } else {
            GxRsSet(GxRs_Fog, 1);
        }
    } else {
        if (CShaderEffect::s_enableShaders && !GxCaps().int138) {
            C4Vector fogParams = { 0.0f, 1.0f, 1.0f, 0.0f };
            GxShaderConstantsSet(GxSh_Vertex, 30, &fogParams, 1);
        } else {
            GxRsSet(GxRs_Fog, 0);
        }
    }
}

void CShaderEffect::SetFogParams(float fogStart, float fogEnd, float fogRate, const CImVector& fogColor) {
    if (CShaderEffect::s_enableShaders) {
        CShaderEffect::s_fogColorAlphaRef.x = fogColor.r / 255.0f;
        CShaderEffect::s_fogColorAlphaRef.y = fogColor.g / 255.0f;
        CShaderEffect::s_fogColorAlphaRef.z = fogColor.b / 255.0f;

        float v4 = 1.0f / (fogEnd - fogStart);
        CShaderEffect::s_fogParams.x = -(CShaderEffect::s_fogMul * v4);
        CShaderEffect::s_fogParams.y = fogEnd * v4;
        CShaderEffect::s_fogParams.z = fogRate;
        CShaderEffect::s_fogParams.w = 0.0f;

        if (!GxCaps().int134) {
            GxShaderConstantsSet(GxSh_Pixel, 2, reinterpret_cast<C4Vector*>(&CShaderEffect::s_fogColorAlphaRef), 1);
            return;
        }
    } else {
        GxRsSet(GxRs_FogStart, fogStart);
        GxRsSet(GxRs_FogEnd, fogEnd);
    }

    GxRsSet(GxRs_FogColor, fogColor.value);
}

void CShaderEffect::SetLocalLighting(CM2Lighting* lighting, int32_t lightEnabled, const C3Vector* a3) {
    CShaderEffect::s_lightEnabled = lightEnabled;

    if (!CShaderEffect::s_enableShaders) {
        GxRsSet(GxRs_Lighting, lightEnabled);
    }

    CShaderEffect::s_localLightCount = lighting ? lighting->m_lightCount : 0;

    if (!lightEnabled) {
        return;
    }

    if (CShaderEffect::s_enableShaders) {
        CShaderEffect::s_sunDir = lighting->m_sunDir;

        if (CShaderEffect::s_sunDir.x != 0.0f || CShaderEffect::s_sunDir.y != 0.0f || CShaderEffect::s_sunDir.z != 0.0f) {
            CShaderEffect::s_sunDir.Normalize();
        }

        CShaderEffect::s_sunAmbient = lighting->m_sunAmbient;

        CShaderEffect::s_sunDiffuse = {
            std::min(lighting->m_sunDiffuse.x, 1.0f),
            std::min(lighting->m_sunDiffuse.y, 1.0f),
            std::min(lighting->m_sunDiffuse.z, 1.0f)
        };

        GxShaderConstantsSet(GxSh_Vertex, 10, &C4Vector(CShaderEffect::s_sunDiffuse), 1);
        GxShaderConstantsSet(GxSh_Vertex, 11, &C4Vector(CShaderEffect::s_sunAmbient), 1);
        GxShaderConstantsSet(GxSh_Vertex, 12, &C4Vector(CShaderEffect::s_sunDir), 1);

        if (CShaderEffect::s_localLightCount) {
            CShaderEffect::ComputeLocalLights(
                &CShaderEffect::s_localLights,
                CShaderEffect::s_localLightCount,
                lighting->m_lights,
                a3
            );

            GxShaderConstantsSet(GxSh_Vertex, 17, reinterpret_cast<C4Vector*>(&CShaderEffect::s_localLights), 11);
        }

        // TODO
        // CShadowCache::SetShadowMapGenericInterior(lighting->m_flags & 0x8);
    } else {
        lighting->SetupGxLights(a3);
    }
}

void CShaderEffect::SetShaders(uint32_t vertexPermute, uint32_t pixelPermute) {
    int32_t useAlphaRef = 1;

    if (CShaderEffect::s_enableShaders) {
        GxRsSet(GxRs_VertexShader, CShaderEffect::s_curEffect->m_vertexShaders[vertexPermute]);
        GxRsSet(GxRs_PixelShader, CShaderEffect::s_curEffect->m_pixelShaders[pixelPermute]);

        useAlphaRef = (pixelPermute & 0x8) == 0;
    }

    if (CShaderEffect::s_useAlphaRef != useAlphaRef) {
        CShaderEffect::s_useAlphaRef = useAlphaRef;

        if (useAlphaRef) {
            GxRsSet(GxRs_AlphaRef, static_cast<uint8_t>(CShaderEffect::s_fogColorAlphaRef.w * 255.0f));
        } else {
            GxShaderConstantsSet(GxSh_Pixel, 2, reinterpret_cast<C4Vector*>(&CShaderEffect::s_fogColorAlphaRef), 1);
            GxRsSet(GxRs_AlphaRef, 0);
        }
    }
}

void CShaderEffect::SetTexMtx_Identity(uint32_t a1) {
    if (CShaderEffect::s_enableShaders) {
        float matrix[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f
        };

        GxShaderConstantsSet(GxSh_Vertex, 2 * a1 + 6, reinterpret_cast<C4Vector*>(matrix), 2);
    } else {
        // TODO
        // - non-shader code path
    }
}

// OFFSET: 0x873160
void CShaderEffect::SetDefaultShaders(uint32_t value) {
    if (!CShaderEffect::s_enableShaders) {
        return;
    }

    uint32_t shadow = CShaderEffect::s_shadowValue;

    if (shadow > 2) {
        shadow = 2;
    }

    if (value > 2) {
        value = 2;
    }

    uint32_t permute = value + (3 * shadow);
    uint32_t vertexPermute = CShaderEffect::s_lightEnabled + (2 * (CShaderEffect::s_localLightCount + (5 * permute)));

    CShaderEffect::SetShaders(vertexPermute, CShaderEffect::SelectShadowShader());
}

// OFFSET: 0x872B00
void CShaderEffect::UpdateWorldViewMatrix() {
    if (!CShaderEffect::s_enableShaders) {
        return;
    }

    C44Matrix view = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
    C44Matrix world = g_theGxDevicePtr->m_xforms[GxXform_World].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_World].m_level];

    C44Matrix worldView = world * view;
    worldView = worldView.Transpose();

    GxShaderConstantsSet(GxSh_Vertex, 31, reinterpret_cast<const C4Vector*>(&worldView), 4);
}

// OFFSET: 0x873620
void CShaderEffect::SetTexMtx(C44Matrix& mat, uint32_t a2) {
    if (CShaderEffect::s_enableShaders) {
        float matrix[] = {
            mat.a0, mat.b0, mat.c0, mat.d0,
            mat.a1, mat.b1, mat.c1, mat.d1
        };
        GxShaderConstantsSet(GxSh_Vertex, 2 * a2 + 6, reinterpret_cast<C4Vector*>(&matrix), 2);
    } else {
        // TODO
        // - non-shader code path
    }
}

void CShaderEffect::SetTexMtx_SphereMap(uint32_t a1) {
    if (CShaderEffect::s_enableShaders) {
        float matrix[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f
        };

        GxShaderConstantsSet(GxSh_Vertex, 2 * a1 + 6, reinterpret_cast<C4Vector*>(matrix), 2);
    } else {
        // TODO
        // - non-shader code path
    }
}

void CShaderEffect::UpdateProjMatrix() {
    if (!CShaderEffect::s_enableShaders) {
        return;
    }

    C44Matrix proj;
    GxXformProjNativeTranspose(proj);

    GxShaderConstantsSet(GxSh_Vertex, 2, reinterpret_cast<C4Vector*>(&proj), 4);
}

void CShaderEffect::InitEffect(const char* vsName, const char* psName) {
    memset(this->m_vertexShaders, 0, sizeof(this->m_vertexShaders));
    memset(this->m_pixelShaders, 0, sizeof(this->m_pixelShaders));

    this->m_fixedFuncOpCount = 0;

    if (CShaderEffect::s_enableShaders) {
        if (vsName && psName) {
            g_theGxDevicePtr->ShaderCreate(this->m_vertexShaders, GxSh_Vertex, "Shaders\\Vertex", vsName, 90);
            g_theGxDevicePtr->ShaderCreate(this->m_pixelShaders, GxSh_Pixel, "Shaders\\Pixel", psName, 16);
        }
    }
}

// OFFSET: 0x8728C0
void CShaderEffect::InitFixedFuncPass(const uint32_t* colorOps, const uint32_t* alphaOps, uint32_t count) {
    this->m_fixedFuncOpCount = count;

    for (uint32_t i = 0; i < count; i++) {
        this->m_colorOps[i] = colorOps[i];
        this->m_alphaOps[i] = alphaOps[i];
    }
}

void CShaderEffect::SetCurrent() {
    CShaderEffect::s_curEffect = this;

    // TODO
    // - non-shader code path
}
