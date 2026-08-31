#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"
#include <gameui/CGInputControl.hpp>
#include <gameui/CGGameUI.hpp>
#include <util/StringTo.hpp>
#include "clientobject/Unit_C.hpp"
#include "clientobject/ObjectMgrClient.hpp"

// OFFSET: 0x5FBF80
static int32_t Script_JumpOrAscendStart(lua_State* L) {
    CGUnit_C* unit = ClntObjMgrObjectPtr<CGUnit_C*>(CGUnit_C::s_activeMover, TYPEMASK_UNIT);
    if (!unit)
        return 0;

    if (!CGGameUI::CanPerformAction(0))
        return 0;

    if (CGInputControl::s_inputControl->SetControlBit(0x2000, CSimpleTop::m_eventTime)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }

    if ((unit->m_passenger->m_flags & 0x2200000) == 0) {
        //CheckToCancelCurrentChannelSpell();
        if (unit->m_unit->UNIT_FIELD_HEALTH > 0
        //    && !unit->IsOnSpline()
        //    && !unit->IsAlteredFormTransitionPreventingMovement()
        //    && unit->sub_5FA9E0()
        //    && !unit->AnimSuppressesMovement()
           ) {

            //#####TESTING
            unit->movementData.AddPlayerMoveEvent(CSimpleTop::m_eventTime, 10, 1, 0, 0, 0, 0);
            //######
            WHOA_UNIMPLEMENTED(0);
        //
        //    if (unit->ukn78())
        //        unit->TryChangeStandState(0);
        //
        //    //auto player = ClntObjMgrGetActivePlayerObj();
        //    //if (player)
        //    //    player->ClearAFK(0);
        //
        //    if (!unit->GetCanFly()) {
        //        unit->TryJumpOrAscend(CSimpleTop::m_eventTime);
        //        return 0;
        //    }
        //    unit->OnFlightLocal(v3, 1);
        }
    }
    return 0;
}

// OFFSET: 0x5FC0A0
static int32_t Script_AscendStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x2000, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC140
static int32_t Script_DescendStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x4000, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FAAE0
static int32_t Script_ToggleRun(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC190
static int32_t Script_ToggleAutoRun(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;

    auto v1 = (CGInputControl::s_inputControl->m_flags & 0x1000) == 0;
    bool v3;
    if (v1)
        v3 = CGInputControl::s_inputControl->SetControlBit(0x1000, CSimpleTop::m_eventTime);
    else
        v3 = CGInputControl::s_inputControl->UnsetControlBit(0x1000, CSimpleTop::m_eventTime, 0);

    if (v3) {
        if (v1) {
            // CheckToCancelCurrentChannelSpell();
        }
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC200
static int32_t Script_MoveForwardStart(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->SetControlBit(0x10, CSimpleTop::m_eventTime)) {
        //CheckToCancelCurrentChannelSpell();
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC250
static int32_t Script_MoveForwardStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x10, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC290
static int32_t Script_MoveBackwardStart(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->SetControlBit(0x20, CSimpleTop::m_eventTime)) {
        // CheckToCancelCurrentChannelSpell();
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC2E0
static int32_t Script_MoveBackwardStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x20, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC320
static int32_t Script_TurnLeftStart(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->SetControlBit(0x100, CSimpleTop::m_eventTime)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC360
static int32_t Script_TurnLeftStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x100, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC3B0
static int32_t Script_TurnRightStart(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->SetControlBit(0x200, CSimpleTop::m_eventTime)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC3F0
static int32_t Script_TurnRightStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x200, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC440
static int32_t Script_StrafeLeftStart(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->SetControlBit(0x40, CSimpleTop::m_eventTime)) {
        // CheckToCancelCurrentChannelSpell();
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC490
static int32_t Script_StrafeLeftStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x40, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC4D0
static int32_t Script_StrafeRightStart(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->SetControlBit(0x80, CSimpleTop::m_eventTime)) {
        // CheckToCancelCurrentChannelSpell();
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC520
static int32_t Script_StrafeRightStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x80, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC8E0
static int32_t Script_PitchUpStart(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->SetControlBit(0x400, CSimpleTop::m_eventTime)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC570
static int32_t Script_PitchUpStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x400, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC920
static int32_t Script_PitchDownStart(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->SetControlBit(0x800, CSimpleTop::m_eventTime)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC5C0
static int32_t Script_PitchDownStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x800, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC610
static int32_t Script_TurnOrActionStart(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC680
static int32_t Script_TurnOrActionStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x1, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC6C0
static int32_t Script_CameraOrSelectOrMoveStart(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC730
static int32_t Script_CameraOrSelectOrMoveStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x2, CSimpleTop::m_eventTime, StringToBOOL(L, 1, 0))) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FC780
static int32_t Script_MoveAndSteerStart(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC830
static int32_t Script_MoveAndSteerStop(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0))
        return 0;
    if (CGInputControl::s_inputControl->UnsetControlBit(0x2, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    if (CGInputControl::s_inputControl->UnsetControlBit(0x1, CSimpleTop::m_eventTime, 0)) {
        CGInputControl::s_inputControl->UpdatePlayer(CSimpleTop::m_eventTime, 1);
    }
    return 0;
}

// OFFSET: 0x5FD550
static int32_t Script_SetMouselookOverrideBinding(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FCC10
static int32_t Script_MouselookStart(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC890
static int32_t Script_MouselookStop(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5F9DD0
static int32_t Script_IsMouselooking(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB660
static int32_t Script_VehicleExit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB6D0
static int32_t Script_VehiclePrevSeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB720
static int32_t Script_VehicleNextSeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC8E0
static int32_t Script_VehicleAimUpStart(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC570
static int32_t Script_VehicleAimUpStop(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC920
static int32_t Script_VehicleAimDownStart(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FC5C0
static int32_t Script_VehicleAimDownStop(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB770
static int32_t Script_VehicleAimIncrement(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB7D0
static int32_t Script_VehicleAimDecrement(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB820
static int32_t Script_VehicleAimRequestAngle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5F9E10
static int32_t Script_VehicleAimGetAngle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB8C0
static int32_t Script_VehicleAimRequestNormAngle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5F9E60
static int32_t Script_VehicleAimGetNormAngle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5F9F10
static int32_t Script_VehicleAimSetNormPower(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5F9550
static int32_t Script_VehicleAimGetNormPower(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB970
static int32_t Script_IsUsingVehicleControls(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FB9C0
static int32_t Script_CanExitVehicle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FBA10
static int32_t Script_CanSwitchVehicleSeats(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5F9F70
static int32_t Script_IsVehicleAimAngleAdjustable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5F9FE0
static int32_t Script_IsVehicleAimPowerAdjustable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5FA050
static int32_t Script_DetectWowMouse(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void InputControlRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_INPUT_CONTROL; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_InputControl[i].name,
            GameScript::s_ScriptFunctions_InputControl[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_InputControl[NUM_SCRIPT_FUNCTIONS_INPUT_CONTROL] = {
    { "JumpOrAscendStart", &Script_JumpOrAscendStart },
    { "AscendStop", &Script_AscendStop },
    { "DescendStop", &Script_DescendStop },
    { "ToggleRun", &Script_ToggleRun },
    { "ToggleAutoRun", &Script_ToggleAutoRun },
    { "MoveForwardStart", &Script_MoveForwardStart },
    { "MoveForwardStop", &Script_MoveForwardStop },
    { "MoveBackwardStart", &Script_MoveBackwardStart },
    { "MoveBackwardStop", &Script_MoveBackwardStop },
    { "TurnLeftStart", &Script_TurnLeftStart },
    { "TurnLeftStop", &Script_TurnLeftStop },
    { "TurnRightStart", &Script_TurnRightStart },
    { "TurnRightStop", &Script_TurnRightStop },
    { "StrafeLeftStart", &Script_StrafeLeftStart },
    { "StrafeLeftStop", &Script_StrafeLeftStop },
    { "StrafeRightStart", &Script_StrafeRightStart },
    { "StrafeRightStop", &Script_StrafeRightStop },
    { "PitchUpStart", &Script_PitchUpStart },
    { "PitchUpStop", &Script_PitchUpStop },
    { "PitchDownStart", &Script_PitchDownStart },
    { "PitchDownStop", &Script_PitchDownStop },
    { "TurnOrActionStart", &Script_TurnOrActionStart },
    { "TurnOrActionStop", &Script_TurnOrActionStop },
    { "CameraOrSelectOrMoveStart", &Script_CameraOrSelectOrMoveStart },
    { "CameraOrSelectOrMoveStop", &Script_CameraOrSelectOrMoveStop },
    { "MoveAndSteerStart", &Script_MoveAndSteerStart },
    { "MoveAndSteerStop", &Script_MoveAndSteerStop },
    { "SetMouselookOverrideBinding", &Script_SetMouselookOverrideBinding },
    { "MouselookStart", &Script_MouselookStart },
    { "MouselookStop", &Script_MouselookStop },
    { "IsMouselooking", &Script_IsMouselooking },
    { "VehicleExit", &Script_VehicleExit },
    { "VehiclePrevSeat", &Script_VehiclePrevSeat },
    { "VehicleNextSeat", &Script_VehicleNextSeat },
    { "VehicleAimUpStart", &Script_VehicleAimUpStart },
    { "VehicleAimUpStop", &Script_VehicleAimUpStop },
    { "VehicleAimDownStart", &Script_VehicleAimDownStart },
    { "VehicleAimDownStop", &Script_VehicleAimDownStop },
    { "VehicleAimIncrement", &Script_VehicleAimIncrement },
    { "VehicleAimDecrement", &Script_VehicleAimDecrement },
    { "VehicleAimRequestAngle", &Script_VehicleAimRequestAngle },
    { "VehicleAimGetAngle", &Script_VehicleAimGetAngle },
    { "VehicleAimRequestNormAngle", &Script_VehicleAimRequestNormAngle },
    { "VehicleAimGetNormAngle", &Script_VehicleAimGetNormAngle },
    { "VehicleAimSetNormPower", &Script_VehicleAimSetNormPower },
    { "VehicleAimGetNormPower", &Script_VehicleAimGetNormPower },
    { "IsUsingVehicleControls", &Script_IsUsingVehicleControls },
    { "CanExitVehicle", &Script_CanExitVehicle },
    { "CanSwitchVehicleSeats", &Script_CanSwitchVehicleSeats },
    { "IsVehicleAimAngleAdjustable", &Script_IsVehicleAimAngleAdjustable },
    { "IsVehicleAimPowerAdjustable", &Script_IsVehicleAimPowerAdjustable },
    { "DetectWowMouse", &Script_DetectWowMouse },
};
