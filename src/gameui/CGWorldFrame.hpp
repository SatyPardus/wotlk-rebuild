#ifndef GAME_UI_CGWORLDFRAME_HPP
#define GAME_UI_CGWORLDFRAME_HPP

#include "ui/CSimpleFrame.hpp"
#include "ui/CSimpleTop.hpp"
#include "clientobject/CGObject_C.hpp"
#include "common/DataAllocator.hpp"
#include <storm/List.hpp>
#include <limits>

class CGCamera;
class CM2Model;

struct CModelRecord : public TSLinkedNode<CModelRecord> {
    /* 0x08 */ CM2Model* m_model = nullptr;
    /* 0x0C */ float m_dist = std::numeric_limits<float>::infinity();
    /* 0x10 */ WGUID m_guid;
    /* 0x18 */ CGObject_C* m_object = nullptr;
    /* 0x1C */ uint32_t m_unk1C;
};

struct KEYDOWNSTATE {
    /* 0x00 */ char m_keyString[0x20];
    /* 0x20 */ uint32_t m_modifiers;
};

struct HITTESTRESULT {
    WGUID guid;        // +0x00
    C3Vector point;    // +0x08
    float distance;    // +0x14
    C3Vector segStart; // +0x18
    C3Vector segEnd;   // +0x24
};

class CGWorldFrame : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;
    static void operator delete(void* ptr);

    CGWorldFrame(CSimpleFrame* parent);

    void UpdateObject(CGObject_C* obj, int a3);
    void UpdateDayNightInfo(float delta);

    void OnWorldUpdate();
    void OnWorldRender();
    void OnFrameRender(CRenderBatch* batch, uint32_t layer) override;
    int32_t OnLayerKeyDown(const CKeyEvent& evt) override;
    int32_t OnLayerKeyUp(const CKeyEvent& evt) override;
    int32_t OnLayerMouseDown(const CMouseEvent& evt, const char* btn) override;
    int32_t OnLayerMouseUp(const CMouseEvent& evt, const char* btn) override;
    int32_t OnLayerMouseWheel(const CMouseEvent& evt) override;
    void SetupDefaultAction();
    void OnMouseModeRelative();
    void OnMouseModeNormal();
    void PerformDefaultAction(MOUSEBUTTON button);
    bool GetLineSegment(float mouseX, float mouseY, C3Vector* start, C3Vector* end);
    uint32_t GetHitTestFilterFlags(uint32_t unused);
    void MoveToFreeList(STORM_LIST(CModelRecord)* list);
    int32_t HitTest(C3Vector* start, C3Vector* end, uint32_t flags, HITTESTRESULT* result);
    int32_t HitTestPoint(float mouseX, float mouseY, int32_t a4, HITTESTRESULT* result);
    void AddObjectToHitTestList(CModelRecord* record, uint32_t flags);
    WGUID FindClosestModel(C3Vector* start, C3Vector* end, uint32_t flags, float* dist);
    bool GetScreenCoordinates(C3Vector* worldPos, C3Vector* screenPos, int32_t* clipFlags);
    bool IsLegalSelection(CGObject_C* obj, uint32_t a2);

    /* 18 */ void OnFrameSizeChanged(const CRect& rect) override;
    /* 30 */ void OnLayerUpdate(float elapsedSec) override;

    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void RenderWorld(void* param);
    static CGCamera* GetActiveCamera();
    static bool ObjectEnumProc(void* param, uint32_t status, uint64_t param64, uint32_t param32);

    /* 029C */ STORM_LIST(CModelRecord) m_modelList;
    /* 02A8 */ STORM_LIST(CModelRecord) m_hitModelList;
    /* 02B4 */ STORM_LIST(CModelRecord) m_freeModelList;
    /* 02C0 */ uint32_t unk_02C0;
    /* 02C4 */ uint32_t unk_02C4;
    /* 02C8 */ WGUID m_trackedGuid;
    /* 02D0 */ WGUID m_hitGuid;
    /* 02D8 */ int32_t m_defaultActionHitKind;
    /* 02DC */ uint32_t unk_02DC;
    /* 02E0 */ HITTESTRESULT m_defaultActionHit;
    /* 0310 */ C2Vector m_defaultActionPointDDC;
    /* 0318 */ uint32_t unk_0318;
    /* 031C */ uint32_t m_worldFlags;
    /* 0320 */ CRect m_viewportDDC;
    /* 0330 */ CRect m_viewportNDC;
    /* 0340 */ C44Matrix m_viewProjection;
    /* 0380 */ WGUID m_trackedEffectGuidA;
    /* 0388 */ WGUID m_trackedEffectGuidB;
    /* 0390 */ uint8_t unk_0390[0x780];
    /* 0B10 */ uint32_t m_namePlateFlags;
    /* 0B14 */ float m_elapsedSec;
    /* 0B18 */ KEYDOWNSTATE m_keyDown[787];
    /* 79C4 */ KEYDOWNSTATE m_mouseDown[31];
    /* 7E20 */ CGCamera* m_camera = nullptr;
    /* 7E24 */ uint32_t unk_7E24;

    public:
    static CGWorldFrame* s_currentWorldFrame;
};

#endif // GAME_UI_CGWORLDFRAME_HPP
