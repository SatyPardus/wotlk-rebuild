#include "gameui/CGDressUpModelFrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"

// OFFSET: 0x597E90
static int32_t Script_Undress(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x597ED0
static int32_t Script_Dress(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x598830
static int32_t Script_TryOn(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}


FrameScript_Method CGDressUpModelFrameMethods[NUM_CGDRESS_UP_MODEL_FRAME_SCRIPT_METHODS] = {
    { "Undress", &Script_Undress },
    { "Dress", &Script_Dress },
    { "TryOn", &Script_TryOn }
};
