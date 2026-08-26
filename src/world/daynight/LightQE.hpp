#ifndef WORLD_DAY_NIGHT_LIGHT_QE_HPP
#define WORLD_DAY_NIGHT_LIGHT_QE_HPP

#include <cstdint>

class LightRec;

namespace DayNight {

    class LightQE {
        public:
        float m_key;        // +0x00
        LightRec** m_light; // +0x04

        static int32_t s_localSort;

        static bool HasHigherPriority(const LightQE& a, const LightQE& b);
    };

} // namespace DayNight

#endif
