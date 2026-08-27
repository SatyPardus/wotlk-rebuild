#ifndef CLIENTOBJECT_DYNAMIC_OBJECT_C_HPP
#define CLIENTOBJECT_DYNAMIC_OBJECT_C_HPP

#include "clientobject/CGObject_C.hpp"
#include <cstdint>

struct CGDynamicObjectData {
    WGUID DYNAMICOBJECT_CASTER;
    uint32_t DYNAMICOBJECT_BYTES;
    uint32_t DYNAMICOBJECT_SPELLID;
    float DYNAMICOBJECT_RADIUS;
    uint32_t DYNAMICOBJECT_CASTTIME;
};

class CGDynamicObject {
    public:
    CGDynamicObjectData* m_dynamicObject;
    void* m_dynamicObjectMirror;

    // OFFSET: 0x4F5670
    static uint32_t TotalFields() {
        return CGObject::TotalFields();
    }

    static uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGDynamicObjectData);
    }
};

class CGDynamicObject_C : public CGObject_C, public CGDynamicObject {
    public:
    CGDynamicObject_C();
    CGDynamicObject_C(CClientObjCreate& objCreate, uint32_t time);
    void PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3);

    static void SetStorage(CGDynamicObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_DYNAMIC_OBJECT_C_HPP
