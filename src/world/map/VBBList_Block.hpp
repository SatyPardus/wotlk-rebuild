#ifndef WORLD_MAP_VBBLIST_BLOCK_HPP
#define WORLD_MAP_VBBLIST_BLOCK_HPP

#include "async/CAsyncObject.hpp"
#include "gx/Device.hpp"
#include "storm/list/TSLink.hpp"
#include "tempest/Plane.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include <tempest/Box.hpp>
#include <tempest/Vector.hpp>

class VBBList_Block {
    public:
    TSLink<VBBList_Block> link;
    EGxPoolUsage poolUsage = GxPoolUsage_Static;
    uint32_t offset = 0;
    uint32_t capacity = 0;
    CGxPool* pool = nullptr;
    CGxBuf* buffer = nullptr;
    VBBList_Block* listHead = nullptr;

    void Set(uint32_t itemSize, uint32_t itemCount, uint32_t offset);
};

#endif
