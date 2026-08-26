#ifndef GAME_UI_CGWORLDFRAME_HPP
#define GAME_UI_CGWORLDFRAME_HPP

#include "ui/CSimpleFrame.hpp"
#include "ui/CSimpleTop.hpp"
#include "clientobject/CGObject_C.hpp"

class CGCamera;

class CGWorldFrame : public CSimpleFrame {
    public:
    CGWorldFrame(CSimpleFrame* parent);

    void UpdateObject(CGObject_C* obj, int a3);
    void UpdateDayNightInfo(float delta);

    virtual void OnFrameRender(CRenderBatch* batch, uint32_t layer);
    virtual int32_t OnLayerKeyDown(const CKeyEvent& evt);
    virtual int32_t OnLayerKeyDownRepeat(const CKeyEvent& evt);

    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void RenderWorld(void* param);
    static void OnWorldUpdate();
    static void OnWorldRender();
    static CGCamera* GetActiveCamera();
    static bool ObjectEnumProc(void* param, uint32_t status, uint64_t param64, uint32_t param32);

    CGCamera* m_camera = nullptr;

    public:
    static CGWorldFrame* s_currentWorldFrame;
};

#endif // GAME_UI_CGWORLDFRAME_HPP
