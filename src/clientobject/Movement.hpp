#ifndef CLIENTOBJECT_MOVEMENT_HPP
#define CLIENTOBJECT_MOVEMENT_HPP

#include <cstdint>
#include "clientobject/Movement_C.hpp"

class CVar;

extern CVar* s_cvSplineOpt;

struct CMovementGlobals {
    char logFile[260];
    //DWORD ukn66;
    //DWORD ukn67;
    //DWORD ukn68;
    //DWORD ukn69;
    //DWORD ukn70;
    STORM_EXPLICIT_LIST(CMovement_C, m_link) m_movementUnits;
    //TSList_CMovementData_C units;
    uint32_t flags;
    uint32_t lastIdleTime;
    uint32_t idleTime;
    //DWORD ukn77;
    //DWORD ukn78;
    //DWORD ukn79;
    uint32_t timeCounterMaybe;
};

void MovementInit();

void MovementInitialize(const char* logFile);

int32_t MovementIdleMoveUnits(const void*, void*);

void MovementSetGlobals(CMovementGlobals* globals);

CMovementGlobals* MovementGetGlobals();

#endif // CLIENTOBJECT_PASSENGER_HPP
