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
#include <clientobject/ObjectMgrClient.hpp>
#include <world/daynight/DayNight.hpp>
#include <world/daynight/DNInfo.hpp>
#include "gameui/CGUIBindings.hpp"

CDataAllocator CGWorldFrame::s_allocator(sizeof(CGWorldFrame), 1);

void CGWorldFrame::operator delete(void* ptr) {
    if (ptr) {
        ALLOCATOR_PUT(CGWorldFrame::s_allocator, ptr);
    }
}

CGWorldFrame* CGWorldFrame::s_currentWorldFrame = nullptr;


CGWorldFrame::CGWorldFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    // TODO

    this->m_camera = NEW(CGCamera);

    s_currentWorldFrame = this;

    this->EnableEvent(SIMPLE_EVENT_KEY, -1);
    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
    this->EnableEvent(SIMPLE_EVENT_MOUSEWHEEL, -1);
}

// OFFSET: 0x4F8D10
void CGWorldFrame::UpdateObject(CGObject_C* obj, int a3) {
    CM2Model* model = obj->GetObjectModel();
    if (!model)
        return;

    bool a2a = true;
    uint32_t v18 = 0;
    uint32_t v17 = 0;
    obj->ShouldRender(a3, &v18, &v17);
    if (v18 || v17)
        a2a = false;
    //obj->PreAnimate(this);
    if (!obj->Animate(0.0f /*this->unk_0B14*/))
        return;

    //if ((a2->m_obj->OBJECT_FIELD_TYPE & (TYPEMASK_CORPSE | TYPEMASK_GAMEOBJECT | TYPEMASK_UNIT)) != 0 && a2a && (a2->m_modelFlags & 0x100000) == 0) {
    //    if (sub_4F7180(&this->unk_02B4)) {
    //        v6 = maybe_CGWorldFrame__AllocModelRecord(&this->unk_029C, 2, 0, 0);
    //    } else {
    //        unk_02BC = this->unk_02BC;
    //        if ((unk_02BC & 1) == 0 && unk_02BC)
    //            v6 = this->unk_02BC;
    //        else
    //            v6 = 0;
    //        bn_TSList_UnlinkNode(v6);
    //        TSList::LinkToTail(&this->unk_029C, v6);
    //    }
    //    v8 = a2->GetObjectModel(a2);
    //    *(v6 + 2) = v8;
    //    ++v8->m_refCount;
    //    m_obj = a2->m_obj;
    //    v6[4] = *&m_obj->OBJECT_FIELD_GUID.guid_low;
    //    v6[5] = *&m_obj->OBJECT_FIELD_GUID.guid_high;
    //}
    model->SetVisible(a2a);
    //m_attachedParent = v5->m_attachedParent;
    //m_bitFlags = v5->m_bitFlags;
    //v12 = a2a & 1;
    //if (m_attachedParent) {
    //    v13 = v12 << 7;
    //    v14 = m_bitFlags & 0xFFFFFF7F;
    //} else {
    //    v13 = 8 * v12;
    //    v14 = m_bitFlags & 0xFFFFFFF7;
    //}
    //v15 = v14 | v13;
    //v5->m_bitFlags = v15;
    //if (m_attachedParent)
    //    v5->m_bitFlags = v15 & 0xFFFDFFFF | (v12 << 17);
    //else
    //    v5->m_bitFlags = v5->m_bitFlags & 0xFFFEFFFF | (v12 << 16);
}

void CGWorldFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    CSimpleFrame::OnFrameRender(batch, layer);
    if (!layer) {
        batch->QueueCallback(&CGWorldFrame::RenderWorld, this);
    }
}

// OFFSET: 0x4F6AE0
int32_t CGWorldFrame::OnLayerKeyDown(const CKeyEvent& evt) {
    if (CSimpleFrame::OnLayerKeyDown(evt)) {
        return 1;
    }

    if (evt.key >= 787)
        return 0;

    if (evt.key <= 5) {
        //KeyName = CSimpleFrame__GetKeyName(metaKeyState);
        //FrameScript::SignalEvent(EVENT_MODIFIER_STATE_CHANGED, "%s%d", KeyName, 1);
        return 1;
    }

    auto keyDown = &this->m_keyDown[evt.key];
    if (!CGUIBindings::KeyEventToString(evt, keyDown->m_keyString, 32))
        return 0;
    keyDown->m_modifiers = evt.metaKeyState;
    return CGUIBindings::s_bindings->ExecKey(evt.metaKeyState, keyDown->m_keyString, 1, 1, BINDING_MODE_4);
}

// OFFSET: 0x4F6B70
int32_t CGWorldFrame::OnLayerKeyUp(const CKeyEvent& evt) {
    if (CSimpleFrame::OnLayerKeyUp(evt)) {
        return 1;
    }

    if (evt.key >= 787)
        return 0;

    if (evt.key <= 5) {
        // KeyName = CSimpleFrame__GetKeyName(metaKeyState);
        // FrameScript::SignalEvent(EVENT_MODIFIER_STATE_CHANGED, "%s%d", KeyName, 0);
        return 1;
    }

    auto keyDown = this->m_keyDown[evt.key];
    int32_t result = 0;
    if (keyDown.m_keyString[0]) {
        keyDown.m_modifiers |= evt.metaKeyState;
        result = CGUIBindings::s_bindings->ExecKey(keyDown.m_modifiers, keyDown.m_keyString, 0, 1, BINDING_MODE_4);
        keyDown.m_keyString[0] = 0;
        return result;
    }

    keyDown.m_modifiers = 0;
    if (CGUIBindings::KeyEventToString(evt, keyDown.m_keyString, 32) && keyDown.m_keyString[0]) {
        keyDown.m_modifiers |= evt.metaKeyState;
        result = CGUIBindings::s_bindings->ExecKey(keyDown.m_modifiers, keyDown.m_keyString, 0, 1, BINDING_MODE_4);
        keyDown.m_keyString[0] = 0;
    }

    return result;
}

CSimpleFrame* CGWorldFrame::Create(CSimpleFrame* parent) {
    auto m = ALLOCATOR_GET(CGWorldFrame::s_allocator);
    return m ? (new (m) CGWorldFrame(parent)) : nullptr;
}

void CGWorldFrame::RenderWorld(void* param) {
    CGWorldFrame* worldFrame = reinterpret_cast<CGWorldFrame*>(param);

    C44Matrix saved_proj;
    GxXformProjection(saved_proj);

    C44Matrix saved_view;
    GxXformView(saved_view);

    worldFrame->OnWorldUpdate();

    // TODO: PlayerNameUpdateWorldText();

    worldFrame->OnWorldRender();

    // TODO: PlayerNameRenderWorldText();

    GxXformSetProjection(saved_proj);
    GxXformSetView(saved_view);

    CShaderEffect::UpdateProjMatrix();
}

// DEBUG: free-fly camera. (Obviously AI made)
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
    auto activePlayer = ClntObjMgrGetActivePlayerObj();
    if (CGUnit_C::s_activeMover) {
        auto moverUnit = ClntObjMgrObjectPtr<CGUnit_C*>(CGUnit_C::s_activeMover, TYPEMASK_UNIT);
        if (moverUnit) {
            C3Vector moverPosition;
            moverUnit->GetPosition(moverPosition);
            //CBarrier::SetViewerPos(moverPosition);
        }
    }

    auto cameraTargetObj = ClntObjMgrObjectPtr<CGObject_C*>(this->m_camera->m_targetGUID, TYPEMASK_OBJECT);
    if (activePlayer && activePlayer->IsCommentatorUberOrInArena()) {
        if (this->m_camera->m_targetGUID)
            this->m_camera->SetTarget(nullptr, 0);

        //CGCommentator::OnUpdate(&CGCommentator::s_Commentator, *&this->unk_0B14);
    } else if (!ClntObjMgrObjectPtr<CGObject_C*>(this->m_camera->m_targetGUID, TYPEMASK_OBJECT) /*&& !ClntObjMgrObjectPtr<CGObject_C*>(this->m_camera->unk_0090, TYPEMASK_OBJECT)*/) {
        if (activePlayer) {
            //if ((activePlayer->unk_1020[1] & 1) != 0)
            //    maybe_CGPlayer_C__ToggleFarSight(v10, 0);
            this->m_camera->SetTarget(activePlayer, 0);
            //maybe_CGUnit_C__OnVehicleCameraPossiblyNeeded(v11);
        }
    }

    if (cameraTargetObj && cameraTargetObj->m_worldObject) {
        //v13 = (cameraTargetObj->ObjectBase.GetTransportGUID)(cameraTargetObj, v39);
        //World::SetCameraTarget(m_worldObject, v13);
    }

    //bn_CVehiclePassenger_C_UpdateAll(FrameTime::s_curTimeMs);
    this->m_camera->UpdateCallback();
    C3Vector camPos = this->m_camera->m_position;
    C3Vector camForward = this->m_camera->Forward();
    C3Vector camTarget = camPos + camForward;

    CRect rect;
    this->GetRect(&rect); // VALIDATE - Binary does not check any flag
    this->m_camera->SetupWorldProjection(rect);
    CGWorldFrame::s_currentWorldFrame->UpdateDayNightInfo(0.0f);

    // TODO
    //CGCamera* cam = CGWorldFrame::GetActiveCamera();
    //
    //UpdateDebugCamera(cam);
    //
    //C3Vector camPos = cam->m_position;
    //C3Vector camForward = cam->Forward();
    //C3Vector camTarget = camPos + camForward;
    //CRect rect;
    //CGWorldFrame::s_currentWorldFrame->GetRect(&rect);
    //CGWorldFrame::GetActiveCamera()->SetupWorldProjection(rect);


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
    CWorld::Render(this->m_camera->m_position, elapsed);

    if (CWorldScene::s_m2Scene) {
        CWorldScene::s_m2Scene->Draw(M2PASS_0);
    }

    GxRsPop();
}

// OFFSET: 0x4F8410
void CGWorldFrame::UpdateDayNightInfo(float delta) {
    auto dayNight = DayNight::GetInfo();
    dayNight->m_farClip = this->m_camera->m_farZ;
    dayNight->m_deltaSec = delta;
    dayNight->m_timeSec = OsGetAsyncTimeMs() * 0.001;
    dayNight->m_cameraPos = this->m_camera->m_position;
    dayNight->m_cameraDir = this->m_camera->Forward();
    auto v9 = 1.0f / sqrt(dayNight->m_cameraDir.z * dayNight->m_cameraDir.z + dayNight->m_cameraDir.y * dayNight->m_cameraDir.y + dayNight->m_cameraDir.x * dayNight->m_cameraDir.x);
    dayNight->m_cameraDir.x = dayNight->m_cameraDir.x * v9;
    dayNight->m_cameraDir.y = dayNight->m_cameraDir.y * v9;
    dayNight->m_cameraDir.z = v9 * dayNight->m_cameraDir.z;

    // TODO
    static uint64_t lastTime;

    float elapsed = static_cast<float>(OsGetAsyncTimeMs() - lastTime) / 1000.0f;
    dayNight->m_dayProgression += elapsed * 0.01f;
    if (dayNight->m_dayProgression >= 1.0f)
        dayNight->m_dayProgression = 0.0f;
    lastTime = OsGetAsyncTimeMs();

    dayNight->m_lightRefPos = this->m_camera->m_position;
}

CGCamera* CGWorldFrame::GetActiveCamera() {
    STORM_ASSERT(CGWorldFrame::s_currentWorldFrame);
    STORM_ASSERT(CGWorldFrame::s_currentWorldFrame->m_camera);
    return CGWorldFrame::s_currentWorldFrame->m_camera;
}

// OFFSET: 0x4F9F70
bool CGWorldFrame::ObjectEnumProc(void* param, uint32_t status, uint64_t param64, uint32_t param32) {
    CGObject_C* obj = ClntObjMgrObjectPtr<CGObject_C*>(WGUID(param64), TYPEMASK_OBJECT);
    if (!obj)
        return true;

    if ((status & 4) != 0)
        obj->m_modelFlags |= 0x800000;
    else
        obj->m_modelFlags &= ~0x800000;

    if ((obj->m_modelFlags & 0x10000) == 0 && obj->IsReadyToDraw()) {
        CGWorldFrame::s_currentWorldFrame->UpdateObject(obj, status);
        return true;
    }

    CM2Model* model = obj->GetObjectModel();
    if (model) {
        model->SetVisible(0);
        if (model->m_attachParent)
            model->m_flag20000 = 0;
        else
            model->m_flag10000 = 0;
    }
    return true;
}
