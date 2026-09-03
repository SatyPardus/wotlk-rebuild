#include "gameui/CGMinimapFrame.hpp"
#include "gameui/CGMinimapFrameScript.hpp"
#include <bc/Memory.hpp>

CDataAllocator CGMinimapFrame::s_allocator(sizeof(CGMinimapFrame), 1);

void CGMinimapFrame::operator delete(void* ptr) {
    if (ptr) {
        ALLOCATOR_PUT(CGMinimapFrame::s_allocator, ptr);
    }
}

int32_t CGMinimapFrame::s_metatable;

CSimpleFrame* CGMinimapFrame::Create(CSimpleFrame* parent) {
    return ALLOCATOR_NEW(CGMinimapFrame::s_allocator, CGMinimapFrame, parent);
}

void CGMinimapFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    int32_t ref = FrameScript_Object::CreateScriptMetaTable(L, &CGMinimapFrame::RegisterScriptMethods);
    CGMinimapFrame::s_metatable = ref;
}

void CGMinimapFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, CGMinimapFrameMethods, NUM_CGMINIMAP_FRAME_SCRIPT_METHODS);
}

int32_t CGMinimapFrame::GetScriptMetaTable() {
    return CGMinimapFrame::s_metatable;
}

CGMinimapFrame::CGMinimapFrame(CSimpleFrame* parent)
    : CSimpleFrame(parent) {
}
