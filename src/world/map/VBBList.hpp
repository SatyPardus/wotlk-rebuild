#ifndef WORLD_MAP_VBBLIST_HPP
#define WORLD_MAP_VBBLIST_HPP

#include "async/CAsyncObject.hpp"
#include "storm/list/TSLink.hpp"
#include "tempest/Plane.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include <tempest/Box.hpp>
#include <tempest/Vector.hpp>
#include "gx/Device.hpp"
#include "world/map/VBBList_Block.hpp"

class VBBList {
    public:
    int32_t singlePool = 0;
    EGxPoolTarget target = GxPoolTarget_Vertex;
    EGxPoolUsage usage = GxPoolUsage_Static;
    CGxPool* pool = nullptr;
    STORM_EXPLICIT_LIST(VBBList_Block, link) unk_10;
    STORM_EXPLICIT_LIST(VBBList_Block, link) unk_1C;

    VBBList_Block* AllocBlock();
    void AllocVBB(VBBList_Block** a1, uint32_t itemSize, uint32_t itemCount);
    void FreeVBB(VBBList_Block* block);
    void AssignBlock(VBBList_Block* a1, VBBList_Block** a2, uint32_t itemSize, uint32_t itemCount);
};

#endif
