#include "gameui/CGTabardModelFrame.hpp"
#include "gameui/CGTabardModelFrameScript.hpp"
#include <bc/Memory.hpp>

CDataAllocator CGTabardModelFrame::s_allocator(sizeof(CGTabardModelFrame), 1);

void CGTabardModelFrame::operator delete(void* ptr) {
    if (ptr) {
        ALLOCATOR_PUT(CGTabardModelFrame::s_allocator, ptr);
    }
}

int32_t CGTabardModelFrame::s_metatable;

CSimpleFrame* CGTabardModelFrame::Create(CSimpleFrame* parent) {
    return ALLOCATOR_NEW(CGTabardModelFrame::s_allocator, CGTabardModelFrame, parent);
}

void CGTabardModelFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    int32_t ref = FrameScript_Object::CreateScriptMetaTable(L, &CGTabardModelFrame::RegisterScriptMethods);
    CGTabardModelFrame::s_metatable = ref;
}

void CGTabardModelFrame::RegisterScriptMethods(lua_State* L) {
    CGCharacterModelBase::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, CGTabardModelFrameMethods, NUM_CGTABARD_MODEL_FRAME_SCRIPT_METHODS);
}

int32_t CGTabardModelFrame::GetScriptMetaTable() {
    return CGTabardModelFrame::s_metatable;
}

CGTabardModelFrame::CGTabardModelFrame(CSimpleFrame* parent)
    : CGCharacterModelBase(parent) {
}
