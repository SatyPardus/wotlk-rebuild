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
    static uint32_t TotalFields() {
        return CGItem::TotalFields() + 72;
    }

    static uint32_t GetDataSize() {
        return CGItem::GetDataSize() + sizeof(CGContainer);
    }
};

class CGContainer_C : public CGItem_C, public CGContainer {
    public:
    CGContainer_C();
    CGContainer_C(CClientObjCreate& objCreate, uint32_t time);

    static void SetStorage(CGContainer_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_CONTAINER_C_HPP
