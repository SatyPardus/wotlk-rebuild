#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5AD070
static int32_t Script_GetGMTicket(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AD080
static int32_t Script_NewGMTicket(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AD0F0
static int32_t Script_UpdateGMTicket(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AD140
static int32_t Script_DeleteGMTicket(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AD150
static int32_t Script_GMResponseNeedMoreHelp(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AD1B0
static int32_t Script_GMResponseResolve(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AD1C0
static int32_t Script_GetGMStatus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AC750
static int32_t Script_GMSurveyQuestion(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AC870
static int32_t Script_GMSurveyNumAnswers(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AC7D0
static int32_t Script_GMSurveyAnswer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AC390
static int32_t Script_GMSurveyAnswerSubmit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AC480
static int32_t Script_GMSurveyCommentSubmit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AD230
static int32_t Script_GMSurveySubmit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AD020
static int32_t Script_GMReportLag(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5AC320
static int32_t Script_RegisterStaticConstants(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void GMTicketInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_GMTICKET_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_GMTicketInfo[i].name,
            GameScript::s_ScriptFunctions_GMTicketInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_GMTicketInfo[NUM_SCRIPT_FUNCTIONS_GMTICKET_INFO] = {
    { "GetGMTicket", &Script_GetGMTicket },
    { "NewGMTicket", &Script_NewGMTicket },
    { "UpdateGMTicket", &Script_UpdateGMTicket },
    { "DeleteGMTicket", &Script_DeleteGMTicket },
    { "GMResponseNeedMoreHelp", &Script_GMResponseNeedMoreHelp },
    { "GMResponseResolve", &Script_GMResponseResolve },
    { "GetGMStatus", &Script_GetGMStatus },
    { "GMSurveyQuestion", &Script_GMSurveyQuestion },
    { "GMSurveyNumAnswers", &Script_GMSurveyNumAnswers },
    { "GMSurveyAnswer", &Script_GMSurveyAnswer },
    { "GMSurveyAnswerSubmit", &Script_GMSurveyAnswerSubmit },
    { "GMSurveyCommentSubmit", &Script_GMSurveyCommentSubmit },
    { "GMSurveySubmit", &Script_GMSurveySubmit },
    { "GMReportLag", &Script_GMReportLag },
    { "RegisterStaticConstants", &Script_RegisterStaticConstants },
};
