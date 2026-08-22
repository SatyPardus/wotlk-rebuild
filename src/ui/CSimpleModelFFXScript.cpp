#include "ui/CSimpleModelFFXScript.hpp"
#include "util/Unimplemented.hpp"
#include <cstdint>

// OFFSET: 0x4E6BE0
int32_t CSimpleModelFFX_ResetLights(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4E6C60
int32_t CSimpleModelFFX_AddLight(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4E6D60
int32_t CSimpleModelFFX_AddCharacterLight(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4E6E60
int32_t CSimpleModelFFX_AddPetLight(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

FrameScript_Method SimpleModelFFXMethods[NUM_SIMPLE_MODEL_FFX_SCRIPT_METHODS] = {
    { "ResetLights",        &CSimpleModelFFX_ResetLights },
    { "AddLight",           &CSimpleModelFFX_AddLight },
    { "AddCharacterLight",  &CSimpleModelFFX_AddCharacterLight },
    { "AddPetLight",        &CSimpleModelFFX_AddPetLight }
};
