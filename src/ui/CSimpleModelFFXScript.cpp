#include "ui/CSimpleModelFFXScript.hpp"
#include "ui/CSimpleModelFFX.hpp"
#include "util/Unimplemented.hpp"
#include "util/Lua.hpp"
#include <cstdint>

// OFFSET: 0x4E6BE0
int32_t CSimpleModelFFX_ResetLights(lua_State* L) {
    auto type = CSimpleModelFFX::GetObjectType();
    auto model = static_cast<CSimpleModelFFX*>(FrameScript_GetObjectThis(L, type));

    model->m_lightArray3[0].m_lightCount = 0;
    model->m_lightArray3[0].m_hasCustomLight = 0;
    model->m_lightArray2[0].m_lightCount = 0;
    model->m_lightArray2[0].m_hasCustomLight = 0;
    model->m_lightArray1[0].m_lightCount = 0;
    model->m_lightArray1[0].m_hasCustomLight = 0;
    model->m_lightArray3[1].m_hasCustomLight = 0;
    model->m_lightArray3[1].m_lightCount = 0;
    model->m_lightArray2[1].m_hasCustomLight = 0;
    model->m_lightArray2[1].m_lightCount = 0;
    model->m_lightArray1[1].m_hasCustomLight = 0;
    model->m_lightArray1[1].m_lightCount = 0;
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
        index = lua_tonumber(L, 2) != 0 ? 1 : 0;

    CM2Light light = CM2Light();
    if (!CSimpleModel::SetLightHelper(L, 3, &light)) {
        v2 = model->GetDisplayName();
        light.Unlink();
        luaL_error(L, "Usage: %s:AddLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    }

    uint32_t count = model->m_lightArray1[index].m_lightCount;
    if (count < 4) {
        memcpy(&model->m_lightArray1[index].m_lights[count], &light, sizeof(model->m_lightArray1[index].m_lights[count]));
        model->m_lightArray1[index].m_lightCount++;
        model->m_lightArray1[index].m_hasCustomLight = true;
    }
    light.Unlink();
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
        index = lua_tonumber(L, 2) != 0 ? 1 : 0;

    CM2Light light = CM2Light();
    if (!CSimpleModel::SetLightHelper(L, 3, &light)) {
        v2 = model->GetDisplayName();
        light.Unlink();
        luaL_error(L, "Usage: %s:AddCharacterLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    }

    uint32_t count = model->m_lightArray2[index].m_lightCount;
    if (count < 4) {
        memcpy(&model->m_lightArray2[index].m_lights[count], &light, sizeof(model->m_lightArray2[index].m_lights[count]));
        model->m_lightArray2[index].m_lightCount++;
        model->m_lightArray2[index].m_hasCustomLight = true;
    }
    light.Unlink();
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
        index = lua_tonumber(L, 2) != 0 ? 1 : 0;

    CM2Light light = CM2Light();
    if (!CSimpleModel::SetLightHelper(L, 3, &light)) {
        v2 = model->GetDisplayName();
        light.Unlink();
        luaL_error(L, "Usage: %s:AddCharacterLight(index, enabled[, omni, dirX, dirY, dirZ, ambIntensity[, ambR, ambG, ambB], dirIntensity[, dirR, dirG, dirB]])", v2);
    }

    uint32_t count = model->m_lightArray3[index].m_lightCount;
    if (count < 4) {
        memcpy(&model->m_lightArray3[index].m_lights[count], &light, sizeof(model->m_lightArray3[index].m_lights[count]));
        model->m_lightArray3[index].m_lightCount++;
        model->m_lightArray3[index].m_hasCustomLight = true;
    }
    light.Unlink();
    return 0;
}

FrameScript_Method SimpleModelFFXMethods[NUM_SIMPLE_MODEL_FFX_SCRIPT_METHODS] = {
    { "ResetLights",        &CSimpleModelFFX_ResetLights },
    { "AddLight",           &CSimpleModelFFX_AddLight },
    { "AddCharacterLight",  &CSimpleModelFFX_AddCharacterLight },
    { "AddPetLight",        &CSimpleModelFFX_AddPetLight }
};
