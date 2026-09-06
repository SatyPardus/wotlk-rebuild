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
#include <util/Input.hpp>
#include "gameui/CGGameUI.hpp"
#include <gx/Coordinate.hpp>
#include <util/Unimplemented.hpp>

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

// OFFSET: 0x4F6C10
int32_t CGWorldFrame::OnLayerMouseDown(const CMouseEvent& evt, const char* btn) {
    if (this->CSimpleFrame::OnLayerMouseDown(evt, btn) || CGGameUI::HandleMouseDown(evt)) {
        return 1;
    }

    auto& state = this->m_mouseDown[MouseButtonToIndex(evt.button)];

    if (!CGUIBindings::MouseEventToString(evt, state.m_keyString, sizeof(state.m_keyString))) {
        return 0;
    }

    state.m_modifiers = evt.metaKeyState;

    return CGUIBindings::s_bindings->ExecKey(state.m_modifiers, state.m_keyString, 1, 1, BINDING_MODE_4);
}

// OFFSET: 0x4F6C90
int32_t CGWorldFrame::OnLayerMouseUp(const CMouseEvent& evt, const char* btn) {
    if (this->CSimpleFrame::OnLayerMouseUp(evt, btn)) {
        return 1;
    }

    int32_t result = 0;

    auto& state = this->m_mouseDown[MouseButtonToIndex((uint32_t)evt.button)];

    if (!state.m_keyString[0]) {
        state.m_modifiers = 0;
        CGUIBindings::MouseEventToString(evt, state.m_keyString, sizeof(state.m_keyString));
    }

    if (state.m_keyString[0]) {
        state.m_modifiers |= evt.metaKeyState;

        result = CGUIBindings::s_bindings->ExecKey(state.m_modifiers, state.m_keyString, 0, 1, BINDING_MODE_4);

        state.m_keyString[0] = '\0';
    }

    return result;
}

// OFFSET: 0x4F5C80
int32_t CGWorldFrame::OnLayerMouseWheel(const CMouseEvent& evt) {
    if (this->CSimpleFrame::OnLayerMouseWheel(evt) || !evt.wheelDistance) {
        return 1;
    }

    char keyString[32];

    if (!CGUIBindings::MouseEventToString(evt, keyString, sizeof(keyString))) {
        return 0;
    }

    int32_t down = CGUIBindings::s_bindings->ExecKey(evt.metaKeyState, keyString, 1, 1, BINDING_MODE_4);

    return CGUIBindings::s_bindings->ExecKey(evt.metaKeyState, keyString, 0, 1, BINDING_MODE_4) + down;
}

// OFFSET: 0x4FA570
void CGWorldFrame::SetupDefaultAction() {
    WHOA_UNIMPLEMENTED();
    //NDCToDDC(this->m_top->m_mousePosition.x, this->m_top->m_mousePosition.y, &this->m_defaultActionPointDDC.x, &this->m_defaultActionPointDDC.y);
    //this->m_defaultActionHitKind = this->HitTestPoint(&this->m_defaultActionPointDDC.x, &this->m_defaultActionPointDDC.y, 0, this->m_defaultActionHit);
}

// OFFSET: 0x4F5D30
void CGWorldFrame::OnMouseModeRelative() {
    this->m_worldFlags |= 2u;

    if (this->m_trackedGuid != 0) {
        //v4[2] = guid_low;
        //v4[0] = 0;
        //v4[1] = 0;
        //v4[3] = guid_high;
        //CGGameUI::HandleSpriteTrack(v4);
        this->m_trackedGuid = 0;
    }
}

// OFFSET: 0x4F5D20
void CGWorldFrame::OnMouseModeNormal() {
    this->m_worldFlags &= ~2u;
}

// OFFSET: 0x4F7880
void CGWorldFrame::PerformDefaultAction(MOUSEBUTTON button) {
    WHOA_UNIMPLEMENTED();
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
        CWorldScene::SetCameraTarget(cameraTargetObj->m_worldObject, cameraTargetObj->GetTransportGUID());
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
