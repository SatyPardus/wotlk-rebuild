#ifndef DB_CACHE_ARENA_TEAM_CACHE_HPP
#define DB_CACHE_ARENA_TEAM_CACHE_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>
#include "util/Unimplemented.hpp"

class ArenaTeamCache {
    public:
    static const uint32_t WDB_VERSION = 1;

    // OFFSET: 0x98D220
    void Unpack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    // OFFSET: 0x98D1B0
    void Pack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    private:
    /* 0x00 */ uint8_t m_unk00[124];
};

#endif
