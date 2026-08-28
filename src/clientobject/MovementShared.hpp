#ifndef CLIENTOBJECT_MOVEMENT_SHARED_HPP
#define CLIENTOBJECT_MOVEMENT_SHARED_HPP

#include <cstdint>
#include "clientobject/Passenger.hpp"
#include "clientobject/CMoveSpline.hpp"

class CMovementShared : public CPassenger {
    public:
    /* 0030 */ uint32_t unk_0030 = 0;
    /* 0034 */ uint32_t unk_0034 = 0;
    /* 0038 */ C3Vector m_groundNormal = { 0.0f, 0.0f, 1.0f };
    /* 0044 */ uint32_t m_flags = 0;
    /* 0048 */ uint32_t m_flags2 = 0;
    /* 004C */ C3Vector m_anchorPos;
    /* 0058 */ float m_anchorFacing = 0;
    /* 005C */ float unk_005C = 0;
    /* 0060 */ uint32_t unk_0060 = 0;
    /* 0064 */ float unk_0064 = 0;
    /* 0068 */ float unk_0068 = 0;
    /* 006C */ float unk_006C= 0;
    /* 0070 */ float unk_0070= 0;
    /* 0074 */ float unk_0074= 0;
    /* 0078 */ float unk_0078= 1.0f;
    /* 007C */ float unk_007C= 0;
    /* 0080 */ uint32_t m_fallTimeMs = 0;
    /* 0084 */ float m_fallStartZ = 0;
    /* 0088 */ float m_stepUpStartZ = 0;
    /* 008C */ float m_currentSpeed = 0;
    /* 0090 */ float m_walkSpeed = 0;
    /* 0094 */ float m_runSpeed = 0;
    /* 0098 */ float m_runBackSpeed = 0;
    /* 009C */ float m_swimSpeed = 0;
    /* 00A0 */ float m_swimBackSpeed = 0;
    /* 00A4 */ float m_flightSpeed = 0;
    /* 00A8 */ float m_flightBackSpeed = 0;
    /* 00AC */ float m_turnRate = 0;
    /* 00B0 */ float m_pitchRate = 0;
    /* 00B4 */ float unk_00B4 = 1.0f;
    /* 00B8 */ float m_fallVelocity = 0;
    /* 00BC */ CMoveSpline* m_spline = nullptr;
    /* 00C0 */ uint32_t unk_00C0 = 0;

    CMovementShared() = default;
    CMovementShared(WGUID* transportGuid, C3Vector& position, float facing, WGUID* guid);
};

#endif // CLIENTOBJECT_MOVEMENT_SHARED_HPP
