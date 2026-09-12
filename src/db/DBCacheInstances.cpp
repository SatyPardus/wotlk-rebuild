#include "db/DBCacheInstances.hpp"
#include "net/Types.hpp"
#include "client/ClientServices.hpp"
#include <util/Unimplemented.hpp>

// OFFSET: 0x00C5D690
DBCache<CreatureStats_C, int32_t, HASHKEY_INT> g_creatureCache('WMOB', "creaturecache.wdb", CMSG_CREATURE_QUERY, 0, 1, 1, 0);

// OFFSET: 0x00C5D718
DBCache<GameObjectStats_C, int32_t, HASHKEY_INT> g_gameObjectCache('WGOB', "gameobjectcache.wdb", CMSG_GAMEOBJECT_QUERY, 0, 1, 1, 0);

// OFFSET: 0x00C5D7A0
DBCache<ItemName_C, int32_t, HASHKEY_INT> g_itemNameCache('WNDB', "itemnamecache.wdb", CMSG_ITEM_NAME_QUERY, 0, 1, 1, 0);

// OFFSET: 0x00C5D828
DBCache<ItemStats_C, int32_t, HASHKEY_INT> g_itemCache('WIDB', "itemcache.wdb", CMSG_ITEM_QUERY_SINGLE, CMSG_ITEM_QUERY_MULTIPLE, 0, 1, 512);

// OFFSET: 0x00C5D8B0
DBCache<NPCText, int32_t, HASHKEY_INT> g_npcCache('WNPC', "npccache.wdb", CMSG_NPC_TEXT_QUERY, 0, 1, 1, 0);

// OFFSET: 0x00C5D938
DBCache<NameCache, uint64_t, CHashKeyGUID> g_nameCache('WNAM', "namecache.wdb", CMSG_NAME_QUERY, 0, 0, 0, 256);

// OFFSET: 0x00C5D9C0
DBCache<GuildStats_C, int32_t, HASHKEY_INT> g_guildCache('WGLD', "guildcache.wdb", CMSG_GUILD_QUERY, 0, 0, 0, 0);

// OFFSET: 0x00C5DA48
DBCache<QuestCache, int32_t, HASHKEY_INT> g_questCache('WQST', "questcache.wdb", CMSG_QUEST_QUERY, 0, 0, 1, 0);

// OFFSET: 0x00C5DAD0
DBCache<PageTextCache_C, int32_t, HASHKEY_INT> g_pageTextCache('WPTX', "pagetextcache.wdb", CMSG_PAGE_TEXT_QUERY, 0, 1, 1, 0);

// OFFSET: 0x00C5DB58
DBCache<PetNameCache, int32_t, HASHKEY_INT> g_petNameCache('WPNM', "petnamecache.wdb", CMSG_PET_NAME_QUERY, 0, 1, 0, 0);

// OFFSET: 0x00C5DBE0
DBCache<CGPetition, int32_t, HASHKEY_INT> g_petitionCache('WPTN', "petitioncache.wdb", CMSG_PETITION_QUERY, 0, 1, 0, 0);

// OFFSET: 0x00C5DC68
DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID> g_itemTextCache('WITX', "itemtextcache.wdb", CMSG_ITEM_TEXT_QUERY, 0, 0, 1, 0);

// OFFSET: 0x00C5DCF0
DBCache<WardenCachedModule, CWardenKey, CWardenKey> g_wardenCache('WRDN', "wowcache.wdb", 0, 0, 1, 1, 0);

// OFFSET: 0x00C5DD78
DBCache<ArenaTeamCache, int32_t, HASHKEY_INT> g_arenaTeamCache('WATM', "arenateamcache.wdb", CMSG_ARENA_TEAM_QUERY, 0, 0, 0, 0);

// OFFSET: 0x00C5DE00
DBCache<DanceCache, int32_t, HASHKEY_INT> g_danceCache('WDAN', "dancecache.wdb", CMSG_DANCE_QUERY, 0, 0, 0, 0);

// OFFSET: 0x635060
void DbCache_LoadAll() {
    g_creatureCache.Load();
    g_gameObjectCache.Load();
    g_itemNameCache.Load();
    g_itemCache.Load();
    g_npcCache.Load();
    g_nameCache.Load();
    g_guildCache.Load();
    g_questCache.Load();
    g_pageTextCache.Load();
    g_petNameCache.Load();
    g_petitionCache.Load();
    g_itemTextCache.Load();
    g_wardenCache.Load();
    g_arenaTeamCache.Load();
    g_danceCache.Load();
}

int32_t Packet_SMSG_CREATURE_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_GAMEOBJECT_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_ITEM_NAME_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_Group_8(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_ITEM_QUERY_MULTIPLE_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_NPC_TEXT_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_NAME_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_GUILD_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_QUEST_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_PAGE_TEXT_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_PET_NAME_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_PETITION_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_ITEM_TEXT_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_INVALIDATE_PLAYER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_ARENA_TEAM_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_INVALIDATE_DANCE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Packet_SMSG_DANCE_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x635B40
void DbCache_RegisterHandlers() {
    ClientServices::SetMessageHandler(SMSG_CREATURE_QUERY_RESPONSE, Packet_SMSG_CREATURE_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GAMEOBJECT_QUERY_RESPONSE, Packet_SMSG_GAMEOBJECT_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_NAME_QUERY_RESPONSE, Packet_SMSG_ITEM_NAME_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_QUERY_SINGLE_RESPONSE, Packet_Group_8, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_QUERY_MULTIPLE_RESPONSE, Packet_SMSG_ITEM_QUERY_MULTIPLE_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_NPC_TEXT_UPDATE, Packet_SMSG_NPC_TEXT_UPDATE, nullptr);
    ClientServices::SetMessageHandler(SMSG_NAME_QUERY_RESPONSE, Packet_SMSG_NAME_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GUILD_QUERY_RESPONSE, Packet_SMSG_GUILD_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUEST_QUERY_RESPONSE, Packet_SMSG_QUEST_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_PAGE_TEXT_QUERY_RESPONSE, Packet_SMSG_PAGE_TEXT_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_NAME_QUERY_RESPONSE, Packet_SMSG_PET_NAME_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_PETITION_QUERY_RESPONSE, Packet_SMSG_PETITION_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_TEXT_QUERY_RESPONSE, Packet_SMSG_ITEM_TEXT_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_INVALIDATE_PLAYER, Packet_SMSG_INVALIDATE_PLAYER, nullptr);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_QUERY_RESPONSE, Packet_SMSG_ARENA_TEAM_QUERY_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_CHEAT_DUMP_ITEMS_DEBUG_ONLY_RESPONSE, Packet_Group_8, nullptr);
    ClientServices::SetMessageHandler(SMSG_INVALIDATE_DANCE, Packet_SMSG_INVALIDATE_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_DANCE_QUERY_RESPONSE, Packet_SMSG_DANCE_QUERY_RESPONSE, nullptr);
}
