#include "clientobject/CClientMoveUpdate.hpp"
#include "clientobject/Types.hpp"

void CClientMoveUpdate::Skip(CDataStore* msg) {
    uint32_t moveFlags = CMovementStatus::Skip(msg);

    void* data;
    msg->GetDataInSitu(data, 9 * sizeof(float));

    if (moveFlags & 0x8000000) {
        CMoveSpline::Skip(msg);
    }
}

CDataStore& operator>>(CDataStore& msg, CClientMoveUpdate& move) {
    msg >> move.status;

    msg.Get(move.m_walkSpeed);
    msg.Get(move.m_runSpeed);
    msg.Get(move.m_runBackSpeed);
    msg.Get(move.m_swimSpeed);
    msg.Get(move.m_swimBackSpeed);
    msg.Get(move.m_flightSpeed);
    msg.Get(move.m_flightBackSpeed);
    msg.Get(move.m_turnRate);
    msg.Get(move.m_pitchRate);

    if ((move.status.m_moveFlags & 0x8000000) != 0) {
        msg >> move.m_moveSpline;
    }

    return msg;
}

CDataStore& operator<<(CDataStore& msg, CClientMoveUpdate& move) {
    msg.Put(move.status.m_moveFlags);
    msg.Put(move.status.m_moveExtraFlags);
    msg.Put(move.status.m_gameTime);
    msg.Put(move.status.m_position.x);
    msg.Put(move.status.m_position.y);
    msg.Put(move.status.m_position.z);
    msg.Put(move.status.m_facing);
    if ((move.status.m_moveFlags & MOVEMENTFLAG_ONTRANSPORT) != 0) {
        SErrDisplayAppFatal("Not implemented");
    }
    if ((move.status.m_moveFlags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING)) != 0 || (move.status.m_moveExtraFlags & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING) != 0)
        msg.Put(move.status.m_pitch);
    msg.Put(move.status.m_fallTime);
    if ((move.status.m_moveFlags & MOVEMENTFLAG_FALLING) != 0) {
        msg.Put(move.status.m_zSpeed);
        msg.Put(move.status.m_sinAngle);
        msg.Put(move.status.m_cosAngle);
        msg.Put(move.status.m_xySpeed);
    }
    if ((move.status.m_moveFlags & MOVEMENTFLAG_SPLINE_ELEVATION) != 0)
        msg.Put(move.status.m_splineElevation);

    return msg;
}
