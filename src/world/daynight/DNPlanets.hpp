#ifndef WORLD_DAY_NIGHT_PLANETS_HPP
#define WORLD_DAY_NIGHT_PLANETS_HPP

#include <cstdint>
#include <storm/Array.hpp>
#include <tempest/Vector.hpp>
#include "world/daynight/DNPlanet.hpp"

namespace DayNight {

class DNPlanets {
    public:
    DNPlanet sun;
    DNPlanet moon;
    DNPlanet moon2;
    uint32_t unk_0060;
    uint32_t unk_0064;
    uint32_t unk_0068;
    uint32_t unk_006C;
};

} // namespace DayNight

#endif
