#include "gameui/CGWorldFrame.hpp"

#include "gx/Transform.hpp"
#include "gx/Draw.hpp"
#include "gx/Shader.hpp"
#include "gx/Device.hpp"
#include "gx/RenderState.hpp"
#include "world/CWorld.hpp"
#include "world/CWorldScene.hpp"
#include "gameui/camera/CGCamera.hpp"
#include "event/EvtKeyDown.hpp"
#include "event/Event.hpp"
#include "event/Input.hpp"
#include "event/Types.hpp"
#include "console/Console.hpp"

#include "model/Model2.hpp"

#include <bc/Memory.hpp>
#include <tempest/Matrix.hpp>
#include <tempest/Vector.hpp>
#include <common/Time.hpp>
#include <cmath>


CGWorldFrame* CGWorldFrame::s_currentWorldFrame = nullptr;


CGWorldFrame::CGWorldFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    // TODO

    this->m_camera = NEW(CGCamera);

    s_currentWorldFrame = this;

    this->EnableEvent(SIMPLE_EVENT_KEY, -1);
    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
    this->EnableEvent(SIMPLE_EVENT_MOUSEWHEEL, -1);
}

void CGWorldFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    CSimpleFrame::OnFrameRender(batch, layer);
    if (!layer) {
        batch->QueueCallback(&CGWorldFrame::RenderWorld, this);
    }
}

int32_t CGWorldFrame::OnLayerKeyDown(const CKeyEvent& evt) {
    if (CSimpleFrame::OnLayerKeyDown(evt)) {
        return 1;
    }


    return 1;
}

int32_t CGWorldFrame::OnLayerKeyDownRepeat(const CKeyEvent& evt) {
    if (CSimpleFrame::OnLayerKeyDownRepeat(evt)) {
        return 1;
    }

    return 1;
}

CSimpleFrame* CGWorldFrame::Create(CSimpleFrame* parent) {
    // TODO:  Data = CDataAllocator__GetData(0, ".?AVCGWorldFrame@@", -2);

    auto m = SMemAlloc(sizeof(CGWorldFrame), __FILE__, __LINE__, 0);
    return m ? (new (m) CGWorldFrame(parent)) : nullptr;
}

void CGWorldFrame::RenderWorld(void* param) {
    C44Matrix saved_proj;
    GxXformProjection(saved_proj);

    C44Matrix saved_view;
    GxXformView(saved_view);

    CGWorldFrame::OnWorldUpdate();

    // TODO: PlayerNameUpdateWorldText();

    CGWorldFrame::OnWorldRender();

    // TODO: PlayerNameRenderWorldText();

    GxXformSetProjection(saved_proj);
    GxXformSetView(saved_view);

    CShaderEffect::UpdateProjMatrix();
}

// DEBUG: free-fly camera.
//   hold RMB   look
//   W/S        forward / back along camera forward
//   A/D        strafe along camera right
//   Q/E        down / up along camera up
//   LSHIFT     x5 speed        LCONTROL  x0.2 speed
//   R          reset position and orientation
//
// Polled once per frame rather than driven from key events, so movement is
// smooth and multiple keys combine. Delete this whole function and its call in
// OnWorldUpdate when the real camera lands.
static void UpdateDebugCamera(CGCamera* cam) {
    static float s_yaw = 0.0f;
    static float s_pitch = 0.0f;
    static C2iVector s_lastMouse(0, 0);
    static bool s_looking = false;
    static uint64_t s_lastMs = 0;

    // ---- frame time ----
    uint64_t now = OsGetAsyncTimeMs();
    float dt = static_cast<float>(now - s_lastMs) / 1000.0f;
    s_lastMs = now;

    // first frame, and anything after a breakpoint, must not teleport
    if (dt <= 0.0f || dt > 0.25f) {
        dt = 1.0f / 60.0f;
    }

    if (EventIsKeyDown(KEY_R)) {
        s_yaw = 0.0f;
        s_pitch = 0.0f;
        cam->m_position.Set(0.0f, 0.0f, 0.0f);
    }

    // ---- look: hold right mouse button ----
    bool rmb = (Input::s_buttonState & MOUSE_BUTTON_RIGHT) != 0;

    if (rmb) {
        if (!s_looking) {
            // first frame of the drag: capture the anchor, do not move
            s_looking = true;
            s_lastMouse = Input::s_currentMouse;
        } else {
            const float SENSITIVITY = 0.0035f; // radians per pixel

            int32_t dx = Input::s_currentMouse.x - s_lastMouse.x;
            int32_t dy = Input::s_currentMouse.y - s_lastMouse.y;
            s_lastMouse = Input::s_currentMouse;

            s_yaw -= static_cast<float>(dx) * SENSITIVITY;
            s_pitch += static_cast<float>(dy) * SENSITIVITY;

            // clamp just short of straight up/down so the basis never degenerates
            const float PITCH_LIMIT = 1.5533f; // ~89 degrees
            if (s_pitch > PITCH_LIMIT) {
                s_pitch = PITCH_LIMIT;
            }
            if (s_pitch < -PITCH_LIMIT) {
                s_pitch = -PITCH_LIMIT;
            }
        }
    } else {
        s_looking = false;
    }

    cam->SetFacing(s_yaw, s_pitch, 0.0f);

    // ---- move ----
    float speed = 40.0f; // world units per second
    if (EventIsKeyDown(KEY_LSHIFT)) {
        speed *= 5.0f;
    }
    if (EventIsKeyDown(KEY_LCONTROL)) {
        speed *= 0.2f;
    }

    // read the basis AFTER SetFacing so it matches this frame's orientation
    C3Vector fwd = cam->Forward();
    C3Vector right = cam->Right();
    C3Vector up = cam->Up();

    C3Vector move(0.0f, 0.0f, 0.0f);
    if (EventIsKeyDown(KEY_W)) { move = move + fwd; }
    if (EventIsKeyDown(KEY_S)) { move = move - fwd; }
    if (EventIsKeyDown(KEY_D)) { move = move - right; }
    if (EventIsKeyDown(KEY_A)) { move = move + right; }
    if (EventIsKeyDown(KEY_E)) { move = move + up; }
    if (EventIsKeyDown(KEY_Q)) { move = move - up; }

    // normalise so diagonal input is not faster
    float len2 = move.x * move.x + move.y * move.y + move.z * move.z;
    if (len2 > 0.0001f) {
        float scale = speed * dt / sqrtf(len2);
        cam->m_position.x += move.x * scale;
        cam->m_position.y += move.y * scale;
        cam->m_position.z += move.z * scale;
    }
}

// OFFSET: 0x4FA5F0
void CGWorldFrame::OnWorldUpdate() {
    CGCamera* cam = CGWorldFrame::GetActiveCamera();

    UpdateDebugCamera(cam);

    C3Vector camPos = cam->m_position;
    C3Vector camForward = cam->Forward();
    C3Vector camTarget = camPos + camForward;
    CRect rect;
    CGWorldFrame::s_currentWorldFrame->GetRect(&rect);
    CGWorldFrame::GetActiveCamera()->SetupWorldProjection(rect);

    C3Vector position = camPos;

    CWorld::Update(&camPos, &camTarget, &position);
}

void CGWorldFrame::OnWorldRender() {
    CRect windowSize;
    GxCapsWindowSize(windowSize);
    if (windowSize.maxY - windowSize.minY == 0.0f || windowSize.maxX - windowSize.minX == 0.0f) {
        return;
    }

    // TODO

    GxRsPush();
    GxRsSet(GxRs_Multisample, 1);

    if (true) {
        CImVector clearColor = { 0x80, 0x80, 0x80, 0xFF };
        GxSceneClear(3, clearColor);
    }

    if (CWorld::GetEnables() & 0x20000000) {
        GxMasterEnableSet(GxMasterEnable_PolygonFill, 0);
    }

    C3Vector saveMin;
    C3Vector saveMax;

    GxXformViewport(saveMin.x, saveMax.x, saveMin.y, saveMax.y, saveMin.z, saveMax.z);

    // TODO

    // WORKAROUND:
    float maxZ = saveMax.z - (saveMax.z - saveMin.z) * 0.050000001;
    GxXformSetViewport(saveMin.x, saveMax.x, saveMin.y, saveMax.y, saveMin.z, maxZ);

    CShaderEffect::UpdateProjMatrix();

    static auto s_time = 0;

    float elapsed = static_cast<float>(OsGetAsyncTimeMs() - s_time) / 1000.0f;
    s_time = OsGetAsyncTimeMs();
    CWorld::Render(CGWorldFrame::GetActiveCamera()->m_position, elapsed);

    if (CWorldScene::s_m2Scene) {
        CWorldScene::s_m2Scene->Draw(M2PASS_0);
    }

    GxRsPop();
}

CGCamera* CGWorldFrame::GetActiveCamera() {
    STORM_ASSERT(CGWorldFrame::s_currentWorldFrame);
    STORM_ASSERT(CGWorldFrame::s_currentWorldFrame->m_camera);
    return CGWorldFrame::s_currentWorldFrame->m_camera;
}
