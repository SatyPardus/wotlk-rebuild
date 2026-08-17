#include "clientobject/CGObject_C.hpp"
#include "clientobject/ObjectMgrClient.hpp"

CGObject_C::CGObject_C() {
    
}

CGObject_C::CGObject_C(CClientObjCreate& objCreate, uint32_t time) {

    ClntObjMgrLinkInNewObject(this);
}

void CGObject_C::SetTypeID(OBJECT_TYPE_ID typeID) {
    this->m_typeID = typeID;

    switch (typeID) {
    case ID_OBJECT:
        this->m_obj->m_type = HIER_TYPE_OBJECT;
        break;

    case ID_ITEM:
        this->m_obj->m_type = HIER_TYPE_ITEM;
        break;

    case ID_CONTAINER:
        this->m_obj->m_type = HIER_TYPE_CONTAINER;
        break;

    case ID_UNIT:
        this->m_obj->m_type = HIER_TYPE_UNIT;
        break;

    case ID_PLAYER:
        this->m_obj->m_type = HIER_TYPE_PLAYER;
        break;

    case ID_GAMEOBJECT:
        this->m_obj->m_type = HIER_TYPE_GAMEOBJECT;
        break;

    case ID_DYNAMICOBJECT:
        this->m_obj->m_type = HIER_TYPE_DYNAMICOBJECT;
        break;

    case ID_CORPSE:
        this->m_obj->m_type = HIER_TYPE_CORPSE;
        break;

    default:
        break;
    }
}

void CGObject_C::AddWorldObject() {

}

void CGObject_C::SetData(uint32_t offset, uint32_t value) {
    reinterpret_cast<uint32_t*>(this->m_obj)[offset] = value;
}

// OFFSET: 0x743640
void CGObject_C::SetStorage(CGObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    obj->m_obj = reinterpret_cast<CGObjectData*>(descriptorPtr);
    obj->m_objMirror = reinterpret_cast<void*>(mirrorPtr);
}

bool CGObject_C::GetModelFileName(const char** fileName) {
    *fileName = nullptr;
    return false;
}
