#include "clientobject/CGObject_C.hpp"

CGObject_C::CGObject_C() {

}

void CGObject_C::SetTypeID(OBJECT_TYPE_ID typeID) {
    this->m_typeID = typeID;

    switch (typeID) {
    case ID_OBJECT:
        this->ObjectData->OBJECT_FIELD_TYPE = HIER_TYPE_OBJECT;
        break;

    case ID_ITEM:
        this->ObjectData->OBJECT_FIELD_TYPE = HIER_TYPE_ITEM;
        break;

    case ID_CONTAINER:
        this->ObjectData->OBJECT_FIELD_TYPE = HIER_TYPE_CONTAINER;
        break;

    case ID_UNIT:
        this->ObjectData->OBJECT_FIELD_TYPE = HIER_TYPE_UNIT;
        break;

    case ID_PLAYER:
        this->ObjectData->OBJECT_FIELD_TYPE = HIER_TYPE_PLAYER;
        break;

    case ID_GAMEOBJECT:
        this->ObjectData->OBJECT_FIELD_TYPE = HIER_TYPE_GAMEOBJECT;
        break;

    case ID_DYNAMICOBJECT:
        this->ObjectData->OBJECT_FIELD_TYPE = HIER_TYPE_DYNAMICOBJECT;
        break;

    case ID_CORPSE:
        this->ObjectData->OBJECT_FIELD_TYPE = HIER_TYPE_CORPSE;
        break;

    default:
        break;
    }
}

void CGObject_C::AddWorldObject() {

}
