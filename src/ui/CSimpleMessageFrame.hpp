#ifndef UI_C_SIMPLE_MESSAGE_FRAME_HPP
#define UI_C_SIMPLE_MESSAGE_FRAME_HPP

#include "ui/CSimpleFrame.hpp"
#include "common/DataAllocator.hpp"

class CSimpleMessageFrame : public CSimpleFrame {
    public:
    // Static variables
    static CDataAllocator s_allocator;
    static int32_t s_metatable;
    static int32_t s_objectType;

    // Static functions
    static void CreateScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);
    static void operator delete(void* ptr);

    // Member functions
    CSimpleMessageFrame(CSimpleFrame* parent);

    // Virtual member functions
    virtual bool IsA(int32_t type);
    virtual int32_t GetScriptMetaTable();
};

#endif
