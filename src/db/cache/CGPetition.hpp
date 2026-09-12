#ifndef DB_CACHE_CG_PETITION_HPP
#define DB_CACHE_CG_PETITION_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>
#include "util/Unimplemented.hpp"

class CGPetition {
    public:
    static const uint32_t WDB_VERSION = 1;

    // OFFSET: 0x98D090
    void Unpack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    // OFFSET: 0x98CF70
    void Pack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    private:
    /* 0x00 */ uint8_t m_unk00[5064];
};

#endif
