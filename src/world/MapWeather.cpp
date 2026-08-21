#include "world/MapWeather.hpp"
#include "console/CVar.hpp"

CVar* Weather::s_useShaders;

Weather::Weather() {
    //*&this->unk_0000 = 0.0;
    //this->unk_000C[5] = -1;
    //*&this->unk_0004 = 0.0;
    //*&this->unk_0008 = 0.0;
    //this->unk_000C[3] = 0;
    //*this->unk_000C = 0.0;
    //this->unk_000C[4] = 0;
    //*&this->unk_000C[1] = 0.0;
    //LOBYTE(this->unk_000C[6]) = 0;
    //*&this->unk_000C[2] = 0.0;
    //BYTE1(this->unk_000C[6]) = 0;
    //*&this->unk_000C[7] = 1.0;
    //*&this->unk_000C[8] = 1.0;
    //*&this->unk_000C[9] = 1.0;
    //this->unk_000C[10] = -1;
    //this->unk_000C[76] = 0;
    //this->unk_000C[77] = 0;
    //this->unk_000C[78] = 0;
    //this->unk_000C[79] = 0;
    //this->unk_000C[82] = 0;
    //this->unk_000C[80] = 0;
    //this->unk_000C[81] = &this->unk_000C[81];
    //this->unk_000C[82] = &this->unk_000C[81] | 1;
    //this->unk_000C[85] = 0;
    //this->unk_000C[83] = 0;
    //this->unk_000C[84] = &this->unk_000C[84];
    //this->unk_000C[85] = &this->unk_000C[84] | 1;
    //*&this->unk_000C[86] = 0.0;
    //*&this->unk_000C[87] = 0.0;
    //*&this->unk_000C[88] = 0.0;
    //*&this->unk_000C[89] = 0.0;
    //*&this->unk_000C[90] = 0.0;
    //*&this->unk_000C[91] = 0.0;
    //*&this->unk_000C[92] = 0.0;
    //*&this->unk_000C[93] = 0.0;
    //*&this->unk_000C[94] = 0.0;
    //LOBYTE(this->unk_000C[95]) = 0;
    //*&this->unk_000C[96] = 0.0;
    //LOBYTE(this->unk_000C[98]) = 0;
    //*&this->unk_000C[97] = 0.0;
    //v2 = SMemAlloc(0x8C, ".\\MapWeather.cpp", 2144, 0);
    //if (v2)
    //    v3 = Weather::InitializeGrid(v2);
    //else
    //    v3 = 0;
    //this->unk_000C[79] = v3;
    CVar::Register("weatherDensity", 0, 0, "2", Weather::WeatherDensityCallback, 5, 0, 0, 0);
    Weather::s_useShaders = CVar::Register("useWeatherShaders", 0, 0, "1", 0, 5, 0, 0, 0);
    //v5 = 0;
    //if (SRegLoadValue("Internal", "force-weather-type-on", 0, &v5) && v5 > 0 && v5 < 4) {
    //    bn_Weather_SetType(v5, 1.0, 0, 0, 1.0);
    //    byte_CD8528 = 1;
    //}
    //LOBYTE(this->unk_000C[11]) = 0;
}

bool Weather::WeatherDensityCallback(CVar*, const char*, const char*, void*) {
    return true;
}
