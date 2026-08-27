#include "world/map/CMapStaticEntity.hpp"
#include "world/daynight/DayNight.hpp"
#include "world/daynight/DNInfo.hpp"

C3Vector CMapStaticEntity::s_interiorSunDir;

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

void CMapStaticEntity::SelectLights(CM2Lighting* lighting) {

}

void CMapStaticEntity::SelectUnderwater(CM2Lighting* lighting) {

}

void CMapStaticEntity::QueryInteriorLighting(CM2Lighting* lighting) {

}
