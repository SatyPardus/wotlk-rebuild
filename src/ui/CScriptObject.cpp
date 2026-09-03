#include "ui/CScriptObject.hpp"
#include "ui/CScriptObjectScript.hpp"
#include "util/CStatus.hpp"
#include "util/Lua.hpp"
#include <cstdint>
#include <common/XML.hpp>
#include <storm/String.hpp>

int32_t CScriptObject::s_objectType;
const char* CScriptObject::s_objectTypeName = "Object";

void CScriptObject::RegisterScriptMethods(lua_State* L) {
    FrameScript_Object::FillScriptMethodTable(L, ScriptObjectMethods, NUM_SCRIPT_OBJECT_SCRIPT_METHODS);
}

int32_t CScriptObject::GetObjectType() {
    if (!CScriptObject::s_objectType) {
        CScriptObject::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CScriptObject::s_objectType;
}

// OFFSET: 0x48B5F0
CScriptObject* CScriptObject::GetScriptObjectByName(const char* name, int32_t type) {
    lua_State* L = FrameScript_GetContext();

    lua_pushstring(L, name);
    lua_rawget(L, LUA_GLOBALSINDEX);

    if (lua_type(L, -1) == 5) {
        lua_rawgeti(L, -1, 0);
        auto v4 = static_cast<CScriptObject*>(lua_touserdata(L, -1));
        lua_settop(L, -3);

        if (v4 && v4->IsA(type)) {
            return v4;
        } else {
            return nullptr;
        }
    } else {
        lua_settop(L, -2);
        return nullptr;
    }
}

// OFFSET: 0x48B730
CScriptObject::~CScriptObject() {
    if (this->m_name.GetString()) {
        this->UnregisterScriptObject(this->m_name.GetString());
        this->m_name.Copy(nullptr);
    }
}

/**
 * @brief Converts the value of the name="" attribute into a fully resolved name.
 * @param source eg. $parentCategory from <FontString name="$parentCategory" />
 */
// OFFSET: 0x48B540
void CScriptObject::CreateName(const char* source, char* dest, uint32_t destsize) {
    // If source is not $parent, use it as the name
    uint32_t v5 = SStrLen("$parent");
    if (SStrCmpI(source, "$parent", v5)) {
        SStrCopy(dest, source, destsize);
        return;
    }

    SStrCopy(dest, "Top", 0x7FFFFFFF);

    CScriptObject* parent = this->GetScriptObjectParent();

    if (parent) {
        const char* parentName;

        while (1) {
            parentName = parent->GetName();

            // Found a name
            if (parentName && *parentName) {
                break;
            }

            // Didn't find a name, go up a level
            parent = parent->GetScriptObjectParent();

            // Out of levels to go up
            if (!parent) {
                goto LABEL_8;
            }
        }

        SStrCopy(dest, parentName, destsize);
    }

    LABEL_8:
    // Copy the part of the string after $parent into dest
    int v8 = SStrLen("$parent");
    SStrPack(dest, &source[v8], destsize);
}

// OFFSET: 0x431E80
char* CScriptObject::GetName() {
    return this->m_name.m_str;
}

void CScriptObject::PreLoadXML(const XMLNode* node, CStatus* status) {
    const char* name = node->GetAttributeByName("name");

    if (name && *name) {
        this->SetName(name);
    }
}

// OFFSET: 0x48B6C0
void CScriptObject::SetName(const char* name) {
    // If name is already set, clear it out
    if (this->m_name.m_str) {
        this->UnregisterScriptObject(this->m_name.m_str);
        this->m_name.Copy(0);
    }

    // Set the name
    if (name && *name) {
        // Deal with $parent in name
        char fullName[1024];
        this->CreateName(name, fullName, 1024);

        this->m_name.Copy(fullName);

        this->RegisterScriptObject(fullName);
    }
}

// OFFSET: 0x48B6B0
const char* CScriptObject::GetObjectTypeName() {
    return CScriptObject::s_objectTypeName;
}

// OFFSET: 0x482AF0
bool CScriptObject::IsA(int32_t type) {
    return type == CScriptObject::s_objectType;
}

// OFFSET: 0x482B10
bool CScriptObject::IsA(const char* typeName) {
    return SStrCmpI(typeName, CScriptObject::s_objectTypeName, 0x7FFFFFFFu) == 0;
}
