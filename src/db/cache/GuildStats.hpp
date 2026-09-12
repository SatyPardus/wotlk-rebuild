#ifndef DB_CACHE_GUILD_STATS_C_HPP
#define DB_CACHE_GUILD_STATS_C_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>
#include "util/Unimplemented.hpp"

class GuildStats_C {
    public:
    static const uint32_t WDB_VERSION = 1;

    // OFFSET: 0x75B3E0
    void Unpack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    // OFFSET: 0x98E3F0
    void Pack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    private:
    /* 0x00 */ uint8_t m_unk00[764];
};

#endif
