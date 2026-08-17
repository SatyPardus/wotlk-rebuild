#include "clientobject/Container_C.hpp"

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
