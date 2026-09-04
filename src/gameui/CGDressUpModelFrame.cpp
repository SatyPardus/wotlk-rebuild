#include "gameui/CGDressUpModelFrame.hpp"
#include "gameui/CGDressUpModelFrameScript.hpp"
#include <bc/Memory.hpp>

CDataAllocator CGDressUpModelFrame::s_allocator(sizeof(CGDressUpModelFrame), 5);

void CGDressUpModelFrame::operator delete(void* ptr) {
    if (ptr) {
        ALLOCATOR_PUT(CGDressUpModelFrame::s_allocator, ptr);
    }
}

int32_t CGDressUpModelFrame::s_metatable;

CSimpleFrame* CGDressUpModelFrame::Create(CSimpleFrame* parent) {
    return ALLOCATOR_NEW(CGDressUpModelFrame::s_allocator, CGDressUpModelFrame, parent);
}

void CGDressUpModelFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    int32_t ref = FrameScript_Object::CreateScriptMetaTable(L, &CGDressUpModelFrame::RegisterScriptMethods);
    CGDressUpModelFrame::s_metatable = ref;
}

void CGDressUpModelFrame::RegisterScriptMethods(lua_State* L) {
    CGCharacterModelBase::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, CGDressUpModelFrameMethods, NUM_CGDRESS_UP_MODEL_FRAME_SCRIPT_METHODS);
}

int32_t CGDressUpModelFrame::GetScriptMetaTable() {
    return CGDressUpModelFrame::s_metatable;
}

CGDressUpModelFrame::CGDressUpModelFrame(CSimpleFrame* parent)
    : CGCharacterModelBase(parent) {
}
