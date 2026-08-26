#ifndef WORLD_DAY_NIGHT_OVERRIDE_SKY_HPP
#define WORLD_DAY_NIGHT_OVERRIDE_SKY_HPP

#include <cstdint>
#include <storm/Hash.hpp>

class CM2Model;

namespace DayNight {

class DNOverrideSky : public TSHashObject<DNOverrideSky, HASHKEY_STRI> {
    public:
    CM2Model* m_model;
    uint32_t m_animLengthMs;
    uint32_t m_lastTimeOfDay;
    uint32_t m_flags;
};

} // namespace DayNight

#endif
