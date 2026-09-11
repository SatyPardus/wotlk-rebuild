#include "world/map/CMapStaticEntity.hpp"
#include "world/daynight/DayNight.hpp"
#include "world/daynight/DNInfo.hpp"
#include "world/CWorld.hpp"
#include "world/map/CMap.hpp"

C3Vector CMapStaticEntity::s_interiorSunDir;
float CMapStaticEntity::s_characterAmbient = 1.0f;

// OFFSET: 0x780CD0
void CMapStaticEntity::ModelLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg) {
    CMapStaticEntity* entity = reinterpret_cast<CMapStaticEntity*>(userArg);

    lighting->m_flags |= 0x10;
    auto dayNight = DayNight::GetInfo();
    if (entity) {
        lighting->SetFog(C3Vector(dayNight->m_fog.color), dayNight->m_fog.start, dayNight->m_fog.end, dayNight->m_fog.m_density);
        entity->SelectLights(lighting);
        entity->SelectUnderwater(lighting);
        if ((entity->flags & 2) != 0) {
            lighting->m_flags |= 8;
            return;
        }
    } else {
        lighting->AddAmbient(C3Vector(dayNight->m_light1.m_ambient));
        lighting->AddDiffuse(C3Vector(dayNight->m_light1.m_diffuse), dayNight->m_light1.m_dir);
    }
    lighting->m_flags &= ~8u;
}

// OFFSET: 0x7C1730
void CMapStaticEntity::SelectLights(CM2Lighting* lighting) {
    if (CWorld::farFog > this->m_distanceToCamera) {
        if ((this->flags & 1) != 0) {
            this->flags &= ~1u;
        }

        C3Vector diffuse = { 0.0f, 0.0f, 0.0f };
        C3Vector dir = { 0.0f, 0.0f, 0.0f };
        C3Vector ambient = { 0.0f, 0.0f, 0.0f };

        if ((this->flags & 4) != 0) {
            CM2Light* light = &CMap::s_mapLight->m_light;

            diffuse.x = light->m_dirColor.x * this->diffuseLightScale;
            diffuse.y = light->m_dirColor.y * this->diffuseLightScale;
            diffuse.z = light->m_dirColor.z * this->diffuseLightScale;

            dir = light->m_dir;

            ambient = C3Vector(this->m2AmbietColor);
        } else {
            diffuse = C3Vector(this->m2DiffuseColor);
            diffuse.x *= this->diffuseLightScale;
            diffuse.y *= this->diffuseLightScale;
            diffuse.z *= this->diffuseLightScale;

            ambient = C3Vector(this->m2AmbietColor);

            dir = CMapStaticEntity::s_interiorSunDir;

            if ((this->unk_07C & 0x1000) != 0) {
                DayNight::DNInfo* dayNight = DayNight::GetInfo();
                float blend = this->m2DiffuseColor.a / 255.0f;

                dir.x += (dayNight->m_light1.m_dir.x - dir.x) * blend;
                dir.y += (dayNight->m_light1.m_dir.y - dir.y) * blend;
                dir.z += (dayNight->m_light1.m_dir.z - dir.z) * blend;

                dir.Normalize();
            }
        }

        if ((this->unk_07C & 0x8000) != 0) {
            ambient.x *= CMapStaticEntity::s_characterAmbient;
            ambient.y *= CMapStaticEntity::s_characterAmbient;
            ambient.z *= CMapStaticEntity::s_characterAmbient;

            if (ambient.x >= 1.0f) {
                ambient.x = 1.0f;
            }

            if (ambient.y >= 1.0f) {
                ambient.y = 1.0f;
            }

            if (ambient.z >= 1.0f) {
                ambient.z = 1.0f;
            }
        }

        lighting->AddAmbient(ambient);
        lighting->AddDiffuse(diffuse, dir);
    }

    DayNight::DNInfo* dayNight = DayNight::GetInfo();

    if ((this->flags & 0x8000) != 0) {
        lighting->SetFog(C3Vector(dayNight->m_fogInterior.color), dayNight->m_fogInterior.start, dayNight->m_fogInterior.end, dayNight->m_fogInterior.m_density);
    } else {
        lighting->SetFog(C3Vector(dayNight->m_fog.color), dayNight->m_fog.start, dayNight->m_fog.end, dayNight->m_fog.m_density);
    }
}

void CMapStaticEntity::SelectUnderwater(CM2Lighting* lighting) {

}

void CMapStaticEntity::QueryInteriorLighting(CM2Lighting* lighting) {

}
