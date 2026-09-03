#include "clientobject/CMoveSpline.hpp"
#include "util/DataStore.hpp"
#include <common/Time.hpp>

void CMoveSpline::Skip(CDataStore* msg) {
    uint32_t flags;
    msg->Get(flags);

    uint32_t faceBytes = 0;

    if (flags & 0x20000) {
        faceBytes = 4;
    } else if (flags & 0x10000) {
        faceBytes = 8;
    } else if (flags & 0x8000) {
        faceBytes = 12;
    }

    void* data;
    msg->GetDataInSitu(data, faceBytes + 28);

    uint32_t splinePoints = 0;
    msg->Get(splinePoints);

    msg->GetDataInSitu(data, (splinePoints * sizeof(C3Vector)) + 13);
}

// OFFSET: 0x6F1240
void CMoveSpline::CopyFrom(CMoveSpline* source) {
    this->unk_0008 = source->unk_0008;
    this->face = source->face;
    this->unk_001C = source->unk_001C;
    this->flags = source->flags;
    this->m_timePassed = source->m_timePassed;
    this->start = source->start;
    this->m_duration = source->m_duration;
    this->m_id = source->m_id;

    this->spline = source->spline;

    this->m_finalDestination = source->m_finalDestination;
    this->m_durationMod = source->m_durationMod;
    this->m_durationModNext = source->m_durationModNext;
    this->m_verticalAcceleration = source->m_verticalAcceleration;
    this->m_effectStartTime = source->m_effectStartTime;
}

CDataStore& operator>>(CDataStore& msg, CMoveSpline& spline) {
    msg.Get(spline.flags);

    if (spline.flags & 0x20000) {
        msg.Get(spline.face.facing);
    } else if (spline.flags & 0x10000) {
        msg >> spline.face.guid;
    } else if (spline.flags & 0x8000) {
        msg >> spline.face.spot;
    }

    uint32_t val;
    msg.Get(val);
    spline.start = OsGetAsyncTimeMsPrecise() - val;

    msg.Get(spline.m_duration);
    msg.Get(spline.m_id);
    msg.Get(spline.m_durationMod);
    msg.Get(spline.m_durationModNext);
    msg.Get(spline.m_verticalAcceleration);
    msg.Get(spline.m_effectStartTime);

    msg >> spline.spline;

    msg >> spline.m_finalDestination;

    return msg;
}
