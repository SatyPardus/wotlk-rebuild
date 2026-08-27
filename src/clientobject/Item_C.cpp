#include "clientobject/Item_C.hpp"

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
