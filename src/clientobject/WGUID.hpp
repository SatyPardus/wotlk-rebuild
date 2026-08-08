#ifndef CLIENTOBJECT_WGUID_HPP
#define CLIENTOBJECT_WGUID_HPP

#include <cstdint>
#include "common/DataStore.hpp"
#include <os/Debug.hpp>

class WGUID {
    public:
    uint32_t guid_low;
    uint32_t guid_high;

    WGUID()
        : guid_low(0)
        , guid_high(0) {}

    WGUID(uint64_t value)
        : guid_low(static_cast<uint32_t>(value & 0xFFFFFFFFu))
        , guid_high(static_cast<uint32_t>((value >> 32) & 0xFFFFFFFFu)) {}

    explicit operator uint64_t() const {
        return (static_cast<uint64_t>(guid_high) << 32) | guid_low;
    }

    explicit operator bool() const {
        return guid_low != 0 || guid_high != 0;
    }

    bool operator==(const WGUID& other) const {
        return guid_low == other.guid_low && guid_high == other.guid_high;
    }
    bool operator!=(const WGUID& other) const {
        return !(*this == other);
    }

    bool operator==(uint64_t value) const {
        return guid_low == static_cast<uint32_t>(value & 0xFFFFFFFFu) &&
               guid_high == static_cast<uint32_t>((value >> 32) & 0xFFFFFFFFu);
    }
    bool operator!=(uint64_t value) const {
        return !(*this == value);
    }
};

inline bool operator==(uint64_t value, const WGUID& guid) {
    return guid == value;
}

inline bool operator!=(uint64_t value, const WGUID& guid) {
    return guid != value;
}

inline CDataStore& operator>>(CDataStore& msg, WGUID& guid) {
    uint64_t guidFull = 0;

    uint8_t mask;
    msg.Get(mask);

    for (int32_t i = 0; i < 8; i++) {
        if (mask & (1 << i)) {
            uint8_t byte;
            msg.Get(byte);

            guidFull |= static_cast<uint64_t>(byte) << (i * 8);
        }
    }

    OsOutputDebugString("GUID %ull\n", guidFull);

    guid = guidFull;

    return msg;
}

#endif // CLIENTOBJECT_WGUID_HPP
