#include "client/FrameTime.hpp"

uint32_t FrameTime::s_curTimeMs;
float FrameTime::s_curTimeSec;
uint32_t FrameTime::s_tickTimeMs;
float FrameTime::s_tickTimeSec;

// OFFSET: 0x77ECB0
void FrameTime::Update(float elapsedSec, int32_t currTime) {
    FrameTime::s_curTimeMs = currTime;
    FrameTime::s_tickTimeMs = (uint32_t)(elapsedSec * 1000.0f - 0.5f);
    FrameTime::s_curTimeSec = currTime * 0.001f;
    FrameTime::s_tickTimeSec = elapsedSec;
    //flt_CD769C = flt_CD769C + a1;
    //v2 = sub_783B30(a1);
    //dword_CD7694 += v2;
    //dword_CD7698 = v2;
}
