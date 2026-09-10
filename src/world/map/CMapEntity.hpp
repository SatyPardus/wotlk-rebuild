#ifndef WORLD_MAP_C_MAP_ENTITY_HPP
#define WORLD_MAP_C_MAP_ENTITY_HPP

#include "storm/list/TSExplicitList.hpp"
#include "storm/list/TSLink.hpp"
#include "world/map/CMapStaticEntity.hpp"
#include "world/map/Types.hpp"
#include "clientobject/WGUID.hpp"

class CMapEntity : public CMapStaticEntity {
    public:
    MAP_OBJECT_FUNC m_func = nullptr;
    void* m_funcParam = nullptr;
    uint64_t m_funcParam64 = 0;
    uint32_t m_funcParam32 = 0;
    uint32_t unk_00A4 = -1;
    float unk_00A8 = 0;
    float unk_00AC = 0;
    float unk_00B0 = 0;
    uint32_t unk_00B4 = 0;
    uint32_t unk_00B8 = 0;
    uint32_t unk_00BC = 0;
    CImVector unk_00C0 = { 0x00, 0x00, 0x00, 0xFF };
    float unk_00C4 = 0.0f;
    TSLink<CMapEntity> sortEntryLink;

    CMapEntity();
};

#endif
