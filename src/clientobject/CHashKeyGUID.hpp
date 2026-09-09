#ifndef CLIENTOBJECT_C_HASH_KEY_GUID_HPP
#define CLIENTOBJECT_C_HASH_KEY_GUID_HPP

#include "clientobject/WGUID.hpp"
#include <cstdint>

class CHashKeyGUID {
    public:
    // Public member functions
    CHashKeyGUID();
    CHashKeyGUID(WGUID guid);
    bool operator==(WGUID guid) const;
    bool operator==(const CHashKeyGUID& key) const;
    WGUID m_guid;
};

#endif // CLIENTOBJECT_C_HASH_KEY_GUID_HPP
