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

#define MAX_CHANGE_MASKS 42

thread_local WowTlsBlock g_tlsBlock {};

// OFFSET: 0x4D4BB0
template <typename T>
T ObjectMgr::GetObjectPtr(WGUID guid) {
    return this->m_objects.Ptr(guid.guid_low, guid);
}

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

int32_t FillInPartialObjectData(CGObject_C* object, WGUID guid, CDataStore* msg, bool forFullUpdate, bool zeroZeroBits) {
    uint8_t changeMaskCount;
    uint32_t changeMasks[MAX_CHANGE_MASKS];
    if (!ExtractDirtyMasks(msg, &changeMaskCount, changeMasks)) {
        return 0;
    }

    OBJECT_TYPE_ID typeID = ID_OBJECT;
    uint32_t blockOffset = 0;
    //uint32_t numBlocks = GetNumDwordBlocks(object->GetType(), guid);

    //for (int32_t block = 0; block < numBlocks; block++) {
    //    if (block >= s_objMirrorBlocks[typeID]) {
    //        blockOffset = s_objMirrorBlocks[typeID];
    //        typeID = IncTypeID(object, typeID);
    //    }

    //    if (!forFullUpdate) {
    //        // TODO
    //    }

    //    if (IsMaskBitSet(changeMasks, block)) {
    //        uint32_t blockValue;
    //        msg->GetArray(reinterpret_cast<uint8_t*>(&blockValue), sizeof(blockValue));

    //        object->SetBlock(block, blockValue);
    //    } else if (zeroZeroBits) {
    //        object->SetBlock(block, 0);
    //    }
    //}

    // TODO

    return 1;
}

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
        obj = new (obj) CGItem_C();
        break;
    case OBJECT_TYPE_ID::ID_CONTAINER:
        obj = new (obj) CGContainer_C();
        break;
    case OBJECT_TYPE_ID::ID_UNIT:
        obj = new (obj) CGUnit_C();
        obj->AddWorldObject();
        break;
    case OBJECT_TYPE_ID::ID_PLAYER:
        obj = new (obj) CGPlayer_C();
        obj->AddWorldObject();
        break;
    case OBJECT_TYPE_ID::ID_GAMEOBJECT:
        obj = new (obj) CGGameObject_C();
        obj->AddWorldObject();
        break;
    case OBJECT_TYPE_ID::ID_DYNAMICOBJECT:
        obj = new (obj) CGDynamicObject_C();
        obj->AddWorldObject();
        break;
    case OBJECT_TYPE_ID::ID_CORPSE:
        obj = new (obj) CGCorpse_C();
        obj->AddWorldObject();
        break;
    }
}

bool CreateObject(CDataStore* msg, uint32_t time) {
    WGUID guid;
    *msg >> guid;

    uint8_t _typeID;
    msg->Get(_typeID);
    auto typeID = static_cast<OBJECT_TYPE_ID>(_typeID);

    if (typeID >= NUM_CLIENT_OBJECT_TYPES)
        return false;

    //int32_t reenable;
    //auto existingObject = GetUpdateObject(guid, &reenable);
    //
    //if (existingObject) {
    //    CClientObjCreate::Skip(msg);
    //
    //    if (!FillInPartialObjectData(existingObject, existingObject->GetGUID(), msg, false, true)) {
    //        return 0;
    //    }
    //
    //    if (reenable) {
    //        existingObject->Reenable();
    //    }
    //
    //    return 1;
    //}

    CClientObjCreate objCreate;
    if (!objCreate.Get(msg)) {
        return 0;
    }

    if (objCreate.flags & 0x1) {
        ClntObjMgrSetActivePlayer(guid);
    }

    //### debug
    void* m = STORM_ALLOC(sizeof(CGObject_C));
    auto newObject = new (m) CGObject_C();
    newObject->ObjectData = new ObjectFields();
    newObject->ObjectData->OBJECT_FIELD_GUID = guid;
    //###
    //auto newObject = ClntObjMgrAllocObject(typeID, guid);
    //SetupObjectStorage(typeID, newObject, guid);
    newObject->SetTypeID(typeID);

    //### debug
    SkipPartialObjectUpdate(msg);
    ClntObjMgrLinkInNewObject(newObject);
    //###
    
    //if (!FillInPartialObjectData(newObject, guid, msg, true, false)) {
    //    return 0;
    //}
    //
    InitObject(newObject, objCreate, time);
    //
    //ClntObjMgrGetCurrent()->m_visibleObjects.LinkToTail(newObject);

    OsOutputDebugString("Received CreateObject %d -> %d %d\n", typeID, guid.guid_low, guid.guid_high);

    return true;
}

int32_t ObjectUpdateFirstPass(uint32_t updateIndex, CDataStore* msg, uint32_t time, uint32_t updateCount) {
    if (updateIndex >= updateCount)
        return 1;

    for (uint32_t i = updateIndex; i < updateCount; i++) {
        uint8_t updateType;
        msg->Get(updateType);

        switch (updateType) {
        case UPDATE_PARTIAL:

            break;
        case UPDATE_MOVEMENT:

            break;
        case UPDATE_FULL:
        case UPDATE_3:
            if (!CreateObject(msg, time)) {
                return 0;
            }
            break;
        case UPDATE_IN_RANGE:

            break;
        default:
            return 0;
        }
    }

    return 1;
}

int32_t ObjectUpdateSecondPass(CDataStore* msg, uint32_t updateCount) {
    return 0;
}

int32_t Packet_SMSG_UPDATE_OBJECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t updateCount;
    msg->Get(updateCount);

    uint32_t startPos = msg->Tell();
    uint32_t updateIndex = 0;

    uint8_t updateType;
    msg->Get(updateType);

    if (updateType == UPDATE_OUT_OF_RANGE) {
        // UpdateOutOfRangeObjects();
        updateIndex = 1;
    } else {
        msg->Seek(startPos);
    }

    int32_t result = 0;
    if (ObjectUpdateFirstPass(updateIndex, msg, time, updateCount)) {
        msg->Seek(startPos);
        result = ObjectUpdateSecondPass(msg, updateCount);
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
    //net->SetMessageHandler(SMSG_DESTROY_OBJECT, Packet_SMSG_DESTROY_OBJECT, nullptr);
}

// OFFSET: 0x4D4AC0
void ClntObjMgrInitializeShared() {
    // TODO
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
    g_tlsBlock.pObjMgr->m_objects.Insert(obj, obj->ObjectData->OBJECT_FIELD_GUID.guid_low, obj->ObjectData->OBJECT_FIELD_GUID);
}

// OFFSET: None (inlined)
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

// OFFSET: 0x4D4DB0
template <typename T>
T ClntObjMgrObjectPtr(WGUID guid, TypeMask mask) {
    auto mgr = g_tlsBlock.pObjMgr;
    if (!mgr)
        return nullptr;
    if (!guid)
        return nullptr;
    CGObject_C* result = mgr->GetObjectPtr<CGObject_C*>(guid);
    if (result) {
        if ((mask & result->ObjectData->OBJECT_FIELD_TYPE) == 0) {
            return nullptr;
        }
    }
    return reinterpret_cast<T>(result);
}

template CGPlayer_C* ClntObjMgrObjectPtr<CGPlayer_C*>(WGUID, TypeMask);
