#include "clientobject/Movement.hpp"
#include <event/Event.hpp>
#include "clientobject/ObjectMgrClient.hpp"
#include <common/time/Time.hpp>
#include "console/CVar.hpp"
#include "clientobject/Movement_C.hpp"

CVar* s_cvSplineOpt;

// OFFSET: 0x401520
void MovementInit() {
    //if (!SRegLoadString("World of Warcraft\\Client", "MoveLogFile", 0, v2, 260) || !v2[0]) {
    //    SRegSaveString("World of Warcraft\\Client", "MoveLogFile", 0, "ClientMovement.txt");
    //    SStrCopy(v2, "ClientMovement.txt", 260);
    //}
    //CurrentProcessId = GetCurrentProcessId();
    //v1 = SStrChrR(v2, 46);
    //SStrPrintf(v1, v1 - v2 + 260, "%04d.txt", CurrentProcessId);
    MovementInitialize("ClientMovement.txt");
    EventRegisterEx(EVENT_ID_IDLE, &MovementIdleMoveUnits, nullptr, 2.0f);
}

// OFFSET: 0x6EC2C0
void MovementInitialize(const char* logFile) {
    CMovementGlobals* globals = new (STORM_ALLOC(sizeof(CMovementGlobals))) CMovementGlobals();
    MovementSetGlobals(globals);
    globals = MovementGetGlobals();
    SStrCopy(globals->logFile, logFile, 260);
    auto time = OsGetAsyncTimeMs();
    globals->flags |= 1;
    globals->idleTime = time;
    globals->lastIdleTime = time;
    s_cvSplineOpt = CVar::Register("SplineOpt", "toggles use of spline coll optimization", 1, "1", 0, 0, 0, 0, 0);
}

// OFFSET: 0x6F1490
int32_t MovementIdleMoveUnits(const void*, void*) {
    auto time = OsGetAsyncTimeMs();
    auto globals = MovementGetGlobals();
    auto delta = time - globals->lastIdleTime;
    if (delta > 0) {
        //MovementMoveTransports(time, delta);
        if (!globals->m_movementUnits.IsEmpty())
            CMovement_C::MoveUnits(time, globals->lastIdleTime);
        //v4 = ClntObjMgrObjectPtr(CGUnit_C::m_activeMover, TYPEMASK_UNIT);
        //if (v4)
        //    CGUnit_C::UpdateFloodsafeMoveEvents(v4, AsyncTimeMs);
        globals->idleTime = time;
        globals->lastIdleTime = time;
    }
    return 1;
}

// OFFSET: 0x74B320
void MovementSetGlobals(CMovementGlobals* globals) {
    ClntObjMgrSetMovementGlobals(globals);
}

// OFFSET: 0x74B330
CMovementGlobals* MovementGetGlobals() {
    return ClntObjMgrGetMovementGlobals();
}
