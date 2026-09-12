#ifndef DB_CACHE_NAME_CACHE_HPP
#define DB_CACHE_NAME_CACHE_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>
#include "util/Unimplemented.hpp"

class NameCache {
    public:
    static const uint32_t WDB_VERSION = 4;

    // OFFSET: 0x7F74A0
    void Unpack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    // OFFSET: 0x7F7400
    void Pack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    private:
    /* 0x00 */ uint8_t m_unk00[336];
};

#endif
