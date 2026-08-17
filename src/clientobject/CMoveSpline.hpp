#ifndef OBJECT_MOVEMENT_C_MOVE_SPLINE_HPP
#define OBJECT_MOVEMENT_C_MOVE_SPLINE_HPP

#include "util/C3Spline.hpp"
#include "clientobject/WGUID.hpp"
#include <common/DataStore.hpp>
#include <tempest/Vector.hpp>

struct CMoveSpline {
    uint32_t unk_0000;
    uint32_t unk_0004;
    uint32_t unk_0008;
    uint32_t unk_000C;
    union {
        C3Vector spot = {};
        WGUID guid;
        float facing;
    } face;
    uint32_t unk_001C;
    uint32_t flags;
    uint32_t m_timePassed;
    uint32_t start;
    uint32_t m_duration;
    uint32_t m_id;
    C3Spline_CatmullRom spline;
    C3Vector m_finalDestination;
    float m_durationMod;
    float m_durationModNext;
    float m_verticalAcceleration;
    uint32_t m_effectStartTime;
    uint32_t unk_0214;
    uint32_t unk_0218;

    static void Skip(CDataStore* msg);
};

CDataStore& operator>>(CDataStore& msg, CMoveSpline& spline);

#endif
