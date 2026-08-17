#include "clientobject/DynamicObject_C.hpp"

CGDynamicObject_C::CGDynamicObject_C() {
}

CGDynamicObject_C::CGDynamicObject_C(CClientObjCreate& objCreate, uint32_t time)
    : CGObject_C(objCreate, time) {
    
}

// OFFSET: 0x70CBA0
void CGDynamicObject_C::SetStorage(CGDynamicObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_dynamicObject = reinterpret_cast<CGDynamicObjectData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_dynamicObjectMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
