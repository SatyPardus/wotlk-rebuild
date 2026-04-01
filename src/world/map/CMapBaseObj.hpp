#ifndef WORLD_MAP_C_MAP_BASE_OBJ_HPP
#define WORLD_MAP_C_MAP_BASE_OBJ_HPP

#include "world/map/Types.hpp"
#include "world/map/CMapBaseObjLink.hpp"
#include "storm/list/TSLink.hpp"
#include "storm/list/TSExplicitList.hpp"

class CMapBaseObj {
    public:
    int32_t objectIndex;
    uint16_t type;
    uint16_t refCount;
    int32_t unk_C;
    TSLink<CMapBaseObj>* lameAssLink;
    STORM_EXPLICIT_LIST(CMapBaseObjLink, ownerLink) parentLinkList;
};

#endif
