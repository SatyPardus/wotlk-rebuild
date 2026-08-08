#ifndef WORLD_CWORLDVIEW_HPP
#define WORLD_CWORLDVIEW_HPP

#include <cstdint>

class CWorldView {
    public:
    static float s_fadeDistMax[];
    static float s_fadeSize[];

    static uint8_t GetFadeLevelForDistance(float dist);
};

#endif
