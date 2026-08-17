#include "clientobject/GameObject_C.hpp"

CGGameObject_C::CGGameObject_C() {
}

CGGameObject_C::CGGameObject_C(CClientObjCreate& objCreate, uint32_t time)
    : CGObject_C(objCreate, time) {
    
}

// OFFSET: 0x70CBA0
void CGGameObject_C::SetStorage(CGGameObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_gameObject = reinterpret_cast<CGGameObjectData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_gameObjectMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
