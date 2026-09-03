#include "gameui/CGQuestPOIFrame.hpp"
#include "gameui/CGQuestPOIFrameScript.hpp"
#include <bc/Memory.hpp>

CDataAllocator CGQuestPOIFrame::s_allocator(sizeof(CGQuestPOIFrame), 5);

void CGQuestPOIFrame::operator delete(void* ptr) {
    if (ptr) {
        ALLOCATOR_PUT(CGQuestPOIFrame::s_allocator, ptr);
    }
}

int32_t CGQuestPOIFrame::s_metatable;

CSimpleFrame* CGQuestPOIFrame::Create(CSimpleFrame* parent) {
    return ALLOCATOR_NEW(CGQuestPOIFrame::s_allocator, CGQuestPOIFrame, parent);
}

void CGQuestPOIFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    int32_t ref = FrameScript_Object::CreateScriptMetaTable(L, &CGQuestPOIFrame::RegisterScriptMethods);
    CGQuestPOIFrame::s_metatable = ref;
}

void CGQuestPOIFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, CGQuestPOIFrameMethods, NUM_CGQUEST_POI_FRAME_SCRIPT_METHODS);
}

int32_t CGQuestPOIFrame::GetScriptMetaTable() {
    return CGQuestPOIFrame::s_metatable;
}

CGQuestPOIFrame::CGQuestPOIFrame(CSimpleFrame* parent)
    : CSimpleFrame(parent) {
}
