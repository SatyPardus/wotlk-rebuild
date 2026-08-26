#include "model/CM2Light.hpp"
#include "model/CM2Scene.hpp"
#include "gx/Device.hpp"

// OFFSET: 0x8348D0
void CM2Light::Initialize(CM2Scene* scene) {
    this->m_scene = scene;

    this->m_stamp = scene ? scene->m_frameStamp - 1 : 0;
    // TODO
}

// OFFSET: 0x834C70
void CM2Light::Link() {
    if (!this->m_visible || !this->m_scene) {
        return;
    }

    if (this->m_type == M2LIGHT_1) {
        if (!this->m_scene->m_lightGrid) {
            const size_t gridBytes = 64 * 64 * sizeof(CM2Light*);
            this->m_scene->m_lightGrid = static_cast<CM2Light**>(STORM_ALLOC(gridBytes));

            memset(this->m_scene->m_lightGrid, 0, gridBytes);
        }

        int32_t row = (static_cast<int32_t>(floor(this->m_pos.y * 0.05f)) & 0x3F) << 6;
        int32_t col = static_cast<int32_t>(floor(this->m_pos.x * 0.05f)) & 0x3F;

        CM2Light** bucket = &this->m_scene->m_lightGrid[row + col];

        this->m_lightPrev = bucket;
        this->m_lightNext = *bucket;
        *bucket = this;

        if (this->m_lightNext) {
            this->m_lightNext->m_lightPrev = &this->m_lightNext;
        }
    } else {
        if (!(this->m_scene->m_flags & 0x1)) {
            this->m_lightPrev = &this->m_scene->m_lightList;
            this->m_lightNext = this->m_scene->m_lightList;
            this->m_scene->m_lightList = this;

            if (this->m_lightNext) {
                this->m_lightNext->m_lightPrev = &this->m_lightNext;
            }
        }
    }
}

void CM2Light::SetDirection(const C3Vector& dir) {
    this->m_dir = dir;

    if (this->m_dir.SquaredMag() > 0.00000023841858) {
        this->m_dir.Normalize();
    }
}

void CM2Light::SetPosition(const C3Vector& pos) {
    this->m_pos = pos;

    if (this->m_visible && this->m_scene && this->m_type == M2LIGHT_1) {
        this->Unlink();
        this->Link();
    }
}

void CM2Light::SetLightType(M2LIGHTTYPE lightType) {
    if (this->m_type == lightType) {
        return;
    }

    this->m_type = lightType;

    if (this->m_visible && this->m_scene) {
        this->Unlink();
        this->Link();
    }
}

void CM2Light::SetVisible(int32_t visible) {
    if (this->m_visible == visible) {
        return;
    }

    this->m_visible = visible;

    if (this->m_scene) {
        if (this->m_visible) {
            this->Link();
        } else {
            this->Unlink();
        }
    }
}

void CM2Light::Unlink() {
    if (this->m_lightPrev) {
        *this->m_lightPrev = this->m_lightNext;
    }

    if (this->m_lightNext) {
        this->m_lightNext->m_lightPrev = this->m_lightPrev;
    }

    this->m_lightPrev = nullptr;
    this->m_lightNext = nullptr;
}

// OFFSET: 0x834B50
void CM2Light::ApplyGxLight(uint32_t index) {
    CGxLight light;

    light.m_flags = (light.m_flags & ~0x1u) | (this->m_visible & 0x1u);

    if (this->m_type) {
        light.m_flags |= 0x2;

        light.m_dir = this->m_pos;

        light.m_ambientColor = { 0.0f, 0.0f, 0.0f };
        light.m_specularColor = { 0.0f, 0.0f, 0.0f };
    } else {
        light.m_flags &= ~0x2u;

        light.m_dir = this->m_dir;
        light.m_ambientColor = this->m_ambColor;
        light.m_specularColor = this->m_specColor;
    }

    light.m_dirColor = this->m_dirColor;
    light.m_constantAttenuation = this->m_constantAttenuation;
    light.m_linearAttenuation = this->m_linearAttenuation;
    light.m_quadraticAttenuation = this->m_quadraticAttenuation;

    C3Vector origin = { 0.0f, 0.0f, 0.0f };

    g_theGxDevicePtr->LightSet(index, light, origin);
    g_theGxDevicePtr->LightEnable(index, 1);
}
