#ifndef UI_C_SCRIPT_OBJECT_HPP
#define UI_C_SCRIPT_OBJECT_HPP

#include "ui/FrameScript_Object.hpp"
#include <cstdint>
#include <common/String.hpp>

class CStatus;
class XMLNode;

class CScriptObject : public FrameScript_Object {
    public:
        // Static variables
        static int32_t s_objectType;
        static const char* s_objectTypeName;

        // Static functions
        static int32_t GetObjectType();
        static void RegisterScriptMethods(lua_State* L);
        static CScriptObject* GetScriptObjectByName(const char* name, int32_t type);

        // Member variables
        /* 0014 */ RCString m_name;

        // Virtual member functions
        /* 00 */ virtual ~CScriptObject();
        /* 01 */ virtual char* GetName();
        /* 02 */ virtual bool IsA(int32_t type);
        /* 03 */ virtual CScriptObject* GetScriptObjectParent() = 0;
        /* 04 */ virtual bool IsA(const char* typeName);
        /* 05 */ virtual const char* GetObjectTypeName();

        // Member functions
        void CreateName(const char* source, char* dest, uint32_t destsize);
        void PreLoadXML(const XMLNode* node, CStatus* status);
        void SetName(const char* name);
};

#endif
