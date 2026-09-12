#include "db/DBCache.hpp"
#include "db/DBCacheInstances.hpp"
#include "client/Client.hpp"
#include "client/ClientServices.hpp"
#include "console/Console.hpp"
#include "util/SFile.hpp"
#include <bc/memory/Storm.hpp>
#include <bc/os/file/CloseFile.hpp>
#include <bc/os/file/CreateDirectory.hpp>
#include <bc/os/file/CreateFile.hpp>
#include <bc/os/file/ReadFile.hpp>
#include <bc/os/file/Types.hpp>
#include <bc/os/file/WriteFile.hpp>
#include <common/time/Time.hpp>
#include <storm/String.hpp>

// OFFSET: 0x667EF0
void DBCacheGetDirectory(char* path) {
    if (IsCommonMpqExists()) {
        SStrPrintf(path, 260, "Cache/%s/%s", "WDB", Client::g_currentLocaleName);
    } else {
        SStrCopy(path, "WDB", 260);
    }

    OsCreateDirectory(path, 1);
}

// OFFSET: 0x675C80 (CreatureStats_C)
// OFFSET: 0x675DB0 (GameObjectStats_C)
// OFFSET: 0x675EE0 (ItemName_C)
// OFFSET: 0x676010 (ItemStats_C)
// OFFSET: 0x676140 (NPCText)
// OFFSET: 0x676270 (NameCache)
// OFFSET: 0x6763A0 (GuildStats_C)
// OFFSET: 0x6764D0 (QuestCache)
// OFFSET: 0x676600 (PageTextCache_C)
// OFFSET: 0x676730 (PetNameCache)
// OFFSET: 0x676860 (CGPetition)
// OFFSET: 0x676990 (ItemTextCache_C)
// OFFSET: 0x676AC0 (WardenCachedModule)
// OFFSET: 0x676BF0 (ArenaTeamCache)
// OFFSET: 0x676D20 (DanceCache)
template <class T, class K, class H>
DBCache<T, K, H>::DBCache(uint32_t signature, const char* fileName, uint32_t queryOpcode, uint32_t unk40, uint8_t sendRequester, uint8_t enabled, uint32_t queriesPerMinute) {
    this->m_unk04 = "Blizzard::Using::GlobalUse";
    //Blizzard::Using::StartUsing("Blizzard::Using::GlobalUse");

    this->m_signature = signature;
    this->m_fileName = fileName;
    this->m_queryOpcode = queryOpcode;
    this->m_unk40 = unk40;
    this->m_sendRequester = sendRequester;
    this->m_enabled = enabled;

    this->m_version = 0;
    this->m_loaded = 0;
    this->m_queriesSent = 0;
    this->m_nextQueryTime = 0;
    this->m_maxQueriesPerPump = queriesPerMinute * WDB_IDLE_PERIOD_MS / 60000;
}

// OFFSET: 0x675D20 (CreatureStats_C)
// OFFSET: 0x675E50 (GameObjectStats_C)
// OFFSET: 0x675F80 (ItemName_C)
// OFFSET: 0x6760B0 (ItemStats_C)
// OFFSET: 0x6761E0 (NPCText)
// OFFSET: 0x676310 (NameCache)
// OFFSET: 0x676440 (GuildStats_C)
// OFFSET: 0x676570 (QuestCache)
// OFFSET: 0x6766A0 (PageTextCache_C)
// OFFSET: 0x6767D0 (PetNameCache)
// OFFSET: 0x676900 (CGPetition)
// OFFSET: 0x676A30 (ItemTextCache_C)
// OFFSET: 0x676B60 (WardenCachedModule)
// OFFSET: 0x676C90 (ArenaTeamCache)
// OFFSET: 0x676DC0 (DanceCache)
template <class T, class K, class H>
DBCache<T, K, H>::~DBCache() {
    this->Clear();
}

// OFFSET: 0x66F910 (CreatureStats_C)
// OFFSET: 0x66F980 (GameObjectStats_C + DanceCache)
// OFFSET: 0x66F9F0 (ItemName_C)
// OFFSET: 0x66FA60 (ItemStats_C)
// OFFSET: 0x66FAD0 (NPCText)
// OFFSET: 0x66FB40 (NameCache)
// OFFSET: 0x66FBB0 (GuildStats_C)
// OFFSET: 0x66FC20 (QuestCache)
// OFFSET: 0x66FC90 (PageTextCache_C)
// OFFSET: 0x66FD00 (PetNameCache)
// OFFSET: 0x66FD70 (CGPetition)
// OFFSET: 0x66FDE0 (ItemTextCache_C)
// OFFSET: 0x66FE50 (WardenCachedModule)
// OFFSET: 0x66FEC0 (ArenaTeamCache)
template <class T, class K, class H>
void DBCache<T, K, H>::Destroy() {
    this->Save();
    this->ClearPending();
    this->m_entries.Clear();
    this->m_reverse.Clear();
    this->m_loaded = 0;
}

// OFFSET: 0x66F940 (CreatureStats_C)
// OFFSET: 0x66F9B0 (GameObjectStats_C + DanceCache)
// OFFSET: 0x66FA20 (ItemName_C)
// OFFSET: 0x66FA90 (ItemStats_C)
// OFFSET: 0x66FB00 (NPCText)
// OFFSET: 0x66FB70 (NameCache)
// OFFSET: 0x66FBE0 (GuildStats_C)
// OFFSET: 0x66FC50 (QuestCache)
// OFFSET: 0x66FCC0 (PageTextCache_C)
// OFFSET: 0x66FD30 (PetNameCache)
// OFFSET: 0x66FDA0 (CGPetition)
// OFFSET: 0x66FE10 (ItemTextCache_C)
// OFFSET: 0x66FE80 (WardenCachedModule)
// OFFSET: 0x66FEF0 (ArenaTeamCache)
template <class T, class K, class H>
void DBCache<T, K, H>::VerifyAndUpdateVersion(uint32_t version) {
    if (!this->m_enabled) {
        return;
    }

    if (this->m_version == version) {
        return;
    }

    this->ClearPending();
    this->m_entries.Clear();
    this->m_reverse.Clear();
    this->m_version = version;
}

// OFFSET: 0x67B4F0 (CreatureStats_C)
// OFFSET: 0x67B500 (ItemName_C)
// OFFSET: 0x67B510 (ItemStats_C)
// OFFSET: 0x67B520 (NPCText)
// OFFSET: 0x67B530 (NameCache)
// OFFSET: 0x67B550 (GuildStats_C)
// OFFSET: 0x67B560 (QuestCache)
// OFFSET: 0x67B570 (PageTextCache_C)
// OFFSET: 0x67B580 (PetNameCache)
// OFFSET: 0x67B590 (CGPetition)
// OFFSET: 0x67B5A0 (ItemTextCache_C)
// OFFSET: 0x67B5C0 (WardenCachedModule)
// OFFSET: 0x67B5E0 (ArenaTeamCache)
// OFFSET: 0x67B5F0 (GameObjectStats_C + DanceCache)
template <class T, class K, class H>
void DBCache<T, K, H>::InvalidateIntKey(int32_t key) {
    this->Invalidate(K(key));
}

// OFFSET: 0x67A4C0 (NameCache)
template <class T, class K, class H>
typename DBCache<T, K, H>::NODE* DBCache<T, K, H>::SetNoSave(K key) {
    NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

    if (node) {
        node->m_trailer.m_noSave = 1;
    }

    return node;
}

// OFFSET: 0x67A360 (NameCache)
template <class T, class K, class H>
void DBCache<T, K, H>::AddQueryResult(K key) {
    NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

    if (node) {
        this->m_pending.LinkToTail(node);
    }
}

// OFFSET: 0x679E10 (CreatureStats_C)
// OFFSET: 0x679F00 (GameObjectStats_C)
// OFFSET: 0x67A0F0 (ItemStats_C)
// OFFSET: 0x67A3A0 (NameCache)
// OFFSET: 0x67A570 (GuildStats_C)
// OFFSET: 0x67A8C0 (PetNameCache)
template <class T, class K, class H>
void DBCache<T, K, H>::CancelCallback(K key, DBCACHE_CALLBACK callback, void* arg) {
    NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

    if (!node) {
        return;
    }

    if (node->m_trailer.m_dispatching) {
        ConsolePrintf("DBCache::CancelCallback ignored for id %016I64X.", static_cast<int64_t>(key));
        return;
    }

    for (auto entry = node->m_trailer.m_callbacks.Head(); entry;) {
        auto next = node->m_trailer.m_callbacks.Next(entry);

        if (entry->m_callback == callback && entry->m_arg == arg) {
            node->m_trailer.m_callbacks.DeleteNode(entry);
        }

        entry = next;
    }
}

// OFFSET: 0x66A010 (CreatureStats_C)
// OFFSET: 0x66A310 (GameObjectStats_C + DanceCache)
// OFFSET: 0x66A610 (ItemName_C)
// OFFSET: 0x66A900 (ItemStats_C)
// OFFSET: 0x66AC00 (NPCText)
// OFFSET: 0x66AF00 (NameCache)
// OFFSET: 0x66B210 (GuildStats_C)
// OFFSET: 0x66B510 (QuestCache)
// OFFSET: 0x66B810 (PageTextCache_C)
// OFFSET: 0x66BB00 (PetNameCache)
// OFFSET: 0x66BDF0 (CGPetition)
// OFFSET: 0x66C0F0 (ItemTextCache_C)
// OFFSET: 0x66C400 (WardenCachedModule)
// OFFSET: 0x66C710 (ArenaTeamCache)
template <class T, class K, class H>
void DBCache<T, K, H>::ClearPending() {
    auto node = this->m_entries.Head();

    while (node) {
        auto next = this->m_entries.Next(node);

        if (!node->m_trailer.m_loaded) {
            this->m_entries.Delete(node);
        }

        node = next;
    }
}

// OFFSET: 0x6701F0 (NameCache)
template <class T, class K, class H>
T* DBCache<T, K, H>::GetRecordByName(const char* name) {
    auto entry = this->m_reverse.Ptr(name);

    if (!entry) {
        return nullptr;
    }

    return entry->m_record;
}

// OFFSET: 0x67B6A0 (CreatureStats_C)
// OFFSET: 0x67BD40 (GameObjectStats_C)
// OFFSET: 0x67C3E0 (ItemName_C)
// OFFSET: 0x67CA30 (ItemStats_C)
// OFFSET: 0x67D0D0 (NPCText)
// OFFSET: 0x67D770 (NameCache)
// OFFSET: 0x67D930 (GuildStats_C)
// OFFSET: 0x67DE90 (QuestCache)
// OFFSET: 0x67E3E0 (PageTextCache_C)
// OFFSET: 0x67EA30 (PetNameCache)
// OFFSET: 0x67EF70 (CGPetition)
// OFFSET: 0x67F4C0 (ItemTextCache_C)
// OFFSET: 0x67FA80 (WardenCachedModule)
// OFFSET: 0x680170 (ArenaTeamCache)
// OFFSET: 0x6806D0 (DanceCache)
template <class T, class K, class H>
T* DBCache<T, K, H>::GetRecord(K key, const uint64_t* requester, DBCACHE_CALLBACK callback, void* arg, bool dedupe) {
    if (!key) {
        return nullptr;
    }

    NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

    if (!node) {
        if (!callback) {
            return nullptr;
        }

        node = this->m_entries.New(static_cast<uint32_t>(key), H(key), 0, 0);
        node->m_trailer.m_key = key;

        auto entry = node->m_trailer.m_callbacks.NewNode(STORM_LIST_TAIL, 0, 0);
        entry->m_callback = callback;
        entry->m_requester = *requester;
        entry->m_arg = arg;

        if (this->m_maxQueriesPerPump && this->m_queriesSent >= this->m_maxQueriesPerPump) {
            this->m_pending.LinkToTail(node);
            return nullptr;
        }

        this->SendSingleQuery(node);
        return nullptr;
    }

    if (node->m_trailer.m_loaded) {
        return &node->m_record;
    }

    if (!callback) {
        return nullptr;
    }

    if (dedupe) {
        for (auto entry = node->m_trailer.m_callbacks.Head(); entry; entry = node->m_trailer.m_callbacks.Next(entry)) {
            if (entry->m_callback == callback && entry->m_arg == arg) {
                return nullptr;
            }
        }
    }

    auto entry = node->m_trailer.m_callbacks.NewNode(STORM_LIST_TAIL, 0, 0);
    entry->m_callback = callback;
    entry->m_requester = *requester;
    entry->m_arg = arg;

    return nullptr;
}

// OFFSET: 0x67DAD0 (GuildStats_C)
// OFFSET: 0x67E030 (QuestCache)
// OFFSET: 0x67EBD0 (PetNameCache)
// OFFSET: 0x67F110 (CGPetition)
// OFFSET: 0x67F680 (ItemTextCache_C)
// OFFSET: 0x67FC90 (WardenCachedModule)
// OFFSET: 0x680310 (ArenaTeamCache)
// OFFSET: 0x680870 (DanceCache)
// OFFSET: 0x680C60 (NameCache)
template <class T, class K, class H>
void DBCache<T, K, H>::AddItem(const T* record, K key) {
    NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

    if (!node) {
        node = this->m_entries.New(static_cast<uint32_t>(key), H(key), 0, 0);
    }

    node->m_trailer.m_dispatching = 1;
    node->m_record = *record;
    node->m_trailer.m_loaded = 1;
    node->m_trailer.m_key = key;

    for (auto entry = node->m_trailer.m_callbacks.Head(); entry;) {
        auto next = node->m_trailer.m_callbacks.Next(entry);

        entry->m_callback(static_cast<uint32_t>(key), &entry->m_requester, entry->m_arg, 1);

        entry = next;
    }

    node->m_trailer.m_callbacks.Clear();

    bool invalidate = node->m_trailer.m_invalidatePending != 0;
    node->m_trailer.m_dispatching = 0;

    if (invalidate) {
        this->Invalidate(key);
    }
}

// OFFSET: 0x67B840 (CreatureStats_C)
// OFFSET: 0x67BEE0 (GameObjectStats_C)
// OFFSET: 0x67C570 (ItemName_C)
// OFFSET: 0x67CBD0 (ItemStats_C)
// OFFSET: 0x67D270 (NPCText)
// OFFSET: 0x67E570 (PageTextCache_C)
template <class T, class K, class H>
void DBCache<T, K, H>::AddItems(CDataStore* msg, bool single) {
    uint8_t count;

    if (single) {
        count = 1;
    } else {
        msg->Get(count);

        if (!count) {
            return;
        }
    }

    do {
        uint32_t raw;
        msg->Get(raw);

        K key = static_cast<K>(raw & 0x7FFFFFFF);
        bool deleted = (raw & 0x80000000) != 0;

        NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

        if (deleted) {
            if (!node) {
                continue;
            }

            node->m_trailer.m_dispatching = 1;

            this->m_entries.Unlink(node);

            for (auto entry = node->m_trailer.m_callbacks.Head(); entry;) {
                auto next = node->m_trailer.m_callbacks.Next(entry);

                entry->m_callback(static_cast<uint32_t>(key), &entry->m_requester, entry->m_arg, 0);

                entry = next;
            }

            node->m_trailer.m_dispatching = 0;

            this->m_entries.Delete(node);

            continue;
        }

        if (!node) {
            node = this->m_entries.New(static_cast<uint32_t>(key), H(key), 0, 0);
        }

        node->m_trailer.m_dispatching = 1;

        node->m_record.Unpack(msg);

        node->m_trailer.m_loaded = 1;
        node->m_trailer.m_key = key;

        for (auto entry = node->m_trailer.m_callbacks.Head(); entry;) {
            auto next = node->m_trailer.m_callbacks.Next(entry);

            entry->m_callback(static_cast<uint32_t>(key), &entry->m_requester, entry->m_arg, 1);

            entry = next;
        }

        node->m_trailer.m_callbacks.Clear();

        bool invalidate = node->m_trailer.m_invalidatePending != 0;
        node->m_trailer.m_dispatching = 0;

        if (invalidate) {
            this->Invalidate(key);
        }
    } while (--count);
}

// OFFSET: 0x679D90 (CreatureStats_C)
// OFFSET: 0x679F80 (ItemName_C)
// OFFSET: 0x67A070 (ItemStats_C)
// OFFSET: 0x67A1E0 (NPCText)
// OFFSET: 0x67A2D0 (NameCache)
// OFFSET: 0x67A4F0 (GuildStats_C)
// OFFSET: 0x67A660 (QuestCache)
// OFFSET: 0x67A750 (PageTextCache_C)
// OFFSET: 0x67A840 (PetNameCache)
// OFFSET: 0x67A9B0 (CGPetition)
// OFFSET: 0x67AAA0 (ItemTextCache_C)
// OFFSET: 0x67ABB0 (WardenCachedModule)
// OFFSET: 0x67AD70 (ArenaTeamCache)
// OFFSET: 0x67AE60 (GameObjectStats_C + DanceCache)
template <class T, class K, class H>
void DBCache<T, K, H>::DenyItem(K key) {
    NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

    if (!node) {
        return;
    }

    node->m_trailer.m_dispatching = 1;

    for (auto entry = node->m_trailer.m_callbacks.Head(); entry;) {
        auto next = node->m_trailer.m_callbacks.Next(entry);

        entry->m_callback(static_cast<uint32_t>(key), &entry->m_requester, entry->m_arg, 0);

        entry = next;
    }

    node->m_trailer.m_dispatching = 0;

    this->m_entries.Delete(node);
}

// OFFSET: 0x679E90 (CreatureStats_C)
// OFFSET: 0x67A000 (ItemName_C)
// OFFSET: 0x67A170 (ItemStats_C)
// OFFSET: 0x67A260 (NPCText)
// OFFSET: 0x67A430 (NameCache)
// OFFSET: 0x67A5F0 (GuildStats_C)
// OFFSET: 0x67A6E0 (QuestCache)
// OFFSET: 0x67A7D0 (PageTextCache_C)
// OFFSET: 0x67A940 (PetNameCache)
// OFFSET: 0x67AA30 (CGPetition)
// OFFSET: 0x67AB30 (ItemTextCache_C)
// OFFSET: 0x67ACA0 (WardenCachedModule)
// OFFSET: 0x67ADF0 (ArenaTeamCache)
// OFFSET: 0x67AEE0 (GameObjectStats_C + DanceCache)
template <class T, class K, class H>
void DBCache<T, K, H>::Invalidate(K key) {
    NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

    if (!node) {
        return;
    }

    if (!node->m_trailer.m_loaded) {
        this->DenyItem(key);
        return;
    }

    if (node->m_trailer.m_dispatching) {
        node->m_trailer.m_invalidatePending = 1;
        return;
    }

    this->m_entries.Delete(node);
}

// OFFSET: 0x669350 (CreatureStats_C)
// OFFSET: 0x669420 (GameObjectStats_C + DanceCache)
// OFFSET: 0x6694F0 (ItemName_C)
// OFFSET: 0x6695C0 (ItemStats_C)
// OFFSET: 0x669690 (NPCText)
// OFFSET: 0x669760 (NameCache)
// OFFSET: 0x669830 (GuildStats_C)
// OFFSET: 0x669900 (QuestCache)
// OFFSET: 0x6699D0 (PageTextCache_C)
// OFFSET: 0x669AA0 (PetNameCache)
// OFFSET: 0x669B70 (CGPetition)
// OFFSET: 0x669C40 (ItemTextCache_C)
// OFFSET: 0x669D10 (WardenCachedModule)
// OFFSET: 0x669DE0 (ArenaTeamCache)
template <class T, class K, class H>
void DBCache<T, K, H>::Idle() {
    if (!this->m_maxQueriesPerPump || !this->m_loaded) {
        return;
    }

    if (this->m_nextQueryTime && static_cast<int32_t>(static_cast<uint32_t>(OsGetAsyncTimeMs()) - this->m_nextQueryTime) < 0) {
        return;
    }

    this->m_queriesSent = 0;
    this->m_nextQueryTime = static_cast<uint32_t>(OsGetAsyncTimeMs()) + WDB_IDLE_PERIOD_MS;

    auto node = this->m_pending.Head();

    while (node) {
        auto next = this->m_pending.Next(node);

        this->m_pending.UnlinkNode(node);
        this->SendSingleQuery(node);

        if (this->m_maxQueriesPerPump && this->m_queriesSent >= this->m_maxQueriesPerPump) {
            break;
        }

        node = next;
    }
}

// OFFSET: 0x67BA40 (CreatureStats_C)
// OFFSET: 0x67C0E0 (GameObjectStats_C)
// OFFSET: 0x67C740 (ItemName_C)
// OFFSET: 0x67CDD0 (ItemStats_C)
// OFFSET: 0x67D470 (NPCText)
// OFFSET: 0x67DB90 (GuildStats_C)
// OFFSET: 0x67E0E0 (QuestCache)
// OFFSET: 0x67E740 (PageTextCache_C)
// OFFSET: 0x67EC80 (PetNameCache)
// OFFSET: 0x67F1C0 (CGPetition)
// OFFSET: 0x67F760 (ItemTextCache_C)
// OFFSET: 0x67FE00 (WardenCachedModule)
// OFFSET: 0x6803D0 (ArenaTeamCache)
// OFFSET: 0x680920 (DanceCache)
// OFFSET: 0x680D70 (NameCache)
template <class T, class K, class H>
void DBCache<T, K, H>::Load() {
    this->m_loaded = 1;

    if (!this->m_enabled) {
        return;
    }

    char directory[260];
    DBCacheGetDirectory(directory);

    char path[260];
    SStrPrintf(path, sizeof(path), "%s/%s", directory, this->m_fileName);

    HOSFILE file = OsCreateFile(path, OS_GENERIC_READ, OS_FILE_SHARE_READ, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_DEFAULT);

    if (file == HOSFILE_INVALID) {
        return;
    }

    uint8_t block[16384];
    uint8_t* buffer = block;
    uint32_t bufferSize = sizeof(block);
    uint32_t read;

    OsReadFile(file, block, sizeof(WDBHeader), &read);

    if (read != sizeof(WDBHeader)) {
        OsCloseFile(file);
        return;
    }

    CDataStore header(block, sizeof(WDBHeader));

    uint32_t signature;
    uint32_t build;
    uint32_t locale;
    uint32_t recordSize;
    uint32_t version;

    header.Get(signature);

    if (signature != this->m_signature || (header.Get(build), build != WDB_EXPECTED_BUILD) || (header.Get(locale), locale != static_cast<uint32_t>(SFile::s_locale)) || (header.Get(recordSize), recordSize != sizeof(T)) || (header.Get(version), version != T::WDB_VERSION)) {
        OsCloseFile(file);
        return;
    }

    uint32_t cacheVersion;
    header.Get(cacheVersion);
    this->m_version = cacheVersion;

    bool failed = false;

    OsReadFile(file, buffer, sizeof(K) + sizeof(uint32_t), &read);

    if (read != sizeof(K) + sizeof(uint32_t)) {
        failed = true;
    } else {
        while (1) {
            {
                CDataStore entry(buffer, sizeof(K) + sizeof(uint32_t));

                K key;
                entry.GetArray(reinterpret_cast<uint8_t*>(&key), sizeof(K));

                uint32_t size;
                entry.Get(size);

                if (!key) {
                    break;
                }

                if (size > bufferSize) {
                    if (bufferSize > sizeof(block)) {
                        SMemFree(buffer, __FILE__, __LINE__, 0);
                    }

                    bufferSize = size;
                    buffer = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
                }

                OsReadFile(file, buffer, size, &read);

                if (read != size) {
                    failed = true;
                    break;
                }

                {
                    CDataStore body(buffer, size);

                    NODE* node = this->m_entries.Ptr(static_cast<uint32_t>(key), H(key));

                    if (!node) {
                        node = this->m_entries.New(static_cast<uint32_t>(key), H(key), 0, 0);
                    }

                    node->m_record.Unpack(&body);
                    node->m_trailer.m_loaded = 1;
                    node->m_trailer.m_key = key;
                }
            }

            OsReadFile(file, buffer, sizeof(K) + sizeof(uint32_t), &read);

            if (read != sizeof(K) + sizeof(uint32_t)) {
                failed = true;
                break;
            }
        }
    }

    if (bufferSize > sizeof(block)) {
        SMemFree(buffer, __FILE__, __LINE__, 0);
    }

    OsCloseFile(file);

    if (failed) {
        this->Clear();
    }
}

// OFFSET: 0x66CC90
template <class T, class K, class H>
void DBCache<T, K, H>::Clear() {
    this->m_entries.Clear();
    this->m_reverse.Clear();
}

// OFFSET: 0x66A090 (CreatureStats_C)
// OFFSET: 0x66A390 (GameObjectStats_C)
// OFFSET: 0x66A690 (ItemName_C)
// OFFSET: 0x66A980 (ItemStats_C)
// OFFSET: 0x66AC80 (NPCText)
// OFFSET: 0x66AF80 (NameCache)
// OFFSET: 0x66B290 (GuildStats_C)
// OFFSET: 0x66B590 (QuestCache)
// OFFSET: 0x66B890 (PageTextCache_C)
// OFFSET: 0x66BB80 (PetNameCache)
// OFFSET: 0x66BE70 (CGPetition)
// OFFSET: 0x66C170 (ItemTextCache_C)
// OFFSET: 0x66C480 (WardenCachedModule)
// OFFSET: 0x66C790 (ArenaTeamCache)
// OFFSET: 0x66CA10 (DanceCache)
template <class T, class K, class H>
void DBCache<T, K, H>::Save() {
    if (!this->m_enabled || !this->m_loaded) {
        return;
    }

    char directory[260];
    DBCacheGetDirectory(directory);

    char path[260];
    SStrPrintf(path, sizeof(path), "%s/%s", directory, this->m_fileName);

    HOSFILE file = OsCreateFile(path, OS_GENERIC_WRITE, OS_FILE_SHARE_READ, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_DEFAULT);

    if (file == HOSFILE_INVALID) {
        return;
    }

    void* data;
    uint32_t written;

    CDataStore header;
    header.Put(this->m_signature);
    header.Put(static_cast<uint32_t>(WDB_EXPECTED_BUILD));
    header.Put(static_cast<uint32_t>(SFile::s_locale));
    header.Put(static_cast<uint32_t>(sizeof(T)));
    header.Put(static_cast<uint32_t>(T::WDB_VERSION));
    header.Put(this->m_version);

    header.Seek(0);
    header.GetDataInSitu(data, header.Size());
    OsWriteFile(file, data, header.Size(), &written);

    CDataStore record;

    for (auto node = this->m_entries.Head(); node; node = this->m_entries.Next(node)) {
        if (!node->m_trailer.m_loaded || node->m_trailer.m_noSave) {
            continue;
        }

        record.Reset();
        record.PutArray(reinterpret_cast<const uint8_t*>(&node->m_trailer.m_key), sizeof(K));

        uint32_t sizePos = record.Size();
        record.Put(static_cast<uint32_t>(0));

        node->m_record.Pack(&record);
        record.Set(sizePos, static_cast<uint32_t>(record.Size() - (sizeof(K) + sizeof(uint32_t))));

        record.Seek(0);
        record.GetDataInSitu(data, record.Size());
        OsWriteFile(file, data, record.Size(), &written);
    }

    K terminatorKey = 0;
    uint32_t terminatorSize = 0;

    OsWriteFile(file, &terminatorKey, sizeof(terminatorKey), &written);
    OsWriteFile(file, &terminatorSize, sizeof(terminatorSize), &written);

    OsCloseFile(file);
}

// OFFSET: 0x668970 (CreatureStats_C)
// OFFSET: 0x668A20 (GameObjectStats_C + DanceCache)
// OFFSET: 0x668AD0 (ItemName_C)
// OFFSET: 0x668B80 (ItemStats_C)
// OFFSET: 0x668C30 (NPCText)
// OFFSET: 0x668CE0 (NameCache)
// OFFSET: 0x668DA0 (GuildStats_C)
// OFFSET: 0x668E50 (QuestCache)
// OFFSET: 0x668F00 (PageTextCache_C)
// OFFSET: 0x668FB0 (PetNameCache)
// OFFSET: 0x669060 (CGPetition)
// OFFSET: 0x669110 (ItemTextCache_C)
// OFFSET: 0x6691D0 (WardenCachedModule)
// OFFSET: 0x6692A0 (ArenaTeamCache)
template <class T, class K, class H>
void DBCache<T, K, H>::SendSingleQuery(NODE* node) {
    CDataStore msg;

    msg.Put(this->m_queryOpcode);
    msg.PutArray(reinterpret_cast<const uint8_t*>(&node->m_trailer.m_key), sizeof(K));

    if (this->m_sendRequester) {
        auto entry = node->m_trailer.m_callbacks.Head();

        if (entry) {
            msg.Put(entry->m_requester);
        } else {
            msg.Put(static_cast<uint64_t>(0));
        }
    }

    msg.Finalize();
    ClientServices::Send2(&msg);

    this->m_queriesSent++;
}

template class DBCache<CreatureStats_C, int32_t, HASHKEY_INT>;
template class DBCache<GameObjectStats_C, int32_t, HASHKEY_INT>;
template class DBCache<ItemName_C, int32_t, HASHKEY_INT>;
template class DBCache<ItemStats_C, int32_t, HASHKEY_INT>;
template class DBCache<NPCText, int32_t, HASHKEY_INT>;
template class DBCache<NameCache, uint64_t, CHashKeyGUID>;
template class DBCache<GuildStats_C, int32_t, HASHKEY_INT>;
template class DBCache<QuestCache, int32_t, HASHKEY_INT>;
template class DBCache<PageTextCache_C, int32_t, HASHKEY_INT>;
template class DBCache<PetNameCache, int32_t, HASHKEY_INT>;
template class DBCache<CGPetition, int32_t, HASHKEY_INT>;
template class DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID>;
template class DBCache<WardenCachedModule, CWardenKey, CWardenKey>;
template class DBCache<ArenaTeamCache, int32_t, HASHKEY_INT>;
template class DBCache<DanceCache, int32_t, HASHKEY_INT>;
