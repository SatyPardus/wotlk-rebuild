#ifndef WORLD_MAP_WEATHER_HPP
#define WORLD_MAP_WEATHER_HPP

#include <cstdint>

class CVar;

class Weather {
    public:
    static CVar* s_useShaders;

    Weather();

    static bool WeatherDensityCallback(CVar*, const char*, const char*, void*);
};

#endif
