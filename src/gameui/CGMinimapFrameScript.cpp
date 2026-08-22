#include "gameui/CGMinimapFrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"

// OFFSET: 0x583860
static int32_t Script_SetMaskTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x583CE0
static int32_t Script_SetIconTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x583E00
static int32_t Script_SetBlipTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x583F60
static int32_t Script_SetClassBlipTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x583980
static int32_t Script_SetPOIArrowTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x583AA0
static int32_t Script_SetStaticPOIArrowTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x583BC0
static int32_t Script_SetCorpsePOIArrowTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57E100
static int32_t Script_SetPlayerTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57E1C0
static int32_t Script_SetPlayerTextureHeight(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57E280
static int32_t Script_SetPlayerTextureWidth(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57BF50
static int32_t Script_GetZoomLevels(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57BF90
static int32_t Script_GetZoom(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57BFD0
static int32_t Script_SetZoom(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57ED70
static int32_t Script_PingLocation(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57EFE0
static int32_t Script_GetPingPosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}


FrameScript_Method CGMinimapFrameMethods[NUM_CGMINIMAP_FRAME_SCRIPT_METHODS] = {
    { "SetMaskTexture", &Script_SetMaskTexture },
    { "SetIconTexture", &Script_SetIconTexture },
    { "SetBlipTexture", &Script_SetBlipTexture },
    { "SetClassBlipTexture", &Script_SetClassBlipTexture },
    { "SetPOIArrowTexture", &Script_SetPOIArrowTexture },
    { "SetStaticPOIArrowTexture", &Script_SetStaticPOIArrowTexture },
    { "SetCorpsePOIArrowTexture", &Script_SetCorpsePOIArrowTexture },
    { "SetPlayerTexture", &Script_SetPlayerTexture },
    { "SetPlayerTextureHeight", &Script_SetPlayerTextureHeight },
    { "SetPlayerTextureWidth", &Script_SetPlayerTextureWidth },
    { "GetZoomLevels", &Script_GetZoomLevels },
    { "GetZoom", &Script_GetZoom },
    { "SetZoom", &Script_SetZoom },
    { "PingLocation", &Script_PingLocation },
    { "GetPingPosition", &Script_GetPingPosition }
};
