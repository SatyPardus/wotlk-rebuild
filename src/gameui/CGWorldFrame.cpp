#include "gameui/CGWorldFrame.hpp"
#include "model/CM2Model.hpp"
#include "model/CM2Scene.hpp"

#include "gx/Transform.hpp"
#include "gx/Draw.hpp"
#include "gx/Shader.hpp"
#include "gx/Device.hpp"
#include "gx/RenderState.hpp"
#include "world/CWorld.hpp"
#include "world/World.hpp"
#include "world/map/CMap.hpp"
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
#include "gameui/CGInputControl.hpp"
#include <gx/Coordinate.hpp>
#include <util/Unimplemented.hpp>
#include <clientobject/PlayerName.hpp>
#include "console/DebugScreen.hpp"
#include "gameui/Types.hpp"

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
    obj->PreAnimate(this);
    if (!obj->Animate(this->m_elapsedSec))
        return;

    if ((obj->m_obj->m_type & (TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_CORPSE)) && a2a && (obj->m_modelFlags & 0x100000) == 0) {
        CModelRecord* record;

        if (this->m_freeModelList.IsEmpty()) {
            record = this->m_modelList.NewNode(STORM_LIST_TAIL, 0, 0);
        } else {
            record = this->m_freeModelList.Head();
            this->m_freeModelList.UnlinkNode(record);
            this->m_modelList.LinkToTail(record);
        }

        record->m_model = obj->GetObjectModel();
        record->m_model->m_refCount++;
        record->m_guid = obj->m_obj->m_guid;
    }

    model->SetVisible(a2a);
    if (model->m_attachParent) {
        model->m_flag20000 = a2a;
    } else {
        model->m_flag10000 = a2a;
    }
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
    NDCToDDC(this->m_top->m_mousePosition.x, this->m_top->m_mousePosition.y, &this->m_defaultActionPointDDC.x, &this->m_defaultActionPointDDC.y);
    this->m_defaultActionHitKind = this->HitTestPoint(this->m_defaultActionPointDDC.x, this->m_defaultActionPointDDC.y, 0, &this->m_defaultActionHit);
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
    CGUnit_C* player = ClntObjMgrObjectPtr<CGUnit_C*>(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER);

    if (!player || player->m_unit->UNIT_FIELD_CHARMEDBY) {
        CWorldClickEvent evt;

        evt.segStart.x = 0.0f;
        evt.segStart.y = 0.0f;
        evt.segStart.z = 0.0f;
        evt.segEnd.x = 0.0f;
        evt.segEnd.y = 0.0f;
        evt.segEnd.z = 0.0f;
        evt.button = button;

        CGGameUI::HandleWorldClick(&evt);
        return;
    }

    // CGNamePlateFrame* focus = CGNamePlateFrame::GetNamePlateFocus();
    //
    // if (focus) {
    //     if (button == MOUSE_BUTTON_LEFT) {
    //         return CGGameUI::OnSpriteLeftClick(focus->m_trackedGuid.guid_low, focus->m_trackedGuid.guid_high);
    //     }
    //
    //     return CGGameUI::OnSpriteRightClick(focus->m_trackedGuid.guid_low, focus->m_trackedGuid.guid_high);
    // }

    switch (this->m_defaultActionHitKind) {
    case 0: {
        CWorldClickEvent evt;

        evt.segStart = this->m_defaultActionHit.segStart;
        evt.segEnd = this->m_defaultActionHit.segEnd;
        evt.button = button;

        CGGameUI::HandleWorldClick(&evt);
        break;
    }

    case 1:
    case 3: {
        CTerrainClickEvent evt;

        uint32_t guidHigh = this->m_defaultActionHit.guid.guid_high;

        if ((guidHigh & 0xF0000000) == 0x10000000 && (guidHigh & 0x0FF00000) == 0x0FC00000) {
            evt.guid = this->m_defaultActionHit.guid;
        } else {
            evt.guid.guid_low = 0;
            evt.guid.guid_high = 0;
        }

        evt.point = this->m_defaultActionHit.point;
        evt.button = button;

        CGGameUI::HandleTerrainClick(&evt);
        break;
    }

    case 2: {
        CSpriteClickEvent evt;

        evt.guid = this->m_defaultActionHit.guid;
        evt.button = button;

        CGGameUI::HandleSpriteClick(&evt);
        break;
    }
    }
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

    this->m_camera->SetupWorldProjection(this->m_viewportDDC);
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

    this->MoveToFreeList(&this->m_modelList);
    this->MoveToFreeList(&this->m_hitModelList);

    CGUnit_C::UpdateAllSmoothFacing();
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

    PlayerNameTestRender();

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

// OFFSET: 0x4F9310
void CGWorldFrame::MoveToFreeList(STORM_LIST(CModelRecord)* list) {
    for (CModelRecord* record = list->Head(); record; record = list->Next(record)) {
        if (record->m_model) {
            record->m_model->Release();
            record->m_model = nullptr;
        }
    }

    STORM_ASSERT(list);
    STORM_ASSERT(list != &this->m_freeModelList);
    STORM_ASSERT(list->m_linkoffset == this->m_freeModelList.m_linkoffset);

    while (CModelRecord* record = list->Head()) {
        list->UnlinkNode(record);
        this->m_freeModelList.LinkToTail(record);
    }
}

// OFFSET: 0x4F6370
static void AddModelToHitTestList(CM2Model* model, int32_t group, void* owner, uint32_t mode) {
    if (model->f_flags & 0x1) {
        if (!model->m_hitTestPrev) {
            CM2Model** head = &model->m_scene->m_hitTestList;

            model->m_hitTestPrev = head;
            model->m_hitTestNext = *head;
            *head = model;

            if (model->m_hitTestNext) {
                model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
            }
        }

        model->m_hitTestMode = mode;
        model->m_hitTestGroup = group;
        model->m_hitTestOwner = owner;
    }

    for (CM2Model* child = model->m_attachList; child; child = child->m_attachNext) {
        if (!child->m_hitTestPrev) {
            AddModelToHitTestList(child, group, reinterpret_cast<void*>(static_cast<intptr_t>(-1)), mode);
        }
    }
}

// OFFSET: 0x4F6400
void CGWorldFrame::AddObjectToHitTestList(CModelRecord* record, uint32_t flags) {
    int32_t group = -1;

    if (record->m_guid != this->m_hitGuid) {
        group = 0;

        //CGObject_C* obj = record->m_object;
        //
        //if (obj) {
        //    switch (obj->m_obj->m_type) {
        //    case TYPEMASK_OBJECT | TYPEMASK_UNIT:
        //    case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER:
        //        group = 3;
        //        if (obj->AsUnit()->unk_00D0->unk_48 <= 0) {
        //            group = obj->AsUnit()->CanBeLooted(OsGetAsyncTimeMs()) ? 2 : 0;
        //        }
        //        break;
        //    case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT:
        //        group = static_cast<CGGameObject_C*>(obj)->CanUse();
        //        break;
        //    case TYPEMASK_OBJECT | TYPEMASK_CORPSE:
        //        group = static_cast<CGCorpse_C*>(obj)->CanBeLooted() ? 2 : 0;
        //        break;
        //    }
        //}
    }

    record->m_object = nullptr;

    AddModelToHitTestList(record->m_model, group, record, (flags >> 25) & 4);
}

// OFFSET: 0x4F6450
bool CGWorldFrame::GetLineSegment(float mouseX, float mouseY, C3Vector* start, C3Vector* end) {
    float x = (mouseX - this->m_viewportDDC.minX) / (this->m_viewportDDC.maxX - this->m_viewportDDC.minX);
    float y = (mouseY - this->m_viewportDDC.minY) / (this->m_viewportDDC.maxY - this->m_viewportDDC.minY);

    if (x < 0.0f || y < 0.0f || x > 1.0f || y > 1.0f) {
        return false;
    }

    CameraGetLineSegment(x, y, start, end);

    const C3Vector& position = this->m_camera->Position();

    start->x += position.x;
    start->y += position.y;
    start->z += position.z;

    end->x += position.x;
    end->y += position.y;
    end->z += position.z;

    return true;
}

// OFFSET: 0x4F7650
uint32_t CGWorldFrame::GetHitTestFilterFlags(uint32_t unused) {
    //if (Spell_C_IsTargeting()) {
    //    uint32_t flags = 0;
    //    if (Spell_C_CanTargetTerrain())
    //        flags = 3;
    //    if (Spell_C_CanTargetObjects())
    //        flags |= 4;
    //    if (Spell_C_CanTargetUnits()) {
    //        flags |= 0x18;
    //        if (Spell_C_CanTargetMe())
    //            flags |= 0x20;
    //        if (Spell_C_CanTargetParty())
    //            flags |= 0x10000;
    //        if (Spell_C_CanTargetFriends())
    //            flags |= 0x40000;
    //        if (Spell_C_CanTargetEnemies())
    //            flags |= 0x80000;
    //        if (Spell_C_CanTargetRaid())
    //            flags |= 0x20000;
    //        if (Spell_C_CanTargetDead())
    //            flags |= 0x200040;
    //        if (Spell_C_CanTargetAlive())
    //            flags |= 0x100000;
    //        if (Spell_C_CanTargetNonCombatPet())
    //            flags |= 0x400000;
    //        if (Spell_C_CanTargetPossessedFriends())
    //            flags |= 0x800000;
    //    }
    //    if (Spell_C_CanTargetFriendCorpses())
    //        flags |= 0x40040;
    //    if (Spell_C_CanTargetEnemyCorpses())
    //        flags |= 0x80040;
    //    return flags;
    //}

    uint32_t flags = 0;
    auto player = ClntObjMgrObjectPtr<CGPlayer_C*>(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER);
    auto mover = ClntObjMgrObjectPtr<CGUnit_C*>(CGUnit_C::s_activeMover, TYPEMASK_UNIT);

    //if (mover && mover->CanAutoInteract() && CGGameUI::m_cursorItemType == 0) {
    //    uint32_t f = mover->m_passenger->unk_44;
    //    flags = 1;
    //    if ((f & 0x10000000) && !(f & 0x200000)) {
    //        flags = 3;
    //    }
    //}

    if (player) {
        flags |= 0x5C;
    }

    return flags;
}


// OFFSET: 0x4F9930
int32_t CGWorldFrame::HitTest(C3Vector* start, C3Vector* end, uint32_t flags, HITTESTRESULT* result) {
    C3Vector hitPoint = { 0.0f, 0.0f, 0.0f };
    float t = 1.0f;
    float objDist = 0.0f;
    WGUID collisionGuid;

    uint32_t queryFlags = 0x1000124;

    if (flags & 0x2) {
        queryFlags = 0x1020124;
    }

    C3Vector segEnd = *end;

    float dirX = segEnd.x - start->x;
    float dirY = segEnd.y - start->y;
    float dirZ = segEnd.z - start->z;

    float len = dirZ * dirZ + dirY * dirY + dirX * dirX;

    if (fabsf(len) >= 0.00000023841858f) {
        len = sqrtf(len);
        dirX = dirX * (1.0f / len);
        dirY = dirY * (1.0f / len);
        dirZ = (1.0f / len) * dirZ;
    }

    bool terrainHit = World::Intersect(start, &segEnd, &hitPoint, &t, queryFlags, nullptr);

    if (terrainHit) {
        float dist = len * t;
        collisionGuid = CMap::s_lastCollisionGUID;
        len = dist;
        segEnd.x = dirX * dist + start->x;
        segEnd.y = dirY * dist + start->y;
        segEnd.z = dirZ * dist + start->z;
        t = dist;
    }

    WGUID modelGuid;

    if (flags & 0x7C) {
        modelGuid = this->FindClosestModel(start, &segEnd, flags, &objDist);

        if (modelGuid) {
            len = objDist;
            segEnd.x = dirX * objDist + start->x;
            segEnd.y = dirY * objDist + start->y;
            segEnd.z = objDist * dirZ + start->z;
        }
    }

    uint32_t terrainFlags = flags & 0x3;

    if (terrainFlags) {
        t = 1.0f;

        if (World::Intersect(start, &segEnd, &hitPoint, &t, 0x100111, nullptr)) {
            float dist = len * t;
            collisionGuid = CMap::s_lastCollisionGUID;
            terrainHit = true;
            segEnd.x = dirX * dist + start->x;
            segEnd.y = dirY * dist + start->y;
            segEnd.z = dirZ * dist + start->z;
            t = dist;
        }
    }

    C3Vector screenPos;
    screenPos.z = 0.0f;

    if (this->GetScreenCoordinates(&segEnd, &screenPos, nullptr)) {
        g_theGxDevicePtr->CursorSetDepth(screenPos.z);
    }

    if (modelGuid) {
        result->distance = objDist;
        result->point = segEnd;
        result->guid = modelGuid;

        return 2;
    }

    if (!terrainHit) {
        return 0;
    }

    if (collisionGuid) {
        result->point = segEnd;

        CGObject_C* obj = ClntObjMgrObjectPtr<CGObject_C*>(collisionGuid, TYPEMASK_OBJECT);

        if (obj) {
            result->guid = collisionGuid;

            if (obj->IsTransport()) {
                C44Matrix matrix;
                obj->GetMatrix(matrix);
                result->point *= matrix.AffineInverse();
                result->distance = t;

                return 3;
            }
        } else {
            result->guid = 0;
        }

        result->distance = t;

        return 3;
    }

    result->point = segEnd;
    result->distance = t;
    result->guid = 0;

    return terrainFlags != 0;
}

// OFFSET: 0x4F9550
WGUID CGWorldFrame::FindClosestModel(C3Vector* start, C3Vector* end, uint32_t flags, float* dist) {
    C44Matrix view;
    GxXformView(view);

    const C3Vector& camPos = this->m_camera->Position();

    C3Vector local;

    local.x = start->x - camPos.x;
    local.y = start->y - camPos.y;
    local.z = start->z - camPos.z;

    C3Vector segStart = view.TransformPoint(local);

    local.x = end->x - camPos.x;
    local.y = end->y - camPos.y;
    local.z = end->z - camPos.z;

    C3Vector segEnd = view.TransformPoint(local);

    STORM_LIST(CModelRecord) lowPriorityList;

    int32_t time = static_cast<int32_t>(OsGetAsyncTimeMs());

    CWorldScene::s_m2Scene->BeginHitTest();

    //bool allowTerrain = Spell_C_TargetingSpellAllowsTerrain();
    bool allowTerrain = false;

    CModelRecord* record = this->m_modelList.Head();

    while (record) {
        CModelRecord* next = this->m_modelList.Next(record);

        CGObject_C* obj = ClntObjMgrObjectPtr<CGObject_C*>(record->m_guid, TYPEMASK_OBJECT);

        if (obj && this->IsLegalSelection(obj, flags) && (obj->CanHighlight() || allowTerrain)) {
            record->m_object = obj;
        
            if ((obj->m_obj->m_type & TYPEMASK_UNIT) && obj->AsUnit()->IsLowPrioritySelection(time)) {
                lowPriorityList.LinkToTail(record);
            } else {
                this->AddObjectToHitTestList(record, flags);
            }
        }

        record = next;
    }

    float t = 1.0f;
    CModelRecord* hit = static_cast<CModelRecord*>(CWorldScene::s_m2Scene->EndHitTest(segStart, segEnd, &t, 1));

    if (!hit) {
        CWorldScene::s_m2Scene->BeginHitTest();

        for (CModelRecord* low = lowPriorityList.Head(); low; low = lowPriorityList.Next(low)) {
            this->AddObjectToHitTestList(low, flags);
        }

        t = 1.0f;
        hit = static_cast<CModelRecord*>(CWorldScene::s_m2Scene->EndHitTest(segStart, segEnd, &t, 1));
    }

    STORM_ASSERT(&lowPriorityList != &this->m_modelList);
    STORM_ASSERT(lowPriorityList.m_linkoffset == this->m_modelList.m_linkoffset);

    while (CModelRecord* low = lowPriorityList.Head()) {
        lowPriorityList.UnlinkNode(low);
        this->m_modelList.LinkToTail(low);
    }

    if (!hit) {
        return WGUID();
    }

    float dx = segEnd.x - segStart.x;
    float dy = segEnd.y - segStart.y;
    float dz = segEnd.z - segStart.z;

    float hitDist = sqrtf(dz * dz + dy * dy + dx * dx) * t;

    hit->m_dist = hitDist;
    *dist = hitDist;

    return hit->m_guid;
}

// OFFSET: 0x4F6D20
bool CGWorldFrame::GetScreenCoordinates(C3Vector* worldPos, C3Vector* screenPos, int32_t* clipFlags) {
    const C3Vector& camPos = this->m_camera->Position();

    C4Vector local;
    local.x = worldPos->x - camPos.x;
    local.y = worldPos->y - camPos.y;
    local.z = worldPos->z - camPos.z;
    local.w = 0.0f;

    C4Vector clip = this->m_viewProjection.TransformPoint(local);

    if (clip.z < this->m_camera->NearZ()) {
        return false;
    }

    screenPos->z = clip.z;

    float invW = 1.0f / clip.w;

    float ndcX = (clip.x * invW + 1.0f) * 0.5f;
    float ndcY = (clip.y * invW + 1.0f) * 0.5f;

    float ddcX;
    float ddcY;
    NDCToDDC(ndcX * (this->m_viewportNDC.maxX - this->m_viewportNDC.minX), ndcY * (this->m_viewportNDC.maxY - this->m_viewportNDC.minY), &ddcX, &ddcY);

    if (this->m_resizeRect.minX < 0.0f) {
        ddcX = ddcX - this->m_resizeRect.minX;
    }

    if (this->m_resizeRect.minY < 0.0f) {
        ddcY = ddcY - this->m_resizeRect.minY;
    }

    screenPos->x = ddcX;
    screenPos->y = ddcY;

    float width = this->m_resizeRect.maxX - this->m_resizeRect.minX;
    float height = this->m_resizeRect.maxY - this->m_resizeRect.minY;

    if (!clipFlags) {
        return ddcX >= 0.0f && width >= ddcX && ddcY >= 0.0f && height >= ddcY;
    }

    int32_t inside = (ddcX >= 0.0f) | (2 * (ddcY >= 0.0f)) | (4 * (width >= ddcX)) | (8 * (height >= ddcY));

    *clipFlags = inside;

    return inside == 15;
}

// OFFSET: 0x4F7530
bool CGWorldFrame::IsLegalSelection(CGObject_C* obj, uint32_t a2) {
    //switch (a1->m_obj->m_type) {
    //case TYPEMASK_UNIT | TYPEMASK_OBJECT:
    //    if ((a2 & 8) == 0)
    //        goto LABEL_12;
    //    goto LABEL_3;
    //case TYPEMASK_PLAYER | TYPEMASK_UNIT | TYPEMASK_OBJECT:
    //    if ((a2 & 0x10) == 0)
    //        goto LABEL_12;
//LABEL_3:
    //    result = maybe_CGWorldFrame__IsUnitLegalSelection(a1, a2);
    //    break;
    //case TYPEMASK_GAMEOBJECT | TYPEMASK_OBJECT:
    //    if ((a2 & 4) == 0 || Spell_C_IsTargeting() && !bn_Spell_C_CanTargetObject(a1))
    //        goto LABEL_12;
    //    result = 1;
    //    break;
    //case TYPEMASK_CORPSE | TYPEMASK_OBJECT:
    //    if ((a2 & 0x40) == 0)
    //        goto LABEL_12;
    //    result = maybe_CGWorldFrame__CanTargetCorpseSelection(a1, a2);
    //    break;
    //default:
//LABEL_12:
    //    result = 0;
    //    break;
    //}
    //return result;
    return true;
}

// OFFSET: 0x4F5A90
void CGWorldFrame::OnFrameSizeChanged(const CRect& rect) {
    this->CSimpleFrame::OnFrameSizeChanged(rect);

    this->m_viewportDDC = this->m_rect;

    if (this->m_viewportDDC.minX <= 0.0f) {
        this->m_viewportDDC.minX = 0.0f;
    }

    if (this->m_viewportDDC.minY <= 0.0f) {
        this->m_viewportDDC.minY = 0.0f;
    }

    if (NDCToDDCWidth(1.0f) <= this->m_viewportDDC.maxX) {
        this->m_viewportDDC.maxX = NDCToDDCWidth(1.0f);
    }

    if (NDCToDDCHeight(1.0f) <= this->m_viewportDDC.maxY) {
        this->m_viewportDDC.maxY = NDCToDDCHeight(1.0f);
    }

    //if (this->unk_7E00) {
    //    CCameraManager::SetScreenAspect(&this->m_viewportDDC);
    //}

    DDCToNDC(this->m_rect.minX, this->m_rect.minY, &this->m_viewportNDC.minX, &this->m_viewportNDC.minY);
    DDCToNDC(this->m_rect.maxX, this->m_rect.maxY, &this->m_viewportNDC.maxX, &this->m_viewportNDC.maxY);

    if (this->m_viewportNDC.minX <= 0.0f) {
        this->m_viewportNDC.minX = 0.0f;
    }

    if (this->m_viewportNDC.minY <= 0.0f) {
        this->m_viewportNDC.minY = 0.0f;
    }

    if (this->m_viewportNDC.maxX >= 1.0f) {
        this->m_viewportNDC.maxX = 1.0f;
    }

    if (this->m_viewportNDC.maxY >= 1.0f) {
        this->m_viewportNDC.maxY = 1.0f;
    }
}

// OFFSET: 0x4FA040
void CGWorldFrame::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    // CGChatBubbleFrame::OnWorldLayerUpdate();

    CSimpleTop* top = this->m_top;

    HITTESTRESULT hit = {};
    int32_t kind = -1;

    CSimpleFrame* mouseFocus = top->m_mouseFocus;

    if (mouseFocus == this) {
        float x = 0.0f;
        float y = 0.0f;

        NDCToDDC(top->m_mousePosition.x, top->m_mousePosition.y, &x, &y);

        // if (Spell_C_IsTargeting() && Spell_C_CanTargetTerrain() && !Spell_C_CanTargetUnits()) {
        //     CGUnit_C::ClearNamePlateFocus();
        // } else {
        //     CGUnit_C::SetNamePlateFocus(&x);
        // }

        // if (CGNamePlateFrame::GetNamePlateFocus()) {
        //     kind = 2;
        //     hit.guid = CGNamePlateFrame::GetNamePlateFocus()->m_trackedGuid;
        // } else {
        kind = this->HitTestPoint(x, y, 1, &hit);
        //}
    } else if (mouseFocus && (mouseFocus->m_layoutFlags & 0x10000)) {
        // lua_State* L = FrameScript_GetContext();
        //
        // FrameScript_TaintExpectedBegin();
        //
        // lua_rawgeti(L, LUA_REGISTRYINDEX, FrameScript_GetErrorHandlerReference());
        // lua_pushstring(L, "SecureButton_GetModifiedUnit");
        // lua_rawget(L, LUA_GLOBALSINDEX);
        //
        // if (!mouseFocus->luaRegistered) {
        //     mouseFocus->RegisterScriptObject(0);
        // }
        //
        // lua_rawgeti(L, LUA_REGISTRYINDEX, mouseFocus->lua_objectRef);
        // lua_pushstring(L, CGUIMacros::m_macroRunning);
        //
        // if (!lua_pcall(L, 2, 1, -4)) {
        //     const char* token = lua_tolstring(L, -1, nullptr);
        //
        //     if (token && !*lua_tainted && Script_GetGUIDFromToken(token, &hit.guid, 0)) {
        //         kind = 2;
        //     }
        // }
        //
        // lua_settop(L, -3);
        //
        // FrameScript_TaintExpectedEnd();
    }

    // s_spellShadowStyle = 3;

    switch (kind) {
    case 0:
        // if (Spell_C_IsTargeting()) {
        //     CursorSetMode(CURSORMODE(28));
        // } else {
        //     CursorResetMode();
        // }
        //
        // this->SendObjectTrackEvent(0, 0);
        break;

    case 1:
        // this->OnLayerTrackTerrain(&hit.guid);
        break;

    case 2:
        // this->OnLayerTrackObject(&hit.guid);
        break;

    case 3:
        //{
        //    CGGameObject_C* obj = ClntObjMgrObjectPtr<CGGameObject_C*>(hit.guid, TYPEMASK_GAMEOBJECT);
        //
        //    bool ownTransport = false;
        //
        //    if (obj && obj->m_gameObjectDef->m_type == 33 && !Spell_C_CanTargetTerrain()) {
        //        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
        //
        //        if (player && obj->m_obj->m_guid == World::QueryObjectParentParam64(player->m_worldObject)) {
        //            ownTransport = true;
        //        }
        //    } else {
        //        ownTransport = true;
        //    }
        //
        //    if (ownTransport) {
        //        this->OnLayerTrackTerrain(&hit.guid);
        //    } else {
        //        this->OnLayerTrackObject(&hit.guid);
        //    }
        //}
        break;

    default:
        break;
    }

    DebugScreenSet("Mouse Target Type", "%d", kind);
    DebugScreenSet("Hit Guid", "%u", hit.guid);

    // CGInputControl::GetActive()->OnUpdate(elapsedSec);

    this->m_elapsedSec = elapsedSec;

    // CGGameUI::UpdateInteractTarget();
    // CGGameUI::UpdateCorpseDistance();
    // CGGameUI::UpdateAreaSpiritHealerDistance();
    // CGMailInfo::UpdatePendingMail(elapsedSec);
}

// OFFSET: 0x4F9DA0
int32_t CGWorldFrame::HitTestPoint(float mouseX, float mouseY, int32_t a4, HITTESTRESULT* result) {
    if ((CGInputControl::GetActive()->m_mouseModeFlags & 0x1) == 0) {
        return 0;
    }

    C44Matrix savedProjection;
    GxXformProjection(savedProjection);

    C44Matrix savedView;
    GxXformView(savedView);

    this->m_camera->SetupWorldProjection(this->m_viewportDDC);

    int32_t kind = 0;

    uint32_t filterFlags = this->GetHitTestFilterFlags(a4);

    if (filterFlags) {
        C3Vector start = { 0.0f, 0.0f, 0.0f };
        C3Vector end = { 0.0f, 0.0f, 0.0f };

        if (this->GetLineSegment(mouseX, mouseY, &start, &end)) {
            result->segStart = start;
            result->segEnd = end;

            kind = this->HitTest(&start, &end, filterFlags, result);

            if (kind >= 2) {
                this->m_hitGuid = result->guid;
            } else {
                this->MoveToFreeList(&this->m_hitModelList);
                this->m_hitGuid = 0;
            }
        }
    }

    GxXformSetProjection(savedProjection);
    GxXformSetView(savedView);

    return kind;
}
