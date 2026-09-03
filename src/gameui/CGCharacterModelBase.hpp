#ifndef GAME_UI_CGCHARACTER_MODEL_BASE_HPP
#define GAME_UI_CGCHARACTER_MODEL_BASE_HPP

#include "ui/CSimpleModel.hpp"
#include "ui/CSimpleTop.hpp"
#include "common/DataAllocator.hpp"

class CGCharacterModelBase : public CSimpleModel {
    public:
    // Static variables
    static CDataAllocator s_allocator;
    static int32_t s_metatable;

    // Static functions
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);
    static void operator delete(void* ptr);

    // Virtual member functions
    virtual int32_t GetScriptMetaTable();

    // Member functions
    CGCharacterModelBase(CSimpleFrame* parent);
};

#endif // GAME_UI_CGCHARACTER_MODEL_BASE_HPP
