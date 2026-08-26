#include "world/map/CMapObjDefGroup.hpp"
#include "world/map/CMap.hpp"
#include "world/CWorld.hpp"
#include "world/daynight/DayNight.hpp"
#include "world/daynight/DNInfo.hpp"

// OFFSET: 0x7B3F30
void CMapObjDefGroup::SelectLights(CM2Lighting* lighting) {
    lighting->AddLight(&CMap::s_mapLight->m_light);
    //if (CWorld::farFog > this->distanceToCamera) {
    //    flags = this->flags;
    //    if ((flags & 1) != 0)
    //        this->flags = flags & 0xFFFFFFFE;
    //    unk_98 = this->unk_98;
    //    if ((unk_98 & 1) != 0 || !unk_98)
    //        unk_98 = 0;
    //    while ((unk_98 & 1) == 0 && unk_98) {
    //        v5 = *(unk_98 + 4);
    //        if (*(v5 + 184)) {
    //            if (!*(v5 + 209))
    //                CM2Lighting::AddLight(a2, (v5 + 88));
    //        }
    //        unk_98 = *(unk_98 + this->unk_90 + 4);
    //    }
    //}

    auto dayNight = DayNight::GetInfo();
    C3Vector fogColor;
    float fogStart;
    float fogEnd;
    float fogDensity;
    if ((this->flags & 0x8000) != 0) {
        fogColor = C3Vector(dayNight->m_fogInterior.color);
        fogDensity = dayNight->m_fogInterior.m_density;
        fogEnd = dayNight->m_fogInterior.end;
        fogStart = dayNight->m_fogInterior.start;
    } else {
        fogColor = C3Vector(dayNight->m_fog.color);
        fogDensity = dayNight->m_fog.m_density;
        fogEnd = dayNight->m_fog.end;
        fogStart = dayNight->m_fog.start;
    }
    lighting->SetFog(fogColor, fogStart, fogEnd, fogDensity);
}

// OFFSET: 0x7BDD70
void CMapObjDefGroup::MarkPrepared() {
    this->flags |= 0x10;
}
