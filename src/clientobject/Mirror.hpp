#ifndef CLIENTOBJECT_MIRROR_HPP
#define CLIENTOBJECT_MIRROR_HPP

#include <cstdint>
#include <storm/List.hpp>
#include "clientobject/Types.hpp"
#include "clientobject/WGUID.hpp"

class CGObject_C;

typedef void (*MIRRORHANDLERFUNC)(WGUID guid, uint32_t blockByteOffset, uint32_t fieldByteSize, void* mirror, void* functionParam);

struct CMirrorHandler {
    /* 00 */ TSLink<CMirrorHandler> m_link;
    /* 00 */ TSLink<CMirrorHandler> m_link2;
    /* 10 */ MIRRORHANDLERFUNC function;
    /* 14 */ void* functionParam;
    /* 18 */ uint32_t count;
    /* 1C */ uint32_t fieldByteOffset;
    /* 20 */ uint32_t mirrorByteOffset;
    /* 24 */ uint32_t fieldByteSize;
    /* 28 */ uint32_t linkPositionSelector;
    /* 2C */ uint8_t m_dispatching;
    /* 2D */ uint8_t m_deletePending;
    /* 2E */ uint8_t m_alwaysFire;
    /* 2F */ uint8_t unk_002F;
};


void Mirror_ClearLists(CGObject_C* obj);
void Mirror_ExpirePending(STORM_EXPLICIT_LIST(CMirrorHandler, m_link2)* pending);
STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* GetObjectMirrorList(OBJECT_TYPE_ID typeId, CGObject_C* obj, uint32_t index);
int32_t Mirror_LinkPending(STORM_EXPLICIT_LIST(CMirrorHandler, m_link2)* pending, STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* list);
void Mirror_CopyFields(STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* list, CGObject_C* obj);
uint32_t ObjDescriptorToMirrorOffset(OBJECT_TYPE_ID typeId, int32_t localPlayer, uint32_t fieldByteOffset, uint32_t fieldByteSize);
uint32_t TypeDescriptorToMirrorOffset(OBJECT_TYPE_ID typeId, uint32_t dataOffset, uint32_t fieldByteSize);
uint32_t GetObjectTypeFieldByteOffset(OBJECT_TYPE_ID typeId);
void CallMirrorFunctions(STORM_EXPLICIT_LIST(CMirrorHandler, m_link2)* pending, WGUID guid, CGObject_C* obj, OBJECT_TYPE_ID typeId);
CMirrorHandler* AssignMirrorHandler(uint32_t fieldByteOffset, uint32_t mirrorByteOffset, uint32_t fieldByteSize, MIRRORHANDLERFUNC func, void* functionParam, uint32_t linkPositionSelector, int32_t alwaysFire, STORM_EXPLICIT_LIST(CMirrorHandler, m_link)* list);

#endif // CLIENTOBJECT_MIRROR_HPP
