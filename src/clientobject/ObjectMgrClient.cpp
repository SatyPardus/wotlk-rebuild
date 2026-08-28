#include "clientobject/ObjectMgrClient.hpp"
#include "client/ClientServices.hpp"
#include "common/DataStore.hpp"
#include <os/Debug.hpp>
#include <util/ZLib.hpp>
#include "clientobject/CClientObjCreate.hpp"
#include "clientobject/Unit_C.hpp"
#include "clientobject/Item_C.hpp"
#include "clientobject/Container_C.hpp"
#include "clientobject/Corpse_C.hpp"
#include "clientobject/GameObject_C.hpp"
#include "clientobject/DynamicObject_C.hpp"
#include <common/ObjectAlloc.hpp>

#define MAX_CHANGE_MASKS 42

thread_local WowTlsBlock g_tlsBlock {};

bool s_heapsAllocated;
uint32_t s_objTotalSize[8] {
    sizeof(uint32_t) * CGObject::TotalFields() + sizeof(CGObject_C) + CGObject::GetDataSize(),
    sizeof(uint32_t) * CGItem::TotalFields() + sizeof(CGItem_C) + CGItem::GetDataSize(),
    sizeof(uint32_t) * CGContainer::TotalFields() + sizeof(CGContainer_C) + CGContainer::GetDataSize(),
    sizeof(uint32_t) * CGUnit::TotalFields() + sizeof(CGUnit_C) + CGUnit::GetDataSize(),
    sizeof(uint32_t) * CGPlayer::TotalRemoteFields() + sizeof(CGPlayer_C) + CGPlayer::GetDataSize(),
    sizeof(uint32_t) * CGGameObject::TotalFields() + sizeof(CGGameObject_C) + CGGameObject::GetDataSize(),
    sizeof(uint32_t) * CGDynamicObject::TotalFields() + sizeof(CGDynamicObject_C) + CGDynamicObject::GetDataSize(),
    sizeof(uint32_t) * CGCorpse::TotalFields() + sizeof(CGCorpse_C) + CGCorpse::GetDataSize()
};
char* s_objNames[8] = {
    "CGObject_C",
    "CGItem_C",
    "CGContainer_C",
    "CGUnit_C",
    "CGPlayer_C",
    "CGGameObject_C",
    "CGDynamicObject_C",
    "CGCorpse_C"
};
uint32_t s_objHeapId[8];
uint32_t s_objHeapSize[8] = {
    0x0,
    0x200,
    0x20,
    0x40,
    0x40,
    0x40,
    0x20,
    0x20
};

// OFFSET: 0x4D6AE0
CGObject_C* GetUpdateObject(WGUID guid, bool* reenable) {
    *reenable = false;
    CGObject_C* obj = GetObjectPtr<CGObject_C*>(&g_tlsBlock.pObjMgr->m_objects, guid);
    if (obj) {
        // obj->SetDisablePending(0);
        return obj;
    }

    obj = GetObjectPtr<CGObject_C*>(&g_tlsBlock.pObjMgr->m_lazyCleanupObjects, guid);
    if (obj) {
        // sub_4D4790(result);
        // TSLink::Unlink(&v9->ukn_0054);
        // TSHashTable__CGObject_C__Insert(&pObjMgr->m_objects, v9, __PAIR64__(v12, a4));
        // v10 = v13->pObjMgr;
        // if (!*(&v9->unk_0004 + v10->unk_A4.m_linkoffset)) {
        //     p_unk_B0 = &v10->unk_B0;
        //     if (!List::Contains(&v10->unk_B0.m_linkoffset, v9)) {
        //         *a3 = 1;
        //         HashTable::AddEntry(p_unk_B0, v9);
        //     }
        // }
        return obj;
    }
    return nullptr;
}

// OFFSET: 0x4D3F10
int32_t ExtractDirtyMasks(CDataStore* msg, uint8_t* maskCount, uint32_t* masks) {
    uint8_t count;
    msg->Get(count);

    *maskCount = count;

    if (count > MAX_CHANGE_MASKS) {
        return 0;
    }

    for (int32_t i = 0; i < count; i++) {
        msg->Get(masks[i]);
    }

    // Zero out masks that aren't present
    memset(&masks[count], 0, (MAX_CHANGE_MASKS - count) * sizeof(uint32_t));

    return 1;
}

int32_t IsMaskBitSet(uint32_t* masks, uint32_t block) {
    return masks[block / 32] & (1 << (block % 32));
}

// OFFSET: 0x4D3DF0
int32_t GetNumDwordBlocks(OBJECT_TYPE mask, WGUID guid) {
    switch (mask) {
    case HIER_TYPE_OBJECT:
        return CGObject::GetDataSize() / sizeof(uint32_t);
    case HIER_TYPE_ITEM:
        return CGItem::GetDataSize() / sizeof(uint32_t);
    case HIER_TYPE_CONTAINER:
        return CGContainer::GetDataSize() / sizeof(uint32_t);
        break;
    case HIER_TYPE_UNIT:
        return CGUnit::GetDataSize() / sizeof(uint32_t);
        break;
    case HIER_TYPE_PLAYER:
        if (guid == ClntObjMgrGetActivePlayer()) {
            return CGPlayer::GetDataSize() / sizeof(uint32_t);
        }
        return CGPlayer::GetRemoteDataSize() / sizeof(uint32_t);
    case HIER_TYPE_GAMEOBJECT:
        return CGGameObject::GetDataSize() / sizeof(uint32_t);
        break;
    case HIER_TYPE_DYNAMICOBJECT:
        return CGDynamicObject::GetDataSize() / sizeof(uint32_t);
        break;
    case HIER_TYPE_CORPSE:
        return CGCorpse::GetDataSize() / sizeof(uint32_t);
        break;
    }
    return 0;
}

// OFFSET 0x4D53C0
int32_t FillInPartialObjectData(CGObject_C* object, WGUID guid, CDataStore* msg, bool forFullUpdate, bool zeroZeroBits) {
    uint8_t changeMaskCount;
    uint32_t changeMasks[MAX_CHANGE_MASKS];
    if (!ExtractDirtyMasks(msg, &changeMaskCount, changeMasks)) {
        return 0;
    }

    OBJECT_TYPE_ID typeID = ID_OBJECT;
    uint32_t blockOffset = 0;
    uint32_t numBlocks = GetNumDwordBlocks(object->m_obj->m_type, guid);

    //v17.m_linkoffset = 8;
    //v17.m_terminator.m_prevlink = &v17.m_terminator;
    //v17.m_terminator.m_next = &v17.m_terminator.m_prevlink + 1;

    for (int32_t block = 0; block < numBlocks; block++) {
        //if (block >= s_objMirrorBlocks[typeID]) {
        //    blockOffset = s_objMirrorBlocks[typeID];
        //    typeID = IncTypeID(object, typeID);
        //}

        if (!forFullUpdate) {
            //sub_4D4850(&v17);
            //v9 = (g_objFieldMirrorLists + 12 * v7 + 12 * (1326 * a1 - v16));
            //if (CallMirrorHandlers__LinkPendingNodes(&v17, v9))
            //    maybe_ClntObjMgr__CopyMirrorFields(v9, obj);
            //ActiveObj = GetObjectMirrorList(a1, obj, v7 - v16);
            //if (CallMirrorHandlers__LinkPendingNodes(&v17, ActiveObj))
            //    maybe_ClntObjMgr__CopyMirrorFields(ActiveObj, obj);
        }

        if (IsMaskBitSet(changeMasks, block)) {
            uint32_t blockValue;
            msg->GetArray(reinterpret_cast<uint8_t*>(&blockValue), sizeof(blockValue));

            object->SetData(block, blockValue);
        } else if (zeroZeroBits) {
            object->SetData(block, 0);
        }
    }

    //bn_TSList_TSGetExplicitLink_UnlinkAll(&v17);
    //if (v17.m_terminator.m_prevlink) {
    //    if ((v17.m_terminator.m_next & 1) == 0 && v17.m_terminator.m_next)
    //        v11 = v17.m_terminator.m_next + &v17.m_terminator - v17.m_terminator.m_prevlink->m_next;
    //    else
    //        v11 = (v17.m_terminator.m_next & 0xFFFFFFFE);
    //    *v11 = v17.m_terminator.m_prevlink;
    //    v17.m_terminator.m_prevlink->m_next = v17.m_terminator.m_next;
    //}

    return 1;
}

// OFFSET: 0x4D6E80
int32_t PartialUpdateFromFullUpdate(CDataStore* msg) {
    WGUID guid;
    *msg >> guid;

    bool reenable;
    CGObject_C* obj = GetUpdateObject(guid, &reenable);
    if (!obj) {
        //NOP("Failed to update object data.  Object (0x%016I64X) unknown to client!");
        return 0;
    }

    if (!FillInPartialObjectData(obj, obj->m_obj->m_guid, msg, false, false)) {
        return 0;
    }

    if (reenable)
        obj->Reenable();
    return 1;
}

// OFFSET: 0x4D3F80
int32_t SkipPartialObjectUpdate(CDataStore* msg) {
    uint8_t changeMaskCount;
    uint32_t changeMasks[MAX_CHANGE_MASKS];
    if (!ExtractDirtyMasks(msg, &changeMaskCount, changeMasks)) {
        return 0;
    }

    for (int32_t block = 0; block < changeMaskCount * 32; block++) {
        if (IsMaskBitSet(changeMasks, block)) {
            uint32_t blockValue;
            msg->Get(blockValue);
        }
    }

    return 1;
}

// OFFSET: 0x4D3FF0
void InitObject(CGObject_C* obj, CClientObjCreate& objCreate, uint32_t time) {
    switch (obj->m_typeID) {
    case OBJECT_TYPE_ID::ID_ITEM:
        obj = new (obj) CGItem_C(objCreate, time);
        break;
    case OBJECT_TYPE_ID::ID_CONTAINER:
        obj = new (obj) CGContainer_C(objCreate, time);
        break;
    case OBJECT_TYPE_ID::ID_UNIT:
        obj = new (obj) CGUnit_C(objCreate, time);
        obj->AddWorldObject();
        break;
    case OBJECT_TYPE_ID::ID_PLAYER:
        obj = new (obj) CGPlayer_C(objCreate, time);
        obj->AddWorldObject();
        break;
    case OBJECT_TYPE_ID::ID_GAMEOBJECT:
        obj = new (obj) CGGameObject_C(objCreate, time);
        obj->AddWorldObject();
        break;
    case OBJECT_TYPE_ID::ID_DYNAMICOBJECT:
        obj = new (obj) CGDynamicObject_C(objCreate, time);
        obj->AddWorldObject();
        break;
    case OBJECT_TYPE_ID::ID_CORPSE:
        obj = new (obj) CGCorpse_C(objCreate, time);
        obj->AddWorldObject();
        break;
    }
}

// OFFSET: 0x4D45B0
void SetupObjectStorage(CGObject_C* obj, OBJECT_TYPE_ID typeId, WGUID guid) {
    uintptr_t offset = 0;
    uint32_t dataSize = 0;

    switch (typeId) {
    case OBJECT_TYPE_ID::ID_OBJECT:
        offset = reinterpret_cast<uintptr_t>(obj) + sizeof(CGObject_C);
        dataSize = CGObject::GetDataSize();

        CGObject_C::SetStorage(obj, offset, offset + dataSize);
        memset(reinterpret_cast<void*>(offset), 0, dataSize);
        break;
    case OBJECT_TYPE_ID::ID_ITEM:
        offset = reinterpret_cast<uintptr_t>(obj) + sizeof(CGItem_C);
        dataSize = CGItem::GetDataSize();

        CGItem_C::SetStorage(reinterpret_cast<CGItem_C*>(obj), offset, offset + dataSize);
        memset(reinterpret_cast<void*>(offset), 0, dataSize);
        break;
    case OBJECT_TYPE_ID::ID_CONTAINER:
        offset = reinterpret_cast<uintptr_t>(obj) + sizeof(CGContainer_C);
        dataSize = CGContainer::GetDataSize();

        CGContainer_C::SetStorage(reinterpret_cast<CGContainer_C*>(obj), offset, offset + dataSize);
        memset(reinterpret_cast<void*>(offset), 0, dataSize);
        break;
    case OBJECT_TYPE_ID::ID_UNIT:
        offset = reinterpret_cast<uintptr_t>(obj) + sizeof(CGUnit_C);
        dataSize = CGUnit::GetDataSize();

        CGUnit_C::SetStorage(reinterpret_cast<CGUnit_C*>(obj), offset, offset + dataSize);
        memset(reinterpret_cast<void*>(offset), 0, dataSize);
        break;
    case OBJECT_TYPE_ID::ID_PLAYER:
        offset = reinterpret_cast<uintptr_t>(obj) + sizeof(CGPlayer_C);
        if ((guid == ClntObjMgrGetActivePlayer())) {
            dataSize = CGPlayer::GetDataSize();
            //*(a1 + 0x614) = &a1[4 * CGPlayer::TotalFields() + 0x2E10];
        } else {
            dataSize = CGPlayer::GetRemoteDataSize();
            //*(a1 + 0x614) = 0;
        }

        CGPlayer_C::SetStorage(reinterpret_cast<CGPlayer_C*>(obj), offset, offset + dataSize);
        memset(reinterpret_cast<void*>(offset), 0, dataSize);
        break;
    case OBJECT_TYPE_ID::ID_GAMEOBJECT:
        offset = reinterpret_cast<uintptr_t>(obj) + sizeof(CGGameObject_C);
        dataSize = CGGameObject::GetDataSize();

        CGGameObject_C::SetStorage(reinterpret_cast<CGGameObject_C*>(obj), offset, offset + dataSize);
        memset(reinterpret_cast<void*>(offset), 0, dataSize);
        break;
    case OBJECT_TYPE_ID::ID_DYNAMICOBJECT:
        offset = reinterpret_cast<uintptr_t>(obj) + sizeof(CGDynamicObject_C);
        dataSize = CGDynamicObject::GetDataSize();

        CGDynamicObject_C::SetStorage(reinterpret_cast<CGDynamicObject_C*>(obj), offset, offset + dataSize);
        memset(reinterpret_cast<void*>(offset), 0, dataSize);
        break;
    case OBJECT_TYPE_ID::ID_CORPSE:
        offset = reinterpret_cast<uintptr_t>(obj) + sizeof(CGCorpse_C);
        dataSize = CGCorpse::GetDataSize();

        CGCorpse_C::SetStorage(reinterpret_cast<CGCorpse_C*>(obj), offset, offset + dataSize);
        memset(reinterpret_cast<void*>(offset), 0, dataSize);
        break;
    }
}

// OFFSET: 0x4D6C00
bool CreateObject(CDataStore* msg, uint32_t time) {
    WGUID guid;
    *msg >> guid;

    uint8_t _typeID;
    msg->Get(_typeID);
    auto typeID = static_cast<OBJECT_TYPE_ID>(_typeID);

    if (typeID >= NUM_CLIENT_OBJECT_TYPES)
        return false;

    bool reenable;
    auto existingObject = GetUpdateObject(guid, &reenable);
    
    if (existingObject) {
        CClientObjCreate::Skip(msg);
    
        if (!FillInPartialObjectData(existingObject, existingObject->m_obj->m_guid, msg, false, true)) {
            return 0;
        }
    
    //    if (reenable) {
    //        existingObject->Reenable();
    //    }
    
        return 1;
    }

    CClientObjCreate objCreate;
    objCreate.flags = 0;
    objCreate.m_targetGuid = 0;
    objCreate.m_packedRotation = 0;
    if (!objCreate.Get(msg)) {
        return 0;
    }

    if (objCreate.flags & 0x1) {
        ClntObjMgrSetActivePlayer(guid);
    }

    auto newObject = ClntObjMgrAllocObject(typeID, guid);
    SetupObjectStorage(newObject, typeID, guid);
    newObject->SetTypeID(typeID);
    
    if (!FillInPartialObjectData(newObject, guid, msg, true, false)) {
        return 0;
    }
    
    InitObject(newObject, objCreate, time);
    //
    //ClntObjMgrGetCurrent()->m_visibleObjects.LinkToTail(newObject);

    OsOutputDebugString("Received CreateObject %d -> %d %d\n", typeID, guid.guid_low, guid.guid_high);

    return true;
}

void UpdateInRangeObjects(CDataStore* msg) {
    uint32_t count;
    msg->Get(count);

    WGUID guid;
    for (uint32_t i = 0; i < count; i++) {
        *msg >> guid;
        if (guid == ClntObjMgrGetActivePlayer())
            continue;

        bool reenable;
        CGObject_C* obj = GetUpdateObject(guid, &reenable);
        if (!obj || !reenable)
            continue;

        obj->Reenable();
    }
}

int32_t ObjectUpdateFirstPass(uint32_t updateIndex, CDataStore* msg, uint32_t time, uint32_t updateCount) {
    if (updateIndex >= updateCount)
        return 1;

    WGUID guid;
    for (uint32_t i = updateIndex; i < updateCount; i++) {
        uint8_t updateType;
        msg->Get(updateType);

        switch (updateType) {
        case UPDATE_PARTIAL:
            if (!PartialUpdateFromFullUpdate(msg))
                return 0;
            break;
        case UPDATE_MOVEMENT:
            *msg >> guid;

            OsOutputDebugString("Move update for %ull\n", guid);
            CClientMoveUpdate::Skip(msg);
            break;
        case UPDATE_FULL:
        case UPDATE_3:
            if (!CreateObject(msg, time)) {
                return 0;
            }
            break;
        case UPDATE_IN_RANGE:
            UpdateInRangeObjects(msg);
            break;
        default:
            //NOP("Unknown client update packet type (%d)!");
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x4D7230
void UpdateOutOfRangeObjects(CDataStore* msg) {
    uint32_t count;
    msg->Get(count);

    uint32_t readPosition = msg->Tell();

    WGUID guid;
    for (uint32_t i = 0; i < count; i++) {
        *msg >> guid;
        if (guid == ClntObjMgrGetActivePlayer())
            continue;

        CGObject_C* obj = GetObjectPtr<CGObject_C*>(&g_tlsBlock.pObjMgr->m_objects, guid);
        if (!obj)
            continue;

        //if (bnl_CGBattlefieldInfo__m_instanceType == 4) {
        //    OBJECT_FIELD_GUID = ObjectPtr->m_obj->OBJECT_FIELD_GUID;
        //    maybe_UpdateArenaOpponents(&OBJECT_FIELD_GUID);
        //}
        //(v4->ukn4)(v4, 0);
        //if (CGObject_C__IsObjectLocked(v4)) {
        //    CGObject_C::SetDisablePending(v4, 1);
        //} else {
        //    CGObject_C::SetDisablePending(v4, 0);
        //    v4->Disable(v4);
        //}
    }

    msg->Seek(readPosition);

    for (uint32_t i = 0; i < count; i++) {
        *msg >> guid;
        if (guid == ClntObjMgrGetActivePlayer())
            continue;

        CGObject_C* obj = GetObjectPtr<CGObject_C*>(&g_tlsBlock.pObjMgr->m_objects, guid);
        if (!obj)
            continue;

        // if (!CGObject_C__IsObjectLocked(v5))
        //     ObjDelete(v6);
    }

    // CVehiclePassenger_C::ExecutePendingRescueTransitions();
}

// OFFSET: 0x4D5550
bool CallMirrorHandlers(CDataStore* msg, bool a2, WGUID a3) {
    // TODO
    return SkipPartialObjectUpdate(msg);
}

// OFFSET: 0x4D41C0
void SkipSetOfObjects(CDataStore* msg) {
    uint32_t count;
    msg->Get(count);

    WGUID guid;
    for (uint32_t i = 0; i < count; i++) {
        *msg >> guid;
    }
}

// OFFSET: 0x4D63B0
bool PostInitObject(CDataStore* msg, uint32_t time, bool isUpdate3) {
    WGUID guid;
    *msg >> guid;

    uint8_t _typeID;
    msg->Get(_typeID);
    auto typeID = static_cast<OBJECT_TYPE_ID>(_typeID);

    if (typeID >= NUM_CLIENT_OBJECT_TYPES)
        return false;

    CGObject_C* obj = GetObjectPtr<CGObject_C*>(&g_tlsBlock.pObjMgr->m_objects, guid);
    if (!obj)
        return false;

    CClientObjCreate objCreate;
    //CClientMoveUpdate::CClientMoveUpdate(&v11.move);
    //CGObject_C::PostInit(&v11.move.m_moveSpline);
    objCreate.flags = 0;
    objCreate.m_targetGuid = 0;
    objCreate.m_packedRotation = 0;
    if (!objCreate.Get(msg)) {
        //CMoveSpline::sub_4D4F00(&v11.move.m_moveSpline);
        return 0;
    }

    if ((obj->m_modelFlags & 0x20000) != 0 && (obj->m_obj->m_type & TYPE_UNIT) != 0) {
        auto activePlayerGuid = ClntObjMgrGetActivePlayer();
        //obj->sub_73C260(&objCreate, obj->m_obj->m_guid == activePlayerGuid);
    }
    if ((obj->m_modelFlags & 0x40000) != 0) {
        auto v8 = CallMirrorHandlers(msg, 1, guid);
        //CMoveSpline::sub_4D4F00(&v11.move.m_moveSpline);
        return v8;
    } else {
        switch (typeID) {
        case ID_OBJECT:
            reinterpret_cast<CGObject_C*>(obj)->PostInit(time, &objCreate, isUpdate3);
            break;
        case ID_ITEM:
        case ID_CONTAINER:
            reinterpret_cast<CGItem_C*>(obj)->PostInit(time, &objCreate, isUpdate3);
            break;
        case ID_UNIT:
            reinterpret_cast<CGUnit_C*>(obj)->PostInit(time, &objCreate, isUpdate3);
            break;
        case ID_PLAYER:
            reinterpret_cast<CGPlayer_C*>(obj)->PostInit(time, &objCreate, isUpdate3);
            break;
        case ID_GAMEOBJECT:
            reinterpret_cast<CGGameObject_C*>(obj)->PostInit(time, &objCreate, isUpdate3);
            break;
        case ID_DYNAMICOBJECT:
            reinterpret_cast<CGDynamicObject_C*>(obj)->PostInit(time, &objCreate, isUpdate3);
            break;
        case ID_CORPSE:
            reinterpret_cast<CGCorpse_C*>(obj)->PostInit(time, &objCreate, isUpdate3);
            break;
        default:
            // CMoveSpline::sub_4D4F00(&v11.move.m_moveSpline);
            return 0;
        }
        auto v10 = SkipPartialObjectUpdate(msg);
        //CMoveSpline::sub_4D4F00(&v11.move.m_moveSpline);
        return v10;
    }
}

// OFFSET: 0x4D7100
int32_t ObjectUpdateSecondPass(CDataStore* msg, uint32_t time, uint32_t updateCount) {
    WGUID guid;
    for (uint32_t i = 0; i < updateCount; i++) {
        uint8_t updateType;
        msg->Get(updateType);

        switch (updateType) {
        case UPDATE_PARTIAL:
            if (!CallMirrorHandlers(msg, 0, 0))
                return 0;
            break;
        case UPDATE_MOVEMENT:
            // TODO
            *msg >> guid;
            CClientMoveUpdate::Skip(msg);
            break;
        case UPDATE_FULL:
        case UPDATE_3:
            if (!PostInitObject(msg, time, updateType == UPDATE_3)) {
                return 0;
            }
            break;
        case UPDATE_IN_RANGE:
            SkipSetOfObjects(msg);
            break;
        default:
            // NOP("Unknown client update packet type (%d)!");
            return 0;
        }
    }

    //v15 = *(NtCurrentTeb()->ThreadLocalStoragePointer + TlsIndex);
    //while (1) {
    //    pObjMgr = v15->pObjMgr;
    //    m_next = pObjMgr->unk_B0.m_terminator.m_next;
    //    if ((m_next & 1) != 0 || !m_next)
    //        break;
    //    m_linkoffset = pObjMgr->unk_A4.m_linkoffset;
    //    v9 = *&m_next[m_linkoffset];
    //    v10 = &m_next[m_linkoffset];
    //    if (v9) {
    //        v11 = v10->m_next;
    //        if ((v11 & 1) == 0 && v11)
    //            v12 = (&v10->m_prevlink + v11 - *(v9 + 4));
    //        else
    //            v12 = (v11 & 0xFFFFFFFE);
    //        *v12 = v9;
    //        v10->m_prevlink->m_next = v10->m_next;
    //        v10->m_prevlink = 0;
    //        v10->m_next = 0;
    //    }
    //    m_prevlink = pObjMgr->unk_A4.m_terminator.m_prevlink;
    //    v10->m_prevlink = m_prevlink;
    //    v10->m_next = m_prevlink->m_next;
    //    m_prevlink->m_next = m_next;
    //    pObjMgr->unk_A4.m_terminator.m_prevlink = v10;
    //    (*(*m_next + 12))(m_next);
    //}

    return 1;
}

int32_t Packet_SMSG_UPDATE_OBJECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t updateCount;
    msg->Get(updateCount);

    uint32_t startPos = msg->Tell();
    uint32_t updateIndex = 0;

    uint8_t updateType;
    msg->Get(updateType);

    if (updateType == UPDATE_OUT_OF_RANGE) {
        UpdateOutOfRangeObjects(msg);
        updateIndex = 1;
    } else {
        msg->Seek(startPos);
    }

    int32_t result = 0;
    if (ObjectUpdateFirstPass(updateIndex, msg, time, updateCount)) {
        msg->Seek(startPos);
        result = ObjectUpdateSecondPass(msg, time, updateCount);
    }
    // v8 = (WowTlsBlock*)*((_DWORD*)NtCurrentTeb()->ThreadLocalStoragePointer + TlsIndex);
    // v9 = 0x54;
    // for (i = 0x54; i < 168; i += 12) {
    //     v10 = *(CGObject_C**)((char*)&v8->pObjMgr->m_objects.m_fulllist.m_linkoffset + v9);
    //     if (((unsigned __int8)v10 & 1) == 0) {
    //         if (v10) {
    //             ukn_0040 = v10->ukn_0024;
    //             if ((int)(-120000 - ukn_0040 + OsGetAsyncTimeMs()) >= 0) {
    //                 sub_4D4790(v10);
    //                 ukn_0038 = v10->ukn_001C;
    //                 p_ukn_0038 = &v10->ukn_001C;
    //                 if (ukn_0038) {
    //                     ukn_003C = v10->ukn_0020;
    //                     if ((ukn_003C & 1) == 0 && ukn_003C)
    //                         v15 = (DWORD*)((char*)p_ukn_0038 + ukn_003C - *(_DWORD*)(ukn_0038 + 4));
    //                     else
    //                         v15 = (_DWORD*)(ukn_003C & 0xFFFFFFFE);
    //                     *v15 = ukn_0038;
    //                     *(_DWORD*)(*p_ukn_0038 + 4) = v10->ukn_0020;
    //                     *p_ukn_0038 = 0;
    //                     v10->ukn_0020 = 0;
    //                 }
    //                 sub_4D4090(v10);
    //             }
    //         }
    //     }
    //     v9 = i + 12;
    // }
    // return v17;

    OsOutputDebugString("Received Packet_SMSG_UPDATE_OBJECT with %d updates\n", updateCount);
    return result;
}

int32_t Packet_SMSG_DESTROY_OBJECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WGUID guid;
    *msg >> guid;

    uint8_t v9;
    msg->Get(v9);

    auto obj = GetObjectPtr<CGObject_C*>(&g_tlsBlock.pObjMgr->m_objects, guid);
    if (obj) {
        //if (v9 && (ObjectPtr->ObjectBase.m_obj->OBJECT_FIELD_TYPE & TYPEMASK_UNIT) != 0 && ObjectPtr->m_unit->UNIT_FIELD_HEALTH > 0)
        //    CGUnit_C::OnDeath(ObjectPtr);
        //(p_ObjectBase->ukn4)(p_ObjectBase, 1);
        //if (CGObject_C__IsObjectLocked(p_ObjectBase)) {
        //    CGObject_C::SetDisablePending(p_ObjectBase, 1);
        //    return 1;
        //}
        //CGObject_C::SetDisablePending(p_ObjectBase, 0);
        //p_ObjectBase->Disable(p_ObjectBase);
        //ObjDelete(p_ObjectBase);
    }
    return 1;
}

int32_t Packet_SMSG_COMPRESSED_UPDATE_OBJECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t origSize;
    msg->Get(origSize);

    uint32_t sourceSize = msg->Size() - msg->Tell();

    void* source;
    msg->GetDataInSitu(source, sourceSize);

    void* buffer;
    bool bufferOnStack;

    // Stack allocate buffer if original size is less than 8KB
    if (origSize >= 8192) {
        buffer = STORM_ALLOC(origSize);
        bufferOnStack = false;
    } else {
        buffer = alloca(origSize);
        bufferOnStack = true;
    }

    auto dest = buffer;
    auto destSize = origSize;

    auto zlibResult = ZlibDecompress(dest, &destSize, source, sourceSize);

    // Error during decompression

    if (zlibResult != 0) {
        if (!bufferOnStack) {
            STORM_FREE(buffer);
        }

        return 0;
    }

    // Successful decompression

    STORM_ASSERT(origSize == destSize);

    // TODO WDataStore
    CDataStore decompMsg;
    decompMsg.PutData(dest, destSize);
    decompMsg.Finalize();

    int32_t result = Packet_SMSG_UPDATE_OBJECT(nullptr, SMSG_UPDATE_OBJECT, time, &decompMsg);

    if (!bufferOnStack) {
        STORM_FREE(buffer);
    }

    return result;
}

// OFFSET: 0x4D76E0
void ClntObjMgrInitialize() {
    NetClient* net = g_tlsBlock.pObjMgr->realmConnection;
    net->SetMessageHandler(SMSG_UPDATE_OBJECT, &Packet_SMSG_UPDATE_OBJECT, nullptr);
    net->SetMessageHandler(SMSG_COMPRESSED_UPDATE_OBJECT, &Packet_SMSG_COMPRESSED_UPDATE_OBJECT, nullptr);
    net->SetMessageHandler(SMSG_DESTROY_OBJECT, Packet_SMSG_DESTROY_OBJECT, nullptr);
}

// OFFSET: 0x4D4AC0
void ClntObjMgrInitializeShared() {
    if (!s_heapsAllocated) {
        for (uint32_t i = 1; i < 8; ++i)
            s_objHeapId[i] = ObjectAllocAddHeap(s_objTotalSize[i], s_objHeapSize[i], s_objNames[i], 1);
        s_heapsAllocated = true;
    }
    //MirrorInitialize(a1, a2);
    //ConsoleCommandRegister("ObjUsage", CCommand_ObjUsage, 4, 0);
}

// OFFSET: 0x4D7750
void ClntObjMgrInitializeStd(int32_t zoneId) {
    //v6 = CGGameUI::m_iCurrentMapID;
    //v1 = time(0);
    //CGGameUI::SetLastInstanceTime(v1, v6, a1);
    void* m = STORM_ALLOC(sizeof(ObjectMgr));
    auto mgr = new (m) ObjectMgr();
    ClientServices::g_clientConnection->m_ObjectMgr = mgr;
    g_tlsBlock.pObjMgr = mgr;
    mgr->realmConnection = ClientServices::g_clientConnection;
    ClntObjMgrInitialize();
    g_tlsBlock.pObjMgr->mapId = zoneId;
}

// OFFSET: 0x4D6BC0
void ClntObjMgrLinkInNewObject(CGObject_C* obj) {
    g_tlsBlock.pObjMgr->m_objects.Insert(obj, obj->m_obj->m_guid.guid_low, obj->m_obj->m_guid);
}

// OFFSET: none (inlined)
void ClntObjMgrSetActivePlayer(WGUID guid) {
    auto mgr = g_tlsBlock.pObjMgr;
    if (mgr)
        mgr->playerGuid = guid;
}

// OFFSET: 0x4D3790
WGUID ClntObjMgrGetActivePlayer() {
    auto mgr = g_tlsBlock.pObjMgr;
    if (mgr)
        return mgr->playerGuid;
    return WGUID();
}

// OFFSET: 0x4038F0
CGPlayer_C* ClntObjMgrGetActivePlayerObj() {
    return ClntObjMgrObjectPtr<CGPlayer_C*>(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER);
}

// OFFSET: 0x4D37E0
int32_t ClntObjMgrGetMapID() {
    auto mgr = g_tlsBlock.pObjMgr;
    if (mgr)
        return mgr->mapId;
    return 0;
}

// OFFSET: 0x4D4930
CGObject_C* ClntObjMgrAllocObject(OBJECT_TYPE_ID typeId, WGUID guid) {
    if (guid == g_tlsBlock.pObjMgr->playerGuid) {
        void* m = STORM_ALLOC(sizeof(uint32_t) * CGPlayer::TotalFields() + sizeof(CGPlayer_C) + CGPlayer::GetDataSize());
        CGObject_C* obj = new (m) CGObject_C();
        return obj;
    }

    //maybe_ClntObjMgr__ExpireOldObjects(10000, a1);
    if (ObjectAlloc(s_objHeapId[typeId], &guid.guid_high, nullptr, false)) {
        void* m = ObjectPtr(s_objHeapId[typeId], guid.guid_high);
        CGObject_C* obj = new (m) CGObject_C();
        obj->m_heapIndex = guid.guid_high;
        return obj;
    }
    return nullptr;
}

// OFFSET: 0x4D3840
void ClntObjMgrSetMovementGlobals(CMovementGlobals* globals) {
    if (g_tlsBlock.pObjMgr)
        g_tlsBlock.pObjMgr->m_movementGlobals = globals;
}

// OFFSET: 0x4D3810
CMovementGlobals* ClntObjMgrGetMovementGlobals() {
    if (g_tlsBlock.pObjMgr)
        return g_tlsBlock.pObjMgr->m_movementGlobals;
    return nullptr;
}

// OFFSET: 0x4D4BB0
template <typename T>
T GetObjectPtr(TSHashTable<CGObject_C, WGUID>* table, WGUID guid) {
    return table->Ptr(guid.guid_low, guid);
}

// OFFSET: 0x4D4DB0
template <typename T>
T ClntObjMgrObjectPtr(WGUID guid, TypeMask mask) {
    auto mgr = g_tlsBlock.pObjMgr;
    if (!mgr)
        return nullptr;
    if (!guid)
        return nullptr;
    CGObject_C* result = GetObjectPtr<CGObject_C*>(&mgr->m_objects, guid);
    if (result) {
        if ((mask & result->m_obj->m_type) == 0) {
            return nullptr;
        }
    }
    return reinterpret_cast<T>(result);
}

template CGObject_C* ClntObjMgrObjectPtr<CGObject_C*>(WGUID, TypeMask);
template CGPlayer_C* ClntObjMgrObjectPtr<CGPlayer_C*>(WGUID, TypeMask);
