#ifndef CLIENTOBJECT_MOVEMENT_C_HPP
#define CLIENTOBJECT_MOVEMENT_C_HPP

#include "clientobject/MovementShared.hpp"
#include <cstdint>

class CGUnit_C;

class CMovement_C : public CMovementShared {
    public:
    // Static methods
    static void MoveUnits(uint32_t time, uint32_t prevTime);

    // Member variables
    /* 0000 */ //DWORD ukn1;
    /* 0000 */ //float ukn2;
    /* 0000 */ //float ukn3;
    /* 0000 */ //float ukn4;
    /* 0000 */ //float ukn5;
    /* 0000 */ //float ukn6;
    /* 0000 */ //float ukn7;
    /* 0000 */ //DWORD ukn8;
    /* 0000 */ //DWORD ukn9;
    /* 0000 */ //DWORD ukn10;
    /* 0000 */ //DWORD ukn11;
    /* 0000 */ //DWORD ukn12;
    /* 0000 */ //DWORD ukn13;
    /* 0000 */ //DWORD ukn14;
    /* 0000 */ //DWORD ukn15;
    /* 0000 */ //DWORD ukn16;
    /* 0000 */ //DWORD ukn17;
    /* 0000 */ //DWORD ukn18;
    /* 0000 */ //DWORD ukn19;
    /* 0000 */ //DWORD ukn20;
    /* 0000 */ //DWORD ukn21;
    /* 0000 */ //DWORD ukn22;
    /* 0000 */ //DWORD ukn23;
    /* 0000 */ //DWORD ukn24;
    /* 0000 */ //DWORD ukn25;
    /* 0000 */ //DWORD ukn26;
    /* 0000 */ //DWORD ukn27;
    /* 0000 */ //DWORD ukn28;
    /* 0000 */ //DWORD ukn29;
    /* 0000 */ //DWORD ukn30;
    /* 0000 */ //DWORD ukn31;
    /* 0000 */ //DWORD ukn32;
    /* 0144 */ CGUnit_C* m_unit = nullptr;

    // Member methods
    CMovement_C() = default;
    CMovement_C(WGUID* transportGuid, C3Vector& position, float facing, WGUID* guid, CGUnit_C* unit);
    void ExecuteMovement(uint32_t time, uint32_t prevTime);
};

#endif // CLIENTOBJECT_MOVEMENT_C_HPP
