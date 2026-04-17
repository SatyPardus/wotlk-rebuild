#ifndef CLIENT_FRAMETIME_HPP
#define CLIENT_FRAMETIME_HPP

#include <cstdint>

class FrameTime {
    public:
    static uint32_t s_curTimeMs;
    static float s_curTimeSec;
    static uint32_t s_tickTimeMs;
    static float s_tickTimeSec;

    static void Update(float elapsedSec, int32_t currTime);
};

#endif
