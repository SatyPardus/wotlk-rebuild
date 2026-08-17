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
    static uint32_t TotalFields() {
        return CGObject::TotalFields() + 4;
    }

    static uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGGameObjectData);
    }
};

class CGGameObject_C : public CGObject_C, public CGGameObject {
    public:
    CGGameObject_C();
    CGGameObject_C(CClientObjCreate& objCreate, uint32_t time);

    static void SetStorage(CGGameObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_GAME_OBJECT_C_HPP
