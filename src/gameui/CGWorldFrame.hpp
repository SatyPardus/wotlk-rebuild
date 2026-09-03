#ifndef GAME_UI_CGWORLDFRAME_HPP
#define GAME_UI_CGWORLDFRAME_HPP

#include "ui/CSimpleFrame.hpp"
#include "ui/CSimpleTop.hpp"
#include "clientobject/CGObject_C.hpp"
#include "common/DataAllocator.hpp"

class CGCamera;

struct KEYDOWNSTATE {
    /* 0x00 */ char m_keyString[0x20];
    /* 0x20 */ uint32_t m_modifiers;
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

    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void RenderWorld(void* param);
    static CGCamera* GetActiveCamera();
    static bool ObjectEnumProc(void* param, uint32_t status, uint64_t param64, uint32_t param32);

    /* 0B18 */ KEYDOWNSTATE m_keyDown[787];
    /* 79C4 */
    /* 7E20 */ CGCamera* m_camera = nullptr;

    public:
    static CGWorldFrame* s_currentWorldFrame;
};

#endif // GAME_UI_CGWORLDFRAME_HPP
