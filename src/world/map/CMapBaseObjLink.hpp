#ifndef WORLD_MAP_C_MAP_BASE_OBJ_LINK_HPP
#define WORLD_MAP_C_MAP_BASE_OBJ_LINK_HPP

#include "storm/list/TSLink.hpp"
#include "world/map/Types.hpp"

class CMapBaseObj;

class CMapBaseObjLink {
    public:
    int32_t objectIndex;
    CMapBaseObj* owner;
    CMapBaseObj* ref;
    TSLink<CMapBaseObjLink> refLink;
    TSLink<CMapBaseObjLink> ownerLink;
};

#endif
