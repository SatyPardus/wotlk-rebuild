#ifndef CLIENTOBJECT_OBJECTMGRCLIENT_HPP
#define CLIENTOBJECT_OBJECTMGRCLIENT_HPP

#include "clientobject/CGObject_C.hpp"
#include "clientobject/Player_C.hpp"
#include "clientobject/Types.hpp"
#include "net/connection/ClientConnection.hpp"
#include "storm/Hash.hpp"
#include <cstdint>

struct CMovementGlobals;

class ObjectMgr {
    public:
    // Member variables
    TSHashTable<CGObject_C, WGUID> m_objects;
    TSHashTable<CGObject_C, WGUID> m_lazyCleanupObjects;
    STORM_EXPLICIT_LIST(CGObject_C, m_link) m_deletedObjects[7];
    STORM_EXPLICIT_LIST(CGObject_C, m_link) m_visibleObjects;
    STORM_EXPLICIT_LIST(CGObject_C, m_link) m_pendingReenableObjects;
    // DWORD unk_BC;
    WGUID playerGuid;
    // DWORD unk_C8;
    int32_t mapId;
    ClientConnection* realmConnection;
    CMovementGlobals* m_movementGlobals;
};

struct WowTlsBlock {
    uint32_t _tls_start;      // offset 0x00
    uint32_t unused_04;       // offset 0x04
    ObjectMgr* pObjMgr;       // offset 0x08
    uint32_t pad_0C;          // offset 0x0C
    WGUID loginCharacterGuid; // offset 0x10
};

extern thread_local WowTlsBlock g_tlsBlock;

void ClntObjMgrInitialize();
void ClntObjMgrInitializeShared();
void ClntObjMgrInitializeStd(int32_t zoneId);
void ClntObjMgrLinkInNewObject(CGObject_C* obj);
void ClntObjMgrSetActivePlayer(WGUID guid);
WGUID ClntObjMgrGetActivePlayer();
CGPlayer_C* ClntObjMgrGetActivePlayerObj();
int32_t ClntObjMgrGetMapID();
CGObject_C* ClntObjMgrAllocObject(OBJECT_TYPE_ID typeId, WGUID guid);
void ClntObjMgrSetMovementGlobals(CMovementGlobals* globals);
CMovementGlobals* ClntObjMgrGetMovementGlobals();
bool ClntObjMgrEnumVisibleObjects(bool (*func)(WGUID guid, void* param), void* param);

template <typename T>
T GetObjectPtr(TSHashTable<CGObject_C, WGUID>* table, WGUID guid);

template <typename T>
T ClntObjMgrObjectPtr(WGUID guid, TypeMask mask);

#endif // CLIENTOBJECT_OBJECTMGRCLIENT_HPP
