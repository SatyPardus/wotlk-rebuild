#ifndef WORLD_MAP_C_MAP_STATIC_ENTITY_HPP
#define WORLD_MAP_C_MAP_STATIC_ENTITY_HPP

#include "storm/list/TSExplicitList.hpp"
#include "storm/list/TSLink.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include "model/CM2Model.hpp"

class CMapStaticEntity : public CMapBaseObj {
    public:
    int32_t unk_024;
    uint32_t unkFlags_28;
    int32_t unkCounter;
    float unk_030;
    CM2Model* model;
    CAaSphere sphere;
    CAaBox bbox;
    C3Vector vec2;
    C3Vector position;
    float scale;
    int32_t unk_07C;
    int32_t unk_080;
    CImVector m2AmbietColor;
    CImVector m2DiffuseColor;
    float unk_08C;
};

#endif
