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
    static constexpr uint32_t TotalFields() {
        return CGObject::TotalFields();
    }

    static constexpr uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGDynamicObjectData);
    }

    static constexpr uint32_t GetTotalFieldCount() {
        return CGDynamicObject::GetDataSize() / sizeof(uint32_t);
    }

    static constexpr uint32_t GetFieldCount() {
        return sizeof(CGDynamicObjectData) / sizeof(uint32_t);
    }

    static uint32_t MirrorIndexFromFieldIndex(uint32_t fieldIndex);
    static uint32_t DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset);
};

class CGDynamicObject_C : public CGObject_C, public CGDynamicObject {
    public:

    STORM_EXPLICIT_LIST(CMirrorHandler, m_link) m_dynamicObjectMirrorLists[CGDynamicObject::GetFieldCount()];

    CGDynamicObject_C();
    CGDynamicObject_C(CClientObjCreate& objCreate, uint32_t time);
    void PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3);

    static void SetStorage(CGDynamicObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_DYNAMIC_OBJECT_C_HPP
