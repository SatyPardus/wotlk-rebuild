#include "clientobject/CClientMoveUpdate.hpp"

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

    if (move.status.m_moveFlags & 0x8000000) {
        msg >> move.m_moveSpline;
    }

    return msg;
}
