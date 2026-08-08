#include "clientobject/CHashKeyGUID.hpp"

CHashKeyGUID::CHashKeyGUID() {
    this->m_guid = 0;
}

CHashKeyGUID::CHashKeyGUID(WGUID guid) {
    this->m_guid = guid;
}

bool CHashKeyGUID::operator==(WGUID guid) const {
    return this->m_guid == guid;
}

bool CHashKeyGUID::operator==(const CHashKeyGUID& key) const {
    return this->m_guid == key.m_guid;
}
