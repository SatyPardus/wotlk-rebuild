#ifndef CLIENTOBJECT_GAME_OBJECT_C_HPP
#define CLIENTOBJECT_GAME_OBJECT_C_HPP

#include "clientobject/CGObject_C.hpp"
#include <cstdint>

struct CGGameObjectData {
    WGUID OBJECT_FIELD_CREATED_BY;
    uint32_t GAMEOBJECT_DISPLAYID;
    uint32_t GAMEOBJECT_FLAGS;
    float GAMEOBJECT_PARENTROTATION[4];
    uint32_t GAMEOBJECT_DYNAMIC;
    uint32_t GAMEOBJECT_FACTION;
    uint32_t GAMEOBJECT_LEVEL;
    uint32_t GAMEOBJECT_BYTES_1;
};

class CGGameObject {
    public:
    CGGameObjectData* m_gameObject;
    void* m_gameObjectMirror;

    // OFFSET: 0x4F55B0
    static constexpr uint32_t TotalFields() {
        return CGObject::TotalFields() + 4;
    }

    static constexpr uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGGameObjectData);
    }

    static constexpr uint32_t GetTotalFieldCount() {
        return CGGameObject::GetDataSize() / sizeof(uint32_t);
    }

    static constexpr uint32_t GetFieldCount() {
        return sizeof(CGGameObjectData) / sizeof(uint32_t);
    }

    static uint32_t MirrorIndexFromFieldIndex(uint32_t fieldIndex);
    static uint32_t DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset);
};

class CGGameObject_C : public CGObject_C, public CGGameObject {
    public:

    STORM_EXPLICIT_LIST(CMirrorHandler, m_link) m_gameObjectMirrorLists[CGGameObject::GetFieldCount()];

    CGGameObject_C();
    CGGameObject_C(CClientObjCreate& objCreate, uint32_t time);
    void PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3);

    static void SetStorage(CGGameObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_GAME_OBJECT_C_HPP
