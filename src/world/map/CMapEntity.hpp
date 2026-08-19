#ifndef WORLD_MAP_C_MAP_ENTITY_HPP
#define WORLD_MAP_C_MAP_ENTITY_HPP

#include "storm/list/TSExplicitList.hpp"
#include "storm/list/TSLink.hpp"
#include "world/map/CMapStaticEntity.hpp"
#include "world/map/Types.hpp"
#include "clientobject/WGUID.hpp"

class CMapEntity : public CMapStaticEntity {
    public:
    MAP_OBJECT_FUNC m_func;
    void* m_funcParam;
    uint64_t m_funcParam64;
    uint32_t m_funcParam32;
    uint32_t unk_00A4;
    uint32_t unk_00A8;
    uint32_t unk_00AC;
    uint32_t unk_00B0;
    uint32_t unk_00B4;
    uint32_t unk_00B8;
    uint32_t unk_00BC;
    uint32_t unk_00C0;
    uint32_t unk_00C4;
    TSLink<CMapEntity> sortEntryLink;
};

#endif
