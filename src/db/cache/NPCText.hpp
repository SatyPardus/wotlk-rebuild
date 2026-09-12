#ifndef DB_CACHE_NPC_TEXT_HPP
#define DB_CACHE_NPC_TEXT_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>
#include "util/Unimplemented.hpp"

class NPCText {
    public:
    static const uint32_t WDB_VERSION = 1;

    // OFFSET: 0x98E2C0
    void Unpack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    // OFFSET: 0x98E1F0
    void Pack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    private:
    /* 0x00 */ uint8_t m_unk00[320];
};

#endif
