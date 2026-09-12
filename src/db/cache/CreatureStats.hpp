#ifndef DB_CACHE_CREATURE_STATS_C_HPP
#define DB_CACHE_CREATURE_STATS_C_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>

class CreatureStats_C {
    public:
    static const uint32_t WDB_VERSION = 1;

    /* 0x00 */ uint32_t m_id;
    /* 0x04 */ char* m_subName;
    /* 0x08 */ char* m_iconName;
    /* 0x0C */ uint32_t m_typeFlags;
    /* 0x10 */ uint32_t m_type;
    /* 0x14 */ uint32_t m_family;
    /* 0x18 */ uint32_t m_rank;
    /* 0x1C */ uint32_t m_killCredit1;
    /* 0x20 */ uint32_t m_killCredit2;
    /* 0x24 */ uint32_t m_displayId1;
    /* 0x28 */ uint32_t m_displayId2;
    /* 0x2C */ uint32_t m_displayId3;
    /* 0x30 */ uint32_t m_displayId4;
    /* 0x34 */ float m_hpModifier;
    /* 0x38 */ float m_mpModifier;
    /* 0x3C */ uint8_t m_racialLeader;
    /* 0x3D */ uint8_t m_unk3D[3];
    /* 0x40 */ uint32_t m_questItem[6];
    /* 0x58 */ uint32_t m_movementId;
    /* 0x5C */ char* m_name[4];

    void Unpack(CDataStore* msg);
    void Pack(CDataStore* msg);
};

#endif
