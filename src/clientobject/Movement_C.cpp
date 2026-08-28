#include "clientobject/Movement_C.hpp"
#include "clientobject/Movement.hpp"
#include "clientobject/Unit_C.hpp"
#include "clientobject/Passenger.hpp"

// OFFSET: 0x6F13E0
void CMovement_C::MoveUnits(uint32_t time, uint32_t prevTime) {
    auto globals = MovementGetGlobals();
    if (!globals)
        return;

    for (CMovement_C* shared = globals->m_movementUnits.Head(); shared;) {
        auto next = globals->m_movementUnits.Next(shared);

        if ((shared->m_flags & 0x8000000) == 0) {
            shared->ExecuteMovement(time, prevTime);
        } else {
            if (shared->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
                uint32_t delta = time - prevTime;
                // shared->m_unit->SendTimeSkip(delta);
                globals->timeCounterMaybe += delta;
            }
        }

        shared = next;
    }
}

CMovement_C::CMovement_C(WGUID* transportGuid, C3Vector& position, float facing, WGUID* guid, CGUnit_C* unit)
    : CMovementShared(&WGUID(), position, facing, guid) {
    //this->ukn2 = 0.33333334;
    //this->ukn3 = 2.0277777;
    //this->ukn4 = 1.0;
    //this->ukn5 = 0.0;
    //this->ukn6 = 0.0;
    //this->ukn7 = 0.0;
    //this->ukn26 = 0;
    //this->ukn28 = 0;
    //this->ukn29 = 0;
    //this->ukn32 = 0;
    //this->ukn31 = &a1->ukn31;
    //this->ukn32 = &a1->ukn31 | 1;
    //this->ukn30 = 0;
    this->m_unit = unit;
    //CMovementShared::CalcDirection(a1, v8, 0, v6);
    //this->ukn10 = -3211314;
    //this->ukn11 = -3211314;
    //this->ukn12 = -3211314;
    //this->ukn13 = -3211314;
    //this->ukn14 = -3211314;
    //this->ukn15 = -3211314;
    //this->ukn16 = -3211314;
    //this->ukn17 = -3211314;
    //this->ukn18 = 3276850;
    //this->ukn19 = 3276850;
    //this->ukn20 = 3276850;
    //this->ukn21 = 3276850;
    //this->ukn22 = 3276850;
    //this->ukn23 = 3276850;
    //this->ukn24 = 3276850;
    //this->ukn25 = 3276850;
    //this->ukn27 = 50;
}

// OFFSET: 0x6F09F0
void CMovement_C::ExecuteMovement(uint32_t time, uint32_t prevTime) {
    uint32_t delta = time - prevTime;
    uint32_t cursor = prevTime;
    uint32_t consumed = 0;

    auto globals = MovementGetGlobals();

    if (delta > 250) {
        cursor = time - 250;
        //this->SendTimeSkip(delta - 250);
        delta = 250;
    } else if (delta == 0) {
        //this->m_unit->OnMoveUpdate(time, 0, 0);
        if (isnan(this->m_position.x) || isnan(this->m_position.y) || isnan(this->m_position.z)) {
            //ConsolePrintf("Mover at invalid position");
        }

        this->m_unit->m_modelFlags &= ~0x800000u;
        return;
    }

    //uint32_t result = 0;
    //bool completed = true;
    //while (consumed < delta) {
    //    uint32_t step = delta - consumed;
    //
    //    //ukn32 = this->ukn32;
    //    //if ((ukn32 & 1) == 0 && ukn32) {
    //    //    if ((ukn32 & 1) != 0)
    //    //        ukn32 = 0;
    //    //    v8 = *(ukn32 + 8) - v3;
    //    //    if (v8 < 0)
    //    //        v8 = 0;
    //    //    if (step > v8)
    //    //        step = v8;
    //    //}
    //
    //    if (*&this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover && !CMovement_C::HasSpline_IsNotSplineMover(this) && (&unk_C0100F & this->m_flags) != 0) {
    //        if (globals->timeCounterMaybe - cursor < step)
    //            step = globals->timeCounterMaybe - cursor;
    //    }
    //
    //    cursor += step;
    //    globals->idleTime = cursor;
    //
    //    if (step) {
    //        if ((this->m_flags & 0x40C010FF) != 0) {
    //            if ((this->m_flags & 0x200) != 0) {
    //                globals->timeCounterMaybe += step;
    //            } else {
    //                this->ApplyMovement(cursor, step);
    //                if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover && (&unk_C0100F & this->m_flags) != 0 && !CMovement_C::HasSpline_IsNotSplineMover(this) && (cursor - globals->timeCounterMaybe) >= 0)
    //                    this->m_unit->SendMovementUpdate(cursor, 238, 0.0, 0, 0, 0, 255);
    //            }
    //        }
    //        consumed += step;
    //    }
    //
    //    result = this->UpdatePlayerMovement(cursor);
    //    if (result != 1) {
    //        completed = false;
    //        break;
    //    }
    //}
    //
    //if (!completed) {
    //    if (result) {
    //        if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover)
    //            globals->timeCounterMaybe += delta - consumed;
    //    } else {
    //        this->RemoveFromMoversList(1);
    //    }
    //}

    // this->m_unit->OnMoveUpdate(time, 0, 0);
    if (isnan(this->m_position.x) || isnan(this->m_position.y) || isnan(this->m_position.z)) {
        //ConsolePrintf("Mover at invalid position");
    }

    this->m_unit->m_modelFlags &= ~0x800000u;
}
