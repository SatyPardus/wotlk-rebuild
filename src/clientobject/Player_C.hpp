#ifndef CLIENTOBJECT_PLAYER_C_HPP
#define CLIENTOBJECT_PLAYER_C_HPP

#include <cstdint>
#include "clientobject/CGObject_C.hpp"

class CreatureModelDataRec;


const CreatureModelDataRec* Player_C_GetModelName(uint32_t race, uint32_t sex);
uint32_t Player_C_GetDisplayId(uint32_t race, uint32_t sex);

class CGPlayer_C : public CGObject_C {
    public:
    CGPlayer_C();
};


#endif // CLIENTOBJECT_PLAYER_C_HPP
