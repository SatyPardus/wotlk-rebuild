#ifndef CLIENTOBJECT_MOVEMENT_SHARED_HPP
#define CLIENTOBJECT_MOVEMENT_SHARED_HPP

#include <cstdint>
#include "clientobject/Passenger.hpp"
#include "clientobject/CMoveSpline.hpp"

class CMovementShared : public CPassenger {
    public:
    /* 0030 */ TSLink<CMovementShared> m_globalUnitLink;
    /* 0038 */ C3Vector m_groundNormal = { 0.0f, 0.0f, 1.0f };
    /* 0044 */ uint32_t m_flags = 0;
    /* 0048 */ uint32_t m_flags2 = 0;
    /* 004C */ C3Vector m_anchorPos;
    /* 0058 */ float m_anchorFacing = 0;
    /* 005C */ float m_anchorPitch = 0;
    /* 0060 */ uint32_t m_anchorElapsedMs;
    /* 0064 */ C3Vector m_moveDir;
    /* 0070 */ C2Vector m_moveDir2D;
    /* 0078 */ float m_pitchCos = 1.0f;
    /* 007C */ float m_pitchSin = 0;
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
    /* 00B4 */ float m_hoverHeight = 1.0f;
    /* 00B8 */ float m_fallVelocity = 0;
    /* 00BC */ CMoveSpline* m_spline = nullptr;
    /* 00C0 */ uint32_t m_statusTimeMs = 0;

    CMovementShared() = default;
    CMovementShared(WGUID* transportGuid, C3Vector& position, float facing, WGUID* guid);

    bool IsOnSpline();
    bool IsOnFlyingSpline();
    bool IsOnFallingSpline();
    void CalcDirection();
    void CalcDirection(bool a2);
    void UpdateAnchors(bool a2);
    float GetBaseSpeed(bool a2);
    bool StartMove(bool a2, bool a3);
    bool StopMove();
    void ForceStopMove(bool a2);

    int32_t PlotUnitMovement(int32_t time, C3Vector* out);
    int32_t PlotUnitMovement(int32_t time, C3Vector* out, float* outFacing, float* outPitch);
    void PlotSpiralPosition(float dt, C3Vector* out);
    void PlotVertCircularPosition(float dt, C3Vector* out);
    void PlotHorzCircularPosition(float dt, C3Vector* out);
    void PlotAscendDescend(float dt, C3Vector* out);
    void PlotStraight(float dt, C3Vector* out);
    float PlotPitch(float dt);
    float PlotFacing(float dt);
};

#endif // CLIENTOBJECT_MOVEMENT_SHARED_HPP
