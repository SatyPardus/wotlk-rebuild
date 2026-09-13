#include "clientobject/Mirror.hpp"
#include "clientobject/ObjectMgrClient.hpp"
#include <cstring>
#include <bc/Memory.hpp>
#include "clientobject/CGObject_C.hpp"
#include "clientobject/Unit_C.hpp"
#include "clientobject/Player_C.hpp"
#include "clientobject/Container_C.hpp"
#include "clientobject/Corpse_C.hpp"
#include "clientobject/GameObject_C.hpp"
#include "clientobject/DynamicObject_C.hpp"

STORM_EXPLICIT_LIST(CMirrorHandler, m_link) g_globalMirrorList[NUM_CLIENT_OBJECT_TYPES][CGPlayer::GetTotalFieldCount()];

// OFFSET: 0x4D5720
void Mirror_FreeChildren(STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* lists, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        lists[i].Clear();
    }
}

// OFFSET: 0x4D5770
void Mirror_ClearLists(CGObject_C* obj) {
    switch (obj->m_typeID) {
    case ID_ITEM:
        Mirror_FreeChildren(obj->AsItem()->m_itemMirrorLists, CGItem::GetFieldCount());
        break;
    case ID_CONTAINER:
        Mirror_FreeChildren(obj->AsContainer()->m_containerMirrorLists, CGContainer::GetFieldCount());
        Mirror_FreeChildren(obj->AsContainer()->m_itemMirrorLists, CGItem::GetFieldCount());
        break;
    case ID_UNIT:
        Mirror_FreeChildren(obj->AsUnit()->m_unitMirrorLists, CGUnit::GetFieldCount());
        break;
    case ID_PLAYER:
        Mirror_FreeChildren(obj->AsPlayer()->m_playerMirrorLists, CGPlayer::GetRemoteFieldCount());
        if (obj->AsPlayer()->IsActivePlayer())
            Mirror_FreeChildren(obj->AsPlayer()->m_playerLocalMirrorLists, CGPlayer::GetLocalFieldCount());
        Mirror_FreeChildren(obj->AsPlayer()->m_unitMirrorLists, CGUnit::GetFieldCount());
        break;
    case ID_GAMEOBJECT:
        Mirror_FreeChildren(obj->AsGameObject()->m_gameObjectMirrorLists, CGGameObject::GetFieldCount());
        break;
    case ID_DYNAMICOBJECT:
        Mirror_FreeChildren(obj->AsDynamicObject()->m_dynamicObjectMirrorLists, CGDynamicObject::GetFieldCount());
        break;
    case ID_CORPSE:
        Mirror_FreeChildren(obj->AsCorpse()->m_corpseMirrorLists, CGCorpse::GetFieldCount());
        break;
    }

    Mirror_FreeChildren(obj->m_objMirrorLists, CGObject::GetFieldCount());
}

// OFFSET: 0x4D4850
void Mirror_ExpirePending(STORM_EXPLICIT_LIST(CMirrorHandler, m_link2)* pending) {
    for (CMirrorHandler* handler = pending->Head(); handler;) {
        CMirrorHandler* next = pending->Next(handler);

        if (--handler->count == 0) {
            pending->UnlinkNode(handler);
        }

        handler = next;
    }
}

// OFFSET: 0x4D3D40
STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* GetObjectMirrorList(OBJECT_TYPE_ID typeId, CGObject_C* obj, uint32_t index) {
    switch (typeId) {
    case ID_OBJECT:
        return &obj->m_objMirrorLists[index];
    case ID_ITEM:
        return &obj->AsItem()->m_itemMirrorLists[index];
    case ID_CONTAINER:
        return &obj->AsContainer()->m_containerMirrorLists[index];
    case ID_UNIT:
        return &obj->AsUnit()->m_unitMirrorLists[index];
    case ID_PLAYER:
        if (obj->AsUnit()->IsActivePlayer() && index >= CGPlayer::GetRemoteFieldCount()) {
            return &obj->AsPlayer()->m_playerLocalMirrorLists[index - CGPlayer::GetRemoteFieldCount()];
        }
        return &obj->AsPlayer()->m_playerMirrorLists[index];
    case ID_GAMEOBJECT:
        return &obj->AsGameObject()->m_gameObjectMirrorLists[index];
    case ID_DYNAMICOBJECT:
        return &obj->AsDynamicObject()->m_dynamicObjectMirrorLists[index];
    case ID_CORPSE:
        return &obj->AsCorpse()->m_corpseMirrorLists[index];
    }

    return nullptr;
}

// OFFSET: 0x4D5350
int32_t Mirror_LinkPending(STORM_EXPLICIT_LIST(CMirrorHandler, m_link2)* pending, STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* list) {
    if (!list) {
        return 0;
    }

    if (list->IsEmpty()) {
        return 0;
    }

    for (CMirrorHandler* handler = list->Head(); handler; handler = list->Next(handler)) {
        uint32_t misalign = handler->fieldByteOffset & 3;

        pending->LinkNode(handler, handler->linkPositionSelector == 1 ? STORM_LIST_HEAD : STORM_LIST_TAIL, nullptr);

        handler->count = (handler->fieldByteSize + misalign + 3) >> 2;
    }

    return 1;
}

// OFFSET: 0x4D52B0
void Mirror_CopyFields(STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* list, CGObject_C* obj) {
    auto objMgr = g_tlsBlock.pObjMgr;

    WGUID playerGuid;

    if (objMgr) {
        playerGuid = objMgr->playerGuid;
    }

    int32_t localPlayer = obj->m_obj->m_guid == playerGuid;

    for (CMirrorHandler* handler = list->Head(); handler; handler = list->Next(handler)) {
        uint32_t mirrorOffset = ObjDescriptorToMirrorOffset(obj->m_typeID, localPlayer, handler->fieldByteOffset, handler->fieldByteSize);

        uint8_t* dst = reinterpret_cast<uint8_t*>(obj->m_objMirror) + mirrorOffset;
        uint8_t* src = reinterpret_cast<uint8_t*>(obj->m_obj) + handler->fieldByteOffset;

        memcpy(dst, src, handler->fieldByteSize);
    }
}

// OFFSET: 0x4D43F0
uint32_t ObjDescriptorToMirrorOffset(OBJECT_TYPE_ID typeId, int32_t localPlayer, uint32_t fieldByteOffset, uint32_t fieldByteSize) {
    switch (typeId) {
    case ID_CONTAINER:
        if (fieldByteOffset >= CGItem::GetDataSize()) {
            return CGContainer::DescriptorToMirrorOffset(fieldByteOffset, fieldByteSize, 0, CGItem::GetDataSize());
        }
        // fall through
    case ID_ITEM:
        if (fieldByteOffset >= CGObject::GetDataSize()) {
            return CGItem::DescriptorToMirrorOffset(fieldByteOffset, fieldByteSize, 0, CGObject::GetDataSize());
        }
        break;

    case ID_PLAYER:
        if (fieldByteOffset >= CGUnit::GetDataSize()) {
            return CGPlayer::DescriptorToMirrorOffset(fieldByteOffset, fieldByteSize, localPlayer, CGUnit::GetDataSize());
        }
        // fall through
    case ID_UNIT:
        if (fieldByteOffset >= CGObject::GetDataSize()) {
            return CGUnit::DescriptorToMirrorOffset(fieldByteOffset, fieldByteSize, localPlayer, CGObject::GetDataSize());
        }
        break;

    case ID_GAMEOBJECT:
        if (fieldByteOffset >= CGObject::GetDataSize()) {
            return CGGameObject::DescriptorToMirrorOffset(fieldByteOffset, fieldByteSize, 0, CGObject::GetDataSize());
        }
        break;

    case ID_DYNAMICOBJECT:
        if (fieldByteOffset >= CGObject::GetDataSize()) {
            return CGDynamicObject::DescriptorToMirrorOffset(fieldByteOffset, fieldByteSize, 0, CGObject::GetDataSize());
        }
        break;

    case ID_CORPSE:
        if (fieldByteOffset >= CGObject::GetDataSize()) {
            return CGCorpse::DescriptorToMirrorOffset(fieldByteOffset, fieldByteSize, 0, CGObject::GetDataSize());
        }
        break;

    default:
        break;
    }

    return (fieldByteOffset & 3) + 4 * CGObject::MirrorIndexFromFieldIndex(fieldByteOffset >> 2);
}

// OFFSET: 0x4D44F0
uint32_t TypeDescriptorToMirrorOffset(OBJECT_TYPE_ID typeId, uint32_t dataOffset, uint32_t fieldByteSize) {
    switch (typeId) {
    case ID_ITEM:
        return CGItem::DescriptorToMirrorOffset(dataOffset, fieldByteSize, 0, 0);

    case ID_CONTAINER:
        return CGContainer::DescriptorToMirrorOffset(dataOffset, fieldByteSize, 0, 0);

    case ID_UNIT:
        return CGUnit::DescriptorToMirrorOffset(dataOffset, fieldByteSize, 0, 0);

    case ID_PLAYER:
        return CGPlayer::DescriptorToMirrorOffset(dataOffset, fieldByteSize, 0, 0);

    case ID_GAMEOBJECT:
        return CGGameObject::DescriptorToMirrorOffset(dataOffset, fieldByteSize, 0, 0);

    case ID_DYNAMICOBJECT:
        return CGDynamicObject::DescriptorToMirrorOffset(dataOffset, fieldByteSize, 0, 0);

    case ID_CORPSE:
        return CGCorpse::DescriptorToMirrorOffset(dataOffset, fieldByteSize, 0, 0);

    default:
        break;
    }

    return (dataOffset & 3) + 4 * CGObject::MirrorIndexFromFieldIndex(dataOffset >> 2);
}

// OFFSET: 0x4D3BF0
uint32_t GetObjectTypeFieldByteOffset(OBJECT_TYPE_ID typeId) {
    switch (typeId) {
    case ID_ITEM:
    case ID_UNIT:
    case ID_GAMEOBJECT:
    case ID_DYNAMICOBJECT:
    case ID_CORPSE:
        return CGObject::GetDataSize();
    case ID_CONTAINER:
        return CGItem::GetDataSize();
    case ID_PLAYER:
        return CGUnit::GetDataSize();
    default:
        return 0;
    }
}

// OFFSET: 0x4D5150
void CallMirrorFunctions(STORM_EXPLICIT_LIST(CMirrorHandler, m_link2)* pending, WGUID guid, CGObject_C* obj, OBJECT_TYPE_ID typeId) {
    CMirrorHandler* handler = pending->Head();

    while (handler) {
        uint8_t* mirror = reinterpret_cast<uint8_t*>(obj->m_objMirror) + handler->mirrorByteOffset;

        handler->m_dispatching = 1;
        handler->count = 1;

        int32_t changed = 1;

        if (!handler->m_alwaysFire) {
            uint8_t* field = reinterpret_cast<uint8_t*>(obj->m_obj) + handler->fieldByteOffset;
            changed = memcmp(field, mirror, handler->fieldByteSize);
        }

        if (changed) {
            uint32_t blockByteOffset = handler->fieldByteOffset - GetObjectTypeFieldByteOffset(typeId);
            handler->function(guid, blockByteOffset, handler->fieldByteSize, mirror, handler->functionParam);
        }

        CMirrorHandler* next = pending->Next(handler);

        if (handler->m_deletePending) {
            delete handler;
        } else {
            handler->m_dispatching = 0;
        }

        handler = next;
    }
}

// OFFSET: 0x4D5850
CMirrorHandler* AssignMirrorHandler(uint32_t fieldByteOffset, uint32_t mirrorByteOffset, uint32_t fieldByteSize, MIRRORHANDLERFUNC func, void* functionParam, uint32_t linkPositionSelector, int32_t alwaysFire, STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* list) {
    CMirrorHandler* handler = list->NewNode(STORM_LIST_TAIL, 0, 0);

    handler->fieldByteOffset = fieldByteOffset;
    handler->mirrorByteOffset = mirrorByteOffset;
    handler->fieldByteSize = fieldByteSize;
    handler->function = func;
    handler->functionParam = functionParam;
    handler->linkPositionSelector = linkPositionSelector;
    handler->m_dispatching = 0;
    handler->m_deletePending = 0;
    handler->m_alwaysFire = alwaysFire != 0;

    return handler;
}
