#include "ui/ScriptFunctions.hpp"
#include "ui/Types.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"
#include <common/Time.hpp>
#include <cstdint>

// OFFSET: 0x6081F0
int32_t Script_GetTime(lua_State* L) {
    uint64_t ms = OsGetAsyncTimeMs();
    lua_pushnumber(L, static_cast<double>(ms) / 1000.0);

    return 1;
}

// OFFSET: 0x608230
int32_t Script_GetGameTime(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x608270
int32_t Script_ConsoleExec(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x510BB0
int32_t Script_AccessDenied(lua_State* L) {
    return luaL_error(L, "Access Denied");
}

FrameScript_Method FrameScript::s_ScriptFunctions_System[NUM_SCRIPT_FUNCTIONS_SYSTEM] = {
    { "GetTime", &Script_GetTime },
    { "GetGameTime", &Script_GetGameTime },
    { "ConsoleExec", &Script_ConsoleExec },
    { "ReadFile", &Script_AccessDenied },
    { "DeleteFile", &Script_AccessDenied },
    { "AppendToFile", &Script_AccessDenied },
    { "GetAccountExpansionLevel", &Script_GetAccountExpansionLevel }
};
