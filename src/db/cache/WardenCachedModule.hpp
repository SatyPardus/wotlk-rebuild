#ifndef DB_CACHE_WARDEN_CACHED_MODULE_HPP
#define DB_CACHE_WARDEN_CACHED_MODULE_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>
#include "util/Unimplemented.hpp"

class WardenCachedModule {
    public:
    static const uint32_t WDB_VERSION = 1;

    void Unpack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    void Pack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    private:
    /* 0x00 */ uint8_t m_unk00[8];
};

#endif
