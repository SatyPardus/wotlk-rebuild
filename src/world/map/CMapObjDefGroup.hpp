#ifndef WORLD_MAP_C_MAP_OBJ_DEF_GROUP_HPP
#define WORLD_MAP_C_MAP_OBJ_DEF_GROUP_HPP

#include "storm/list/TSLink.hpp"
#include "tempest/Plane.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include <tempest/Vector.hpp>

class CMapObjDefGroup : public CMapBaseObj {
    public:
    CAaBox bbox;
    CAaSphere sphere;
    float unk_4C;
    uint32_t groupNum;
    uint32_t unkFlags;
    int32_t unk_58;
    uint32_t ambientColor;
    int32_t unk_60;
    int32_t unk_64;
    int32_t unk_68;
    int32_t TSExplicitList__m_linkoffset_unk_6C;
    void* TSExplicitList__m_ptr1_unk_70;
    void* TSExplicitList__m_ptr2_unk_74;

    void MarkPrepared();
};

#endif
