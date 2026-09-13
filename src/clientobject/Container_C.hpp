#ifndef CLIENTOBJECT_CONTAINER_C_HPP
#define CLIENTOBJECT_CONTAINER_C_HPP

#include "clientobject/Item_C.hpp"
#include <cstdint>

struct CGContainerData {
    uint32_t CONTAINER_FIELD_NUM_SLOTS;
    uint32_t CONTAINER_ALIGN_PAD;
    WGUID CONTAINER_FIELD_SLOT_1[36];
};

class CGContainer {
    public:
    CGContainerData* m_container;
    void* m_containerMirror;

    // OFFSET: 0x4F5200
    static constexpr uint32_t TotalFields() {
        return CGItem::TotalFields() + 72;
    }

    static constexpr uint32_t GetDataSize() {
        return CGItem::GetDataSize() + sizeof(CGContainer);
    }

    static constexpr uint32_t GetTotalFieldCount() {
        return CGContainer::GetDataSize() / sizeof(uint32_t);
    }

    static constexpr uint32_t GetFieldCount() {
        return sizeof(CGContainer) / sizeof(uint32_t);
    }

    static uint32_t MirrorIndexFromFieldIndex(uint32_t fieldIndex);
    static uint32_t DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset);
};

class CGContainer_C : public CGItem_C, public CGContainer {
    public:

    STORM_EXPLICIT_LIST(CMirrorHandler, m_link) m_containerMirrorLists[CGContainer::GetFieldCount()];

    CGContainer_C();
    CGContainer_C(CClientObjCreate& objCreate, uint32_t time);

    static void SetStorage(CGContainer_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_CONTAINER_C_HPP
