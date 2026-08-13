#ifndef WORLD_MAP_C_PORTAL_VIEW_HPP
#define WORLD_MAP_C_PORTAL_VIEW_HPP

#include "storm/list/TSLink.hpp"
#include "tempest/Plane.hpp"
#include "world/map/Types.hpp"
#include <tempest/Vector.hpp>

class CPortalView {
    public:
    CRect rect;
    float maxViewDepth;
    C3Vector* verts;
    uint32_t vertCount;
};

#endif
