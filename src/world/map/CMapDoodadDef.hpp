#ifndef WORLD_MAP_C_MAP_DOODAD_DEF_HPP
#define WORLD_MAP_C_MAP_DOODAD_DEF_HPP

#include "storm/list/TSLink.hpp"
#include "tempest/Plane.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include <tempest/Vector.hpp>
#include <model/CM2Model.hpp>
#include "world/map/CMapStaticEntity.hpp"

class CMapDoodadDef : public CMapStaticEntity, public TSHashObject<CMapDoodadDef, uint32_t> {
    public:
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

    void SelectLights(CM2Lighting* lighting) override;
    void SelectUnderwater(CM2Lighting* lighting) override;
    void QueryInteriorLighting(CM2Lighting* lighting) override;

    void UpdateBounds();
    void ExtendBounds(CMapBaseObj* parent);
    void ExtendChunkBounds();
};

#endif
