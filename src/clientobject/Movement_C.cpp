#include "clientobject/Movement_C.hpp"
#include "clientobject/Movement.hpp"
#include "clientobject/Unit_C.hpp"
#include "clientobject/Passenger.hpp"
#include <tempest/math/CMath.hpp>
#include "world/World.hpp"
#include "clientobject/CClientMoveUpdate.hpp"
#include "gameui/CGInputControl.hpp"

STORM_EXPLICIT_LIST(CPlayerMoveEvent, m_link) CMovement_C::s_playerMoveEventFreeList;

// OFFSET: 0x6EBC70
CPlayerMoveEvent* CMovement_C::AllocPlayerMoveEvent(int32_t eventTime, uint32_t eventId) {
    CPlayerMoveEvent* moveEvent = s_playerMoveEventFreeList.Head();
    if (!moveEvent) {
        moveEvent = new (SMemAlloc(sizeof(CPlayerMoveEvent), __FILE__, __LINE__, 8)) CPlayerMoveEvent();
    } else {
        s_playerMoveEventFreeList.UnlinkNode(moveEvent);
    }

    moveEvent->m_eventTime = eventTime;
    moveEvent->m_eventId = eventId;
    moveEvent->unk_0050 = 0;
    return moveEvent;
}

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

// OFFSET: 0x6F1520
void CMovement_C::SetUpdateInfo(int32_t time, CClientMoveUpdate* update, uint32_t a4) {
    this->m_walkSpeed = update->m_walkSpeed;
    this->m_runSpeed = update->m_runSpeed;
    this->m_runBackSpeed = update->m_runBackSpeed;
    this->m_swimSpeed = update->m_swimSpeed;
    this->m_swimBackSpeed = update->m_swimBackSpeed;
    this->m_flightSpeed = update->m_flightSpeed;
    this->m_flightBackSpeed = update->m_flightBackSpeed;
    this->m_turnRate = update->m_turnRate;
    this->m_anchorElapsedMs = 0;
    this->m_pitchRate = update->m_pitchRate;
    this->m_flags = 0;
    //if ((v5->status.m_moveFlags & 0x8000000) != 0) {
    //    CMovement::CreateSplineAndSetDest(this, &this->position.x);
    //    CMovement_C::FlushMoveQueue(this, 0, 1);
    //    CMovement_C::RemoveFromMoversList(this, 0);
    //    CMoveSpline::CopyFrom(this->m_spline, &v5->m_moveSpline);
    //    m_spline = this->m_spline;
    //    m_spline->spline.unk_0000[112] = (m_spline->flags & 0x42000) != 0;
    //} else {
    //    v8 = this->m_spline;
    //    if (v8 && (v8->flags & 0x800) != 0)
    //        CGUnit_C::OnCollideFallLand(this->unit, 0, 1);
    //    CMovement::sub_98B730(this);
    //}
    //CMovement::sub_6EB730(this, a4, a2, v5, &update, a4, 1);
    //if ((&unk_C010FF & this->m_flags) != 0 || (m_next = this->m_moveQueue.m_terminator.m_next, (m_next & 1) == 0) && m_next) {
    //    if (!this->m_globalUnitsLink.m_next) {
    //        Globals = MovementGetGlobals();
    //        if (Globals)
    //            TSList::LinkToHead(&Globals->units, this);
    //    }
    //} else {
    //    CMovement_C::RemoveFromMoversList(this, 1);
    //}
    //if ((&unk_C0100F & this->m_flags) != 0 && *&this->unit->ObjectBase.m_obj->OBJECT_FIELD_GUID == CGUnit_C::m_activeMover)
    //    MovementGetGlobals()->timeCounterMaybe = a2 + 500;
    //if ((&unk_C010FF & this->m_flags) == 0) {
    //    if (a4) {
    //        CMovement::sub_6ECCF0(this, a2);
    //    } else if (CMovementShared::TryStartFalling(this)) {
    //        if (!this->m_globalUnitsLink.m_next) {
    //            v11 = MovementGetGlobals();
    //            if (v11)
    //                TSList::LinkToHead(&v11->units, this);
    //        }
    //    }
    //}
    //guid2 = this->guid2;
    //guid_low = guid2->guid_low;
    //guid_high = guid2->guid_high;
    //v15 = CGUnit_C::sub_71C4D0(this->unit);
    //MovementUpdateCameraYaw(guid_low, guid_high, v15, v16);
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
        this->m_unit->OnMoveUpdate(time, 0, 0);
        if (isnan(this->m_position.x) || isnan(this->m_position.y) || isnan(this->m_position.z)) {
            //ConsolePrintf("Mover at invalid position");
        }

        this->m_unit->m_modelFlags &= ~0x800000u;
        return;
    }

    uint32_t result = 0;
    bool completed = true;
    while (consumed < delta) {
        uint32_t step = delta - consumed;

        auto moveEvent = this->m_moveQueue.Head();
        if (moveEvent) {
            int32_t v8 = moveEvent->m_eventTime - cursor;
            if (v8 < 0)
                v8 = 0;
            if (step > v8)
                step = v8;
        }
    
        if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover && !this->IsOnSpline() && (this->m_flags & MOVEMASK_ANIMATING) != 0) {
            if (globals->timeCounterMaybe - cursor < step)
                step = globals->timeCounterMaybe - cursor;
        }
    
        cursor += step;
        globals->idleTime = cursor;
    
        if (step) {
            if ((this->m_flags & (MOVEMASK_ANIMATING | MOVEMENTFLAG_HOVER)) != 0) {
                if ((this->m_flags & MOVEMENTFLAG_ONTRANSPORT) != 0) {
                    globals->timeCounterMaybe += step;
                } else {
                    this->ApplyMovement(cursor, step);
                    //if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover && (this->m_flags & MOVEMASK_ANIMATING) != 0 && !this->IsOnSpline() && (cursor - globals->timeCounterMaybe) >= 0)
                    //    this->m_unit->SendMovementUpdate(cursor, 238, 0.0, 0, 0, 0, 255);
                }
            }
            consumed += step;
        }
    
        result = this->UpdatePlayerMovement(cursor);
        if (result != 1) {
            completed = false;
            break;
        }
    }
    
    if (!completed) {
        if (result) {
            if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover)
                globals->timeCounterMaybe += delta - consumed;
        } else {
            this->RemoveFromMoversList(1);
        }
    }

    this->m_unit->OnMoveUpdate(time, 0, 0);
    if (isnan(this->m_position.x) || isnan(this->m_position.y) || isnan(this->m_position.z)) {
        //ConsolePrintf("Mover at invalid position");
    }

    this->m_unit->m_modelFlags &= ~0x800000u;
}

// OFFSET: 0x6EF860
int32_t CMovement_C::UpdatePlayerMovement(int32_t time) {
    while (true) {
        CPlayerMoveEvent* moveEvent = this->m_moveQueue.Head();

        if (!moveEvent || time - moveEvent->m_eventTime < 0)
            break;

        //if ((this->m_unit->objectclass1[24] && (*(this->m_unit->objectclass1[24] + 16) & 0x200) != 0) || (moveEvent->m_eventId == 44 && CGUnit_C::MaybeDeferTeleport(&moveEvent->m_transportGuid))) {
        //    return 2;
        //}

        this->m_moveQueue.UnlinkNode(moveEvent);
        this->ukn29 = 0;

        uint32_t flags = this->m_flags;
        uint32_t id = moveEvent->m_eventId;
        bool isMoveEvent = (id >= 0 && id <= 5)  || id == 9 || id == 10 || id == 45 || id == 46;
        bool isSwimEvent = (id == 21 || id == 22);
        bool isKnockback = (id == 34);

        bool blocked = (isMoveEvent && (flags & (MOVEMENTFLAG_ONTRANSPORT | MOVEMENTFLAG_ROOT | MOVEMENTFLAG_PENDING_ROOT)) != 0)
                    || (isSwimEvent && (flags & (MOVEMENTFLAG_ONTRANSPORT | MOVEMENTFLAG_ROOT)) != 0)
                    || (isKnockback && (flags & MOVEMENTFLAG_ONTRANSPORT) != 0);

        if (blocked) {
            moveEvent->m_link.Unlink();
            s_playerMoveEventFreeList.LinkToTail(moveEvent);
            continue;
        }

        bool wasFalling = this->IsFalling();
        bool stoppedFalling = false;
        if (moveEvent->unk_0050) {
            if ((moveEvent->m_moveFlags & MOVEMENTFLAG_SPLINE_ENABLED) == 0) {
                if (this->m_spline && (this->m_spline->flags & SPLINE_FLAG_PARABOLIC) != 0) {
                    //this->m_unit->OnCollideFallLand(0, 1);
                }
                //this->sub_98B730();
            }
            if ((this->m_flags & MOVEMENTFLAG_FALLING) != 0 && (moveEvent->m_moveFlags & MOVEMENTFLAG_FALLING) == 0) {
                //this->StopFalling();
                stoppedFalling = true;
            }
        }

        bool updated = false;
        switch (moveEvent->m_eventId) {
        case 0:
            this->StartMove(1, 0);
            //updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_START_FORWARD, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 1:
            this->StartMove(0, 0);
            // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_START_BACKWARD, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 2:
            this->StopMove();
            // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_STOP, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 3:
            this->StartStrafe(1);
            // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_START_STRAFE_LEFT, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 4:
            this->StartStrafe(0);
            // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_START_STRAFE_RIGHT, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 5:
            this->StopStrafe();
            // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_STOP_STRAFE, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 6:
            if (this->StartAscensionDescension(1)) {
                // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_START_ASCEND, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            }
            break;
        case 7:
            if (this->StartAscensionDescension(0)) {
                // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_START_DESCEND, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            }
            break;
        case 8:
            if (this->StopAscensionDescension()) {
                // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_STOP_ASCEND, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            }
            break;
        case 9u:
            //if (!CMovementShared::TryStartFalling(this))
            //    break;
            //CGUnit_C::OnCollideFalling(this->unit);
            //if (!v4->m_needAck)
            //    goto LABEL_146;
            //CGUnit_C::SendMovementUpdate(this->unit, a2, MSG_MOVE_HEARTBEAT, 0.0, 0, 0, 0, 255);
            //updated = 1;
            break;
        case 10u:
            //if (CMovementShared::Jump(this, 1))
            //    updated = CGUnit_C::MoveEventHappened(this->unit, a2, MSG_MOVE_JUMP, v4->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 11:
            this->StartTurn(1);
            // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_START_TURN_LEFT, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 12:
            this->StartTurn(0);
            // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_START_TURN_RIGHT, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 13:
        case 50:
            this->StopTurn();
            // updated = this->m_unit->MoveEventHappened(time, MSG_MOVE_STOP_TURN, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            CGInputControl::GetActive()->OnTurnToAngleStop();
            break;
        }

        //if (moveEvent->m_needAck && !updated && (&unk_C0100F & this->m_flags) != 0 && (&unk_C0100F & v45) == 0)
        //    CMovement::sub_6E9B70(this, a2);
        //if (moveEvent->m_eventId != 44) {
        //    this->unit->unk_0A30 |= 0x20000000u;
        //    if (v4->unk_0050 && CMovement_C::HeartBeat(this, v4) && v46)
        //        CGUnit_C::OnCollideFallLand(this->unit, v45, IsFalling);
        //    this->unit->unk_0A30 &= ~0x20000000u;
        //}
        //CMovement_C::SetInterpolation(this, a2);

        moveEvent->m_link.Unlink();
        s_playerMoveEventFreeList.LinkToTail(moveEvent);
    }
    return this->m_moveQueue.Head() || (this->m_flags & MOVEMASK_ANIMATING) != 0;
}

// OFFSET: 0x6EAC40
void CMovement_C::ApplyMovement(uint32_t a2, uint32_t a3) {
    if (!World::IsValidPosition(this->m_position.x, this->m_position.y, this->m_position.z, 2.0f))
        return;
    if (!a3)
        return;

    C3Vector dst;
    if (this->IsOnSpline())
        dst = this->m_spline->m_finalDestination;

    int32_t v24 = 0;
    bool updateAnchors = false;
    while (true) {
        if ((this->m_flags & 0xC010FF) == 0 && ((this->m_flags & MOVEMENTFLAG_HOVER) == 0 || (this->m_flags & MOVEMENTFLAG_ROOT) != 0))
            break;

        C3Vector vec;

        int32_t delta = a3 - v24;
        this->m_anchorElapsedMs += delta;
        if (this->IsOnSpline()) {
            //if (!this->PlotUnitSplineMovement(this, a2, delta, dst))
            //    return;
            //vec.x = dst.x - this->m_anchorPos.x;
            //vec.y = dst.y - this->m_anchorPos.y;
            //vec.z = dst.z - this->m_anchorPos.z;
        } else {
            int32_t moveStart = this->GetMoveStartTime(delta);
            if (!this->PlotUnitMovement(moveStart, &vec) && (this->m_flags & MOVEMENTFLAG_HOVER) == 0)
                return;

            if ((this->m_flags2 & 0x1C00) != 0) {
                C3Vector v18;
                v18.x = this->m_anchorPos.x + vec.x;
                v18.y = this->m_anchorPos.y + vec.y;
                v18.z = this->m_anchorPos.z + vec.z;
                if (this->Interpolate(a2 - delta, delta, &v18, &this->m_facing, &this->m_pitch)) {
                    updateAnchors = 1;
                    vec.x = v18.x - this->m_anchorPos.x;
                    vec.y = v18.y - this->m_anchorPos.y;
                    vec.z = v18.z - this->m_anchorPos.z;
                }
            }
        }
        int32_t v10 = this->RequestMove(a2 - delta, delta, &vec);
        v24 += v10;
        if (updateAnchors)
            this->UpdateAnchors(0);

        if (v24 >= a3)
            break;
    }

    //if (CMovement::IsValidPosition(this)) {
    //    v11 = this->m_spline;
    //    if (v11 && (v11->flags & 0x400) == 0)
    //        CMovement_C::SnapToSpline(this, &dst, 0);
    //    if (CMovement::sub_4F5260(this))
    //        CMovementShared::UpdateAnchors(this, 0);
    //}
}

// OFFSET: 0x6EAF50
void CMovement_C::RemoveFromMoversList(bool a2) {
    if (!this->m_globalUnitLink.IsLinked() || (this->m_flags & MOVEMASK_ANIMATING) != 0)
        return;

    auto moveEvent = this->m_moveQueue.Head();
    float hoverHeight;
    if (!moveEvent && ((this->m_flags & MOVEMENTFLAG_HOVER) == 0 || this->GetCurrentHoverHeight(&hoverHeight, 0, 0) && !CMath::fnotequal(hoverHeight, this->m_hoverHeight))) {
        if (this->m_spline && (this->m_spline->flags & SPLINE_FLAG_NO_SPLINE) == 0) {
            if (a2) {
                auto globals = MovementGetGlobals();
                this->OnSplineStop(globals->idleTime);
                if ((this->m_flags & MOVEMASK_ANIMATING) != 0)
                    return;
            } else {
                this->m_spline->flags |= SPLINE_FLAG_NO_SPLINE;
            }
        }
        this->m_globalUnitLink.Unlink();
    }
}

// OFFSET: 0x75F520
bool CMovement_C::GetCurrentHoverHeight(float* height, bool* a3, uint32_t* a4) {
    // TODO
    *height = 0.0f;
    if (a3)
        *a3 = 0;
    if (a4)
        *a4 = 0;
    return true;
}

// OFFSET: 0x6EAE70
void CMovement_C::OnSplineStop(uint32_t time) {
    this->m_spline->flags |= SPLINE_FLAG_NO_SPLINE;
    //if ((this->m_spline->flags & 0x800) != 0)
    //    CGUnit_C::OnCollideFallLand(this->unit, 0, 1);
    //if ((this->m_spline->flags & 0xA00) != 0)
    //    CMovementShared::StopFalling(this);
    //CMovement::ToggleMovementFlag2_0x80(this, 0);
    //unit = this->unit;
    //if (*&unit->ObjectBase.m_obj->OBJECT_FIELD_GUID == CGUnit_C::m_activeMover) {
    //    m_spline = this->m_spline;
    //    m_id = m_spline->m_id;
    //    if (m_spline && (m_spline->flags & 0x800) != 0)
    //        CGUnit_C::OnCollideFallLand(unit, 0, 1);
    //    CMovement::sub_98B730(this);
    //    if (CMovementShared::TryStartFalling(this) && !this->m_globalUnitsLink.m_next) {
    //        Globals = MovementGetGlobals();
    //        if (Globals)
    //            TSList::LinkToHead(&Globals->units, this);
    //    }
    //    CGUnit_C::SendSplineDone(&this->unit->ObjectBase.__vftable, a2, m_id);
    //}
}

// OFFSET: 0x5FEDE0
bool CMovement_C::IsFalling() {
    return (this->m_flags & MOVEMENTFLAG_FALLING) != 0 && 0.0 != this->m_fallVelocity;
}

// OFFSET: 0x6E9440
bool CMovement_C::IsValidPosition() {
    return World::IsValidPosition(this->m_position.x, this->m_position.y, this->m_position.z, 2.0f);
}

// OFFSET: 0x6ECB50
void CMovement_C::OnMoveStartLocal(int32_t eventTime, bool forward) {
    AddPlayerMoveEvent(eventTime, forward ? 0 : 1, true, 0, 0.0f, 0.0f, 0);
}

// OFFSET: 0x6ECDE0
void CMovement_C::OnMoveStopLocal(int32_t eventTime) {
    AddPlayerMoveEvent(eventTime, 2, true, 0, 0.0f, 0.0f, 0);
}

// OFFSET: 0x6ECBB0
void CMovement_C::OnStrafeStartLocal(int32_t eventTime, bool left) {
    AddPlayerMoveEvent(eventTime, left ? 3 : 4, true, 0, 0.0f, 0.0f, 0);
}

// OFFSET: 0x6ECE40
void CMovement_C::OnStrafeStopLocal(int32_t eventTime) {
    AddPlayerMoveEvent(eventTime, 5, true, 0, 0.0f, 0.0f, 0);
}

// OFFSET: 0x6EF2A0
void CMovement_C::OnAscendDescendStartLocal(int32_t eventTime, bool up) {
    AddPlayerMoveEvent(eventTime, up ? 6 : 7, true, 0, 0.0f, 0.0f, 0);
}

// OFFSET: 0x6EF310
void CMovement_C::OnAscendDescendStopLocal(int32_t eventTime) {
    AddPlayerMoveEvent(eventTime, 8, true, 0, 0.0f, 0.0f, 0);
}

// OFFSET: 0x6F1310
void CMovement_C::OnPitchStartLocal(int32_t eventTime, bool up) {
    // TODO
}

// OFFSET: 0x6EECA0
void CMovement_C::OnPitchStopLocal(int32_t eventTime) {
    // TODO
}

// OFFSET: 0x6F0F70
void CMovement_C::OnTurnStartLocal(int32_t eventTime, bool left) {
    //if ((CMovement_C::ComputeLegalRawFacingRange)(&v5, &v6)) {
    //    if (a3)
    //        CMovement_C::OnTurnToAngleLocal(this, a2, v6);
    //    else
    //        CMovement_C::OnTurnToAngleLocal(this, a2, v5);
    //} else {
    this->AddPlayerMoveEvent(eventTime, 12 - (left != 0), 1, 0, 0.0, 0.0, 0);
    this->UnlinkMoveEventById(&this->m_moveQueue, 50);
    CGInputControl::GetActive()->OnTurnToAngleStop();
    //}
}

// OFFSET: 0x6ECEA0
void CMovement_C::OnTurnStopLocal(int32_t eventTime) {
    this->AddPlayerMoveEvent(eventTime, 13, 1, 0, 0.0, 0.0, 0);
    this->UnlinkMoveEventById(&this->m_moveQueue, 50);
}

void CMovement_C::AddPlayerMoveEvent(int32_t eventTime, uint32_t eventId, bool needAck, int32_t ackCounter, float facing, float pitch, uint16_t flags) {
    CPlayerMoveEvent* moveEvent = AllocPlayerMoveEvent(eventTime, eventId);
    moveEvent->m_facing = facing;
    moveEvent->m_pitch = pitch;
    moveEvent->m_needAck = needAck;
    moveEvent->m_ackCounter = ackCounter;
    moveEvent->m_moveExtraFlags = flags;
    this->m_moveQueue.LinkToTail(moveEvent);
    if (!this->m_globalUnitLink.IsLinked()) {
        auto globals = MovementGetGlobals();
        if (globals)
            globals->m_movementUnits.LinkToHead(this);
    }
}

// OFFSET: 0x6EB590
void CMovement_C::UnlinkMoveEventById(STORM_EXPLICIT_LIST(CPlayerMoveEvent, m_link)* list, uint32_t eventId) {
    for (CPlayerMoveEvent* event = list->Head(); event;) {
        auto next = list->Next(event);

        if (event->m_eventId == eventId) {
            event->m_link.Unlink();
            s_playerMoveEventFreeList.LinkToTail(event);
            return;
        }

        event = next;
    }
}

// OFFSET: 0x6E9E20
int32_t CMovement_C::RequestMove(int32_t a2, int32_t a3, C3Vector* a4) {
    C3Vector v16;
    v16.x = this->m_anchorPos.x + a4->x;
    v16.y = this->m_anchorPos.y + a4->y;
    v16.z = this->m_anchorPos.z + a4->z;
    if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover || !this->m_spline || (this->m_unit->m_modelFlags & 0x800000) != 0) {
        //v16 -=  this->m_position;

        //####TESTING
        this->m_position = v16;
        this->GetPosition(&v16, &this->m_position);
        return a3;
        //#############
        //return this->CollideRequestMove(a2, a3, v16.x, v16.y, v16.z);
    } else {
        //if ((this->m_spline->flags & 0x200) == 0)
        //    this->StopFalling();
        this->m_anchorElapsedMs += a3;
        this->m_position = v16;
        this->GetPosition(&v16, &this->m_position);
        
        return a3;
    }
}

bool CMovement_C::Interpolate(int32_t now, int32_t time, C3Vector* pos, float* facing, float* pitch) {
    auto moveEvent = this->m_moveQueue.Head();
    if (!moveEvent || moveEvent->m_eventTime - now < 0) {
        this->m_flags2 &= ~0x1C00;
        return false;
    }

    float delta = moveEvent->m_eventTime - now;
    float t = delta / this->m_interpolation;

    //v9 = delta / this->m_interpolation;
    //v21 = v9;
    //v10 = 1.0 - v9;
    //nowb = 1.0 - v9;
    //if ((this->m_flags2 & 0x400) != 0) {
    //    v11 = (m_next->m_position.x - this->m_interpolationPos.x * v9) * v10 + pos->x * v9;
    //    v12 = (m_next->m_position.y - this->m_interpolationPos.y * v9) * v10 + pos->y * v9;
    //    v20.z = v10 * (m_next->m_position.z - this->m_interpolationPos.z * v9) + pos->z * v9;
    //    v13 = v11 - this->position.x;
    //    if (v13 * v13 + (v12 - this->position.y) * (v12 - this->position.y) + 0.00000095367432 <= time * 0.001 * 60.0 * (time * 0.001 * 60.0)) {
    //        v20.x = v11;
    //        v20.y = v12;
    //        *pos = v20;
    //    } else {
    //        LOWORD(this->m_flags2) &= ~0x400u;
    //    }
    //}
    //if ((this->m_flags2 & 0x800) != 0) {
    //    v14 = m_next->m_facing - v9 * *&this->m_interpolationFacing;
    //    v15 = CMath::normalizeangle0to2pi_(v14) - *facing;
    //    v16 = CMath::normalizeAngleNegPiToPi_(v15) * nowb + *facing;
    //    *facing = CMath::normalizeangle0to2pi_(v16);
    //    v9 = v21;
    //}
    //if ((this->m_flags2 & 0x1000) != 0) {
    //    v17 = m_next->m_pitch - v9 * *&this->m_interpolationPitch;
    //    v18 = CMath::normalizeangle0to2pi_(v17) - *pitch;
    //    v19 = CMath::normalizeAngleNegPiToPi_(v18) * nowb + *pitch;
    //    *pitch = CMath::normalizeangle0to2pi_(v19);
    //}

    return this->m_flags2 & MOVEMENTFLAG2_INTERPOLATED_MOVEMENT;
}

// OFFSET: 0x6E9F50
int32_t CMovement_C::GetMoveStartTime(int32_t elapsed) {
    if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover)
        return this->m_anchorElapsedMs;

    if ((this->m_flags & 0xC0100F) == 0)
        return this->m_anchorElapsedMs;

    this->ukn29 += elapsed;

    if (this->ukn29 <= 500)
        return this->m_anchorElapsedMs;

    int32_t lead = this->ukn29 - 1500;
    if (lead < 0)
        lead = 0;

    int32_t t = this->ukn29 - 500;
    if (t > 1000)
        t = 1000;

    int32_t result = this->m_anchorElapsedMs - lead;
    if (result < 0)
        result = 0;

    result -= (t * t) / 1000;

    return result < 0 ? 0 : result;
}
