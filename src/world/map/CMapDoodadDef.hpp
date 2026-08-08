#ifndef WORLD_MAP_C_MAP_DOODAD_DEF_HPP
#define WORLD_MAP_C_MAP_DOODAD_DEF_HPP

#include "storm/list/TSLink.hpp"
#include "tempest/Plane.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include <tempest/Vector.hpp>
#include <model/CM2Model.hpp>

class CMapDoodadDef : public CMapBaseObj, public TSHashObject<CMapDoodadDef, uint32_t> {
    public:
    uint8_t fadeLevel;
    uint8_t unk_025;
    uint8_t unk_026;
    uint8_t unk_027;
    //uint32_t unkFlags_28;
    //int32_t unkCounter;
    //float unk_030;
    CM2Model* model;
    CAaSphere sphere;
    CAaBox bboxStaticEntity;
    C3Vector vec2;
    C3Vector position;
    float scale;
    int32_t unk_07C;
    //int32_t unk_080;
    CImVector m2AmbietColor;
    CImVector m2DiffuseColor;
    float unk_08C;
    uint32_t uniqueId;
    //void* unk_094;
    //void* unk_098;
    //void* prevMapDoodadDef_09C;
    //void* nextMapDoodadDef_0A0;
    //int32_t unk_0A4;
    TSLink<CMapDoodadDef> doodadDefLink;
    //int32_t unk_0B0;
    //int32_t unk_0B4;
    //int32_t unk_0B8;
    //int32_t unk_0BC;
    CAaBox bboxDoodadDef;
    C44Matrix mat;
    C44Matrix identity;
    //int32_t unk_158;
    //int32_t unk_15C;
    //int32_t unk_160;
    //int32_t unk_164;
    //int32_t unk_168;
    //int32_t unk_16C;

    void UpdateBounds();
};

#endif
