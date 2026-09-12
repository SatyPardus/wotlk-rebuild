#ifndef DB_CACHE_ITEM_TEXT_CACHE_C_HPP
#define DB_CACHE_ITEM_TEXT_CACHE_C_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>
#include "util/Unimplemented.hpp"

class ItemTextCache_C {
    public:
    static const uint32_t WDB_VERSION = 1;

    void Unpack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    void Pack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    private:
    /* 0x00 */ uint8_t m_unk00[8000];
};

#endif
