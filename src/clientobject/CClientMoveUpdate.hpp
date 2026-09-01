#ifndef OBJECT_CLIENT_C_CLIENT_MOVE_UPDATE_HPP
#define OBJECT_CLIENT_C_CLIENT_MOVE_UPDATE_HPP

#include "clientobject/CMoveSpline.hpp"
#include "clientobject/CMovementStatus.hpp"
#include <common/datastore/CDataStore.hpp>

struct CClientMoveUpdate {
    CMovementStatus status;
    float m_walkSpeed;
    float m_runSpeed;
    float m_runBackSpeed;
    float m_swimSpeed;
    float m_swimBackSpeed;
    float m_flightSpeed;
    float m_flightBackSpeed;
    float m_turnRate;
    float m_pitchRate;
    uint32_t unk_0084;
    CMoveSpline m_moveSpline;

    static void Skip(CDataStore* msg);
};

CDataStore& operator>>(CDataStore& msg, CClientMoveUpdate& move);
CDataStore& operator<<(CDataStore& msg, CClientMoveUpdate& move);

#endif
