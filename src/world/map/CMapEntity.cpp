#include "world/map/CMapEntity.hpp"

// OFFSET: 0x7A0FE0
CMapEntity::CMapEntity() {
    this->unk_00A8 = 0.0;
    this->unk_00AC = 0.0;
    this->sortEntryLink.m_prevlink = 0;
    this->unk_00B0 = 0.0;
    this->sortEntryLink.m_next = 0;
    this->unk_00A8 = 0.0;
    this->unk_00AC = 0.0;
    this->m_func = 0;
    this->m_funcParam = 0;
    this->m_funcParam64 = 0;
    this->m_funcParam32 = 0;
    this->unk_00B4 = 0;
    this->unk_00B8 = 0;
    this->unk_00BC = 0;
    this->unk_00A4 = -1;
    this->unk_00B0 = 0.0f;
    this->unk_00C0 = { 0x00, 0x00, 0x00, 0xFF };
    this->unk_00C4 = 0.0;
    this->type |= 0x20u;
}
