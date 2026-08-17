#include "clientobject/Corpse_C.hpp"

CGCorpse_C::CGCorpse_C() {
}

CGCorpse_C::CGCorpse_C(CClientObjCreate& objCreate, uint32_t time) : CGObject_C(objCreate, time) {
    
}

// OFFSET: 0x70CBA0
void CGCorpse_C::SetStorage(CGCorpse_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_corpse = reinterpret_cast<CGCorpseData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_corpseMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
