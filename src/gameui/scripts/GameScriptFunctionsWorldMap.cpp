#include "gameui/GameScriptFunctions.hpp"
#include "gameui/CGWorldMap.hpp"
#include "ui/CSimpleFrame.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x544B20
static int32_t Script_GetMapContinents(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x544B90
static int32_t Script_GetMapZones(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5469E0
static int32_t Script_SetMapZoom(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x546A90
static int32_t Script_ZoomOut(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x547B80
static int32_t Script_SetDungeonMapLevel(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x546290
static int32_t Script_GetNumDungeonMapLevels(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x544C40
static int32_t Script_DungeonUsesTerrainMap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x547C10
static int32_t Script_SetMapToCurrentZone(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x544CA0
static int32_t Script_GetMapInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x544D40
static int32_t Script_GetCurrentMapContinent(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x544E10
static int32_t Script_GetCurrentMapAreaID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x544E80
static int32_t Script_GetCurrentMapZone(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x544FC0
static int32_t Script_GetCurrentMapDungeonLevel(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x546C50
static int32_t Script_SetMapByID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x545050
static int32_t Script_IsZoomOutAvailable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x546E80
static int32_t Script_ProcessMapClick(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x545110
static int32_t Script_UpdateMapHighlight(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x545880
static int32_t Script_GetPlayerMapPosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x545950
static int32_t Script_GetCorpseMapPosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5459C0
static int32_t Script_GetDeathReleasePosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x543020
static int32_t Script_GetNumMapLandmarks(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x545A30
static int32_t Script_GetMapLandmarkInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x543060
static int32_t Script_GetNumMapOverlays(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x545C80
static int32_t Script_GetMapOverlayInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x545E60
static int32_t Script_CreateWorldMapArrowFrame(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Usage: CreateWorldMapArrowFrame(parent)");
    }

    lua_rawgeti(L, 1, 0);
    auto frame = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!frame) {
        luaL_error(L, "CreateWorldMapArrowFrame(): Couldn't find 'this' in parent object");
    }

    if (!frame->IsA(CSimpleFrame::GetObjectType())) {
        luaL_error(L, "CreateWorldMapArrowFrame(): Wrong object type, expected frame");
    }

    CGWorldMap::CreateArrowFrame(frame);

    return 0;
}

// OFFSET: 0x545FF0
static int32_t Script_InitWorldMapPing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x545F20
static int32_t Script_CreateMiniWorldMapArrowFrame(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x545FE0
static int32_t Script_UpdateWorldMapArrowFrames(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5430A0
static int32_t Script_PositionWorldMapArrowFrame(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5432C0
static int32_t Script_PositionMiniWorldMapArrowFrame(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5434E0
static int32_t Script_ShowWorldMapArrowFrame(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x543540
static int32_t Script_ShowMiniWorldMapArrowFrame(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x546EF0
static int32_t Script_ClickLandmark(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x608560
static int32_t Script_GetNumMapDebugObjects(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x8E5250
static int32_t Script_GetMapDebugObjectInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x8E5250
static int32_t Script_TeleportToDebugObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x8E5250
static int32_t Script_HasDebugZoneMap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x8E5250
static int32_t Script_GetDebugZoneMap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5435A0
static int32_t Script_GetWintergraspWaitTime(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x543600
static int32_t Script_CanQueueForWintergrasp(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void WorldMapRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_WORLD_MAP; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_WorldMap[i].name,
            GameScript::s_ScriptFunctions_WorldMap[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_WorldMap[NUM_SCRIPT_FUNCTIONS_WORLD_MAP] = {
    { "GetMapContinents", &Script_GetMapContinents },
    { "GetMapZones", &Script_GetMapZones },
    { "SetMapZoom", &Script_SetMapZoom },
    { "ZoomOut", &Script_ZoomOut },
    { "SetDungeonMapLevel", &Script_SetDungeonMapLevel },
    { "GetNumDungeonMapLevels", &Script_GetNumDungeonMapLevels },
    { "DungeonUsesTerrainMap", &Script_DungeonUsesTerrainMap },
    { "SetMapToCurrentZone", &Script_SetMapToCurrentZone },
    { "GetMapInfo", &Script_GetMapInfo },
    { "GetCurrentMapContinent", &Script_GetCurrentMapContinent },
    { "GetCurrentMapAreaID", &Script_GetCurrentMapAreaID },
    { "GetCurrentMapZone", &Script_GetCurrentMapZone },
    { "GetCurrentMapDungeonLevel", &Script_GetCurrentMapDungeonLevel },
    { "SetMapByID", &Script_SetMapByID },
    { "IsZoomOutAvailable", &Script_IsZoomOutAvailable },
    { "ProcessMapClick", &Script_ProcessMapClick },
    { "UpdateMapHighlight", &Script_UpdateMapHighlight },
    { "GetPlayerMapPosition", &Script_GetPlayerMapPosition },
    { "GetCorpseMapPosition", &Script_GetCorpseMapPosition },
    { "GetDeathReleasePosition", &Script_GetDeathReleasePosition },
    { "GetNumMapLandmarks", &Script_GetNumMapLandmarks },
    { "GetMapLandmarkInfo", &Script_GetMapLandmarkInfo },
    { "GetNumMapOverlays", &Script_GetNumMapOverlays },
    { "GetMapOverlayInfo", &Script_GetMapOverlayInfo },
    { "CreateWorldMapArrowFrame", &Script_CreateWorldMapArrowFrame },
    { "InitWorldMapPing", &Script_InitWorldMapPing },
    { "CreateMiniWorldMapArrowFrame", &Script_CreateMiniWorldMapArrowFrame },
    { "UpdateWorldMapArrowFrames", &Script_UpdateWorldMapArrowFrames },
    { "PositionWorldMapArrowFrame", &Script_PositionWorldMapArrowFrame },
    { "PositionMiniWorldMapArrowFrame", &Script_PositionMiniWorldMapArrowFrame },
    { "ShowWorldMapArrowFrame", &Script_ShowWorldMapArrowFrame },
    { "ShowMiniWorldMapArrowFrame", &Script_ShowMiniWorldMapArrowFrame },
    { "ClickLandmark", &Script_ClickLandmark },
    { "GetNumMapDebugObjects", &Script_GetNumMapDebugObjects },
    { "GetMapDebugObjectInfo", &Script_GetMapDebugObjectInfo },
    { "TeleportToDebugObject", &Script_TeleportToDebugObject },
    { "HasDebugZoneMap", &Script_HasDebugZoneMap },
    { "GetDebugZoneMap", &Script_GetDebugZoneMap },
    { "GetWintergraspWaitTime", &Script_GetWintergraspWaitTime },
    { "CanQueueForWintergrasp", &Script_CanQueueForWintergrasp },
};
