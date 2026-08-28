#ifndef CLIENTOBJECT_PASSENGER_HPP
#define CLIENTOBJECT_PASSENGER_HPP

#include <cstdint>
#include <storm/List.hpp>
#include "clientobject/WGUID.hpp"
#include <tempest/Vector.hpp>

class CPassenger {
    public:
    /* 0000 */ TSLink<CPassenger> m_link;
    /* 0008 */ WGUID m_transportGuid = 0;
    /* 0010 */ C3Vector m_position;
    /* 001C */ uint32_t unk_001C = 0;
    /* 0020 */ float m_facing = 0;
    /* 0024 */ float m_pitch = 0;
    /* 0028 */ WGUID* m_guid2 = 0;
    /* 002C */ uint32_t unk_002C = 0;

    CPassenger() = default;
    CPassenger(WGUID* transportGuid, C3Vector& position, WGUID* guid);
    void GetPosition(C3Vector* out, C3Vector* pos);
    float GetFacing(float facing);
};

#endif // CLIENTOBJECT_PASSENGER_HPP
