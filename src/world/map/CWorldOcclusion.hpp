#ifndef WORLD_MAP_C_WORLD_OCCLUSION_HPP
#define WORLD_MAP_C_WORLD_OCCLUSION_HPP

#include "storm/list/TSLink.hpp"
#include "world/map/Types.hpp"
#include <tempest/Vector.hpp>

class CWorldOcclusion {
    public:

    static void ClearVolumes();
    static uint32_t QueryVolumes(CAaSphere* sphere);
    static uint32_t QueryBuffer(CAaBox* box, uint8_t flags);
};

#endif
