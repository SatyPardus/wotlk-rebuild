#include "model/CM2Lighting.hpp"
#include "model/CM2Light.hpp"
#include "model/CM2Scene.hpp"
#include "tempest/Matrix.hpp"
#include "gx/Device.hpp"
#include <cstring>

void CM2Lighting::AddAmbient(const C3Vector& ambColor) {
    this->m_sunAmbient = this->m_sunAmbient + ambColor;
}

void CM2Lighting::AddDiffuse(const C3Vector& dirColor, const C3Vector& dir) {
    C3Vector viewDir = dir;

    if (this->m_scene) {
        viewDir = {
            this->m_scene->m_view.a0 * dir.x + this->m_scene->m_view.b0 * dir.y + this->m_scene->m_view.c0 * dir.z,
            this->m_scene->m_view.a1 * dir.x + this->m_scene->m_view.b1 * dir.y + this->m_scene->m_view.c1 * dir.z,
            this->m_scene->m_view.a2 * dir.x + this->m_scene->m_view.b2 * dir.y + this->m_scene->m_view.c2 * dir.z
        };
    }

    this->vector18.x = viewDir.x * dirColor.x + this->vector18.x;
    this->vector18.y = viewDir.y * dirColor.x + this->vector18.y;
    this->vector18.z = viewDir.z * dirColor.x + this->vector18.z;

    this->vector24.x = viewDir.x * dirColor.y + this->vector24.x;
    this->vector24.y = viewDir.y * dirColor.y + this->vector24.y;
    this->vector24.z = viewDir.z * dirColor.y + this->vector24.z;

    this->vector30.x = viewDir.x * dirColor.z + this->vector30.x;
    this->vector30.y = viewDir.y * dirColor.z + this->vector30.y;
    this->vector30.z = viewDir.z * dirColor.z + this->vector30.z;

    float v7 = dirColor.y * 0.71516001f + dirColor.x * 0.212671f + dirColor.z * 0.072168998f;

    this->vector3C.x = viewDir.x * v7 + this->vector3C.x;
    this->vector3C.y = viewDir.y * v7 + this->vector3C.y;
    this->vector3C.z = viewDir.z * v7 + this->vector3C.z;

    this->vector48.x = dirColor.x + this->vector48.x;
    this->vector48.y = dirColor.y + this->vector48.y;
    this->vector48.z = dirColor.z + this->vector48.z;

    this->m_sunDir = dir;

    this->m_sunDiffuse = dirColor;
}

void CM2Lighting::AddLight(CM2Light* light) {
    if (!light->m_visible) {
        return;
    }

    if (light->m_type == 1) {
        float dx = light->m_pos.x - this->sphere4.c.x;
        float dy = light->m_pos.y - this->sphere4.c.y;
        float dz = light->m_pos.z - this->sphere4.c.z;

        float distSq = dx * dx + dy * dy + dz * dz;

        uint32_t slot = this->m_lightCount;

        if (slot >= 4) {
            if (distSq >= this->m_lightDist[3]) {
                return;
            }

            slot--;
        }

        while (slot != 0) {
            if (distSq > this->m_lightDist[slot - 1]) {
                break;
            }

            this->m_lights[slot] = this->m_lights[slot - 1];
            this->m_lightDist[slot] = this->m_lightDist[slot - 1];

            slot--;
        }

        this->m_lightDist[slot] = distSq;
        this->m_lights[slot] = light;

        if (this->m_lightCount < 4) {
            this->m_lightCount++;
        }
    } else {
        this->AddAmbient(light->m_ambColor);
        this->AddDiffuse(light->m_dirColor, light->m_dir);
        this->AddSpecular(light->m_specColor);
    }
}

void CM2Lighting::AddSpecular(const C3Vector& specColor) {
    this->m_sunSpecular = this->m_sunSpecular + specColor;
}

void CM2Lighting::CameraSpace() {
    if (this->m_flags & 0x1) {
        return;
    }

    if (this->m_scene) {
        for (uint32_t i = 0; i < this->m_lightCount; i++) {
            this->m_lights[i]->m_viewPos = this->m_scene->m_view.TransformPoint(this->m_lights[i]->m_pos);
        }

        if ((this->m_flags & 0x60) == 0x60) {
            C44Matrix& view = this->m_scene->m_view;

            const C3Vector onPlane = {
                -this->m_liquidPlane.d * this->m_liquidPlane.n.x,
                -this->m_liquidPlane.d * this->m_liquidPlane.n.y,
                -this->m_liquidPlane.d * this->m_liquidPlane.n.z,
            };

            C3Vector normal = {
                view.a0 * this->m_liquidPlane.n.x + view.b0 * this->m_liquidPlane.n.y + view.c0 * this->m_liquidPlane.n.z,
                view.a1 * this->m_liquidPlane.n.x + view.b1 * this->m_liquidPlane.n.y + view.c1 * this->m_liquidPlane.n.z,
                view.a2 * this->m_liquidPlane.n.x + view.b2 * this->m_liquidPlane.n.y + view.c2 * this->m_liquidPlane.n.z,
            };

            this->m_liquidPlane.n.x = normal.x;
            this->m_liquidPlane.n.y = normal.y;
            this->m_liquidPlane.n.z = normal.z;

            float lenSq = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;

            if (lenSq > 0.00000023841858) {
                float inv = 1.0f / sqrt(lenSq);

                this->m_liquidPlane.n.x *= inv;
                this->m_liquidPlane.n.y *= inv;
                this->m_liquidPlane.n.z *= inv;
            }

            C3Vector pt = view.TransformPoint(onPlane);

            this->m_liquidPlane.d = -(this->m_liquidPlane.n.x * pt.x + this->m_liquidPlane.n.y * pt.y + this->m_liquidPlane.n.z * pt.z);
        }
    }

    this->m_flags |= 0x1;
}

void CM2Lighting::Initialize(CM2Scene* scene, const CAaSphere& a3) {
    memset(this, 0, sizeof(CM2Lighting));

    this->m_scene = scene;
    this->m_flags |= 0x20u;
    this->sphere4 = a3;
}

void CM2Lighting::SetFog(const C3Vector& fogColor, float fogStart, float fogEnd) {
    this->m_fogStart = fogStart;
    this->m_fogEnd = fogEnd;
    this->m_fogScale = 1.0f / (fogEnd - fogStart);
    this->m_fogDensity = 1.0f;
    this->m_fogColor = fogColor;
}

void CM2Lighting::SetFog(const C3Vector& fogColor, float fogStart, float fogEnd, float fogDensity) {
    this->m_fogStart = fogStart;
    this->m_fogEnd = fogEnd;
    this->m_fogScale = 1.0f / (fogEnd - fogStart);
    this->m_fogDensity = fogDensity;
    this->m_fogColor = fogColor;
}

void CM2Lighting::SetupGxLights(const C3Vector* origin) {
    CGxLight light;

    light.m_flags |= 0x1;

    this->SetupSunlight();

    light.m_flags &= ~0x2u;
    light.m_dir = this->m_sunDir;
    light.m_ambientColor = this->m_sunAmbient;
    light.m_dirColor = this->m_sunDiffuse;
    light.m_specularColor = this->m_sunSpecular;

    C3Vector zero = { 0.0f, 0.0f, 0.0f };

    g_theGxDevicePtr->LightSet(0, light, zero);
    g_theGxDevicePtr->LightEnable(0, 1);

    uint32_t remaining = this->m_lightCount;

    light.m_flags |= 0x2;
    light.m_ambientColor = { 0.0f, 0.0f, 0.0f };
    light.m_specularColor = { 0.0f, 0.0f, 0.0f };

    int32_t slot = 1;

    while (remaining != 0 && slot < 4) {
        CM2Light* pointLight = this->m_lights[--remaining];

        C3Vector pos;

        if (origin) {
            pos = {
                pointLight->m_pos.x - origin->x,
                pointLight->m_pos.y - origin->y,
                pointLight->m_pos.z - origin->z,
            };
        } else {
            pos = pointLight->m_viewPos;
        }

        light.m_dir = pos;
        light.m_dirColor = pointLight->m_dirColor;
        light.m_constantAttenuation = pointLight->m_constantAttenuation;
        light.m_linearAttenuation = pointLight->m_linearAttenuation;
        light.m_quadraticAttenuation = pointLight->m_quadraticAttenuation;

        C3Vector lightOrigin = { 0.0f, 0.0f, 0.0f };

        g_theGxDevicePtr->LightSet(slot, light, lightOrigin);
        g_theGxDevicePtr->LightEnable(slot, 1);

        slot++;
    }

    while (slot < 4) {
        g_theGxDevicePtr->LightEnable(slot, 0);
        slot++;
    }
}

void CM2Lighting::SetupSunlight() {
    if (this->m_flags & 0x2) {
        return;
    }

    this->m_sunDir.x = this->vector3C.x;
    this->m_sunDir.y = this->vector3C.y;
    this->m_sunDir.z = this->vector3C.z;

    if (this->m_sunDir.SquaredMag() <= 0.0000099999997) {
        this->m_sunDir = { 0.0f, 0.0f, -1.0f };
    } else {
        this->m_sunDir.Normalize();
    }

    float v6 = this->m_sunDir.z * this->vector18.z + this->m_sunDir.y * this->vector18.y + this->m_sunDir.x * this->vector18.x;
    float v7 = this->m_sunDir.z * this->vector24.z + this->m_sunDir.y * this->vector24.y + this->m_sunDir.x * this->vector24.x;
    float v8 = this->m_sunDir.z * this->vector30.z + this->m_sunDir.y * this->vector30.y + this->m_sunDir.x * this->vector30.x;

    this->m_sunDiffuse = {
        v6 * 1.25f - this->vector48.x * 0.25f,
        v7 * 1.25f - this->vector48.y * 0.25f,
        v8 * 1.25f - this->vector48.z * 0.25f
    };

    this->m_sunAmbient = {
        (this->vector48.x - v6) * 0.25f + this->m_sunAmbient.x,
        (this->vector48.y - v7) * 0.25f + this->m_sunAmbient.y,
        (this->vector48.z - v8) * 0.25f + this->m_sunAmbient.z
    };

    this->m_flags |= 0x2;
}

// OFFSET: 0x4E2730
void CM2Lighting::Reset() {
    this->m_sunAmbient.x = 0.0;
    this->m_sunAmbient.y = 0.0;
    this->m_sunAmbient.z = 0.0;
    this->m_sunDiffuse.x = 0.0;
    this->m_sunDiffuse.y = 0.0;
    this->m_sunDiffuse.z = 0.0;
    this->m_sunSpecular.x = 0.0;
    this->m_sunSpecular.y = 0.0;
    this->m_sunSpecular.z = 0.0;
    this->m_sunDir.x = 0.0;
    this->m_sunDir.y = 0.0;
    this->m_sunDir.z = 0.0;
    this->vector18.x = 0.0;
    this->vector18.y = 0.0;
    this->vector18.z = 0.0;
    this->vector24.x = 0.0;
    this->vector24.y = 0.0;
    this->vector24.z = 0.0;
    this->vector30.x = 0.0;
    this->vector30.y = 0.0;
    this->vector30.z = 0.0;
    this->vector3C.x = 0.0;
    this->vector3C.y = 0.0;
    this->vector3C.z = 0.0;
    this->vector48.x = 0.0;
    this->vector48.y = 0.0;
    this->vector48.z = 0.0;
}
