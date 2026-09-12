#ifndef DB_CACHE_QUEST_CACHE_HPP
#define DB_CACHE_QUEST_CACHE_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstdint>
#include "util/Unimplemented.hpp"

class QuestCache {
    public:
    static const uint32_t WDB_VERSION = 3;

    // OFFSET: 0x7F70E0
    void Unpack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    // OFFSET: 0x7F6D60
    void Pack(CDataStore* msg) {
        WHOA_UNIMPLEMENTED();
    }

    private:
    /* 0x00 */ uint8_t m_unk00[10468];
};

#endif
