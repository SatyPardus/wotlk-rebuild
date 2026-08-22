#include "ui/CSimpleModelFFXScript.hpp"
#include "ui/CSimpleModelFFX.hpp"
#include "util/Unimplemented.hpp"
#include "util/Lua.hpp"
#include <cstdint>

// OFFSET: 0x4E6BE0
int32_t CSimpleModelFFX_ResetLights(lua_State* L) {
    auto type = CSimpleModelFFX::GetObjectType();
    auto model = static_cast<CSimpleModelFFX*>(FrameScript_GetObjectThis(L, type));

    //model->m_lightArray3[0].ukn1 = 0;
    //model->m_lightArray3[0].uknByte1 = 0;
    //model->m_lightArray2[0].ukn1 = 0;
    //model->m_lightArray2[0].uknByte1 = 0;
    //model->m_lightArray1[0].ukn1 = 0;
    //model->m_lightArray1[0].uknByte1 = 0;
    //model->m_lightArray3[1].uknByte1 = 0;
    //model->m_lightArray3[1].ukn1 = 0;
    //model->m_lightArray2[1].uknByte1 = 0;
    //model->m_lightArray2[1].ukn1 = 0;
    //model->m_lightArray1[1].uknByte1 = 0;
    //model->m_lightArray1[1].ukn1 = 0;
    return 0;
}

// OFFSET: 0x4E6C60
int32_t CSimpleModelFFX_AddLight(lua_State* L) {
    auto type = CSimpleModelFFX::GetObjectType();
    auto model = static_cast<CSimpleModelFFX*>(FrameScript_GetObjectThis(L, type));

    auto v2 = "model";
    if (!model) {
        luaL_error(L, "Usage: %s:AddLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    }

    uint32_t index = 0;
    if (lua_isnumber(L, 2))
        index = lua_tonumber(L, 2);

    //CM2Light light = CM2Light();
    //if (!maybe_CSimpleModel__CSimpleModel_SetLightHelper(L, 3, &light)) {
    //    v2 = model->GetDisplayName();
    //    light.Unlink();
    //    luaL_error(L, "Usage: %s:AddLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    //}
    //v6 = model->m_lightArray1[index].ukn1;
    //v7 = &model->m_lightArray1[index];
    //if (v6 < 4) {
    //    memcpy(&v7->m_lights[v6], &light, sizeof(v7->m_lights[v6]));
    //    ++v7->ukn1;
    //    model->m_lightArray1[index].uknByte1 = 1;
    //}
    //light.Unlink();
    return 0;
}

// OFFSET: 0x4E6D60
int32_t CSimpleModelFFX_AddCharacterLight(lua_State* L) {
    auto type = CSimpleModelFFX::GetObjectType();
    auto model = static_cast<CSimpleModelFFX*>(FrameScript_GetObjectThis(L, type));

    auto v2 = "model";
    if (!model) {
        luaL_error(L, "Usage: %s:AddCharacterLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    }

    uint32_t index = 0;
    if (lua_isnumber(L, 2))
        index = lua_tonumber(L, 2);

    // CM2Light light = CM2Light();
    // if (!maybe_CSimpleModel__CSimpleModel_SetLightHelper(L, 3, &light)) {
    //     v2 = model->GetDisplayName();
    //     light.Unlink();
    //     luaL_error(L, "Usage: %s:AddCharacterLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    // }
    // v6 = model->m_lightArray2[index].ukn1;
    // v7 = &model->m_lightArray2[index];
    // if (v6 < 4) {
    //     memcpy(&v7->m_lights[v6], &light, sizeof(v7->m_lights[v6]));
    //     ++v7->ukn1;
    //     model->m_lightArray2[index].uknByte1 = 1;
    // }
    // light.Unlink();
    return 0;
}

// OFFSET: 0x4E6E60
int32_t CSimpleModelFFX_AddPetLight(lua_State* L) {
    auto type = CSimpleModelFFX::GetObjectType();
    auto model = static_cast<CSimpleModelFFX*>(FrameScript_GetObjectThis(L, type));

    auto v2 = "model";
    if (!model) {
        luaL_error(L, "Usage: %s:AddCharacterLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    }

    uint32_t index = 0;
    if (lua_isnumber(L, 2))
        index = lua_tonumber(L, 2);

    // CM2Light light = CM2Light();
    // if (!maybe_CSimpleModel__CSimpleModel_SetLightHelper(L, 3, &light)) {
    //     v2 = model->GetDisplayName();
    //     light.Unlink();
    //     luaL_error(L, "Usage: %s:AddCharacterLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    // }
    // v6 = model->m_lightArray3[index].ukn1;
    // v7 = &model->m_lightArray3[index];
    // if (v6 < 4) {
    //     memcpy(&v7->m_lights[v6], &light, sizeof(v7->m_lights[v6]));
    //     ++v7->ukn1;
    //     model->m_lightArray3[index].uknByte1 = 1;
    // }
    // light.Unlink();
    return 0;
}

FrameScript_Method SimpleModelFFXMethods[NUM_SIMPLE_MODEL_FFX_SCRIPT_METHODS] = {
    { "ResetLights",        &CSimpleModelFFX_ResetLights },
    { "AddLight",           &CSimpleModelFFX_AddLight },
    { "AddCharacterLight",  &CSimpleModelFFX_AddCharacterLight },
    { "AddPetLight",        &CSimpleModelFFX_AddPetLight }
};
