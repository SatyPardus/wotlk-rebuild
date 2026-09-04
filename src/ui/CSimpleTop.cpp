#include "ui/CSimpleTop.hpp"
#include "gx/Camera.hpp"
#include "gx/Coordinate.hpp"
#include "gx/Device.hpp"
#include "gx/Font.hpp"
#include "gx/Screen.hpp"
#include "ui/CSimpleFrame.hpp"
#include "ui/CSimpleRender.hpp"
#include "ui/CSimpleTexture.hpp"
#include "ui/CSimpleTitleRegion.hpp"
#include "cursor/Cursor.hpp"
#include <cstring>
#include <tempest/Math.hpp>

CSimpleTop* CSimpleTop::s_instance;
uint32_t CSimpleTop::m_eventTime;

// OFFSET: 0x496100
void CSimpleTop::FreeFrameStrataNodes(CFrameStrata* strata) {
    for (uint32_t i = 0; i < strata->levels.Count(); i++) {
        auto node = strata->levels[i];

        if (node) {
            node->~CFrameStrataNode();
            SMemFree(node, __FILE__, __LINE__, 0x0);
        }

        strata->levels[i] = nullptr;
    }
}

// OFFSET: 0x496170
CSimpleTop::~CSimpleTop() {
    while (auto frame = this->m_destroyed.Head()) {
        this->m_destroyed.UnlinkNode(frame);
    }

    while (auto frame = this->m_frames.Head()) {
        this->m_frames.UnlinkNode(frame);
        delete frame;
    }

    for (int32_t s = 0; s < NUM_FRAME_STRATA; s++) {
        auto strata = this->m_strata[s];

        if (strata) {
            CSimpleTop::FreeFrameStrataNodes(strata);
            strata->~CFrameStrata();
            SMemFree(strata, __FILE__, __LINE__, 0x0);
        }

        this->m_strata[s] = nullptr;
    }

    this->DisableEvents();
    HandleClose(this->m_screenLayer);
    CSimpleTop::s_instance = nullptr;
}

void PaintScreen(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    CSimpleTop::s_instance->OnLayerUpdate(elapsedSec);
    CSimpleTop::s_instance->OnLayerRender();
}

// OFFSET: 0x494280
int32_t CSimpleTop::OnChar(const EVENT_DATA_CHAR* pCharData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);

    int32_t eaten = 0;

    CCharEvent charEvent;
    charEvent = *pCharData;
    charEvent.id = 0x40060067;

    for (int32_t strata = FRAME_STRATA_TOOLTIP; strata >= FRAME_STRATA_WORLD; strata--) {
        if (eaten) {
            break;
        }

        auto priorities = &top->m_eventqueue[strata][SIMPLE_EVENT_CHAR];

        for (uint32_t i = 0; i < priorities->Count(); i++) {
            auto priority = priorities->operator[](i);

            if (!priority || eaten) {
                break;
            }

            auto frame = priority->frame;

            eaten = frame->OnLayerChar(charEvent);
        }
    }

    return eaten == 0;
}

// OFFSET: 0x4955E0
int32_t CSimpleTop::OnDisplaySizeChanged(const EVENT_DATA_SIZE* pSizeData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);

    GxuFontWindowSizeChanged();

    for (auto frame = top->m_frames.Head(); frame; frame = top->m_frames.Link(frame)->Next()) {
        frame->OnScreenSizeChanged();
    }

    if (top->m_displaySizeCallback) {
        CSizeEvent sizeEvent;
        sizeEvent = *pSizeData;
        sizeEvent.id = 0x40040064;

        top->m_displaySizeCallback(sizeEvent);
    }

    return 1;
}

// OFFSET: 0x494970
int32_t CSimpleTop::OnFocusChanged(const void* a1, void* a2) {
    // TODO
    return 0;
}

// OFFSET: 0x494320
int32_t CSimpleTop::OnIme(const void* a1, void* a2) {
    // TODO
    return 0;
}

// OFFSET: 0x4943C0
int32_t CSimpleTop::OnKeyDown(const EVENT_DATA_KEY* pKeyData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    CSimpleTop::m_eventTime = pKeyData->time;

    CKeyEvent keyEvent;
    keyEvent = *pKeyData;
    keyEvent.id = 0x40060064;

    // TODO
    // if (top->unk1239) {
    //     top->unk1239(keyEvent);
    // }

    int32_t eaten = 0;

    for (int32_t strata = FRAME_STRATA_TOOLTIP; strata >= FRAME_STRATA_WORLD; strata--) {
        if (eaten) {
            break;
        }

        auto priorities = &top->m_eventqueue[strata][SIMPLE_EVENT_KEY];

        for (uint32_t i = 0; i < priorities->Count(); i++) {
            auto priority = priorities->operator[](i);

            if (!priority || eaten) {
                break;
            }

            auto frame = priority->frame;

            eaten = frame->OnLayerKeyDown(keyEvent);

            if (eaten) {
                top->m_keydownCapture[keyEvent.key] = frame;
            }
        }
    }

    return eaten == 0;
}

// OFFSET: 0x494530
int32_t CSimpleTop::OnKeyDownRepeat(const EVENT_DATA_KEY* pKeyData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    CSimpleTop::m_eventTime = pKeyData->time;

    int32_t eaten = 0;

    CSimpleFrame* frame = top->m_keydownCapture[pKeyData->key];

    if (frame) {
        CKeyEvent keyEvent;
        keyEvent = *pKeyData;
        keyEvent.id = 0x40060065;

        frame->OnLayerKeyDownRepeat(keyEvent);

        eaten = 1;
    }

    return eaten == 0;
}

// OFFSET: 0x494490
int32_t CSimpleTop::OnKeyUp(const EVENT_DATA_KEY* pKeyData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    CSimpleTop::m_eventTime = pKeyData->time;

    int32_t eaten = 0;

    CSimpleFrame* frame = top->m_keydownCapture[pKeyData->key];

    if (frame) {
        CKeyEvent keyEvent;
        keyEvent = *pKeyData;
        keyEvent.id = 0x40060066;

        frame->OnLayerKeyUp(keyEvent);

        eaten = 1;
    } else if (pKeyData->key == KEY_PRINTSCREEN) {
        eaten = CSimpleTop::OnKeyDown(pKeyData, param);
    }

    top->m_keydownCapture[pKeyData->key] = nullptr;

    return eaten == 0;
}

// OFFSET: 0x495470
int32_t CSimpleTop::OnMouseDown(const EVENT_DATA_MOUSE* pMouseData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);

    CMouseEvent mouseEvent;
    mouseEvent = *pMouseData;
    mouseEvent.id = 0x400500C8;

    CSimpleTop::m_eventTime = pMouseData->time;

    // TODO
    // bool v4 = top->m_layout.enabled;
    bool enabled = false;

    memcpy(&top->m_mousePosition, pMouseData, sizeof(top->m_mousePosition));

    if (enabled && (EventIsKeyDown(KEY_LCONTROL) || EventIsKeyDown(KEY_LALT))) {
        int32_t v5 = EventIsKeyDown(KEY_LCONTROL);
        top->StartMoveOrResizeFrame(mouseEvent, MOVERESIZE_REASON1, v5);

        return 0;
    }

    auto mouseButtonCallback = top->m_mouseButtonCallback;

    if (!top->m_mouseCapture && mouseButtonCallback && mouseButtonCallback(&mouseEvent)) {
        return 0;
    }

    CSimpleFrame* target = top->m_mouseCapture;

    if (!target) {
        target = top->m_mouseFocus;
    }

    if (!target) {
        return 1;
    }

    target->Raise();

    CSimpleTitleRegion* titleRegion = target->m_titleRegion;

    C2Vector pt = { mouseEvent.x, mouseEvent.y };

    if (titleRegion && titleRegion->PtInFrameRect(pt)) {
        top->StartMoveOrResizeFrame(target, MOVERESIZE_REASON2, mouseEvent.x, mouseEvent.y, 4);
    } else {
        top->m_mouseCapture = target;
        target->OnLayerMouseDown(mouseEvent, nullptr);
    }

    return 0;
}

// OFFSET: 0x4945A0
int32_t CSimpleTop::OnMouseMove(const EVENT_DATA_MOUSE* pMouseData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);

    CMouseEvent mouseEvent;
    mouseEvent = *pMouseData;
    mouseEvent.id = 0x400500CA;

    int32_t v18 = pMouseData != &top->m_mousePosition;

    if (v18) {
        if (pMouseData->x == top->m_mousePosition.x && pMouseData->y == top->m_mousePosition.y) {
            return 0;
        }

        CSimpleTop::m_eventTime = pMouseData->time;
    }

    memcpy(&top->m_mousePosition, pMouseData, sizeof(top->m_mousePosition));

    CSimpleFrame* lastFocus = top->m_mouseFocus;
    CSimpleFrame* nextFocus = nullptr;

    if (top->m_layout.frame) {
        CMouseEvent evt;
        evt = *pMouseData;

        top->MoveOrResizeFrame(evt);

        return 0;
    }

    auto mousePositionCallback = top->m_mousePositionCallback;

    if (mousePositionCallback && mousePositionCallback(&mouseEvent)) {
        return 0;
    }

    CSimpleFrame* mouseCapture = top->m_mouseCapture;

    if (mouseCapture) {
        mouseCapture->OnLayerTrackUpdate(mouseEvent);
    }

    for (int32_t strata = FRAME_STRATA_TOOLTIP; strata >= FRAME_STRATA_WORLD; strata--) {
        auto priorities = &top->m_eventqueue[strata][SIMPLE_EVENT_MOUSE];

        for (int32_t i = 0; i < priorities->Count(); i++) {
            auto priority = priorities->operator[](i);
            auto frame = priority->frame;

            if (frame->OnLayerTrackUpdate(mouseEvent)) {
                nextFocus = frame;
                break;
            }
        }

        if (nextFocus) {
            break;
        }
    }

    if (nextFocus != lastFocus) {
        top->m_mouseFocus = nextFocus;

        if (lastFocus) {
            lastFocus->OnLayerCursorExit(v18, 0);
        }

        if (nextFocus) {
            nextFocus->OnLayerCursorEnter(v18);
        }
    }

    return nextFocus == nullptr;
}

// OFFSET: 0x494730
int32_t CSimpleTop::OnMouseMoveRelative(const EVENT_DATA_MOUSE* pMouseData, void* param) {
    // TODO
    return 0;
}

// OFFSET: 0x4947A0
int32_t CSimpleTop::OnMouseUp(const EVENT_DATA_MOUSE* pMouseData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);

    CMouseEvent mouseEvent;
    mouseEvent = *pMouseData;
    mouseEvent.id = 0x400500C9;

    CSimpleTop::m_eventTime = pMouseData->time;

    if (top->m_layout.anchor == 1 || top->m_layout.anchor == 2) {
        if (top->m_layout.frame) {
            top->m_layout.frame->UnflattenFrame(top);
        }

        top->m_layout.frame = nullptr;
        top->m_layout.anchor = FRAMEPOINT_TOPLEFT;

        return 0;
    }

    // TODO
    // if (v11 && !top->m_mouseCapture && v11(mouseEvent)) {
    //     return 0;
    // }

    if (!top->m_mouseCapture) {
        return 1;
    }

    top->m_mouseCapture->OnLayerMouseUp(mouseEvent, nullptr);

    if (!mouseEvent.buttonState) {
        top->m_mouseCapture = nullptr;
    }

    return 0;
}

// OFFSET: 0x494890
int32_t CSimpleTop::OnMouseWheel(const EVENT_DATA_MOUSE* pMouseData, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);

    int32_t handled = 0;

    CMouseEvent mouseEvent;
    mouseEvent = *pMouseData;

    CSimpleTop::m_eventTime = pMouseData->time;

    mouseEvent.id = 0x400500CD;

    float ddcX = 0.0f;
    float ddcY = 0.0f;
    NDCToDDC(top->m_mousePosition.x, top->m_mousePosition.y, &ddcX, &ddcY);

    C2Vector pt = { ddcX, ddcY };

    for (int32_t strata = NUM_FRAME_STRATA - 1; strata >= 0; strata--) {
        if (handled) {
            break;
        }

        auto& queue = top->m_eventqueue[strata][SIMPLE_EVENT_MOUSEWHEEL];

        queue.m_iterator = 0;

        while (queue.m_iterator < queue.m_count) {
            auto framePriority = queue[queue.m_iterator];

            queue.m_iterator++;

            if (!framePriority || handled) {
                break;
            }

            auto frame = framePriority->frame;

            if (frame->TestHitRect(pt)) {
                handled = frame->OnLayerMouseWheel(mouseEvent);
            }
        }
    }

    return handled == 0;
}

// OFFSET: 0x493E00
float CSimpleTop::RoundToPixelHeight(float ddcHeight) {
    if (abs(ddcHeight) < 0.00000023841858) {
        return ddcHeight;
    }

    CRect windowRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(windowRect);
    float windowHeight = windowRect.maxY - windowRect.minY;

    if (abs(windowHeight) < 0.00000023841858) {
        return ddcHeight;
    }

    auto ndcHeight = DDCToNDCHeight(ddcHeight);
    auto pixelHeight = ndcHeight * windowHeight;
    auto roundedPixelHeight = CMath::fuint_pi(pixelHeight);
    auto roundedNdcHeight = static_cast<float>(roundedPixelHeight) / windowHeight;
    auto roundedDdcHeight = NDCToDDCHeight(roundedNdcHeight);

    return roundedDdcHeight;
}

CSimpleTop::CSimpleTop() : CLayoutFrame() {
    //this->m_mouseFocus = 0;
    //this->m_mouseCapture = 0;
    //this->m_frames.m_terminator.m_next = 0;
    //this->m_frames.m_terminator.m_prevlink = &this->m_frames.m_terminator;
    //this->m_frames.m_terminator.m_next = (CSimpleFrame*)((unsigned int)&this->m_frames.m_terminator | 1);
    //this->m_frames.m_linkoffset = 644;
    //this->m_destroyed.m_terminator.m_next = 0;
    //this->m_destroyed.m_terminator.m_prevlink = &this->m_destroyed.m_terminator;
    //this->m_destroyed.m_terminator.m_next = (CSimpleFrame*)((unsigned int)&this->m_destroyed.m_terminator | 1);
    //this->m_destroyed.m_linkoffset = 652;
    //*(float*)&this->ukn14 = 0.0;
    //*(float*)&this->ukn15 = 0.0;
    for (int32_t strata = FRAME_STRATA_WORLD; strata < NUM_FRAME_STRATA; strata++) {
        for (int32_t event = SIMPLE_EVENT_CHAR; event < NUM_SIMPLE_EVENTS; event++) {
            auto priorities = &this->m_eventqueue[strata][event];

            priorities->m_array.m_alloc = 0;
            priorities->m_array.m_count = 0;
            priorities->m_array.m_data = nullptr;
            priorities->m_array.m_chunk = 0;
            priorities->m_count = 0;
            priorities->m_maxcount = 0;
            priorities->m_iterator = 0;
        }
    }
    //this->ukn23 = 1;
    //this->ukn31 = 0;
    //this->mouseButtonHandler = 0;
    //this->mouseMoveHandler = 0;
    //this->displaySizeChangedHandler = 0;
    //this->focusChangedHandler = 0;
    //this->keyDownHandler = 0;
    //this->ukn37 = 1;
    //this->ukn38 = 0;
    //this->ukn39 = 0;

    CSimpleTop::s_instance = this;

    ScrnLayerCreate(nullptr, 1.0, 0x4, nullptr, &PaintScreen, &this->m_screenLayer);

    this->EnableEvents();

    // TODO
    // memset(this->m_keydownCapture, 0, sizeof(this->m_keydownCapture));

    NDCToDDC(1.0f, 1.0f, &this->m_rect.maxX, &this->m_rect.maxY);

    this->m_flags |= 0x1;

    //this->ukn10 = 0;
    //this->ukn11 = 0;
    //this->ukn12 = 0;
    //this->ukn13 = 0;
    //this->ukn14 = 0;
    //this->ukn15 = 0;

    for (int32_t s = 0; s < NUM_FRAME_STRATA; s++) {
        auto m = SMemAlloc(sizeof(CFrameStrata), __FILE__, __LINE__, 0x0);
        this->m_strata[s] = new (m) CFrameStrata();
    }

    //this->mouseEventData.mode = 0;
    //this->mouseEventData.button = 0;
    //this->mouseEventData.buttonState = 0;
    //this->mouseEventData.metaKeyState = 0;
    //this->mouseEventData.flags = 0;
    //this->mouseEventData.mousePosition.x = 0.0;
    //this->mouseEventData.mousePosition.y = 0.0;
    //this->mouseEventData.wheelAmount = 0;
    //this->mouseEventData.eventTime = 0;
    //EventInputGetMousePosition(&v7, &v8);
    //DDCToNDC(v7, v8, &this->mouseEventData.mousePosition.x, &this->mouseEventData.mousePosition.y);

    CSimpleRender::Init();
    CSimpleTexture::Init();
}

// OFFSET: 0x495060
void CSimpleTop::CompressStrata(int32_t strata) {
    // TODO
}

// OFFSET: 0x4959F0
void CSimpleTop::EnableEvents() {
    EventRegisterEx(EVENT_ID_CHAR, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnChar), this, 1.0);
    EventRegisterEx(EVENT_ID_IME, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnIme), this, 1.0);
    EventRegisterEx(EVENT_ID_KEYDOWN, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnKeyDown), this, 1.0);
    EventRegisterEx(EVENT_ID_KEYUP, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnKeyUp), this, 1.0);
    EventRegisterEx(EVENT_ID_KEYDOWN_REPEATING, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnKeyDownRepeat), this, 1.0);
    EventRegisterEx(EVENT_ID_MOUSEMOVE, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseMove), this, 1.0);
    EventRegisterEx(EVENT_ID_MOUSEMOVE_RELATIVE, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseMoveRelative), this, 1.0);
    EventRegisterEx(EVENT_ID_MOUSEDOWN, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseDown), this, 1.0);
    EventRegisterEx(EVENT_ID_MOUSEUP, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseUp), this, 1.0);
    EventRegisterEx(EVENT_ID_MOUSEWHEEL, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseWheel), this, 1.0);
    EventRegisterEx(EVENT_ID_SIZE, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnDisplaySizeChanged), this, 1.0);
    EventRegisterEx(EVENT_ID_FOCUS, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnFocusChanged), this, 1.0);
}

// OFFSET: 0x495B00
void CSimpleTop::DisableEvents() {
    EventUnregisterEx(EVENT_ID_CHAR, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnChar), this, -1);
    EventUnregisterEx(EVENT_ID_IME, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnIme), this, -1);
    EventUnregisterEx(EVENT_ID_KEYDOWN, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnKeyDown), this, -1);
    EventUnregisterEx(EVENT_ID_KEYUP, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnKeyUp), this, -1);
    EventUnregisterEx(EVENT_ID_KEYDOWN_REPEATING, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnKeyDownRepeat), this, -1);
    EventUnregisterEx(EVENT_ID_MOUSEMOVE, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseMove), this, -1);
    EventUnregisterEx(EVENT_ID_MOUSEMOVE_RELATIVE, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseMoveRelative), this, -1);
    EventUnregisterEx(EVENT_ID_MOUSEDOWN, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseDown), this, -1);
    EventUnregisterEx(EVENT_ID_MOUSEUP, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseUp), this, -1);
    EventUnregisterEx(EVENT_ID_MOUSEWHEEL, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnMouseWheel), this, -1);
    EventUnregisterEx(EVENT_ID_SIZE, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnDisplaySizeChanged), this, -1);
    EventUnregisterEx(EVENT_ID_FOCUS, reinterpret_cast<EVENTHANDLERFUNC>(CSimpleTop::OnFocusChanged), this, -1);
}

// OFFSET: 0x493F30
void CSimpleTop::HideFrame(CSimpleFrame* frame, int32_t a4) {
    if (this->m_layout.frame == frame && !a4) {
        if (this->m_layout.frame) {
            this->m_layout.frame->UnflattenFrame(this);
        }

        this->m_layout.frame = nullptr;
        this->m_layout.anchor = FRAMEPOINT_TOPLEFT;
    }

    frame->UnregisterForEvents(a4);
    this->m_strata[frame->m_strata]->RemoveFrame(frame);
}

// OFFSET: 0x495320
void CSimpleTop::OnLayerUpdate(float elapsedSec) {
    // TODO
    // - walk m_destroyed and perform some cleanup

    CLayoutFrame::ResizePending();

    for (int32_t s = 0; s < NUM_FRAME_STRATA; s++) {
        auto strata = this->m_strata[s];
        strata->OnLayerUpdate(elapsedSec);
    }

    CLayoutFrame::ResizePending();

    if (this->m_checkFocus) {
        this->m_checkFocus = 0;
        CSimpleTop::OnMouseMove(&this->m_mousePosition, this);
    }
}

// OFFSET: 0x495410
void CSimpleTop::OnLayerRender() {
    C2Vector v13 = { 0.0f, 0.0f };
    CameraSetupScreenProjection(this->m_rect, v13, 0.0f, 0);

    for (int32_t s = 0; s < NUM_FRAME_STRATA; s++) {
        auto strata = this->m_strata[s];
        strata->BuildBatches(this->m_mouseCapture == nullptr);
        strata->RenderBatches();
    }
}

// OFFSET: 0x493CE0
void CSimpleTop::MoveOrResizeFrame(const CMouseEvent& evt) {
    // TODO
}

// OFFSET: 0x493C10
void CSimpleTop::NotifyFrameLayerChanged(CSimpleFrame* frame, uint32_t layer) {
    auto strata = this->m_strata[frame->m_strata];
    auto level = strata->levels[frame->m_level];
    level->batchDirty |= 1 << layer;
    strata->batchDirty = 1;
}

// OFFSET: 0x495840
void CSimpleTop::NotifyFrameMovedOrResized(CSimpleFrame* frame) {
    auto strata = this->m_strata[frame->m_strata];
    strata->levelsDirty = 1;

    if (this->m_layout.frame) {
        this->RaiseFrame(this->m_layout.frame, 0);
    }

    this->m_checkFocus = 1;
}

// OFFSET: 0x4951A0
int32_t CSimpleTop::RaiseFrame(CSimpleFrame* frame, int32_t checkOcclusion) {
    if (!(frame->m_flags & 0x01)) {
        frame = frame->m_parent;

        if (!frame) {
            return 0;
        }

        while (!(frame->m_flags & 0x01)) {
            frame = frame->m_parent;

            if (!frame) {
                return 0;
            }
        }
    }

    if (checkOcclusion) {
        auto layout = static_cast<CLayoutFrame*>(frame);

        if (!(layout->m_flags & 0x1) || layout->IsResizePending()) {
            frame->Resize(1);
        }

        int32_t occluded = this->m_strata[frame->m_strata]->FrameOccluded(frame);
        frame->SetFrameFlag(0x10, occluded);
    }

    if (frame->m_flags & 0x10) {
        this->CompressStrata(frame->m_strata);
        auto strata = this->m_strata[frame->m_strata];
        frame->SetFrameLevel(strata->topLevel, 1);
        this->m_checkFocus = 1;
    }

    return 1;
}

// OFFSET: 0x495880
void CSimpleTop::RegisterForEvent(CSimpleFrame* frame, CSimpleEventType event, int32_t a4, uint32_t priority) {
    auto& queue = this->m_eventqueue[frame->m_strata][event];

    auto m = SMemAlloc(sizeof(FRAMEPRIORITY), __FILE__, __LINE__, 0x0);
    auto framePriority = new (m) FRAMEPRIORITY();

    framePriority->frame = frame;
    framePriority->priority = priority == -1 ? frame->m_level : priority;

    queue.Insert(framePriority);

    if (!a4 && event == SIMPLE_EVENT_MOUSE) {
        this->m_checkFocus = 1;
    }
}

// OFFSET: 0x494F90
void CSimpleTop::RegisterFrame(CSimpleFrame* frame) {
    this->m_frames.LinkToTail(frame);
}

// OFFSET: 0x493C80
void CSimpleTop::SetCursor(MipBits* image) {
    if (image) {
        auto cursor = g_theGxDevicePtr->CursorLock();
        memcpy(cursor, image->mip[0], CURSOR_IMAGE_BYTES);
        g_theGxDevicePtr->CursorUnlock(0, 0);
        g_theGxDevicePtr->CursorSetVisible(1);
    }
}

// OFFSET: 0x4960D0
void CSimpleTop::ShowFrame(CSimpleFrame* frame, int32_t a3) {
    this->m_strata[frame->m_strata]->AddFrame(frame);
    frame->RegisterForEvents(a3);
}

int32_t CSimpleTop::StartMoveOrResizeFrame(const CMouseEvent& start, MOVERESIZE_REASON reason, int32_t resize) {
    // TODO
    return 0;
}

int32_t CSimpleTop::StartMoveOrResizeFrame(CSimpleFrame* frame, MOVERESIZE_REASON reason, float startx, float starty, int32_t a6) {
    // TODO
    return 0;
}

int32_t CSimpleTop::StartMoveOrResizeFrame(CSimpleFrame* frame, MOVERESIZE_REASON reason, float startx, float starty, FRAMEPOINT a6) {
    // TODO
    return 0;
}

// OFFSET: 0x495900
void CSimpleTop::UnregisterForEvent(CSimpleFrame* frame, CSimpleEventType event, int32_t a4) {
    auto& queue = this->m_eventqueue[frame->m_strata][event];

    int32_t found = 0;

    for (int32_t i = 0; i < queue.Count(); i++) {
        if (queue[i]->frame == frame) {
            auto framePriority = queue[i];

            queue.Remove(i);
            SMemFree(framePriority, __FILE__, __LINE__, 0x0);
            found = 1;

            break;
        }
    }

    if (!found) {
        return;
    }

    if (!a4) {
        if (event == SIMPLE_EVENT_KEY) {
            for (int32_t i = 0; i < 787; i++) {
                if (frame == this->m_keydownCapture[i]) {
                    this->m_keydownCapture[i] = nullptr;
                }
            }
        } else if (event == SIMPLE_EVENT_MOUSE) {
            if (frame == this->m_mouseCapture) {
                this->m_mouseCapture = nullptr;
            }

            if (frame == this->m_mouseFocus) {
                this->m_mouseFocus = nullptr;
                this->m_checkFocus = 1;

                if (frame->m_intAC != 3) {
                    frame->OnLayerCursorExit(0, 1);
                }
            }
        }
    }
}

// OFFSET: 0x494FA0
void CSimpleTop::UnregisterFrame(CSimpleFrame* frame) {
    this->HideFrame(frame, 0);

    auto framesLink = this->m_frames.Link(frame);
    if (framesLink) {
        framesLink->Unlink();
    }
}
