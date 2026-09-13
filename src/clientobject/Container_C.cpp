#include "clientobject/Container_C.hpp"

static const uint32_t s_containerMirrorIndex[CGContainer::TotalFields() - CGItem::TotalFields()] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
    62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
};

// OFFSET: 0x4F5210
uint32_t CGContainer::MirrorIndexFromFieldIndex(uint32_t fieldIndex) {
    for (uint32_t i = 0; i < CGContainer::TotalFields() - CGItem::TotalFields(); i++) {
        if (s_containerMirrorIndex[i] == fieldIndex) {
            return i;
        }
    }

    return CGContainer::GetFieldCount();
}

// OFFSET: 0x4D4280
uint32_t CGContainer::DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset) {
    uint32_t mirrorIndex = CGContainer::MirrorIndexFromFieldIndex((fieldByteOffset - baseByteOffset) >> 2);
    return (fieldByteOffset & 3) + 4 * (CGItem::TotalFields() + mirrorIndex);
}

CGContainer_C::CGContainer_C() {
}

CGContainer_C::CGContainer_C(CClientObjCreate& objCreate, uint32_t time)
    : CGItem_C(objCreate, time) {
    
}

// OFFSET: 0x706960
void CGContainer_C::SetStorage(CGContainer_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGItem_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_container = reinterpret_cast<CGContainerData*>(descriptorPtr + CGItem::GetDataSize());
    obj->m_containerMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGItem::TotalFields());
}
