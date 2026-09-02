#include "world/map/CMapDoodadDef.hpp"
#include "model/CM2Shared.hpp"
#include <world/CWorldView.hpp>
#include "world/CWorldMath.hpp"
#include "world/map/CMap.hpp"
#include "world/daynight/DayNight.hpp"
#include "world/daynight/DNInfo.hpp"
#include "world/map/CMapChunk.hpp"

// OFFSET: 0x7BDB10
void CMapDoodadDef::UpdateBounds() {
    CAaSphere sphere;
    CAaBox box;
    CAaBox m2Box;

    if (this->model) {
        CAaBox boundingBox = this->model->GetBoundingBox();
        CAaSphere boundingSphere = this->model->GetBoundingSphere();

        if (this->model->m_shared->m_m2DataLoaded == 0)
            this->model->WaitForLoad(nullptr);

        box = boundingBox;
        sphere = boundingSphere;
        m2Box = this->model->m_shared->m_data->bounds.extent;
    }
    sphere.c = this->mat.TransformPoint(sphere.c);
    sphere.r *= this->scale;
    this->sphere = sphere;
    if (box.b.x <= box.t.x || box.b.y <= box.t.y || box.b.z <= box.t.z) {
        CWorldMath::TransformAABox(this->mat, box, this->bbox);
    } else {
        this->bbox.b = this->position;
        this->bbox.t = this->position;
    }
    CWorldMath::TransformAABox(this->mat, m2Box, this->bboxDoodadDef);
    C3Vector v23;
    v23.x = (m2Box.t.x + m2Box.b.x) * 0.5;
    v23.y = (m2Box.t.y + m2Box.b.y) * 0.5;
    v23.z = 0.5 * (m2Box.t.z + m2Box.b.z);
    this->vec2 = this->mat.TransformPoint(v23);
    float v16 = std::max(
        this->bbox.t.x - this->bbox.b.x,
        std::max(
            this->bbox.t.y - this->bbox.b.y,
            this->bbox.t.z - this->bbox.b.z
        )
    );
    uint8_t fadeLevel;
    for (fadeLevel = 0; fadeLevel < 4u; ++fadeLevel) {
        if (v16 <= CWorldView::s_fadeSize[fadeLevel])
            break;
    }
    this->fadeLevel = fadeLevel;
    if ((CWorld::s_enables & CWorld::Enables::Enable_8000) == 0 && fadeLevel < 3u)
        this->model->f_flags &= ~0x40u;
}

// OFFSET: 0x7B4FA0
void CMapDoodadDef::ExtendBounds(CMapBaseObj* parent) {
    if (parent->type & 0x4) {
        CMapChunk* chunk = static_cast<CMapChunk*>(parent);
        if (this->bbox.t.z >= chunk->bbox.t.z)
            chunk->bbox.t.z = this->bbox.t.z;
    } else if (parent->type & 0x10) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(parent);
        group->bbox |= this->bbox;

        CMapBaseObjLink* link = group->parentLinkList.Head();
        static_cast<CMapObjDef*>(link->ref)->bbox |= this->bbox;
    }
}

// OFFSET: 0x7B55E0
void CMapDoodadDef::ExtendChunkBounds() {
    for (CMapBaseObjLink* def = this->parentLinkList.Head(); def; def = this->parentLinkList.Next(def)) {
        this->ExtendBounds(def->ref);
    }
}

// OFFSET: 0x7C1150
void CMapDoodadDef::SelectLights(CM2Lighting* lighting) {
    CM2Light* mapLight = &CMap::s_mapLight->m_light;
    if ((this->flags & 2) != 0) {
        lighting->AddAmbient(this->m2AmbietColor);
        lighting->AddDiffuse(this->m2DiffuseColor, CMapStaticEntity::s_interiorSunDir);
    } else {
        lighting->AddAmbient(mapLight->m_ambColor);
        lighting->AddDiffuse(mapLight->m_dirColor * this->diffuseLightScale, mapLight->m_dir);
    }

    auto dayNight = DayNight::GetInfo();
    if ((this->flags & 0x8000) != 0) {
        lighting->SetFog(C3Vector(dayNight->m_fogInterior.color), dayNight->m_fogInterior.start, dayNight->m_fogInterior.end, dayNight->m_fogInterior.m_density);
    } else {
        lighting->SetFog(C3Vector(dayNight->m_fog.color), dayNight->m_fog.start, dayNight->m_fog.end, dayNight->m_fog.m_density);
    }

    if ((this->flags & 2) != 0)
        lighting->m_flags |= 8;
    else
        lighting->m_flags &= ~8;
}

void CMapDoodadDef::SelectUnderwater(CM2Lighting* lighting) {
}

void CMapDoodadDef::QueryInteriorLighting(CM2Lighting* lighting) {
}
