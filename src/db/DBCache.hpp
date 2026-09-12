#ifndef DB_CACHE_DB_CACHE_HPP
#define DB_CACHE_DB_CACHE_HPP

#include <common/datastore/CDataStore.hpp>
#include <cstddef>
#include <cstdint>
#include <storm/Hash.hpp>
#include <storm/List.hpp>

#define WDB_EXPECTED_BUILD 12340
#define WDB_IDLE_PERIOD_MS 30000

void DBCacheGetDirectory(char* path);

typedef void (*DBCACHE_CALLBACK)(uint32_t id, void* data, void* arg, int32_t success);

enum WDB_CACHE_ID {
    WDB_CACHE_CREATURE = 0,
    WDB_CACHE_GAMEOBJECT,
    WDB_CACHE_ITEMNAME,
    WDB_CACHE_ITEM,
    WDB_CACHE_NPC,
    WDB_CACHE_NAME,
    WDB_CACHE_GUILD,
    WDB_CACHE_QUEST,
    WDB_CACHE_PAGETEXT,
    WDB_CACHE_PETNAME,
    WDB_CACHE_PETITION,
    WDB_CACHE_ITEMTEXT,
    WDB_CACHE_WOW,
    WDB_CACHE_ARENATEAM,
    WDB_CACHE_DANCE,
    WDB_CACHE_COUNT
};

struct WDBHeader {
    uint32_t signature;
    uint32_t build;
    uint32_t locale;
    uint32_t recordSize;
    uint32_t version;
    uint32_t cacheVersion;
};

struct DBCACHECALLBACK {
    /* 0x00 */ TSLink<DBCACHECALLBACK> m_link;
    /* 0x08 */ DBCACHE_CALLBACK m_callback;
    /* 0x0C */ uint32_t m_unk0C;
    /* 0x10 */ uint64_t m_requester;
    /* 0x18 */ void* m_arg;
    /* 0x1C */ uint32_t m_unk1C;
};

template <class K, class N>
struct DBCACHETRAILER {
    /* 0x00 */ K m_key;
    /* 0x04 */ uint8_t m_loaded;
    /* 0x05 */ uint8_t m_unk05[3];
    /* 0x08 */ STORM_EXPLICIT_LIST(DBCACHECALLBACK, m_link) m_callbacks;
    /* 0x14 */ uint8_t m_noSave;
    /* 0x15 */ uint8_t m_dispatching;
    /* 0x16 */ uint8_t m_invalidatePending;
    /* 0x17 */ uint8_t m_unk17;
    /* 0x18 */ TSLink<N> m_pendingLink;

    // OFFSET: 0x672DA0 (CreatureStats_C)
    // OFFSET: 0x673050 (GameObjectStats_C)
    // OFFSET: 0x673300 (ItemName_C)
    // OFFSET: 0x6735B0 (ItemStats_C)
    // OFFSET: 0x673860 (NPCText)
    // OFFSET: 0x673B10 (NameCache)
    // OFFSET: 0x673DD0 (GuildStats_C)
    // OFFSET: 0x674070 (QuestCache)
    // OFFSET: 0x674310 (PageTextCache_C)
    // OFFSET: 0x6745C0 (PetNameCache)
    // OFFSET: 0x674870 (CGPetition)
    // OFFSET: 0x674B10 (ItemTextCache_C)
    // OFFSET: 0x674DB0 (WardenCachedModule)
    // OFFSET: 0x675050 (ArenaTeamCache)
    // OFFSET: 0x6752F0 (DanceCache)
    ~DBCACHETRAILER() {
        this->m_callbacks.Clear();
    }
};

template <class T, class K, class H>
class DBCache {
    public:
    struct DBCACHEHASH : public TSHashObject<DBCACHEHASH, H> {
        /* 0x18 */ T m_record;
        /*      */ DBCACHETRAILER<K, DBCACHEHASH> m_trailer;
    };

    struct REVERSEENTRY : public TSHashObject<REVERSEENTRY, HASHKEY_STR> {
        /* 0x18 */ T* m_record;
    };

    typedef DBCACHEHASH NODE;

    DBCache(uint32_t signature, const char* fileName, uint32_t queryOpcode, uint32_t unk40, uint8_t sendRequester, uint8_t enabled, uint32_t queriesPerMinute);
    ~DBCache();

    virtual void InvalidateIntKey(int32_t key);

    T* GetRecord(K key, const uint64_t* requester, DBCACHE_CALLBACK callback, void* arg, bool dedupe);
    T* GetRecordByName(const char* name);

    void AddItem(const T* record, K key);
    void AddItems(CDataStore* msg, bool single);
    void DenyItem(K key);
    void AddQueryResult(K key);
    NODE* SetNoSave(K key);

    void Invalidate(K key);

    void SendSingleQuery(NODE* node);
    void Idle();
    void ClearPending();
    void CancelCallback(K key, DBCACHE_CALLBACK callback, void* arg);

    void Load();
    void Save();
    void Clear();
    void VerifyAndUpdateVersion(uint32_t version);
    void Destroy();

    private:
    /* 0x00 */ // vtable
    /* 0x04 */ const char* m_unk04;
    /* 0x08 */ TSHashTable<NODE, H> m_entries;
    /* 0x30 */ uint32_t m_signature;
    /* 0x34 */ const char* m_fileName;
    /* 0x38 */ uint32_t m_version;
    /* 0x3C */ uint32_t m_queryOpcode;
    /* 0x40 */ uint32_t m_unk40;
    /* 0x44 */ uint8_t m_sendRequester;
    /* 0x45 */ uint8_t m_enabled;
    /* 0x46 */ uint8_t m_loaded;
    /* 0x47 */ uint8_t m_unk47;
    /* 0x48 */ uint32_t m_maxQueriesPerPump;
    /* 0x4C */ uint32_t m_queriesSent;
    /* 0x50 */ uint32_t m_nextQueryTime;
    /* 0x54 */ STORM_EXPLICIT_LIST(NODE, m_trailer.m_pendingLink) m_pending;
    /* 0x60 */ TSHashTable<REVERSEENTRY, HASHKEY_STR> m_reverse;
};

#endif
