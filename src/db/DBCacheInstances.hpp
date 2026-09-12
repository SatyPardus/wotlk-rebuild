#ifndef DB_CACHE_DB_CACHE_INSTANCES_HPP
#define DB_CACHE_DB_CACHE_INSTANCES_HPP

#include "clientobject/CHashKeyGUID.hpp"
#include "db/DBCache.hpp"
#include "db/cache/ArenaTeamCache.hpp"
#include "db/cache/CGPetition.hpp"
#include "db/cache/CreatureStats.hpp"
#include "db/cache/DanceCache.hpp"
#include "db/cache/GameObjectStats.hpp"
#include "db/cache/GuildStats.hpp"
#include "db/cache/ItemName.hpp"
#include "db/cache/ItemStats.hpp"
#include "db/cache/ItemTextCache.hpp"
#include "db/cache/NPCText.hpp"
#include "db/cache/NameCache.hpp"
#include "db/cache/PageTextCache.hpp"
#include "db/cache/PetNameCache.hpp"
#include "db/cache/QuestCache.hpp"
#include "db/cache/WardenCachedModule.hpp"
#include <storm/Hash.hpp>
#include <cstdint>
#include <cstring>

class CWardenKey {
    public:
    CWardenKey() {
        memset(this->m_key, 0, sizeof(this->m_key));
    }

    // OFFSET: 0x67B5C0
    CWardenKey(int32_t) {
        memset(this->m_key, 0, sizeof(this->m_key));
    }

    // OFFSET: 0x67FA90
    bool operator!() const {
        for (uint32_t i = 0; i < sizeof(this->m_key); i++) {
            if (this->m_key[i]) {
                return false;
            }
        }

        return true;
    }

    bool operator==(const CWardenKey& key) const {
        return memcmp(this->m_key, key.m_key, sizeof(this->m_key)) == 0;
    }

    explicit operator int64_t() const {
        return static_cast<int64_t>(static_cast<uint32_t>(*this));
    }

    // OFFSET: 0x67ABC7
    explicit operator uint32_t() const {
        uint32_t v = this->m_key[12] | (this->m_key[13] << 8) | (this->m_key[14] << 16) | (this->m_key[15] << 24);
        uint32_t h = (v << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00) | (v >> 24);

        return h & 0x7FFFFFFF;
    }

    private:
    /* 0x00 */ uint8_t m_key[16];
};

extern DBCache<CreatureStats_C, int32_t, HASHKEY_INT> g_creatureCache;
extern DBCache<GameObjectStats_C, int32_t, HASHKEY_INT> g_gameObjectCache;
extern DBCache<ItemName_C, int32_t, HASHKEY_INT> g_itemNameCache;
extern DBCache<ItemStats_C, int32_t, HASHKEY_INT> g_itemCache;
extern DBCache<NPCText, int32_t, HASHKEY_INT> g_npcCache;
extern DBCache<NameCache, uint64_t, CHashKeyGUID> g_nameCache;
extern DBCache<GuildStats_C, int32_t, HASHKEY_INT> g_guildCache;
extern DBCache<QuestCache, int32_t, HASHKEY_INT> g_questCache;
extern DBCache<PageTextCache_C, int32_t, HASHKEY_INT> g_pageTextCache;
extern DBCache<PetNameCache, int32_t, HASHKEY_INT> g_petNameCache;
extern DBCache<CGPetition, int32_t, HASHKEY_INT> g_petitionCache;
extern DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID> g_itemTextCache;
extern DBCache<WardenCachedModule, CWardenKey, CWardenKey> g_wardenCache;
extern DBCache<ArenaTeamCache, int32_t, HASHKEY_INT> g_arenaTeamCache;
extern DBCache<DanceCache, int32_t, HASHKEY_INT> g_danceCache;

void DbCache_LoadAll();
void DbCache_RegisterHandlers();

#endif
