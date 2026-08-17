#ifndef OBJECT_MOVEMENT_C_MOVEMENT_STATUS_HPP
#define OBJECT_MOVEMENT_C_MOVEMENT_STATUS_HPP

#include "clientobject/WGUID.hpp"
#include <common/DataStore.hpp>
#include <cstdint>
#include <tempest/Vector.hpp>

struct CMovementStatus {
    uint32_t m_gameTime;
    uint32_t unk_0004;
    WGUID m_transportGuid;
    uint32_t m_moveFlags;
    uint16_t m_moveExtraFlags;
    uint8_t m_transportSeat;
    uint8_t uint17;
    C3Vector m_transportPosition;
    float m_transportFacing;
    C3Vector m_position;
    float m_facing;
    float m_pitch;
    uint32_t m_fallTime;
    float m_zSpeed;
    float m_sinAngle;
    float m_cosAngle;
    float m_xySpeed;
    float m_splineElevation;
    float m_transportTime;
    uint32_t m_transportTime2;
    float float5C;

    static uint32_t Skip(CDataStore* msg);
};

CDataStore& operator>>(CDataStore& msg, CMovementStatus& move);

#endif
