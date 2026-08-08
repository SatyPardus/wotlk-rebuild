#ifndef WORLD_MAP_C_MAP_OBJ_DEF_HPP
#define WORLD_MAP_C_MAP_OBJ_DEF_HPP

#include "storm/list/TSLink.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/CMapObj.hpp"
#include "world/map/Types.hpp"
#include "storm/Hash.hpp"

class CMapObjDef : public CMapBaseObj, public TSHashObject<CMapObjDef, uint32_t> {
    public:
    C3Vector position;
    CAaBox bbox;
    CAaSphere sphere;
    C44Matrix mat;
    C44Matrix invMat;
    int32_t nameId;
    CMapObj* owner;
    int32_t unk_F8;
    uint32_t unkFlags;
    int32_t doodadSet;
    int32_t nameSet;
    int32_t unk_108;
    int32_t unk_10C;
    int32_t unk_110;
    //TSExplicitList_CMapObjDefMapObjDefGroupLink mapObjDefGroupLinkList;
    //TSGrowableArray_CMapObjDefGroup defGroups;
    uint32_t groupCount;
    //TSGrowableArray unk;
    uint32_t argbColor;
    int32_t unk_148;
    int32_t unk_14C;
    int32_t unk_150;
    int32_t unk_154;
};

#endif
