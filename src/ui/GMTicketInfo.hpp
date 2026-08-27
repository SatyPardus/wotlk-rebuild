#ifndef UI_GM_TICKET_INFO_HPP
#define UI_GM_TICKET_INFO_HPP

#include <cstdint>

struct StaticConstant {
    const char* name;
    uint32_t value;
};

namespace GMTicketInfo {

    static StaticConstant s_staticConstants[7] = {
        { "STATIC_CONSTANTS", 0 },
        { "Loot", 1 },
        { "AuctionHouse", 2 },
        { "Mail", 3 },
        { "Chat", 4 },
        { "Movement", 5 },
        { "Spell", 6 },
    };

}

#endif
