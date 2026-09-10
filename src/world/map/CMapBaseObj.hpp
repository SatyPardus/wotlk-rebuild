#ifndef WORLD_MAP_C_MAP_BASE_OBJ_HPP
#define WORLD_MAP_C_MAP_BASE_OBJ_HPP

#include "world/map/Types.hpp"
#include "world/map/CMapBaseObjLink.hpp"
#include "storm/list/TSLink.hpp"
#include "storm/list/TSExplicitList.hpp"
#include "world/map/CMapHandle.hpp"

class CM2Lighting;

class CMapBaseObj : public CMapHandle {
    public:
    uint16_t type = 1;
    uint16_t refCount = 0;
    uint32_t flags = 0;
    TSLink<CMapBaseObj> lameAssLink;
    STORM_EXPLICIT_LIST(CMapBaseObjLink, ownerLink) parentLinkList;

    virtual void SelectLights(CM2Lighting* lighting);
    virtual void SelectUnderwater(CM2Lighting* lighting);
};

#endif
