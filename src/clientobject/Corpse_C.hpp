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
    static constexpr uint32_t TotalFields() {
        return CGObject::TotalFields() + 3;
    }

    static constexpr uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGCorpseData);
    }

    static constexpr uint32_t GetTotalFieldCount() {
        return CGCorpse::GetDataSize() / sizeof(uint32_t);
    }

    static constexpr uint32_t GetFieldCount() {
        return sizeof(CGCorpseData) / sizeof(uint32_t);
    }

    static uint32_t MirrorIndexFromFieldIndex(uint32_t fieldIndex);
    static uint32_t DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset);
};

class CGCorpse_C : public CGObject_C, public CGCorpse {
    public:

    STORM_EXPLICIT_LIST(CMirrorHandler, m_link) m_corpseMirrorLists[CGCorpse::GetFieldCount()];

    CGCorpse_C();
    CGCorpse_C(CClientObjCreate& objCreate, uint32_t time);
    void PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3);

    static void SetStorage(CGCorpse_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_CORPSE_C_HPP
