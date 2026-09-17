#include "gameui/CGGameUI.hpp"

#include <common/MD5.hpp>
#include <storm/Log.hpp>

#include "client/Client.hpp"
#include "gameui/GameScriptFunctions.hpp"
#include "gameui/CGWorldFrame.hpp"
#include "gameui/CGTooltip.hpp"
#include "gameui/CGCooldown.hpp"
#include "gameui/CGMinimapFrame.hpp"
#include "gameui/CGCharacterModelBase.hpp"
#include "gameui/CGDressUpModelFrame.hpp"
#include "gameui/CGTabardModelFrame.hpp"
#include "gameui/CGQuestPOIFrame.hpp"
#include "gameui/CGUIBindings.hpp"
#include "gx/Coordinate.hpp"
#include "gx/Device.hpp"
#include "ui/FrameScript.hpp"
#include "ui/FrameXML.hpp"
#include "glue/CGlueMgr.hpp"
#include "util/CStatus.hpp"
#include "util/SysMessage.hpp"
#include "util/SFile.hpp"
#include <util/Unimplemented.hpp>
#include "clientobject/Unit_C.hpp"
#include "CGInputControl.hpp"
#include <event/Input.hpp>
#include <clientobject/ObjectMgrClient.hpp>


CSimpleTop* CGGameUI::m_simpleTop = nullptr;
CSimpleFrame* CGGameUI::m_UISimpleParent = nullptr;
int32_t CGGameUI::m_reloadUIRequested = 0;
bool CGGameUI::m_currentlyReloadingUI = false;
bool CGGameUI::m_inWorld = false;
bool CGGameUI::m_loggingIn = false;
int32_t CGGameUI::m_hasControl = 0;
int32_t CGGameUI::m_screenWidth = 0;
int32_t CGGameUI::m_screenHeight = 0;
float CGGameUI::m_aspect = 0.0;
char* CGGameUI::m_luaTainted = nullptr;
int32_t CGGameUI::m_cursorMoney = 0;
int32_t CGGameUI::m_cursorItemType = 0;
char* CGGameUI::m_subZoneText = nullptr;
int32_t CGGameUI::m_cursorVirtualID = 0;
WGUID CGGameUI::m_lockedTarget = 0;
WGUID CGGameUI::m_lastTarget = 0;
WGUID CGGameUI::m_interactTarget = 0;
WGUID CGGameUI::m_currentObjectTrack = 0;
WGUID CGGameUI::m_focusTarget = 0;
uint32_t CGGameUI::m_areaID = 0;

void CGGameUI::InitializeGame() {
    // TODO

    CGGameUI::m_hasControl = 1;
    CGGameUI::Initialize();
}

void CGGameUI::Initialize() {
    // TODO:
    // sub_4CFB80();
    // s_taintLogCVar =
    // s_scriptProfileCVar =

    CGGameUI::m_loggingIn = 1;
    CGGameUI::m_reloadUIRequested = 0;
    //CGGameUI::m_repopTime = 0;
    //CGGameUI::m_deadNoRepopTimer = 0;
    //CGGameUI::m_corpseReclaimDelay = 0;
    //CGGameUI::m_instanceBootTime = 0;
    //CGGameUI::m_instanceLockTime = 0;
    //CGGameUI::m_instanceLockComletedMask = 0;
    //CGGameUI::m_instanceLockExtending = 0;
    //CGGameUI::m_summonConfirmTime = 0;

    CRect screenRect;
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(screenRect);

    CGGameUI::m_screenWidth = 0;
    CGGameUI::m_screenHeight = 0;

    CGGameUI::m_aspect = CalculateAspectRatio();
    CoordinateSetAspectRatio(CGGameUI::m_aspect);

    auto m = SMemAlloc(sizeof(CSimpleTop), __FILE__, __LINE__, 0x0);
    CGGameUI::m_simpleTop = new (m) CSimpleTop();
    CGGameUI::m_simpleTop->m_mouseButtonCallback = &CGGameUI::FilterMouseButton;
    CGGameUI::m_simpleTop->m_mouseMoveCallback = &CGGameUI::FilterMouseMotion;
    CGGameUI::m_simpleTop->m_displaySizeCallback = &CGGameUI::HandleDisplaySizeChanged;
    //CGGameUI::m_simpleTop->dword1244 = (uint32_t)CGGameUI::HandleFocusChanged;
    //CGGameUI::m_simpleTop->dword124C = 1;
    //CGGameUI::m_simpleTop->dword1254 = (uint32_t)CGGameUI::ShowBlockedFrameFeedback;

    CursorInitialize();
    auto activeInputControl = CGInputControl::GetActive();
    STORM_ASSERT(activeInputControl);
    activeInputControl->m_mouseModeFlags = 3;
    activeInputControl->m_forceCursorOn = 0;
    activeInputControl->UpdateMouseMode(1);
    
    FrameScript_Flush();
    LoadScriptFunctions();
    FrameScript_CreateEvents(g_scriptEvents, 722);
    CGGameUI::RegisterGameCVars();

    CGUIBindings::Initialize();
    CGGameUI::RegisterFrameFactories();

    // STORM_ASSERT(GetDataInterfaceVersion() == GetCodeInterfaceVersion())

    CWOWClientStatus status;

    // OsCreateDirectory("Logs", 0);
    if (!SLogCreate("Logs\\FrameXML.log", 0, &status.m_logFile)) {
        SysMsgPrintf(SYSMSG_WARNING, "Cannot create WOWClient log file \"%s\"!", "Logs\\FrameXML.log");
    }

    //LoadAddOnEnableState(ClientServices::GetCharacterName());

    MD5_CTX md5;
    MD5Init(&md5);

    unsigned char digest1[16];

    switch (FrameXML_CheckSignature("Interface\\FrameXML\\FrameXML.toc", "Interface\\FrameXML\\Bindings.xml", nullptr, digest1)) {
    case 0:
        status.Add(STATUS_WARNING, "FrameXML missing signature");
        ClientPostClose(10);
        break;
    case 1:
        status.Add(STATUS_WARNING, "FrameXML has corrupt signature");
        ClientPostClose(10);
        break;
    case 2:
        status.Add(STATUS_WARNING, "FrameXML is modified or corrupt");
        ClientPostClose(10);
        break;

    case 3:
        break;

    default:
        ClientPostClose(10);
        break;
    }

    int32_t numFiles = 0;
    char* data = nullptr;
    if (SFile::Load(nullptr, "Interface\\FrameXML\\FrameXML.toc", (void**)&data, nullptr, 1, 1, nullptr)) {
        numFiles = FrameXML_GuessNumFiles(data);
        SMemFree(data);
    }

    //numFiles += LoadAddOnFileCount();

    //FrameXML_RegisterLoadProgressCallback(&LoadingScreenXMLCallback, 0, numFiles);
    FrameXML_FreeHashNodes();
    FrameXML_CreateFrames("Interface\\FrameXML\\FrameXML.toc", 0, &md5, &status);

    if (SFile::FileExistsEx("Interface\\FrameXML\\Bindings.xml", 1)) {
        CGUIBindings::s_bindings->Load("Interface\\FrameXML\\Bindings.xml", &md5, &status);
    }

    unsigned char digest2[16];
    MD5Final(digest2, &md5);
    if (memcmp(digest1, digest2, 16)) {
        // DO NOTHING FOR NOW
        //ClientPostClose(10);
    }

    int32_t objectType = CSimpleFrame::GetObjectType();
    CGGameUI::m_UISimpleParent = (CSimpleFrame*) CScriptObject::GetScriptObjectByName("UIParent", objectType);
    STORM_ASSERT(CGGameUI::m_UISimpleParent);

    // TODO: CGGameUI::m_gameTooltip = ...

    //LoadAddOns((int)v23);
    //FrameXML_RegisterLoadProgressCallback(0, 0, 0);
    //CGGameUI::RegisterUIScaleCVars();
    //CGGameUI::SomeSavedAccountVariables((int)v23);
    //LoadAccountData(6, (int(__cdecl*)(int, int, int))sub_5183A0, 0);
    //CGGameUI::UpdateInitCounter();
    CGUIBindings::LoadBindings();
    //CGUIMacros::Initialize();
    //CGUIMacros::LoadMacros();
    //CGChat::LoadChatSettings();
    //CSimpleScriptManager::Create();

    CSizeEvent sizeEvent;
    sizeEvent.w = screenRect.maxX - screenRect.minX;
    sizeEvent.h = screenRect.maxY - screenRect.minY;

    CGGameUI::HandleDisplaySizeChanged(sizeEvent);
    //CGGameUI::m_initialized = 1;
    //EventRegister((LPCGUID)6, (PENABLECALLBACK)CGGameUI::Idle, v11, v12);

    //TODO: GxDeviceCallbacks

    if (ClntObjMgrGetActivePlayer()) {
        CGGameUI::EnterWorld();
    }
    //sub_4CFB90();
    //CGGameUI::m_currentlyReloadingUI = 0;
}

// OFFSET: 0x513880
void CGGameUI::InitClientControlState(WGUID guid) {
    CGGameUI::m_hasControl = true;
    CGUnit_C::InitActiveMover(guid);
}

void CGGameUI::RegisterFrameFactories() {
    FrameXML_RegisterFactory("WorldFrame", CGWorldFrame::Create, 1);
    FrameXML_RegisterFactory("GameTooltip", CGTooltip::Create, 0);
    FrameXML_RegisterFactory("Cooldown", CGCooldown::Create, 0);
    FrameXML_RegisterFactory("Minimap", CGMinimapFrame::Create, 0);
    FrameXML_RegisterFactory("PlayerModel", CGCharacterModelBase::Create, 0);
    FrameXML_RegisterFactory("DressUpModel", CGDressUpModelFrame::Create, 0);
    FrameXML_RegisterFactory("TabardModel", CGTabardModelFrame::Create, 0);
    FrameXML_RegisterFactory("QuestPOIFrame", CGQuestPOIFrame::Create, 0);
}

// OFFSET: 0x528010
void CGGameUI::EnterWorld() {
    if (CGGameUI::m_inWorld)
        return;

    CGGameUI::m_inWorld = true;
    //v15 = v1;
    //v14 = v0;
    //CGChat::LeaveWorld();
    auto player = ClntObjMgrGetActivePlayerObj();
    if (!player)
        SErrDisplayAppFatal("");
    auto playerModel = player->GetObjectModel();
    if (!playerModel)
        SErrDisplayAppFatal("");

    playerModel->IsLoaded(1, 1);
    if (player->m_characterComponent) {
        player->m_characterComponent->RenderPrep(1);
        //    if ((m_characterComponent->dword8 & 0x40) != 0)
        //        maybe_CGPlayer_C__TurnOffGuildTabardPurchase(v3);
    }
    //if (CGPlayer_C::GetFarSightGuid(v3) && (v3->unk_1854[1] & 1) != 0) {
    //    FarSightGuid = CGPlayer_C::GetFarSightGuid(v3);
    //    maybe_CGGameUI__ResetCamera(FarSightGuid);
    //} else {
    //    maybe_CGGameUI__ResetCamera(0i64);
    //}
    //Active = CGInputControl::GetActive();
    //CGInputControl::EnterWorld(Active);
    //bn_FrameScript_MemoryCleanup();
    //if (!CGGameUI::m_currentlyReloadingUI) {
    //    dword_BD0B08 = 0;
    //    dword_BD0B0C = 0;
    //    dword_BD0B10 = 0;
    //    dword_BD0B14 = 0;
    //    dword_BD0B18 = 0;
    //    dword_BD0B1C = 0;
    //    dword_BD0B28 = 0;
    //    dword_BD0B2C = 0;
    //    dword_BD0B30 = 0;
    //    dword_BD0B34 = 0;
    //    dword_BD0B38 = 0;
    //    dword_BD0B3C = 0;
    //    dword_BD0B48 = 0;
    //    dword_BD0B4C = 0;
    //    dword_BD0B50 = 0;
    //    dword_BD0B54 = 0;
    //    dword_BD0B58 = 0;
    //    dword_BD0B5C = 0;
    //    dword_BD0B68 = 0;
    //    dword_BD0B6C = 0;
    //    dword_BD0B70 = 0;
    //    dword_BD0B74 = 0;
    //    dword_BD0B78 = 0;
    //    dword_BD0B7C = 0;
    //}
    //qword_BD08B0 = 0i64;
    //dword_BD08A4 = 0;
    if (CGGameUI::m_loggingIn) {
        CGGameUI::m_loggingIn = 0;
        FrameScript_SignalEvent(EVENT_PLAYER_LOGIN, nullptr);
        //CGLCD::Login(v14, v15);
    }
    FrameScript_SignalEvent(EVENT_PLAYER_ENTERING_WORLD, nullptr);
    //maybe_CGMinimapFrame__EnterWorld();
    //bn_CGChat_EnterWorld(v8);
    //bn_CGActionBar_EnterWorld();
    //bn_CGCharacterInfo_EnterWorld();
    //NOP(v14);
    //maybe_CGContainerInfo__EnterWorld();
    //NOP(v15);
    //NOP(v16);
    //CGQuestInfo::EnterWorld();
    //CGQuestLog::EnterWorld();
    //NOP(v17.vTable);
    //CGClassTrainer::EnterWorld();
    //bn_CGMerchantInfo_EnterWorld();
    //bn_CGTradeInfo_EnterWorld();
    //bn_CGBankInfo_EnterWorld();
    //NOP(v17.m_buffer);
    //SendRequestPetInfo();
    //bn_CGWorldMap_EnterWorld();
    //maybe_CGReputationInfo__OnSetFactionStanding();
    //NOP(v17.m_base);
    //bn_CGTabardCreationFrame_EnterWorld();
    //bn_CGGuildRegistrar_EnterWorld();
    //NOP(v17.m_alloc);
    //bn_CGPetitionInfo_EnterWorld();
    //j_Player__SignalSkillStatChanges();
    //maybe_CGMailInfo__EnterWorld();
    //bn_CGBattlefieldInfo_EnterWorld();
    //maybe_CGGameUI__ResetCamera_0();
    //bn_CGTalentInfo_Inspect_EnterWorld();
    //bn_CGAuctionHouse_EnterWorld();
    //bn_CGStableInfo_EnterWorld();
    //bn_CGRaidInfo_EnterWorld();
    //NOP(v17.m_size);
    //bn_CGItemSocketInfo_EnterWorld();
    //sub_5A1F50();
    //bn_CGLCD_EnterWorld();
    //NOP(v17.m_read);
    //bn_CGLookingForGroup_EnterWorld();
    //bn_CGArenaTeamInfo_EnterWorld();
    //dword_AC8AEC = OsGetAsyncTimeMs();
    //bn_CGGuildBankInfo_EnterWorld();
    //maybe_CGPetInfo__EnterWorld();
    //CGCalendar::EnterWorld();
    //NOP(v18.vTable);
    //bn_CGBarberShop_EnterWorld();
    //bn_CGRuneInfo_InitializeGame();
    //bn_CGCurrencyTypes_EnterWorld();
    //bn_CVehicle_C_EnterWorld();
    //bn_CGEquipmentManager_EnterWorld();
    //bn_CGGMTicketInfo_EnterWorld();
    //NOP(v18.m_buffer);
    //bn_CGInstanceEncounter_C_EnterWorld();
    //v9 = CGGameUI::m_lockedTarget;
    //v20 = SHIDWORD(CGGameUI::m_lockedTarget);
    //if (CGGameUI::m_lockedTarget) {
    //    memset(&v17.m_buffer, 0, 16);
    //    CDataStore::PutInt32(&v17, CMSG_SET_SELECTION);
    //    CDataStore::PutInt64(&v17, v9, v20);
    //    v17.m_read = 0;
    //    ClientServices::Send2(&v17);
    //    v17.vTable = bnl_CDataStore__v_table;
    //    if (v17.m_alloc != -1)
    //        CDataStore::InternalDestroy(&v17.m_buffer, &v17.m_base, &v17.m_alloc);
    //}
    //if ((v3->m_unit->UNIT_FIELD_FLAGS & 0x80000) != 0) {
    //    FrameScript::SignalEvent(EVENT_PLAYER_REGEN_DISABLED, 0);
    //    CGGameUI::m_simpleTop->ukn37 = 0;
    //    maybe_CGGameUI__UpdateCombatMode(v18.m_base);
    //}
    //maybe_CGGameUI__UpdateCorpseLocation();
    //v19[0] = 0.0;
    //v19[1] = 0.0;
    //*&v20 = 0.0;
    //bn_CGGameUI_SetDeathReleaseLocation(-1, v19);
    //CGTutorial::TriggerTutorial(0x29u);
    //CGTutorial::TriggerTutorial(1u);
    //CGTutorial::TriggerTutorial(2u);
    //CGTutorial::TriggerTutorial(0);
    //maybe_CGGameUI__ApplyThreatWarningSetting(*(bnl_g_threatWarningCVar + 48));
    //CGChat::Resume();
    //if (CGGameUI::m_inCinematic)
    //    EnableFadingScreen(0.25, maybe_CGGameUI__BeginCinematicInternal, 0);
    //if (CGGameUI::m_iCurrentMapID >= g_MapDB.minIndex && CGGameUI::m_iCurrentMapID <= g_MapDB.maxIndex) {
    //    v10 = g_MapDB.Rows[CGGameUI::m_iCurrentMapID - g_MapDB.minIndex];
    //    if (v10) {
    //        v11 = *(v10 + 8);
    //        if (v11 == 3 || v11 == 4)
    //            FrameScript::SignalEvent(EVENT_PLAYER_ENTERING_BATTLEGROUND, 0);
    //    }
    //}
    //if (v3->m_unit->UNIT_FIELD_HEALTH <= 0)
    //    CGPlayer_C::HandleRepopRequest(v3, 1);
    //if (dword_D37CF8)
    //    v12 = *(dword_D37CF8 + 48);
    //else
    //    v12 = 0;
    //if (dword_D37CFC)
    //    v13 = *(dword_D37CFC + 48);
    //else
    //    v13 = 0;
    //memset(&v18.m_base, 0, 12);
    //v18.m_read = -1;
    //CDataStore::PutInt32(&v18, 0);
    //CDataStore::PutInt8(&v18, v12 != 0);
    //CDataStore::PutInt8(&v18, v13 != 0);
    //v18.m_read = 0;
    //ClientServices::Send2(&v18);
    //v18.vTable = bnl_CDataStore__v_table;
    //if (v18.m_alloc != -1)
    //    CDataStore::InternalDestroy(&v18.m_buffer, &v18.m_base, &v18.m_alloc);
}

void CGGameUI::Reload() {
    if (CGGameUI::m_luaTainted && CGGameUI::m_simpleTop /* && !CGGameUI__m_simpleTop->dword1250 */) {
        // TODO: CGGameUI::ShowBlockedActionFeedback
    } else {
        CGGameUI::m_reloadUIRequested = 1;
    }
}

int32_t CGGameUI::HandleDisplaySizeChanged(const CSizeEvent& event) {
    return 0;
}

bool CGGameUI::CanPerformAction(int32_t action) {
    // TODO
    return true;
}

// OFFSET: 0x519280
void CGGameUI::ClearCursor(bool a1, bool a2) {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x51FB00
int32_t CGGameUI::HandleMouseDown(const CMouseEvent& evt) {
    if (evt.button == MOUSE_BUTTON_RIGHT) {
        // if (Spell_C_IsTargeting()) {
        //     if (Spell_C_IsCursorWorldObjectHousing()) {
        //         Spell_C_CursorWorldObjectRotate();
        //     } else {
        //         Spell_C_StopTargeting();
        //     }
        // }

        // if (CGPlayer_C::IsGiftWrapping()) {
        //     CGGameUI::ClearCursor(1, 1);
        // }

        // if (CursorGetResetMode() == 17) {
        //     CursorSetResetMode(1);
        //     CursorSetMode(1);
        // }
    }

    return 0;
}

// OFFSET: 0x512D60
void CGGameUI::OnMouseModeRelative() {
    if (!CGWorldFrame::s_currentWorldFrame || !CSimpleTop::s_instance)
        SErrDisplayAppFatal("");

    CGWorldFrame::s_currentWorldFrame->OnMouseModeRelative();
    CSimpleTop::s_instance->SetMouseFocus(CGWorldFrame::s_currentWorldFrame);

    g_theGxDevicePtr->CursorSetVisible(0);

    EventSetMouseMode(MOUSE_MODE_RELATIVE, 0);
}

// OFFSET: 0x512DC0
void CGGameUI::OnMouseModeNormal() {
    if (!CGWorldFrame::s_currentWorldFrame || !CSimpleTop::s_instance)
        SErrDisplayAppFatal("");

    CGWorldFrame::s_currentWorldFrame->OnMouseModeNormal();
    CSimpleTop::s_instance->m_checkFocus = 1;

    if ((CGInputControl::GetActive()->m_mouseModeFlags & 0x1) != 0)
        g_theGxDevicePtr->CursorSetVisible(1);

    EventSetMouseMode(MOUSE_MODE_NORMAL, 0);
}

// OFFSET: 0x512CD0
int32_t CGGameUI::FilterMouseMotion(CMouseEvent* evt) {
    if (evt->id != 0x400500CB)
        return 0;

    CGInputControl::GetActive()->OnMouseMoveRel(evt);
    return 1;
}

// OFFSET: 0x51FA50
int32_t CGGameUI::FilterMouseButton(CMouseEvent* evt) {
    //if (evt->id == 0x400500C8 && evt->button == MOUSE_BUTTON_RIGHT
    //    && (CGGameUI::m_cursorItem
    //        || CGGameUI::m_cursorVirtualID
    //        || CGGameUI::m_cursorMoney
    //        || CGGameUI::m_cursorSpell
    //        || CGGameUI::m_cursorPetAction
    //        || CGGameUI::m_cursorMacro
    //        || CGGameUI::m_cursorPet
    //        || CGPlayer_C::IsGiftWrapping())) {
    //    CGGameUI::ClearCursor(1, 1);
    //    return 1;
    //}

    if (evt->mode != MOUSE_MODE_RELATIVE) {
        return 0;
    }

    if (evt->id == 0x400500C8) {
        CGWorldFrame::s_currentWorldFrame->OnLayerMouseDown(*evt, nullptr);
    } else {
        CGWorldFrame::s_currentWorldFrame->OnLayerMouseUp(*evt, nullptr);
    }

    return 1;
}

// OFFSET: 0x512B00
void CGGameUI::UnitNameUpdate(WGUID guid) {
    auto unit = ClntObjMgrObjectPtr<CGUnit_C*>(guid, TYPEMASK_UNIT);
    if (unit) {
        //bn_CGGameUI_UnitTooltipUpdate(a1);
        unit->UpdateUnitNameText();
    }
    //Script_SendUnitSignal(a1, 144);
}

// OFFSET: 0x5278C0
void CGGameUI::HandleWorldClick(CWorldClickEvent* evt) {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x527830
void CGGameUI::HandleTerrainClick(CTerrainClickEvent* evt) {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x527870
void CGGameUI::HandleSpriteClick(CSpriteClickEvent* evt) {
    if (CGGameUI::m_cursorVirtualID)
        CGGameUI::ClearCursor(1, 1);
    if (evt->button == MOUSE_BUTTON_LEFT)
        CGGameUI::OnSpriteLeftClick(evt->guid);
    else
        CGGameUI::OnSpriteRightClick(evt->guid);
}

// OFFSET: 0x5274F0
void CGGameUI::OnSpriteLeftClick(WGUID guid) {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x5277B0
void CGGameUI::OnSpriteRightClick(WGUID guid) {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x524BF0
void CGGameUI::Target(WGUID guid) {
    if (guid == 0) {
        //dword_BD08CC = 0;
        if (CGGameUI::m_lockedTarget) {
            CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
            CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 1);
            //if (bn_Spell_C_GetAutoRepeatingSpell())
            //    Spell_C_CancelAutoRepeat(1);
        }
        return;
    }

    if (!ClntObjMgrGetCurrent() || CGGameUI::m_currentlyReloadingUI)
        SErrDisplayAppFatal("");

    //if (*lua_tainted) {
    //    CGGameUI::ShowBlockedActionFeedback(0, 0);
    //    return;
    //}

    //v16 = v3;
    //ActivePlayer = ClntObjMgrGetActivePlayer();
    //v5 = ClntObjMgrObjectPtr(ActivePlayer, TYPEMASK_PLAYER);
    //v6 = v5;
    //v18 = v5;
    //if (!v5 || !*&v5->m_unit->UNIT_FIELD_CHARMEDBY) {
    //    v7 = ClntObjMgrObjectPtr(a1, TYPEMASK_OBJECT);
    //    if (!v7) {
    //        if (a1 == CGGameUI::m_lockedTarget)
    //            return;
    //        CGameUI::PlaySound("igCharacterSelect", 0, 0, 0);
    //        goto LABEL_50;
    //    }
    //    if (Spell_C_IsTargeting()) {
    //        Spell_C_HandleSpriteClick(v7, 0);
    //        return;
    //    }
    //    if ((v7->ukn42)(v7) && a1 != CGGameUI::m_lockedTarget) {
    //        if (CGGameUI::m_visible || bnl_g_unitHighlightsCVar && *(bnl_g_unitHighlightsCVar + 48))
    //            bn_CGObject_C_ShowHighlightType(0);
    //        if ((v7->m_obj->m_type & 8) == 0)
    //            goto LABEL_50;
    //        PlayerNameTriggerColorUpdate(v7->m_nameDesc);
    //        CGUnit_C::RegisterScript(v7, 4096);
    //        WowClientDB::GetRow(v17);
    //        v8 = v7->dataA44[10];
    //        if (v8 && ClientDb::GetLocalizedRow(&g_spellDB, v8, v17) || (v9 = v7->dataA44[15]) != 0 && ClientDb::GetLocalizedRow(&g_spellDB, v9, v17))
    //            CGUnit_C::sub_7262E0(v7, v17);
    //        AsyncTimeMs = OsGetAsyncTimeMs();
    //        CGUnit_C::UpdateSpellCastBars(v7, AsyncTimeMs);
    //        if (v6) {
    //            if (CGUnit_C::CanAttack_0(v6, v7)) {
    //                bnl_CGGameUI__m_lastEnemyTarget = a1;
    //            } else if (CGUnit_C::CanAssist(v6, v7, 0)) {
    //                bnl_CGGameUI__m_lastFriendTraget = a1;
    //            }
    //            m_obj = v7->m_obj;
    //            if ((m_obj->m_type & 0x10) != 0) {
    //                guid_low = m_obj->m_guid.guid_low;
    //                guid_high = m_obj->m_guid.guid_high;
    //                if (__PAIR64__(guid_high, guid_low) != ClntObjMgrGetActivePlayer() && sub_522270(0x12u)) {
    //                    CGTutorial::TriggerTutorial(0x12u);
    //                    v6 = v18;
    //                    NOP(v16);
//LABEL_50:
    //                    v14 = v6 && maybe_CGUnit_C__IsInCombatOrMelee(v6);
    //                    CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
    //                    CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 0);
    //                    CGGameUI::m_lockedTarget = a1;
    //                    maybe_CGGameUI__SendTarget(a1.guid_low, a1.guid_high);
    //                    CGSpellBook::UpdateUsable();
    //                    FrameScript::SignalEvent(EVENT_PLAYER_TARGET_CHANGED, 0);
    //                    if (*(bnl_g_stopAutoAttackOnTargetChangeCVar + 48)) {
    //                        if (bn_Spell_C_GetAutoRepeatingSpell())
    //                            Spell_C_CancelAutoRepeat(1);
    //                    } else if (v14) {
    //                        CGPlayer_C::CombatModeEnter(v6, v15, &CGGameUI::m_lockedTarget, 0, 0);
    //                    }
    //                    return;
    //                }
    //                v6 = v18;
    //            }
    //            if ((v7->m_obj->m_type & 0x10) != 0 && maybe_CGUnit_C__CanCooperate(v6, v7) && v6->m_unit->UNIT_FIELD_LEVEL >= 5) {
    //                CGTutorial::TriggerTutorial(0x11u);
    //            } else if (CGUnit_C::CanAttack(v6, v7)) {
    //                CGTutorial::TriggerTutorial(4u);
    //            } else if ((v7->m_unit->UNIT_NPC_FLAGS & 0x2000) != 0) {
    //                CGTutorial::TriggerTutorial(0x22u);
    //            } else if (v7->ukn_0090 == 1) {
    //                CGTutorial::TriggerTutorial(0x2Au);
    //            }
    //        }
    //        NOP(v16);
    //        goto LABEL_50;
    //    }
    //}
}

// OFFSET: 0x5241B0
void CGGameUI::ClearTarget(WGUID guid, bool a2) {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x512A30
bool CGGameUI::IsRaidMemberOrPet(WGUID guid) {
    WHOA_UNIMPLEMENTED(false);
}

// OFFSET: 0x512A00
bool CGGameUI::IsPartyMember(WGUID guid) {
    WHOA_UNIMPLEMENTED(false);
}

