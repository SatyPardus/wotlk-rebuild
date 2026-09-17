#ifndef GAME_UI_CGGAMEUI_HPP
#define GAME_UI_CGGAMEUI_HPP


#include "ui/CSimpleTop.hpp"
#include "ui/CSimpleFrame.hpp"
#include "clientobject/WGUID.hpp"
#include "gameui/Types.hpp"

class CGGameUI {
    public:
        static void InitializeGame();
        static void Initialize();
        static void RegisterGameCVars();
        static void InitClientControlState(WGUID guid);
        static void RegisterFrameFactories();
        static void EnterWorld();
        static void Reload();
        static int32_t HandleDisplaySizeChanged(const CSizeEvent& event);
        static bool CanPerformAction(int32_t action);
        static void ClearCursor(bool a1, bool a2);
        static int32_t HandleMouseDown(const CMouseEvent& evt);
        static void OnMouseModeRelative();
        static void OnMouseModeNormal();
        static int32_t FilterMouseMotion(CMouseEvent* evt);
        static int32_t FilterMouseButton(CMouseEvent* evt);
        static void UnitNameUpdate(WGUID guid);
        static void HandleWorldClick(CWorldClickEvent* evt);
        static void HandleTerrainClick(CTerrainClickEvent* evt);
        static void HandleSpriteClick(CSpriteClickEvent* evt);
        static void OnSpriteLeftClick(WGUID guid);
        static void OnSpriteRightClick(WGUID guid);
        static void Target(WGUID guid);
        static void ClearTarget(WGUID guid, bool a2);
        static bool IsRaidMemberOrPet(WGUID guid);
        static bool IsPartyMember(WGUID guid);

    public:
        static CSimpleTop* m_simpleTop;
        static CSimpleFrame* m_UISimpleParent;
        static int32_t m_reloadUIRequested;
        static bool m_currentlyReloadingUI;
        static bool m_inWorld;
        static bool m_loggingIn;
        static int32_t m_hasControl;
        static int32_t m_screenWidth;
        static int32_t m_screenHeight;
        static float m_aspect;
        static char* m_luaTainted;
        static int32_t m_cursorMoney;
        static int32_t m_cursorItemType;
        static char* m_subZoneText;
        static int32_t m_cursorVirtualID;
        static WGUID m_lockedTarget;
        static WGUID m_lastTarget;
        static WGUID m_interactTarget;
        static WGUID m_currentObjectTrack;
        static WGUID m_focusTarget;
        static uint32_t m_areaID;
};

#endif // GAME_UI_CGGAMEUI_HPP
