#include "sound/SI2.hpp"
#include "ui/Types.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"
#include <ui/FrameScript.hpp>

// OFFSET: 0x9858B0
int32_t Script_PlaySound(lua_State* L) {
    // TODO
    return 0;
}

// OFFSET: 0x985950
int32_t Script_PlayMusic(lua_State* L) {
    // TODO
    return 0;
}

// OFFSET: 0x9859B0
int32_t Script_PlaySoundFile(lua_State* L) {
    // TODO
    return 0;
}

// OFFSET: 0x985A10
int32_t Script_StopMusic(lua_State* L) {
    // TODO
    return 0;
}

// OFFSET: 0x985BB0
int32_t Script_Sound_GameSystem_GetNumInputDrivers(lua_State* L) {
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x985BE0
int32_t Script_Sound_GameSystem_GetInputDriverNameByIndex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985C70
int32_t Script_Sound_GameSystem_GetNumOutputDrivers(lua_State* L) {
    // TODO:
    // NumOutputDrivers = (double)(int)SE3::GetNumOutputDrivers(SE3::sm_pGameSystem, v3);
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x985CA0
int32_t Script_Sound_GameSystem_GetOutputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1))
        luaL_error(L, "Usage: Sound_GetOutputDriverNameByIndex(OutputDriverIndex)");
    auto v1 = lua_tointeger(L, 1);
    char v3[2048];
    memset(v3, 0, sizeof(v3));
    //SE2::GetOutputDriverName_Cached(v1, v3, 2048, 0);
    // TODO
    //##############
    SStrCopy(v3, FrameScript_GetText("SYSTEM_DEFAULT", -1, GENDER_NOT_APPLICABLE), 2048);
    //######
    lua_pushstring(L, v3);
    return 1;
}

// OFFSET: 0x985D30
int32_t Script_Sound_GameSystem_RestartSoundSystem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985A20
int32_t Script_Sound_ChatSystem_GetNumInputDrivers(lua_State* L) {
    // TODO
    // ##############
    int32_t v3 = 0;
    // ######
    //v3 = bn_SE2_GetNumInputDrivers_Cached(1, v1);
    lua_pushnumber(L, v3);
    return 1;
}

// OFFSET: 0x985A50
int32_t Script_Sound_ChatSystem_GetInputDriverNameByIndex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985AE0
int32_t Script_Sound_ChatSystem_GetNumOutputDrivers(lua_State* L) {
    // TODO
    // ##############
    int32_t v3 = 0;
    // ######
    // v3 = bn_SE2_GetNumOutputDrivers_Cached(1, v1);
    lua_pushnumber(L, v3);
    return 1;
}

// OFFSET: 0x985B10
int32_t Script_Sound_ChatSystem_GetOutputDriverNameByIndex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985EF0
int32_t Script_VoiceChat_StartCapture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985D50
int32_t Script_VoiceChat_StopCapture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985D60
int32_t Script_VoiceChat_RecordLoopbackSound(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985DD0
int32_t Script_VoiceChat_StopRecordingLoopbackSound(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985DE0
int32_t Script_VoiceChat_PlayLoopbackSound(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985DF0
int32_t Script_VoiceChat_StopPlayingLoopbackSound(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985E00
int32_t Script_VoiceChat_IsRecordingLoopbackSound(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985E30
int32_t Script_VoiceChat_IsPlayingLoopbackSound(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985E60
int32_t Script_VoiceChat_GetCurrentMicrophoneSignalLevel(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x985E90
int32_t Script_VoiceChat_ActivatePrimaryCaptureCallback(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

FrameScript_Method SI2::s_ScriptFunctions[] = {
    { "PlaySound",                                      &Script_PlaySound },
    { "PlayMusic",                                      &Script_PlayMusic },
    { "PlaySoundFile",                                  &Script_PlaySoundFile },
    { "StopMusic",                                      &Script_StopMusic },
    { "Sound_GameSystem_GetNumInputDrivers",            &Script_Sound_GameSystem_GetNumInputDrivers },
    { "Sound_GameSystem_GetInputDriverNameByIndex",     &Script_Sound_GameSystem_GetInputDriverNameByIndex },
    { "Sound_GameSystem_GetNumOutputDrivers",           &Script_Sound_GameSystem_GetNumOutputDrivers },
    { "Sound_GameSystem_GetOutputDriverNameByIndex",    &Script_Sound_GameSystem_GetOutputDriverNameByIndex },
    { "Sound_GameSystem_RestartSoundSystem",            &Script_Sound_GameSystem_RestartSoundSystem },
    { "Sound_ChatSystem_GetNumInputDrivers",            &Script_Sound_ChatSystem_GetNumInputDrivers },
    { "Sound_ChatSystem_GetInputDriverNameByIndex",     &Script_Sound_ChatSystem_GetInputDriverNameByIndex },
    { "Sound_ChatSystem_GetNumOutputDrivers",           &Script_Sound_ChatSystem_GetNumOutputDrivers },
    { "Sound_ChatSystem_GetOutputDriverNameByIndex",    &Script_Sound_ChatSystem_GetOutputDriverNameByIndex },
    { "VoiceChat_StartCapture",                         &Script_VoiceChat_StartCapture },
    { "VoiceChat_StopCapture",                          &Script_VoiceChat_StopCapture },
    { "VoiceChat_RecordLoopbackSound",                  &Script_VoiceChat_RecordLoopbackSound },
    { "VoiceChat_StopRecordingLoopbackSound",           &Script_VoiceChat_StopRecordingLoopbackSound },
    { "VoiceChat_PlayLoopbackSound",                    &Script_VoiceChat_PlayLoopbackSound },
    { "VoiceChat_StopPlayingLoopbackSound",             &Script_VoiceChat_StopPlayingLoopbackSound },
    { "VoiceChat_IsRecordingLoopbackSound",             &Script_VoiceChat_IsRecordingLoopbackSound },
    { "VoiceChat_IsPlayingLoopbackSound",               &Script_VoiceChat_IsPlayingLoopbackSound },
    { "VoiceChat_GetCurrentMicrophoneSignalLevel",      &Script_VoiceChat_GetCurrentMicrophoneSignalLevel },
    { "VoiceChat_ActivatePrimaryCaptureCallback",       &Script_VoiceChat_ActivatePrimaryCaptureCallback }
};

size_t SI2::s_NumScriptFunctions = sizeof(SI2::s_ScriptFunctions) / sizeof(FrameScript_Method);
