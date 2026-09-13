#include "clientobject/Item_C.hpp"

static const uint32_t s_itemMirrorIndex[CGItem::TotalFields() - CGObject::TotalFields()] = {
    0, 1, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 54, 55,
};

// OFFSET: 0x4F51D0
uint32_t CGItem::MirrorIndexFromFieldIndex(uint32_t fieldIndex) {
    for (uint32_t i = 0; i < CGItem::TotalFields() - CGObject::TotalFields(); i++) {
        if (s_itemMirrorIndex[i] == fieldIndex) {
            return i;
        }
    }

    return CGItem::GetFieldCount();
}

// OFFSET: 0x4D42C0
uint32_t CGItem::DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset) {
    uint32_t mirrorIndex = CGItem::MirrorIndexFromFieldIndex((fieldByteOffset - baseByteOffset) >> 2);
    return (fieldByteOffset & 3) + 4 * (CGObject::TotalFields() + mirrorIndex);
}

CGItem_C::CGItem_C() {
}

CGItem_C::CGItem_C(CClientObjCreate& objCreate, uint32_t time)
    : CGObject_C(objCreate, time) {
    
}

// OFFSET: 0x70AF80
void CGItem_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    this->CGObject_C::PostInit(time, objCreate, isUpdate3);
    //maybe_CGItem_C__RequestItemCacheInfo(this);
}

// OFFSET: 0x706D30
void CGItem_C::SetStorage(CGItem_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_item = reinterpret_cast<CGItemData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_itemMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
