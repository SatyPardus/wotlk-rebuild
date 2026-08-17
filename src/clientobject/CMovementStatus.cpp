#include "clientobject/CMovementStatus.hpp"
#include "util/DataStore.hpp"

uint32_t CMovementStatus::Skip(CDataStore* msg) {
    uint32_t moveFlags = 0;
    msg->Get(moveFlags);

    uint16_t uint14;
    msg->Get(uint14);

    void* data;
    msg->GetDataInSitu(data, 20);

    uint32_t skipBytes = 0;

    if (moveFlags & 0x200) {
        WGUID guid;
        *msg >> guid;

        skipBytes += 21;

        if (uint14 & 0x400) {
            skipBytes += 4;
        }
    }

    if ((moveFlags & (0x200000 | 0x2000000)) || (uint14 & 0x20)) {
        skipBytes += 4;
    }

    skipBytes += 4;

    if (moveFlags & 0x1000) {
        skipBytes += 16;
    }

    if (moveFlags & 0x4000000) {
        skipBytes += 4;
    }

    msg->GetDataInSitu(data, skipBytes);

    return moveFlags;
}

CDataStore& operator>>(CDataStore& msg, CMovementStatus& move) {
    msg.Get(move.m_moveFlags);
    msg.Get(move.m_moveExtraFlags);
    msg.Get(move.m_gameTime);

    msg >> move.m_position;
    msg.Get(move.m_facing);

    if (move.m_moveFlags & 0x200) {
        WGUID guid;
        msg >> guid;

        msg >> move.m_transportPosition;
        msg.Get(move.m_transportFacing);
        msg.Get(move.m_transportTime);
        msg.Get(move.m_transportSeat);

        if ((move.m_moveExtraFlags & 0x400) != 0) {
            move.m_moveExtraFlags &= ~0x400;
            msg.Get(move.m_transportTime2);
        } else {
            move.m_transportTime2 = move.m_transportTime;
        }
    } else {
        move.m_transportGuid = 0;
        move.m_transportSeat = -1;
    }

    if ((move.m_moveFlags & (0x200000 | 0x2000000)) || (move.m_moveExtraFlags & 0x20)) {
        msg.Get(move.m_pitch);
    } else {
        move.m_pitch = 0.0f;
    }

    msg.Get(move.m_fallTime);

    if (move.m_moveFlags & 0x1000) {
        msg.Get(move.m_zSpeed);
        msg.Get(move.m_sinAngle);
        msg.Get(move.m_cosAngle);
        msg.Get(move.m_xySpeed);
    }

    if (move.m_moveFlags & 0x4000000) {
        msg.Get(move.m_splineElevation);
    }

    return msg;
}
