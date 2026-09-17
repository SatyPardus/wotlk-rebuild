#include "clientobject/WGUID.hpp"

// OFFSET: 0x74D0D0
char* GUIDToHexString(uint32_t low, uint32_t high, char* out) {
    out[0] = '0';
    out[1] = 'x';
    out[18] = '\0';

    char* digit = &out[17];

    for (int32_t i = 16; i; i--) {
        uint32_t nibble = low & 0xF;

        *digit-- = static_cast<char>(nibble >= 0xA ? nibble + 0x37 : nibble + 0x30);

        low = (low >> 4) | (high << 28);
        high >>= 4;
    }

    return out;
}
