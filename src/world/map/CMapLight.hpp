#ifndef WORLD_MAP_C_MAP_LIGHT_HPP
#define WORLD_MAP_C_MAP_LIGHT_HPP

#include "world/map/Types.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "model/CM2Light.hpp"

class CMapLight : public CMapBaseObj {
    public:
    float unk_0024;
    float unk_0028;
    float unk_002C;
    float unk_0030;
    float unk_0034;
    float unk_0038;
    float unk_003C;
    float unk_0040;
    float unk_0044;
    float unk_0048;
    float unk_004C;
    float unk_0050;
    float unk_0054;
    CM2Light m_light;
    float unk_00C4;
    float unk_00C8;
    float unk_00CC;
    uint8_t unk_00D0;
    uint8_t unk_00D1;
    uint8_t unk_00D2;
    uint8_t unk_00D3;
};

#endif
