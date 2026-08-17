#ifndef CLIENTOBJECT_CORPSE_C_HPP
#define CLIENTOBJECT_CORPSE_C_HPP

#include "clientobject/CGObject_C.hpp"
#include <cstdint>

struct CGCorpseData {
    WGUID CORPSE_FIELD_OWNER;
    WGUID CORPSE_FIELD_PARTY;
    uint32_t CORPSE_FIELD_DISPLAY_ID;
    uint32_t CORPSE_FIELD_ITEM[19];
    uint32_t CORPSE_FIELD_BYTES_1;
    uint32_t CORPSE_FIELD_BYTES_2;
    uint32_t CORPSE_FIELD_GUILD;
    uint32_t CORPSE_FIELD_FLAGS;
    uint32_t CORPSE_FIELD_DYNAMIC_FLAGS;
    uint32_t CORPSE_FIELD_PAD;
};

class CGCorpse {
    public:
    CGCorpseData* m_corpse;
    void* m_corpseMirror;

    // OFFSET: 0x4F5680
    static uint32_t TotalFields() {
        return CGObject::TotalFields() + 3;
    }

    static uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGCorpseData);
    }
};

class CGCorpse_C : public CGObject_C, public CGCorpse {
    public:
    CGCorpse_C();
    CGCorpse_C(CClientObjCreate& objCreate, uint32_t time);

    static void SetStorage(CGCorpse_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_CORPSE_C_HPP
