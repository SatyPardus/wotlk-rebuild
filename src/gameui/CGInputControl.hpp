#ifndef GAME_UI_CGINPUT_CONTROL_HPP
#define GAME_UI_CGINPUT_CONTROL_HPP

#include "ui/CSimpleFrame.hpp"
#include "ui/CSimpleTop.hpp"

class CGUnit_C;

enum CONTROL_BIT : uint32_t {
    CONTROL_TURNORACTION = 0x00000001,                  // bit 0   default: right mouse
    CONTROL_CAMERAORSELECTORMOVE = 0x00000002,          // bit 1   default: left mouse
                                                        // 0x00000004                                     bit 2   never set, never tested
                                                        // 0x00000008                                     bit 3   never set, never tested
    CONTROL_MOVEFORWARD = 0x00000010,                   // bit 4
    CONTROL_MOVEBACKWARD = 0x00000020,                  // bit 5
    CONTROL_STRAFELEFT = 0x00000040,                    // bit 6
    CONTROL_STRAFERIGHT = 0x00000080,                   // bit 7
    CONTROL_TURNLEFT = 0x00000100,                      // bit 8
    CONTROL_TURNRIGHT = 0x00000200,                     // bit 9
    CONTROL_PITCHUP = 0x00000400,                       // bit 10  VehicleAimUp
    CONTROL_PITCHDOWN = 0x00000800,                     // bit 11  VehicleAimDown
    CONTROL_AUTORUN = 0x00001000,                       // bit 12
    CONTROL_ASCEND = 0x00002000,                        // bit 13  JumpOrAscend
    CONTROL_DESCEND = 0x00004000,                       // bit 14
                                                        // 0x00008000                                     bit 15  never set, never tested
    CONTROL_MOVE_SENT = 0x00010000,                     // bit 16  CGInputControl::MovePlayer
    CONTROL_STRAFE_SENT = 0x00020000,                   // bit 17  CGInputControl::StrafePlayer
    CONTROL_TURN_SENT = 0x00040000,                     // bit 18  CGInputControl::TurnPlayer
    CONTROL_PITCH_SENT = 0x00080000,                    // bit 19  CGInputControl::PitchPlayer
    CONTROL_ASCENDDESCEND_SENT = 0x00100000,            // bit 20  CGInputControl::AscendDescendPlayer
    CONTROL_TRACK_MOVE = 0x00200000,                    // bit 21
    CONTROL_TRACK_PITCH = 0x00400000,                   // bit 22
                                                        // 0x00800000                                     bit 23  never set
    CONTROL_TRACK_FACING = 0x01000000,                  // bit 24
    CONTROL_VIRTUAL_TURNORACTION = 0x02000000,          // bit 25
    CONTROL_VIRTUAL_CAMERAORSELECTORMOVE = 0x04000000,  // bit 26
                                                        // bits 27..31                                    never set, never tested
};

class CGInputControl {
    public:
    // Static variables
    static CGInputControl* s_inputControl;

    // Static functions
    static void Initialize();
    static CGInputControl* GetActive();

    // Member variables
    /* 0000 */ uint32_t m_time = 0;
    /* 0000 */ uint32_t m_flags = 0;

    // Virtual member functions

    // Member functions
    CGInputControl();
    void UpdatePlayer(int32_t eventTime, bool a3);
    bool SetControlBit(uint32_t controlBit, int32_t eventTime);
    bool UnsetControlBit(uint32_t controlBit, int32_t eventTime, uint32_t a4);
    bool CanMove(CGUnit_C* unit);
    bool CanTurn(CGUnit_C* unit);
    bool CanControl(CGUnit_C* unit);

    void OnTurnToAngleStop();
    void MovePlayer(int32_t eventTime, CGUnit_C* unit);
    void StrafePlayer(int32_t eventTime, CGUnit_C* unit);
    void AscendDescendPlayer(int32_t eventTime, CGUnit_C* unit);
    void TurnPlayer(int32_t eventTime, CGUnit_C* unit);
    void PitchPlayer(int32_t eventTime, CGUnit_C* unit);
};

#endif // GAME_UI_CGMINIMAP_FRAME_HPP
