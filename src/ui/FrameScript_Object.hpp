#ifndef UI_FRAME_SCRIPT_OBJECT_HPP
#define UI_FRAME_SCRIPT_OBJECT_HPP

#include "ui/FrameScript.hpp"
#include <cstdint>

class FrameScript_Object {
    public:
        // Structs
        struct ScriptData {
            const char* wrapper;
        };

        struct ScriptIx {
            int32_t luaRef = 0;
            const char* unk = nullptr;
        };

        class ScriptFunction {
            public:
                int32_t luaRef = 0;
        };

        // Static members
        static int32_t s_objectTypes;

        // Static functions
        static int32_t CreateScriptMetaTable(lua_State* L, void(*a2)(lua_State*));
        static void FillScriptMethodTable(lua_State* L, FrameScript_Method methods[], int32_t count);

        // Member variables
        /* 0000 */ // vftable
        /* 0004 */ int32_t lua_registered = 0;
        /* 0008 */ int32_t lua_objectRef = -2;
        /* 000C */ ScriptIx m_onEvent;

        // Virtual member functions
        /* 00 */ virtual ~FrameScript_Object();
        /* 01 */ virtual char* GetName() = 0;
        /* 02 */ virtual int32_t GetScriptMetaTable() = 0;
        /* 03 */ virtual ScriptIx* GetScriptByName(const char* name, ScriptData& data);
        /* 04 */ virtual bool IsA(int32_t type) = 0;

        // Member functions
        const char* GetDisplayName();
        int32_t RegisterScriptEvent(const char* name);
        void UnregisterScriptEvent(const char* name);
        void RegisterScriptObject(const char* name);
        void RunScript(ScriptIx const& script, int32_t argCount, const char* a4);
        void UnregisterScriptObject(const char* name);
};

FrameScript_Object* FrameScript_GetObjectThis(lua_State* L, int32_t type);

#endif
