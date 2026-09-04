#ifndef GAME_UI_CGTABARD_MODEL_FRAME_HPP
#define GAME_UI_CGTABARD_MODEL_FRAME_HPP

#include "gameui/CGCharacterModelBase.hpp"
#include "ui/CSimpleTop.hpp"
#include "common/DataAllocator.hpp"

class CGTabardModelFrame : public CGCharacterModelBase {
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
    CGTabardModelFrame(CSimpleFrame* parent);
};

#endif // GAME_UI_CGTABARD_MODEL_FRAME_HPP
