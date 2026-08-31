#include "clientobject/Movement_C.hpp"
#include "clientobject/Movement.hpp"
#include "clientobject/Unit_C.hpp"
#include "clientobject/Player_C.hpp"
#include "clientobject/Passenger.hpp"
#include <tempest/math/CMath.hpp>
#include "world/World.hpp"
#include "clientobject/CClientMoveUpdate.hpp"
#include "gameui/CGInputControl.hpp"
#include <util/Unimplemented.hpp>

STORM_EXPLICIT_LIST(CPlayerMoveEvent, m_link) CMovement_C::s_playerMoveEventFreeList;
World::FacetData CMovement_C::s_moveFacets;
World::FacetData CMovement_C::s_liquidFacets;
CAaBox CMovement_C::s_queryBox;

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
    this->m_collisionRadius = 0.33333334f;
    this->m_collisionHeight = 2.0277777f;
    this->m_stepUpHeight = 1.0f;
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
                this->StopFalling();
                stoppedFalling = true;
            }
        }

        bool updated = false;
        switch (moveEvent->m_eventId) {
        case 0:
            this->StartMove(1, 0);
            //updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_START_FORWARD, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 1:
            this->StartMove(0, 0);
            // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_START_BACKWARD, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 2:
            this->StopMove();
            // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_STOP, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 3:
            this->StartStrafe(1);
            // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_START_STRAFE_LEFT, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 4:
            this->StartStrafe(0);
            // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_START_STRAFE_RIGHT, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 5:
            this->StopStrafe();
            // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_STOP_STRAFE, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 6:
            if (this->StartAscensionDescension(1)) {
                // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_START_ASCEND, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            }
            break;
        case 7:
            if (this->StartAscensionDescension(0)) {
                // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_START_DESCEND, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            }
            break;
        case 8:
            if (this->StopAscensionDescension()) {
                // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_STOP_ASCEND, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            }
            break;
        case 9u:
            //if (!this->TryStartFalling())
            //    break;
            //CGUnit_C::OnCollideFalling(this->unit);
            //if (!v4->m_needAck)
            //    goto LABEL_146;
            //CGUnit_C::SendMovementUpdate(this->unit, a2, MSG_MOVE_HEARTBEAT, 0.0, 0, 0, 0, 255);
            //updated = 1;
            break;
        case 10u:
            if (this->Jump(1)) {
                //updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_JUMP, v4->m_needAck, 0.0, 0, 0, 0, 255);
            }
            break;
        case 11:
            this->StartTurn(1);
            // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_START_TURN_LEFT, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 12:
            this->StartTurn(0);
            // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_START_TURN_RIGHT, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            break;
        case 13:
        case 50:
            this->StopTurn();
            // updated = this->m_unit->ProcessLocalMoveEvent(time, MSG_MOVE_STOP_TURN, moveEvent->m_needAck, 0.0, 0, 0, 0, 255);
            CGInputControl::GetActive()->OnTurnToAngleStop();
            break;
        //case 14u:
        //    CMovementShared::StartPitch(this, 1);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_START_PITCH_UP, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    break;
        //case 15u:
        //    CMovementShared::StartPitch(this, 0);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_START_PITCH_DOWN, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    break;
        //case 16u:
        //case 51u:
        //    CMovementShared::StopPitch(this);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_STOP_PITCH, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    v23 = CGInputControl::GetActive();
        //    CGInputControl::OnPitchToAngleStop(v23);
        //    break;
        //case 17u:
        //    CMovementShared::SetRunMode(this, 1);
        //    CGUnit_C::UpdateObjectEffectMovementStates(this->unit);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_SET_RUN_MODE, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    goto LABEL_49;
        //case 18u:
        //    CMovementShared::SetRunMode(this, 0);
        //    CGUnit_C::UpdateObjectEffectMovementStates(this->unit);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_SET_WALK_MODE, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    break;
        //case 19u:
        //    CMovementShared::SetRawFacing(this, v4->m_facing);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_SET_FACING, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    break;
        //case 20u:
        //    updated = CMovement_C::UpdatePitch(this, a2, v4);
        //    break;
        //case 21u:
        //    CMovementShared::StartSwim(this);
        //    CMovement_C::HandlePendingActions(this);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_START_SWIM, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    break;
        //case 22u:
        //    CMovementShared::StopSwim(this);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_STOP_SWIM, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    goto LABEL_49;
        //case 23u:
        //    CMovementShared::ChangeRunSpeed(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_RUN_SPEED_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 24u:
        //    CMovementShared::ChangeRunBackSpeed(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 25u:
        //    CMovementShared::ChangeWalkSpeed(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_WALK_SPEED_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 26u:
        //    CMovementShared::ChangeSwimSpeed(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_SWIM_SPEED_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 27u:
        //    CMovementShared::ChangeSwimBackSpeed(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 28u:
        //    CMovementShared::ChangeFlightSpeed(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 29u:
        //    CMovementShared::ChangeFlightBackSpeed(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 30u:
        //    CMovementShared::ChangeTurnRate(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_TURN_RATE_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 31u:
        //    CMovementShared::ChangePitchRate(this, v4->m_value);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_PITCH_RATE_CHANGE_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 32u:
        //    if (CMovementShared::EnableGravity(this, 1))
        //        CMovement_C::GravityStateChanged(this);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_GRAVITY_ENABLE_ACK, v4->m_needAck, 0.0, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 33u:
        //    if (CMovementShared::EnableGravity(this, 0))
        //        CMovement_C::GravityStateChanged(this);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_GRAVITY_DISABLE_ACK, v4->m_needAck, 0.0, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 34u:
        //    CMovement::sub_6E9FF0(this, &v4->m_value, *&v4->unk_0044, *&v4->unk_0048);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_KNOCK_BACK_ACK, v4->m_needAck, 0.0, v4->m_ackCounter, 0, 0, 255);
        //    break;
        //case 35u:
        //    CMovementShared::FeatherFall(this, 1);
        //    v24 = 1.0;
        //    goto LABEL_97;
        //case 36u:
        //    CMovementShared::FeatherFall(this, 0);
        //    v24 = 0.0;
//LABEL_97:
        //    v33 = v24;
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_FEATHER_FALL_ACK, v4->m_needAck, v33, v4->m_ackCounter, 0, 0, 255);
        //    CMovementShared::PostFeatherFall(this);
        //    CGUnit_C::OnCollideFalling(this->unit);
        //    break;
        //case 37u:
        //    CMovementShared::Hover(this, 1);
        //    v25 = 1.0;
        //    goto LABEL_100;
        //case 38u:
        //    if (!v4->m_needAck || CGUnit_C::IsActiveMover(&this->unit->ObjectBase.__vftable)) {
        //        CMovement_C::UpdateHoverState(this, 0, 1);
        //        v25 = 0.0;
        //    } else {
        //        CMovement_C::UpdateHoverState(this, 0, 0);
        //        v25 = 0.0;
        //    }
//LABEL_100:
        //    v34 = v25;
        //    v26 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_HOVER_ACK, v4->m_needAck, v34, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_101;
        //case 39u:
        //    CMovementShared::WalkOnWater(this, 1);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_WATER_WALK_ACK, v4->m_needAck, 1.0, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 40u:
        //    CMovementShared::WalkOnWater(this, 0);
        //    if (!v4->m_needAck || CGUnit_C::IsActiveMover(&this->unit->ObjectBase.__vftable))
        //        CMovementShared::TryStartFalling(this);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_WATER_WALK_ACK, v4->m_needAck, 0.0, v4->m_ackCounter, 0, 0, 255);
        //    goto LABEL_49;
        //case 41u:
        //    if (v4->m_needAck && CMovementShared::IsOnSpline(this))
        //        CMovement::sub_6E9F10(this, 0);
        //    v28 = CMovement::IsFalling(this);
        //    v37 = this->m_flags;
        //    v40 = v28;
        //    m_flags2_low = LOWORD(this->m_flags2);
        //    CMovement::sub_98C4F0(this);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_MOVE_ROOT_ACK, v4->m_needAck, 0.0, v4->m_ackCounter, 0, 0, 255);
        //    CMovement_C::CallMoveEventHandlers(this, a2, 0, v37, m_flags2_low, v40, 0);
        //    break;
        //case 42u:
        //    v38 = CMovement::IsFalling(this);
        //    v44 = this->m_flags;
        //    v41 = LOWORD(this->m_flags2);
        //    v29 = !v4->m_needAck || CGUnit_C::IsActiveMover(&this->unit->ObjectBase.__vftable);
        //    CMovementShared::UnRoot(this, v29);
        //    CGUnit_C::UnRootEffects(&this->unit->ObjectBase.__vftable);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_MOVE_UNROOT_ACK, v4->m_needAck, 0.0, v4->m_ackCounter, 0, 0, 255);
        //    CMovement_C::CallMoveEventHandlers(this, a2, 0, v44, v41, v38, 0);
        //    break;
        //case 44u:
        //    LOWORD(this->m_flags2) ^= (LOWORD(this->m_flags2) ^ v4->m_moveExtraFlags) & 0x2040;
        //    if (v4->unk_0050)
        //        CMovement_C::HeartBeat(this, v4);
        //    CMovement_C::Teleport(this, *&v4->m_transportGuid, &v4->m_position.x, v4->m_facing, 1, v4->m_needAck, v4->m_seat);
        //    v26 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, MSG_MOVE_TELEPORT_ACK, v4->m_needAck, 0.0, v4->m_ackCounter, 0, 0, 255);
//LABEL_101:
        //    updated = v26;
        //    break;
        //case 45u:
        //    v39 = this->m_flags;
        //    v42 = LOWORD(this->m_flags2);
        //    if (CMovement::sub_6EBC50(this)) {
        //        updated = CMovement_C::FallStateChangedLocal(a2, 838, v39, v42, v4->m_needAck, 0.0);
        //        CGUnit_C::UpdateObjectEffectMovementStates(this->unit);
        //    }
        //    break;
        //case 46u:
        //    CMovement::DisableFlying(this);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_SET_FLY, v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    goto LABEL_49;
        //case 47u:
        //    this->m_flags |= 0x1000000u;
        //    v27 = 1.0;
        //    goto LABEL_107;
        //case 48u:
        //    if ((this->m_flags & 0x2000000) != 0)
        //        CMovement::DisableFlying(this);
        //    this->m_flags &= ~0x1000000u;
        //    v27 = 0.0;
//LABEL_107:
        //    v35 = v27;
        //    CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_SET_CAN_FLY_ACK, v4->m_needAck, v35, v4->m_ackCounter, 0, 0, 255);
        //    break;
        //case 49u:
        //    v30 = this->m_flags;
        //    if ((v30 & 0x200) != 0) {
        //        this->m_flags = v30 & 0xFFFFFDFF;
        //        if (CMovementShared::TryStartFalling(this))
        //            CGUnit_C::OnCollideFalling(this->unit);
        //        updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_FORCE_MOVE_UNROOT_ACK, 0, 0.0, 0, 0, 0, 255);
        //        CMovement::sub_6E9B70(this, a2);
        //    }
        //    SendTimeSyncResp(v4->m_eventTime, v4->m_ackCounter);
        //    break;
        //case 52u:
        //    CMovement_C::Halt(this);
        //    v19 = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_DISMISS_CONTROLLED_VEHICLE, v4->m_needAck, 0.0, 0, 0, 0, 255);
//LABEL_49:
        //    updated = v19;
        //    break;
        //case 53u:
        //    v20 = maybe_CMovement_C__QueueConstrainedTurnEvent(this, a2, v4->m_facing);
        //    CMovementShared::StartTurn(this, v20);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, (189 - (v20 != 0)), v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    break;
        //case 54u:
        //    v22 = maybe_CMovement_C__QueueTurnEvent(this, a2, v4->m_pitch);
        //    CMovementShared::StartPitch(this, v22);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, (192 - (v22 != 0)), v4->m_needAck, 0.0, 0, 0, 0, 255);
        //    break;
        //case 55u:
        //    CMovement_C::Halt(this);
        //    updated = CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE, v4->m_needAck, 0.0, 0, v4->m_transportGuid.guid_low, v4->m_transportGuid.guid_high, v4->m_seat);
        //    break;
        //case 56u:
        //    LOWORD(this->m_flags2) |= MOVEMENTFLAG2_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY;
        //    CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY_ACK, v4->m_needAck, 1.0, v4->m_ackCounter, 0, 0, 255);
        //    break;
        //case 57u:
        //    LOWORD(this->m_flags2) &= ~0x4000u;
        //    CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY_ACK, v4->m_needAck, 0.0, v4->m_ackCounter, 0, 0, 255);
        //    break;
        //case 58u:
        //    CMovement::sub_6E9600(this, v4->m_value);
        //    CGUnit_C::ProcessLocalMoveEvent(this->unit, a2, CMSG_MOVE_SET_COLLISION_HGT_ACK, v4->m_needAck, v4->m_value, v4->m_ackCounter, 0, 0, 255);
        //    break;
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

    if (this->IsValidPosition()) {
    //    v11 = this->m_spline;
    //    if (v11 && (v11->flags & 0x400) == 0)
    //        CMovement_C::SnapToSpline(this, &dst, 0);
        if (this->IsOnFlyingSpline())
            this->UpdateAnchors(0);
    }
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

// OFFSET: 0x6EB3B0
int32_t CMovement_C::HandlePendingActions() {
    uint32_t previousFlags = this->m_flags;
    if ((this->m_flags & MOVEMENTFLAG_PENDING_STOP) != 0) {
        this->ForceStopMove(1);
        if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover && !this->HasMoveEventBetween(0, 1)) {
            CGInputControl::GetActive()->UpdateMoveStopped();
        }
    }
    if ((this->m_flags & MOVEMENTFLAG_PENDING_STRAFE_STOP) != 0)
        this->ForceStopStrafe();
    if ((this->m_flags & (MOVEMENTFLAG_PENDING_BACKWARD | MOVEMENTFLAG_PENDING_FORWARD)) != 0)
        this->StartMove(this->m_flags & MOVEMENTFLAG_PENDING_FORWARD, 0);
    if ((this->m_flags & (MOVEMENTFLAG_PENDING_STRAFE_RIGHT | MOVEMENTFLAG_PENDING_STRAFE_LEFT)) != 0)
        this->StartStrafe(this->m_flags & MOVEMENTFLAG_PENDING_STRAFE_LEFT);
    this->m_flags &= 0xFFE03FFF;
    return (previousFlags ^ this->m_flags) & 0xF;
}

// OFFSET: 0x6EAC00
bool CMovement_C::HasMoveEventBetween(uint32_t minEventId, uint32_t maxEventId) {
    for (CPlayerMoveEvent* event = this->m_moveQueue.Head(); event; event = this->m_moveQueue.Next(event)) {
        if (event->m_eventId >= minEventId && event->m_eventId <= maxEventId)
            return true;
    }

    return false;
}

// OFFSET: 0x6E9E20
int32_t CMovement_C::RequestMove(int32_t a2, int32_t a3, C3Vector* a4) {
    C3Vector v16;
    v16.x = this->m_anchorPos.x + a4->x;
    v16.y = this->m_anchorPos.y + a4->y;
    v16.z = this->m_anchorPos.z + a4->z;
    if (this->m_unit->m_obj->m_guid == CGUnit_C::s_activeMover || !this->m_spline || (this->m_unit->m_modelFlags & 0x800000) != 0) {
        v16 -=  this->m_position;
        return this->CollideRequestMove(a2, a3, &v16);
    } else {
        if ((this->m_spline->flags & 0x200) == 0)
            this->StopFalling();
        this->m_anchorElapsedMs += a3;
        this->m_position = v16;
        this->GetPosition(&v16, &this->m_position);
        
        return a3;
    }
}

// OFFSET: 0x762E00
int32_t CMovement_C::CollideRequestMove(int32_t a2, int32_t a3, C3Vector* a4) {
    if (!a3)
        return 0;

    C3Vector newPosition = this->m_position + *a4;
    if (!World::IsValidPosition(newPosition.x, newPosition.y, newPosition.z, this->m_collisionRadius + 71.375595f))
        return a3;

    float deltaZ = this->CanCollideWhileFlying() ? a4->z : 0.0f;
    float length = sqrtf(a4->y * a4->y + a4->x * a4->x + deltaZ * deltaZ);
    float speed = length / (a3 * 0.001f);

    float dirX = 0.0f;
    float dirY = 0.0f;
    float dirZ = 0.0f;

    if (fabsf(length) >= 0.00000095367432f) {
        dirX = a4->x * (1.0f / length);
        dirY = a4->y * (1.0f / length);
        dirZ = (1.0f / length) * deltaZ;
    }

    uint32_t consumed = 0;
    bool splineSkip = false;

    while (true) {
        uint32_t flags = this->m_flags;
        if ((flags & (MOVEMASK_MOVING_FALL | MOVEMENTFLAG_HOVER)) == 0 || (flags & MOVEMENTFLAG_ROOT) != 0)
            break;

        int32_t remaining = a3 - consumed;
        int32_t time = a2 + consumed;
        float distance = remaining * 0.001f * speed;

        if (!this->GetMoveFacets(distance, remaining, dirX, dirY, dirZ)) {
            if (this->IsOnSpline()) {
                //this->MoveSplineMoverWithoutCollision(&newPosition, a2, a3);
                splineSkip = true;
            } else {
                //this->SkipTime(remaining);
                this->m_anchorElapsedMs -= remaining;
            }

            consumed = a3;
            break;
        }

        int32_t wasFalling = this->IsFalling();
        WGUID savedTransport = this->m_transportGuid;
        float savedSpeed = this->m_currentSpeed;

        int32_t step;
        if (this->CanCollideWhileFlying()) {
            step = this->Swim(time, remaining, distance, dirX, dirY, dirZ);
        } else if ((this->m_flags & MOVEMENTFLAG_FALLING) != 0) {
            C2Vector dir2D(dirX, dirY);
            step = this->Fall(time, remaining, distance, &dir2D);
        } else if ((this->m_flags & MOVEMENTFLAG_HOVER) != 0) {
            step = this->HoverMove(time, remaining, distance, dirX, dirY, dirZ);
        } else {
            C2Vector dir2D(dirX, dirY);
            step = this->TraceSurface(time, remaining, distance, &dir2D);
        }

        consumed += step;

        bool transportChanged = savedTransport != this->m_transportGuid;
        //this->CallMoveEventHandlers(a2 + consumed, a3 - consumed, this->m_flags, this->m_flags2, wasFalling, transportChanged);

        if (transportChanged || ((this->m_flags & MOVEMENTFLAG_FALLING) == 0 && (this->m_flags & MOVEMENTFLAG_FALLING) != 0)) {
            uint32_t unspent = remaining - step;
            if (this->m_anchorElapsedMs >= unspent)
                this->m_anchorElapsedMs -= unspent;
            else
                this->m_anchorElapsedMs = 0;
            break;
        }

        if (!this->IsOnSpline() && this->m_currentSpeed != savedSpeed) {
            this->CalcCurrentSpeed(0);
            break;
        }

        if (!this->m_anchorElapsedMs && (this->m_flags & MOVEMASK_TRANSLATE) != 0)
            this->m_anchorElapsedMs = a3 - consumed;
        if (consumed >= a3)
            break;
    }

    //if (!splineSkip) {
    //    if (!this->m_spline || (this->m_spline->flags & SPLINE_FLAG_NO_SPLINE) != 0 || (this->m_spline->flags & SPLINE_FLAG_FLYING) == 0)
    //        this->GroundNormal();
    //}

    this->GetPosition(&newPosition, &this->m_position);

    return consumed;
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

// OFFSET: 0x6E8FC0
float CMovement_C::GetStepUpHeight() {
    if (this->m_unit->IsClientControlled())
        return this->m_stepUpHeight;

    return 2.0f;
}

// OFFSET: 0x6E9520
float CMovement_C::GetRemainingStepUpHeight() {
    float height = 0.0f;
    if (this->m_unit->IsClientControlled())
        height = this->m_stepUpHeight;
    else
        height = 2.0f;

    if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) != 0)
        height -= this->m_position.z - this->m_stepUpStartZ;

    if (height <= 0.0f)
        return 0.0f;

    return height;
}

// OFFSET: 0x75CD00
void CMovement_C::BuildCollisionBox(C3Vector* position, CAaBox* box) {
    box->b = *position;
    box->t = *position;

    box->b.x = box->b.x - this->m_collisionRadius;
    box->b.y = box->b.y - this->m_collisionRadius;
    box->t.x = box->t.x + this->m_collisionRadius;
    box->t.y = box->t.y + this->m_collisionRadius;
    box->t.z = this->m_collisionHeight + box->t.z;
}

// OFFSET: 0x75E3D0
int32_t CMovement_C::GetFacetQueryFlags() {
    int32_t flags = 0x100111;
    if (!this->m_unit->IsClientControlled())
        flags = 0x102111;
    if (this->m_unit->IsLocalClientControlled())
        flags |= 0x80000000;

    if ((this->m_flags & MOVEMENTFLAG_WATERWALKING) != 0 && (this->m_flags & MOVEMENTFLAG_SWIMMING) == 0 && (this->m_pitch > -0.6457718f || (this->m_flags2 & MOVEMENTFLAG2_UNK10) != 0))
        flags |= 0x10000u;

    if ((this->m_flags & MOVEMENTFLAG_FLYING) != 0) {
        flags |= 0x200u;
        if ((this->m_flags2 & MOVEMENTFLAG2_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY) == 0)
            flags |= 0x20000u;
    }

    if ((this->m_unit->m_obj->m_type & TYPEMASK_PLAYER) != 0 && (this->m_unit->AsPlayer()->m_player->PLAYER_FLAGS & 0x10) != 0)
        return flags | 0x8000;

    return flags;
}

// OFFSET: 0x75FF90
int32_t CMovement_C::GetMoveFacets(float distance, int32_t deltaMs, float dirX, float dirY, float dirZ) {
    C44Matrix transportMat;
    C3Vector position = this->m_position;

    float x = dirX;
    float y = dirY;
    float z = dirZ;

    if (this->m_transportGuid) {
        MovementGetTransportMtxX(this->m_transportGuid, &transportMat);
        
        position = transportMat.TransformPoint(position);
        
        float rx = transportMat.c0 * z + transportMat.b0 * y + transportMat.a0 * x;
        float ry = z * transportMat.c1 + y * transportMat.b1 + transportMat.a1 * x;
        float rz = z * transportMat.c2 + y * transportMat.b2 + x * transportMat.a2;
        
        x = rx;
        y = ry;
        z = rz;
    }

    World::IsValidPosition(position.x, position.y, position.z, 0.0f);

    CAaBox collisionBox;
    collisionBox.b = { 0.0f, 0.0f, 0.0f };
    collisionBox.t = { 0.0f, 0.0f, 0.0f };
    this->BuildCollisionBox(&position, &collisionBox);

    CMovement_C::s_queryBox = collisionBox;

    C3Vector step;
    step.x = x * distance;
    step.y = y * distance;
    step.z = z * distance;

    if ((this->m_flags & MOVEMENTFLAG_FALLING) != 0)
        step.z = step.z - this->RelDistanceFallen(deltaMs + this->m_fallTimeMs);

    uint32_t moveFlags = this->m_flags;
    CAaBox swept;

    if ((moveFlags & MOVEMENTFLAG_FALLING) != 0) {
        swept.b.x = step.x + CMovement_C::s_queryBox.b.x;
        swept.b.y = CMovement_C::s_queryBox.b.y + step.y;
        swept.b.z = CMovement_C::s_queryBox.b.z + step.z;
        swept.t.x = step.x + CMovement_C::s_queryBox.t.x;
        swept.t.y = step.y + CMovement_C::s_queryBox.t.y;
        swept.t.z = step.z + CMovement_C::s_queryBox.t.z;
        CMovement_C::s_queryBox |= swept;

        float spread = step.z * -1.1866661f;
        if (spread > 0.0f) {
            CMovement_C::s_queryBox.b.x = CMovement_C::s_queryBox.b.x - spread;
            CMovement_C::s_queryBox.t.x = CMovement_C::s_queryBox.t.x + spread;
            CMovement_C::s_queryBox.b.y = CMovement_C::s_queryBox.b.y - spread;
            CMovement_C::s_queryBox.t.y = spread + CMovement_C::s_queryBox.t.y;
        }
    } else if ((moveFlags & MOVEMASK_SWIM_FLY) != 0) {
        float half = distance * 0.5f;
        float cx = x * half + position.x;
        float cy = y * half + position.y;
        float cz = z * half + position.z;
        float radius = this->m_collisionRadius * 1.4142135f;

        float top = radius;
        if (radius <= this->m_collisionHeight)
            top = this->m_collisionHeight;

        swept.b.x = (cx - half) - radius;
        swept.t.x = (cx + half) + radius;
        swept.b.y = (cy - half) - radius;
        swept.t.y = (cy + half) + radius;
        swept.b.z = cz - half;
        swept.t.z = (cz + half) + top;
        CMovement_C::s_queryBox |= swept;
    } else {
        float reach = this->m_collisionRadius + 0.0013888889f;
        if (reach < this->GetStepUpHeight() * 1.1917536f)
            reach = this->GetStepUpHeight() * 1.1917536f;

        float extent = reach + distance;
        float ex = x * extent;
        float ey = y * extent;
        float ez = extent * z;

        swept.b.x = ex + CMovement_C::s_queryBox.b.x;
        swept.b.y = CMovement_C::s_queryBox.b.y + ey;
        swept.b.z = CMovement_C::s_queryBox.b.z + ez;
        swept.t.x = ex + CMovement_C::s_queryBox.t.x;
        swept.t.y = ey + CMovement_C::s_queryBox.t.y;
        swept.t.z = ez + CMovement_C::s_queryBox.t.z;
        CMovement_C::s_queryBox |= swept;

        float half = distance * 0.5f;
        float cx = x * half + position.x;
        float cy = y * half + position.y;
        float cz = z * half + position.z;
        float radius = half + this->m_collisionRadius * 1.4142135f;

        swept.b.x = cx - radius;
        swept.t.x = cx + radius;
        swept.b.y = cy - radius;
        swept.t.y = radius + cy;
        swept.b.z = cz;
        swept.t.z = cz;
        CMovement_C::s_queryBox |= swept;

        float rise = distance;
        if (distance < this->GetStepUpHeight() + this->GetStepUpHeight())
            rise = this->GetStepUpHeight() + this->GetStepUpHeight();

        CMovement_C::s_queryBox.t.z = rise + CMovement_C::s_queryBox.t.z;
        CMovement_C::s_queryBox.b.z = CMovement_C::s_queryBox.b.z - (this->GetStepUpHeight() + distance * 1.1917536f);
    }

    CMovement_C::s_queryBox.b.x = CMovement_C::s_queryBox.b.x - 0.0013888889f;
    CMovement_C::s_queryBox.b.y = CMovement_C::s_queryBox.b.y - 0.0013888889f;
    CMovement_C::s_queryBox.b.z = CMovement_C::s_queryBox.b.z - 0.0013888889f;
    CMovement_C::s_queryBox.t.x = CMovement_C::s_queryBox.t.x + 0.0013888889f;
    CMovement_C::s_queryBox.t.y = CMovement_C::s_queryBox.t.y + 0.0013888889f;
    CMovement_C::s_queryBox.t.z = CMovement_C::s_queryBox.t.z + 0.0013888889f;

    int32_t queryFlags = this->GetFacetQueryFlags();
    if (!World::GetFacets(&collisionBox, &CMovement_C::s_queryBox, &CMovement_C::s_moveFacets, queryFlags, nullptr))
        return 0;

    if ((this->m_flags & MOVEMENTFLAG_SWIMMING) != 0) {
        World::GetFacets(&collisionBox, &CMovement_C::s_queryBox, &CMovement_C::s_liquidFacets, 0x20000, nullptr);

        for (uint32_t i = 0; i < CMovement_C::s_liquidFacets.facets.Count(); i++) {
            CFacet* facet = &CMovement_C::s_liquidFacets.facets[i];
            facet->plane.n.x = -facet->plane.n.x;
            facet->plane.n.y = -facet->plane.n.y;
            facet->plane.n.z = -facet->plane.n.z;
            facet->plane.d = -facet->plane.d;
        }
    } else {
        CMovement_C::s_liquidFacets.facets.SetCount(0);
        CMovement_C::s_liquidFacets.facetIds.SetCount(0);
    }

    if (this->m_transportGuid) {
        C44Matrix inverse = transportMat.AffineInverse();

        for (uint32_t i = 0; i < CMovement_C::s_moveFacets.facets.Count(); i++) {
            CFacet* facet = &CMovement_C::s_moveFacets.facets[i];

            facet->v[0] = inverse.TransformPoint(facet->v[0]);
            facet->v[1] = inverse.TransformPoint(facet->v[1]);
            facet->v[2] = inverse.TransformPoint(facet->v[2]);

            float nx = inverse.b0 * facet->plane.n.y + inverse.c0 * facet->plane.n.z + inverse.a0 * facet->plane.n.x;
            float ny = inverse.b1 * facet->plane.n.y + inverse.c1 * facet->plane.n.z + inverse.a1 * facet->plane.n.x;
            float nz = inverse.b2 * facet->plane.n.y + inverse.c2 * facet->plane.n.z + inverse.a2 * facet->plane.n.x;

            facet->plane.n.x = nx;
            facet->plane.n.y = ny;
            facet->plane.n.z = nz;
            facet->plane.d = -(ny * facet->v[0].y + nz * facet->v[0].z + nx * facet->v[0].x);
        }
    }

    return 1;
}

// OFFSET: 0x7620F0
int32_t CMovement_C::TraceSurface(int32_t time, int32_t deltaMs, float distance, C2Vector* direction) {
    if (fabsf(distance) < 0.00000095367432f)
        return deltaMs;

    if (!CMovement_C::s_moveFacets.facets.Count()) {
        if (this->StartFalling(0.0f))
            return 0;
        return deltaMs;
    }

    C4Plane planes[7];
    for (int32_t i = 0; i < 7; i++) {
        planes[i].n = { 0.0f, 0.0f, 1.0f };
        planes[i].d = 0.0f;
    }

    float totalMs = (float)(uint32_t)deltaMs;
    float remainingMs = totalMs;
    float elapsedMs = 0.0f;
    float stepDistance = distance;
    float remainingDistance = distance;
    float zBudget = distance / this->m_collisionRadius;

    int32_t shortSteps = 0;
    int32_t anchorDirty = 0;
    int32_t detached = 0;
    WGUID transport = 0;

    C2Vector dir2D = *direction;
    C3Vector dir = { direction->x, direction->y, 0.0f };

    uint32_t facetIndex;
    uint32_t planeCount;
    float moveDist;

    if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &dir, distance, &facetIndex, planes, &planeCount, &moveDist, nullptr))
        return deltaMs;

    while (true) {
        float dx = dir.x * moveDist;
        float dy = dir.y * moveDist;
        float dz = dir.z * moveDist;

        if (zBudget >= dz) {
            zBudget = zBudget - dz;
        } else {
            float scale = zBudget / dz;
            anchorDirty = 1;
            dx = dx * scale;
            dy = dy * scale;
            dz = dz * scale;
            moveDist = moveDist * scale;
            zBudget = 0.0f;
        }

        this->m_position.x = dx + this->m_position.x;
        this->m_position.y = dy + this->m_position.y;
        this->m_position.z = dz + this->m_position.z;

        if (facetIndex >= CMovement_C::s_moveFacets.facets.Count()) {
            elapsedMs = remainingMs + elapsedMs;
            float left = totalMs - elapsedMs;

            if (left >= 1.0f) {
                remainingMs = left;
                dir2D = *direction;
                stepDistance = left / totalMs * distance;
                dir = { direction->x, direction->y, 0.0f };

                if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &dir, stepDistance, &facetIndex, planes, &planeCount, &moveDist, nullptr))
                    return deltaMs;
                continue;
            }

            C3Vector down = { 0.0f, 0.0f, -1.0f };
            float drop;

            if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &down, distance * 1.849399f, &facetIndex, planes, &planeCount, &drop, nullptr))
                return deltaMs;

            this->m_position.z = this->m_position.z - drop;

            if (facetIndex == CMovement_C::s_moveFacets.facets.Count()) {
                transport = 0;

                if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) == 0) {
                    this->StartFalling(0.0f);
                    return deltaMs;
                }

                break;
            }

            transport = CMovement_C::s_moveFacets.facetIds[facetIndex];

            if (!this->IsSurfaceTooSteep(facetIndex)) {
                this->m_flags &= ~MOVEMENTFLAG_SPLINE_ELEVATION;
                break;
            }

            if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) == 0) {
                this->StartFalling(0.0f);
                return deltaMs;
            }

            CFacet* ground = &CMovement_C::s_moveFacets.facets[facetIndex];
            if (ground->plane.n.x * dir2D.x + ground->plane.n.y * dir2D.y > -0.0000099999997f) {
                this->StartFalling(0.0f);
                return deltaMs;
            }

            break;
        }

        transport = CMovement_C::s_moveFacets.facetIds[facetIndex];

        float stepMs = moveDist / stepDistance * remainingMs;
        stepDistance = stepDistance - moveDist;

        C2Vector step2D(dx, dy);
        remainingDistance = remainingDistance - sqrtf(step2D.y * step2D.y + step2D.x * step2D.x);

        if (stepMs < 1.0f) {
            if (++shortSteps > 5) {
                anchorDirty = 1;
                break;
            }
        } else {
            shortSteps = 1;
        }

        elapsedMs = elapsedMs + stepMs;
        remainingMs = remainingMs - stepMs;

        if (totalMs - elapsedMs < 1.0f) {
            if (totalMs != elapsedMs)
                anchorDirty = 1;
            break;
        }

        C3Vector n = CMovement_C::s_moveFacets.facets[facetIndex].plane.n;
        C3Vector push;

        if (this->UseWalkableRedirection(facetIndex, &detached)) {
            C3Vector pushNormal = { 0.0f, 0.0f, 0.0f };
            float prevStepDistance = stepDistance;
            int32_t boxPush = GetBoxPushNormal(planes, planeCount, &pushNormal);

            push = this->CalcRunWalkWalkableObstaclePush(&dir, &stepDistance, n, boxPush, &pushNormal);
            remainingMs = stepDistance / prevStepDistance * remainingMs;
        } else {
            if (detached) {
                this->StartFalling(0.0f);
                return deltaMs;
            }

            C3Vector pushNormal = { 0.0f, 0.0f, 0.0f };
            anchorDirty = 1;

            int32_t boxPush = GetBoxPushNormal(planes, planeCount, &pushNormal);
            uint32_t wasElevated = this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION;

            if (!this->AttemptStepUp(&dir2D, n))
                return deltaMs;

            if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) == 0) {
                if (wasElevated) {
                    this->StartFalling(0.0f);
                    return deltaMs;
                }

                push = this->CalcRunWalkBlockingObstaclePush(&dir, stepDistance, remainingDistance, &n);
            } else {
                float prevStepDistance = stepDistance;

                push = this->CalcRunWalkWalkableObstaclePush(&dir, &stepDistance, n, boxPush, &pushNormal);
                remainingMs = stepDistance / prevStepDistance * remainingMs;
            }
        }

        C3Vector delta;
        delta.x = dir.x * stepDistance + push.x;
        delta.y = dir.y * stepDistance + push.y;
        delta.z = stepDistance * dir.z + push.z;
        dir = delta;

        stepDistance = sqrtf(delta.y * delta.y + delta.z * delta.z + delta.x * delta.x);

        if (stepDistance < 0.001f) {
            anchorDirty = 1;
            break;
        }

        dir2D = C2Vector(dir.x, dir.y);

        float inverse = 1.0f / stepDistance;
        dir.x = dir.x * inverse;
        dir.y = dir.y * inverse;
        dir.z = inverse * dir.z;

        float prevRemainingDistance = remainingDistance;
        float length2D = sqrtf(dir2D.y * dir2D.y + dir2D.x * dir2D.x);
        remainingDistance = length2D;

        if (fabsf(length2D) >= 0.00000023841858f) {
            float scale2D = 1.0f / length2D;
            dir2D.x = dir2D.x * scale2D;
            dir2D.y = dir2D.y * scale2D;
        }

        if (prevRemainingDistance - length2D > 0.00000095367432f)
            anchorDirty = 1;

        if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) != 0 && delta.z > 0.00000095367432f) {
            float targetZ = delta.z + this->m_position.z;

            if (this->GetStepUpHeight() + this->m_stepUpStartZ < targetZ) {
                float climb = this->GetStepUpHeight() + this->m_stepUpStartZ - this->m_position.z;
                stepDistance = climb / delta.z * stepDistance;

                if (climb != delta.z)
                    anchorDirty = 1;

                if (stepDistance < 0.001f) {
                    this->StartFalling(0.0f);
                    return deltaMs;
                }
            }
        }

        if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &dir, stepDistance, &facetIndex, planes, &planeCount, &moveDist, nullptr))
            return deltaMs;
    }

    if (detached) {
        this->StartFalling(0.0f);
        return deltaMs;
    }

    if (facetIndex >= CMovement_C::s_moveFacets.facets.Count()) {
        if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) != 0)
            return deltaMs;

        this->StartFalling(0.0f);
        return deltaMs;
    }

    if (anchorDirty)
        this->UpdateAnchors(0);
    //this->SetTransport(transport);

    return deltaMs;
}

// OFFSET: 0x7618B0
int32_t CMovement_C::Fall(int32_t time, int32_t remaining, float distance, C2Vector* dir) {
    if (/*this->m_transportGuid && this->m_link.IsLinked() && this->SetTransport(0i64) ||*/ !remaining)
        return 0;
    float v7 = -this->RelDistanceFallen(remaining + this->m_fallTimeMs);
    float v8 = dir->x * distance;
    float v10 = distance * dir->y;
    int32_t result = remaining;
    if (fabs(sqrt(v10 * v10 + v7 * v7 + v8 * v8)) >= 0.00000023841858) {
        if (s_moveFacets.facets.m_count) {
            result = this->FallDown(time, remaining, v8, v10, v7, 1);
        } else {
            this->m_position.x = v8 + this->m_position.x;
            this->m_position.y = v10 + this->m_position.y;
            this->m_position.z = v7 + this->m_position.z;
            this->m_fallTimeMs += remaining;
            this->UpdateFallingFar();
        }
        this->m_groundNormal.x = 0.0;
        this->m_groundNormal.y = 0.0;
        this->m_groundNormal.z = 1.0;
    } else {
        this->m_fallTimeMs += remaining;
    }
    return result;
}

// OFFSET: 0x75CF80
void CMovement_C::UpdateFallingFar() {
    if ((this->m_flags & (MOVEMENTFLAG_FALLING_SLOW | MOVEMENTFLAG_FALLING_FAR)) == 0) {
        if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0 || 0.0 == this->m_fallVelocity) {
            if (this->m_fallTimeMs >= 500)
                this->m_flags = this->m_flags | MOVEMENTFLAG_FALLING_FAR;
        } else if (this->m_fallStartZ - 0.11111111 >= this->m_position.z) {
            this->m_flags = this->m_flags | MOVEMENTFLAG_FALLING_FAR;
        }
    }
}

// OFFSET: 0x760B40
int32_t CMovement_C::Swim(int32_t time, int32_t remaining, float distance, float dirX, float dirY, float dirZ) {
    WHOA_UNIMPLEMENTED(remaining);
}

// OFFSET: 0x762980
int32_t CMovement_C::HoverMove(int32_t time, int32_t remaining, float distance, float dirX, float dirY, float dirZ) {
    WHOA_UNIMPLEMENTED(remaining);
}

// OFFSET: 0x75F0A0
int32_t CMovement_C::ValidateTestVsFacetQuery(float offsetX, float offsetY, float offsetZ) {
    C44Matrix transportMat;
    C3Vector position = this->m_position;

    float x = offsetX;
    float y = offsetY;
    float z = offsetZ;

    if (this->m_transportGuid) {
        MovementGetTransportMtxX(this->m_transportGuid, &transportMat);

        position = transportMat.TransformPoint(position);

        float rx = transportMat.c0 * z + transportMat.b0 * y + transportMat.a0 * x;
        float ry = z * transportMat.c1 + y * transportMat.b1 + transportMat.a1 * x;
        float rz = z * transportMat.c2 + y * transportMat.b2 + x * transportMat.a2;

        x = rx;
        y = ry;
        z = rz;
    }

    CAaBox collisionBox;
    collisionBox.b = { 0.0f, 0.0f, 0.0f };
    collisionBox.t = { 0.0f, 0.0f, 0.0f };
    this->BuildCollisionBox(&position, &collisionBox);

    CAaBox moved;
    moved.b.x = collisionBox.b.x + x;
    moved.b.y = collisionBox.b.y + y;
    moved.b.z = collisionBox.b.z + z;
    moved.t.x = x + collisionBox.t.x;
    moved.t.y = y + collisionBox.t.y;
    moved.t.z = z + collisionBox.t.z;

    if (CMovement_C::s_queryBox.ContainsPoint(moved.b) && CMovement_C::s_queryBox.ContainsPoint(moved.t))
        return 1;

    CAaBox queryBox;
    queryBox.b.x = moved.b.x - 0.16666667f;
    queryBox.b.y = moved.b.y - 0.16666667f;
    queryBox.b.z = moved.b.z - 0.16666667f;
    queryBox.t.x = moved.t.x + 0.16666667f;
    queryBox.t.y = moved.t.y + 0.16666667f;
    queryBox.t.z = moved.t.z + 0.16666667f;
    queryBox |= CMovement_C::s_queryBox;

    int32_t queryFlags = this->GetFacetQueryFlags();
    if (!World::GetFacets(&collisionBox, &queryBox, &CMovement_C::s_moveFacets, queryFlags, nullptr))
        return 0;

    if ((this->m_flags & MOVEMENTFLAG_SWIMMING) != 0) {
        World::GetFacets(&collisionBox, &queryBox, &CMovement_C::s_liquidFacets, 0x20000, nullptr);

        for (uint32_t i = 0; i < CMovement_C::s_liquidFacets.facets.Count(); i++) {
            CFacet* facet = &CMovement_C::s_liquidFacets.facets[i];
            facet->plane.n.x = -facet->plane.n.x;
            facet->plane.n.y = -facet->plane.n.y;
            facet->plane.n.z = -facet->plane.n.z;
            facet->plane.d = -facet->plane.d;
        }
    } else {
        CMovement_C::s_liquidFacets.facets.SetCount(0);
        CMovement_C::s_liquidFacets.facetIds.SetCount(0);
    }

    CMovement_C::s_queryBox = queryBox;

    if (this->m_transportGuid) {
        C44Matrix inverse = transportMat.AffineInverse();

        for (uint32_t i = 0; i < CMovement_C::s_moveFacets.facets.Count(); i++) {
            CFacet* facet = &CMovement_C::s_moveFacets.facets[i];

            facet->v[0] = inverse.TransformPoint(facet->v[0]);
            facet->v[1] = inverse.TransformPoint(facet->v[1]);
            facet->v[2] = inverse.TransformPoint(facet->v[2]);

            float nx = inverse.b0 * facet->plane.n.y + inverse.c0 * facet->plane.n.z + inverse.a0 * facet->plane.n.x;
            float ny = inverse.b1 * facet->plane.n.y + inverse.c1 * facet->plane.n.z + inverse.a1 * facet->plane.n.x;
            float nz = inverse.b2 * facet->plane.n.y + inverse.c2 * facet->plane.n.z + inverse.a2 * facet->plane.n.x;

            facet->plane.n.x = nx;
            facet->plane.n.y = ny;
            facet->plane.n.z = nz;
            facet->plane.d = -(ny * facet->v[0].y + nz * facet->v[0].z + nx * facet->v[0].x);
        }
    }

    return 1;
}

// OFFSET: 0x75B690
bool CMovement_C::IsSurfaceTooSteep(uint32_t facetId) {
    return CMovement_C::s_moveFacets.facets[facetId].plane.n.z <= 0.64278764f;
}

// OFFSET: 0x75DE80
bool CMovement_C::GetBoxPushNormal(C4Plane* planes, uint32_t count, C3Vector* out) {
    if (!count || count > 4)
        return 0;

    for (uint32_t i = 0; i < count; i++) {
        if (fabs(planes[i].n.z - -0.4756366f) >= 0.00000095367432f)
            return false;
    }
    switch (count) {
    case 1:
        *out = planes->n;
        break;
    case 2:
        out->x = (planes[1].n.x + planes->n.x) * 0.638556f;
        out->y = (planes[1].n.y + planes->n.y) * 0.638556f;
        out->z = (planes[1].n.z + planes->n.z) * 0.638556f;
        break;
    case 3: {
        int32_t xFacing[3];
        int32_t yFacing[3];
        int32_t xFacingCount = 0;
        int32_t yFacingCount = 0;

        for (int32_t i = 0; i < 3; i++) {
            if (fabsf(planes[i].n.y) < 0.00000095367432f)
                xFacing[xFacingCount++] = i;
            else if (fabsf(planes[i].n.x) < 0.00000095367432f)
                yFacing[yFacingCount++] = i;
        }

        int32_t pick = yFacing[0];
        if (xFacingCount == 1)
            pick = xFacing[0];

        *out = planes[pick].n;
        break;
    }
    case 4:
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
        break;
    }
    return true;
}

// OFFSET: 0x75BC50
bool CMovement_C::ExtrudeTriangle(C3Vector* verts, uint8_t* indices, C3Vector* normal, C4Plane* out, C3Vector* extrude) {
    int32_t next[3] = { 1, 2, 0 };
    int32_t prev[3] = { 2, 0, 1 };

    for (int32_t i = 0; i < 3; i++) {
        C3Vector base = verts[indices[i]];

        float topX = extrude->x + base.x;
        float topY = extrude->y + base.y;
        float topZ = extrude->z + base.z;

        C3Vector edge = verts[indices[next[i]]];

        float upX = topX - base.x;
        float upY = topY - base.y;
        float upZ = topZ - base.z;

        float sideX = edge.x - base.x;
        float sideY = edge.y - base.y;
        float sideZ = edge.z - base.z;

        float crossX = sideY * upZ - upY * sideZ;
        float crossY = sideZ * upX - upZ * sideX;
        float crossZ = sideX * upY - upX * sideY;

        float length = crossZ * crossZ + crossY * crossY + crossX * crossX;
        if (length < 0.00000095367432f)
            return 0;

        float inverse = 1.0f / sqrtf(length);

        out[i].n.x = crossX * inverse;
        out[i].n.y = crossY * inverse;
        out[i].n.z = inverse * crossZ;
        out[i].d = -(out[i].n.z * base.z + out[i].n.y * base.y + out[i].n.x * base.x);

        C3Vector* other = &verts[indices[prev[i]]];

        if (out[i].n.y * (other->y - base.y) + (other->z - base.z) * out[i].n.z + (other->x - base.x) * out[i].n.x > 0.0f) {
            out[i].n.x = -out[i].n.x;
            out[i].n.y = -out[i].n.y;
            out[i].n.z = -out[i].n.z;
            out[i].d = -out[i].d;
        }
    }

    C3Vector first = verts[indices[0]];

    float topX = first.x + extrude->x;
    float topY = first.y + extrude->y;
    float topZ = first.z + extrude->z;

    out[3].n = *normal;
    out[3].d = -(topX * normal->x + topY * normal->y + topZ * normal->z);

    return 1;
}

// OFFSET: 0x75CFE0
bool CMovement_C::IsFacetOverhead(int32_t facet) {
    float minX = this->m_position.x - this->m_collisionRadius;
    float maxX = this->m_collisionRadius + this->m_position.x;
    float minY = this->m_position.y - this->m_collisionRadius;
    float maxY = this->m_collisionRadius + this->m_position.y;
    float top = this->m_collisionHeight + this->m_position.z;

    C3Vector corners[4];
    corners[0] = { minX, minY, top };
    corners[1] = { maxX, minY, top };
    corners[2] = { minX, maxY, top };
    corners[3] = { maxX, maxY, top };

    C4Plane* plane = &CMovement_C::s_moveFacets.facets[facet].plane;

    for (int32_t i = 0; i < 4; i++) {
        if (fabsf(corners[i].z * plane->n.z + corners[i].x * plane->n.x + corners[i].y * plane->n.y + plane->d) < 0.0013888889f)
            return 1;
    }

    return 0;
}

// OFFSET: 0x75D0A0
bool CMovement_C::WalkableFacetEnclosesPoint(int32_t facet, C3Vector* point) {
    C4Plane sides[4];
    for (int32_t i = 0; i < 4; i++) {
        sides[i].n = { 0.0f, 0.0f, 1.0f };
        sides[i].d = 0.0f;
    }

    uint8_t indices[3] = { 0, 1, 2 };
    C3Vector up = { 0.0f, 0.0f, 1.0f };
    CFacet* f = &CMovement_C::s_moveFacets.facets[facet];
    this->ExtrudeTriangle(f->v, indices, &f->plane.n, sides, &up);

    for (int32_t i = 0; i < 3; i++) {
        if (sides[i].n.x * point->x + sides[i].n.y * point->y + sides[i].n.z * point->z + sides[i].d > 0.083333336f)
            return false;
    }

    return true;
}

// OFFSET: 0x75D1C0
bool CMovement_C::UseWalkableRedirection(int32_t facet, int32_t* detached) {
    if (detached)
        *detached = 0;

    float limit = 0.17364818f;
    if (this->m_unit->IsClientControlled())
        limit = 0.64278764f;

    if (limit < CMovement_C::s_moveFacets.facets[facet].plane.n.z) {
        if (this->WalkableFacetEnclosesPoint(facet, &this->m_position))
            this->m_flags &= ~MOVEMENTFLAG_SPLINE_ELEVATION;
        return 1;
    }

    if (CMovement_C::s_moveFacets.facets[facet].plane.n.z >= 0.0f)
        return this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION;
    if (!this->IsFacetOverhead(facet))
        return this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION;

    if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) != 0) {
        if (detached)
            *detached = 1;
        this->m_flags &= ~MOVEMENTFLAG_SPLINE_ELEVATION;
    }

    return -CMovement_C::s_moveFacets.facets[facet].plane.n.z > 0.64278764f;
}

// OFFSET: 0x75E0C0
C3Vector CMovement_C::CalcRunWalkWalkableObstaclePush(C3Vector* dir, float* stepDistance, C3Vector n, int32_t boxPush, C3Vector* pushNormal) {
    float lengthSq = pushNormal->z * pushNormal->z + pushNormal->y * pushNormal->y + pushNormal->x * pushNormal->x;

    if (boxPush && n.z <= 0.64278764f && fabsf(lengthSq) >= 0.00000023841858f) {
        n.x = -pushNormal->x;
        n.y = -pushNormal->y;
        n.z = -pushNormal->z;
    }

    float depth = (-n.x * dir->x + -n.y * dir->y + -n.z * dir->z) * *stepDistance;

    float rise;
    if (fabsf(n.z) < 0.00000023841858f) {
        rise = 3.4028235e38f;
        if (depth < 0.0f)
            rise = -3.4028235e38f;
    } else {
        rise = depth / n.z;
    }

    float height;

    if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) != 0 && rise < 0.0f) {
        *stepDistance = 0.0f;
        height = this->GetRemainingStepUpHeight();
    } else if (this->GetRemainingStepUpHeight() >= rise) {
        height = rise;

        if (rise < -this->GetStepUpHeight()) {
            *stepDistance = -(this->GetStepUpHeight() * (*stepDistance / rise));
            height = -this->GetStepUpHeight();
        }
    } else {
        *stepDistance = this->GetRemainingStepUpHeight() * (*stepDistance / rise);
        height = this->GetRemainingStepUpHeight();
    }

    C3Vector push;
    push.x = 0.0f * height;
    push.y = 0.0f * height;
    push.z = height;

    return push;
}

// OFFSET: 0x75E250
C3Vector CMovement_C::CalcRunWalkBlockingObstaclePush(C3Vector* dir, float stepDistance, float remainingDistance, C3Vector* n) {
    C3Vector push;

    if (n->z < 0.0f && -n->z > 0.64278764f) {
        push.x = 0.0f;
        push.y = 0.0f;
        push.z = 0.0f;

        return push;
    }

    C2Vector flat(n->x, n->y);

    float lengthSq = flat.y * flat.y + flat.x * flat.x;
    if (lengthSq > 0.00000023841858f) {
        float inverse = 1.0f / sqrtf(lengthSq);
        flat.x = flat.x * inverse;
        flat.y = flat.y * inverse;
    }

    float depth = (-n->x * dir->x + -n->y * dir->y + -n->z * dir->z) * stepDistance;
    float along = n->y * flat.y + n->x * flat.x + n->z * 0.0f;

    float slide = depth;
    if (fabsf(along) >= 0.00000023841858f)
        slide = depth / along;

    push.x = flat.x * (slide + 0.001f);
    push.y = flat.y * (slide + 0.001f);

    C2Vector step(dir->x, dir->y);
    float stepX = step.x * stepDistance;
    float stepY = stepDistance * step.y;

    float totalX = stepX + push.x;
    float totalY = stepY + push.y;
    float totalSq = totalY * totalY + totalX * totalX;

    if (remainingDistance * remainingDistance < totalSq) {
        float scale = remainingDistance / sqrtf(totalSq);
        push.x = totalX * scale - stepX;
        push.y = totalY * scale - stepY;
    }

    push.z = 0.0f;

    return push;
}

// OFFSET: 0x75BE80
int32_t CMovement_C::ComputeDistanceToMoveImpl(C3Vector* verts, C3Vector* normal, C3Vector* extrude, C4Plane* out, uint8_t* indices) {
    for (int32_t i = 0; i < 4; i++) {
        C3Vector base = verts[indices[i]];

        float topX = extrude->x + base.x;
        float topY = extrude->y + base.y;
        float topZ = extrude->z + base.z;

        C3Vector edge = verts[indices[(i + 1) & 3]];

        float upX = topX - base.x;
        float upY = topY - base.y;
        float upZ = topZ - base.z;

        float sideX = edge.x - base.x;
        float sideY = edge.y - base.y;
        float sideZ = edge.z - base.z;

        float crossX = sideY * upZ - upY * sideZ;
        float crossY = sideZ * upX - upZ * sideX;
        float crossZ = sideX * upY - upX * sideY;

        float length = crossZ * crossZ + crossY * crossY + crossX * crossX;
        if (length < 0.00000095367432f)
            return 0;

        float inverse = 1.0f / sqrtf(length);

        out[i].n.x = crossX * inverse;
        out[i].n.y = crossY * inverse;
        out[i].n.z = inverse * crossZ;
        out[i].d = -(out[i].n.z * base.z + out[i].n.y * base.y + out[i].n.x * base.x);

        C3Vector* other = &verts[indices[(i + 3) & 3]];

        if (out[i].n.y * (other->y - base.y) + (other->z - base.z) * out[i].n.z + (other->x - base.x) * out[i].n.x > 0.0f) {
            out[i].n.x = -out[i].n.x;
            out[i].n.y = -out[i].n.y;
            out[i].n.z = -out[i].n.z;
            out[i].d = -out[i].d;
        }
    }

    C3Vector first = verts[indices[0]];

    float topX = first.x + extrude->x;
    float topY = first.y + extrude->y;
    float topZ = first.z + extrude->z;

    out[4].n = *normal;
    out[4].d = -(topX * normal->x + topY * normal->y + topZ * normal->z);

    return 1;
}

// OFFSET: 0x75C8F0
void CMovement_C::BuildCollisionVolumePlanes(C3Vector* position, float height, float radius, C4Plane* planes) {
    planes[0].n = { -1.0f, 0.0f, 0.0f };
    planes[0].d = position->x - radius;

    planes[1].n = { 1.0f, 0.0f, 0.0f };
    planes[1].d = -position->x - radius;

    planes[2].n = { 0.0f, 1.0f, 0.0f };
    planes[2].d = -position->y - radius;

    planes[3].n = { 0.0f, -1.0f, 0.0f };
    planes[3].d = position->y - radius;

    planes[4].n = { 0.0f, 0.0f, 1.0f };
    planes[4].d = -position->z - height;

    planes[5].n = { -0.87964189f, 0.0f, -0.4756366f };
    planes[5].d = position->y * 0.0f + position->x * 0.87964189f + position->z * 0.4756366f;

    planes[6].n = { 0.87964189f, 0.0f, -0.4756366f };
    planes[6].d = position->z * 0.4756366f - position->x * 0.87964189f + position->y * 0.0f;

    planes[7].n = { 0.0f, 0.87964189f, -0.4756366f };
    planes[7].d = position->z * 0.4756366f + position->x * 0.0f - position->y * 0.87964189f;

    planes[8].n = { 0.0f, -0.87964189f, -0.4756366f };
    planes[8].d = 0.87964189f * position->y + 0.0f * position->x + 0.4756366f * position->z;
}

// OFFSET: 0x75CA80
void CMovement_C::BuildCollisionVolume(C3Vector* position, float height, C4Plane* planes, C3Vector* verts, uint8_t* indices) {
    this->BuildCollisionVolumePlanes(position, height, this->m_collisionRadius, planes);

    float minX = position->x - this->m_collisionRadius;
    float maxX = this->m_collisionRadius + position->x;
    float minY = position->y - this->m_collisionRadius;
    float maxY = this->m_collisionRadius + position->y;
    float shoulderZ = this->m_collisionRadius * 1.849399f + position->z;
    float topZ = height + position->z;

    verts[0] = *position;
    verts[1] = { minX, minY, shoulderZ };
    verts[2] = { minX, maxY, shoulderZ };
    verts[3] = { maxX, maxY, shoulderZ };
    verts[4] = { maxX, minY, shoulderZ };
    verts[5] = { minX, minY, topZ };
    verts[6] = { minX, maxY, topZ };
    verts[7] = { maxX, maxY, topZ };
    verts[8] = { maxX, minY, topZ };

    static const uint8_t faces[32] = {
        1, 2, 6, 5,
        3, 4, 8, 7,
        2, 3, 7, 6,
        4, 1, 5, 8,
        5, 6, 7, 8,
        0, 1, 2,
        0, 3, 4,
        0, 2, 3,
        0, 4, 1
    };

    memcpy(indices, faces, 32);
}

// OFFSET: 0x75B610
ClipPolygon* CMovement_C::InitPolygonBuffer(ClipPolygon* dst, ClipPolygon* src) {
    for (int32_t i = 0; i < 15; i++) {
        dst->v[i].x = 0.0f;
        dst->v[i].y = 0.0f;
        dst->v[i].z = 0.0f;
    }

    dst->count = src->count;
    memcpy(dst->v, src->v, 12 * src->count);
    memcpy(dst->flags, src->flags, 4 * src->count);

    return dst;
}

// OFFSET: 0x75B710
void CMovement_C::ClipPolygonToPlane(ClipPolygon* poly, int32_t planeIndex, C4Plane* plane) {
    float minDist = 3.4028235e38f;
    float maxDist = -3.4028235e38f;

    int32_t count = poly->count;
    float dist[15];

    for (int32_t i = 0; i < count; i++) {
        float d = -(poly->v[i].x * plane->n.x + poly->v[i].z * plane->n.z + poly->v[i].y * plane->n.y + plane->d);

        if (d < minDist)
            minDist = d;
        if (d > maxDist)
            maxDist = d;

        dist[i] = d;
    }

    if (minDist > -0.0013888889f)
        return;

    if (maxDist < 0.0013888889f) {
        poly->count = 0;
        return;
    }

    ClipPolygon scratch;
    this->InitPolygonBuffer(&scratch, poly);

    poly->count = 0;

    int32_t prev = count - 1;

    for (int32_t i = 0; i < count; i++) {
        float distPrev = dist[prev];
        float distCur = dist[i];

        if (distPrev < 0.0f) {
            if (distCur >= 0.0f) {
                if (distCur > 0.0013888889f) {
                    float t = distPrev / (distCur - distPrev);

                    poly->flags[poly->count] = planeIndex;
                    poly->v[poly->count].x = scratch.v[prev].x - (scratch.v[i].x - scratch.v[prev].x) * t;
                    poly->v[poly->count].y = scratch.v[prev].y - (scratch.v[i].y - scratch.v[prev].y) * t;
                    poly->v[poly->count].z = scratch.v[prev].z - (scratch.v[i].z - scratch.v[prev].z) * t;
                    poly->count++;
                }

                poly->flags[poly->count] = scratch.flags[i];
                poly->v[poly->count] = scratch.v[i];
                poly->count++;
            }
        } else if (distCur < 0.0f) {
            if (distPrev > 0.0013888889f) {
                float t = distPrev / (distCur - distPrev);

                poly->flags[poly->count] = planeIndex;
                poly->v[poly->count].x = scratch.v[prev].x - (scratch.v[i].x - scratch.v[prev].x) * t;
                poly->v[poly->count].y = scratch.v[prev].y - (scratch.v[i].y - scratch.v[prev].y) * t;
                poly->v[poly->count].z = scratch.v[prev].z - (scratch.v[i].z - scratch.v[prev].z) * t;
                poly->count++;
            }
        } else {
            poly->flags[poly->count] = scratch.flags[i];
            poly->v[poly->count] = scratch.v[i];
            poly->count++;
        }

        prev = i;
    }

    if (poly->count < 3)
        poly->count = 0;
}

// OFFSET: 0x75C0B0
int32_t CMovement_C::ClipGenericPolygon(ClipPolygon* poly, C4Plane* planes, int32_t planeIndex, float* outDistance, C3Vector* direction) {
    float best = *outDistance;
    int32_t allBehind = 1;

    C4Plane* plane = &planes[planeIndex];

    float denom = direction->z * plane->n.z + direction->y * plane->n.y + plane->n.x * direction->x;
    float absDenom = fabsf(denom);

    for (int32_t i = 0; i < poly->count; i++) {
        float d = poly->v[i].z * plane->n.z + poly->v[i].x * plane->n.x + poly->v[i].y * plane->n.y + plane->d;
        if (absDenom >= 0.00000023841858f)
            d = d / denom;

        if (d < best)
            best = d <= 0.0f ? 0.0f : d;

        if (d > -0.00000095367432f)
            allBehind = 0;
    }

    if (allBehind) {
        for (int32_t i = 0; i < 9; i++) {
            if (i == planeIndex)
                continue;

            this->ClipPolygonToPlane(poly, i, &planes[i]);
            if (!poly->count)
                return 0;
        }

        for (int32_t i = 0; i < poly->count; i++) {
            float d = poly->v[i].z * plane->n.z + poly->v[i].x * plane->n.x + poly->v[i].y * plane->n.y + plane->d;
            if (absDenom >= 0.00000023841858f)
                d = d / denom;

            if (d > -0.027777778f)
                allBehind = 0;
        }

        if (allBehind)
            return 0;
    }

    if (best >= *outDistance)
        return 0;

    *outDistance = best;

    return 1;
}

// OFFSET: 0x75C5A0
int32_t CMovement_C::ClipMovementPyramid(World::FacetData* facets, C3Vector* direction, C4Plane* clipPlanes, uint32_t clipPlaneCount, C4Plane* volumePlanes, int32_t volumePlaneIndex, float* bestDistance, uint32_t* outFacetIndex) {
    ClipPolygon poly;

    for (int32_t i = 0; i < 15; i++) {
        poly.v[i].x = 0.0f;
        poly.v[i].y = 0.0f;
        poly.v[i].z = 0.0f;
    }

    poly.count = 3;

    int32_t hit = 0;
    uint32_t count = facets->facets.Count();

    if (!count)
        return 0;

    for (uint32_t f = 0; f < count; f++) {
        CFacet* facet = &facets->facets[f];

        if (facet->plane.n.z * direction->z + facet->plane.n.y * direction->y + facet->plane.n.x * direction->x > -0.0000099999997f)
            continue;

        memcpy(poly.v, facet->v, 36);

        poly.count = 3;
        poly.flags[0] = -1;
        poly.flags[1] = -1;
        poly.flags[2] = -1;

        uint32_t p = 0;

        while (p < clipPlaneCount) {
            this->ClipPolygonToPlane(&poly, p, &clipPlanes[p]);
            if (!poly.count)
                break;
            p++;
        }

        if (p < clipPlaneCount)
            continue;

        float distance = 3.4028235e38f;

        if (this->ClipGenericPolygon(&poly, volumePlanes, volumePlaneIndex, &distance, direction)) {
            if (distance <= *bestDistance) {
                *bestDistance = distance;
                *outFacetIndex = f;
                hit = 1;
            }
        }
    }

    return hit;
}

// OFFSET: 0x75CD70
uint32_t CMovement_C::DistanceToMovePyramid(World::FacetData* facets, C3Vector* dir, float distance, C3Vector* scaledDir, C4Plane* volumePlanes, C3Vector* verts, uint8_t* faceIndices, uint32_t* outFacetIndex, C4Plane* planes, uint32_t* outPlaneCount, float* best) {
    C4Plane facePlanes[5];
    for (int32_t i = 0; i < 5; i++) {
        facePlanes[i].n = { 0.0f, 0.0f, 1.0f };
        facePlanes[i].d = 0.0f;
    }

    for (uint32_t i = 0; i < 4; i++) {
        C4Plane* skirt = &volumePlanes[i + 5];

        if (skirt->n.z * scaledDir->z + skirt->n.x * scaledDir->x + skirt->n.y * scaledDir->y <= 0.0f)
            continue;

        if (!this->ExtrudeTriangle(verts, &faceIndices[i * 3 + 20], &skirt->n, facePlanes, scaledDir))
            continue;

        float clipped = distance;

        if (!this->ClipMovementPyramid(facets, dir, facePlanes, 3, volumePlanes, i + 5, &clipped, outFacetIndex))
            continue;

        if (clipped >= *best - 0.0013888889f) {
            if (*best + 0.0013888889f > clipped) {
                planes[*outPlaneCount] = *skirt;
                ++*outPlaneCount;
            }
        } else {
            planes[0] = *skirt;
            *outPlaneCount = 1;
        }

        if (clipped < *best) {
            *best = clipped;
            if (*outPlaneCount > 1)
                planes[*outPlaneCount - 1].Swap(&planes[0]);
        }
    }

    return 4;
}

// OFFSET: 0x75F9D0
int32_t CMovement_C::DistanceToMove(World::FacetData* facets, C3Vector* dir, float distance, uint32_t* outFacetIndex, C4Plane* planes, uint32_t* outPlaneCount, float* outMoved, C3Vector* origin) {
    float best = distance;

    *outPlaneCount = 0;

    if (fabsf(distance) < 0.00000095367432f) {
        *outMoved = 0.0f;
        *outFacetIndex = facets->facets.Count();
        return 1;
    }

    C4Plane volumePlanes[9];
    for (int32_t i = 0; i < 9; i++) {
        volumePlanes[i].n = { 0.0f, 0.0f, 1.0f };
        volumePlanes[i].d = 0.0f;
    }

    C3Vector verts[9];
    for (int32_t i = 0; i < 9; i++) {
        verts[i] = { 0.0f, 0.0f, 0.0f };
    }

    uint8_t faceIndices[32];

    float height = this->m_collisionHeight;

    C3Vector* source = origin;
    if (!origin)
        source = &this->m_position;

    C3Vector position = *source;

    if ((this->m_flags & MOVEMENTFLAG_SWIMMING) != 0 && facets == &CMovement_C::s_liquidFacets)
        height = height * 0.75f;

    float step = distance;
    if (step <= 0.027777778f)
        step = 0.027777778f;

    C3Vector scaledDir;
    scaledDir.x = step * dir->x;
    scaledDir.y = dir->y * step;
    scaledDir.z = step * dir->z;

    this->BuildCollisionVolume(&position, height, volumePlanes, verts, faceIndices);

    if (!origin) {
        int32_t valid = this->ValidateTestVsFacetQuery(scaledDir.x, scaledDir.y, scaledDir.z);

        if (!valid) {
            *outMoved = 0.0f;
            return valid;
        }
    }

    *outFacetIndex = facets->facets.Count();

    this->DistanceToMovePyramid(facets, dir, distance, &scaledDir, volumePlanes, verts, faceIndices, outFacetIndex, planes, outPlaneCount, &best);

    C4Plane facePlanes[5];
    for (int32_t i = 0; i < 5; i++) {
        facePlanes[i].n = { 0.0f, 0.0f, 1.0f };
        facePlanes[i].d = 0.0f;
    }

    for (int32_t i = 0; i < 5; i++) {
        if (volumePlanes[i].n.y * scaledDir.y + scaledDir.z * volumePlanes[i].n.z + scaledDir.x * volumePlanes[i].n.x <= 0.0f)
            continue;

        if (!this->ComputeDistanceToMoveImpl(verts, &volumePlanes[i].n, &scaledDir, facePlanes, &faceIndices[i * 4]))
            continue;

        float clipped = distance;

        if (!this->ClipMovementPyramid(facets, dir, facePlanes, 4, volumePlanes, i, &clipped, outFacetIndex))
            continue;

        if (clipped >= best - 0.0013888889f) {
            if (best + 0.0013888889f > clipped) {
                planes[*outPlaneCount] = volumePlanes[i];
                ++*outPlaneCount;
            }
        } else {
            planes[0] = volumePlanes[i];
            *outPlaneCount = 1;
        }

        if (clipped < best) {
            best = clipped;
            if (*outPlaneCount > 1)
                planes[*outPlaneCount - 1].Swap(&planes[0]);
        }
    }

    if (best >= 0.0013888889f)
        *outMoved = best;
    else
        *outMoved = 0.0f;

    return 1;
}

// OFFSET: 0x760FC0
int32_t CMovement_C::TryFallingDown(float elapsedFall, C3Vector* step, float* distance, C2Vector* outSlide, WGUID* outTransport, int32_t* outLanded, int32_t* outCeiling) {
    *outCeiling = 0;
    *outLanded = 0;

    float requested = *distance;

    if (fabsf(requested) < 0.00000023841858f) {
        *outTransport = 0;
        return 0;
    }

    C4Plane planes[7];
    for (int32_t i = 0; i < 7; i++) {
        planes[i].n = { 0.0f, 0.0f, 1.0f };
        planes[i].d = 0.0f;
    }

    C3Vector dir;
    dir.x = step->x * (1.0f / requested);
    dir.y = step->y * (1.0f / requested);
    dir.z = (1.0f / requested) * step->z;

    uint32_t facetIndex;
    uint32_t planeCount;

    if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &dir, requested, &facetIndex, planes, &planeCount, distance, nullptr)) {
        step->x = 0.0f;
        step->y = 0.0f;
        step->z = 0.0f;

        return 2;
    }

    if (facetIndex == CMovement_C::s_moveFacets.facets.Count()) {
        *outTransport = 0;
        return 0;
    }

    float moved = *distance;
    step->x = dir.x * moved;
    step->y = dir.y * moved;
    step->z = moved * dir.z;

    *outTransport = CMovement_C::s_moveFacets.facetIds[facetIndex];

    if (this->IsSlopeFallable(facetIndex, step)) {
        *outLanded = 1;
        return 1;
    }

    if (dir.z > 0.0f) {
        float toPeak = this->TimeToJumpPeak() - elapsedFall;

        if (toPeak >= 0.0f && toPeak * this->m_currentSpeed >= *distance && this->CheckFallingConditions(planeCount, planes)) {
            *outCeiling = 1;
            *outTransport = 0;

            return 1;
        }
    }

    C3Vector push = this->CalcFallObstaclePush(&dir, *distance, requested, facetIndex, planes, planeCount);
    
    outSlide->x = push.x;
    outSlide->y = push.y;

    return 1;
}

// OFFSET: 0x75D340
bool CMovement_C::IsSlopeFallable(int32_t facet, C3Vector* step) {
    C3Vector target;
    target.x = this->m_position.x + step->x;
    target.y = this->m_position.y + step->y;
    target.z = this->m_position.z + step->z;

    float limit = 0.17364818f;
    if (this->m_unit->IsClientControlled())
        limit = 0.64278764f;

    if (limit >= CMovement_C::s_moveFacets.facets[facet].plane.n.z)
        return false;

    return this->WalkableFacetEnclosesPoint(facet, &target) != 0;
}

// OFFSET: 0x75DE40
bool CMovement_C::CheckFallingConditions(uint32_t count, C4Plane* planes) {
    for (uint32_t i = 0; i < count; i++) {
        if (fabsf(planes[i].n.z - 1.0f) < 0.00000095367432f)
            return true;
    }

    return false;
}

// OFFSET: 0x7612B0
int32_t CMovement_C::FallDown(int32_t time, int32_t deltaMs, float dx, float dy, float dz, int32_t apply) {
    float elapsed = 0.0f;
    float total = deltaMs * 0.001f;

    float fallMs = (float)this->m_fallTimeMs;
    if (this->m_fallTimeMs < 0)
        fallMs = fallMs + 4294967300.0f;
    float fallTime = 0.001f * fallMs;

    int32_t retries = 0;
    int32_t steps = 0;
    int32_t anchorDirty = 0;
    int32_t stopped = 0;
    int32_t landed = 0;
    int32_t ceiling = 0;

    WGUID transport = 0;
    C2Vector slide(0.0f, 0.0f);

    C3Vector step = { dx, dy, dz };
    C2Vector step2D(step.x, step.y);

    C2Vector savedDir2D = this->m_moveDir2D;
    C3Vector savedDir = this->m_moveDir;
    float savedSpeed = this->m_currentSpeed;

    float length = sqrtf(step.y * step.y + step.x * step.x + step.z * step.z);
    float length2D = sqrtf(step2D.y * step2D.y + step2D.x * step2D.x);
    float startLength2D = length2D;
    float absZ = fabsf(step.z);

    C2Vector startPos(this->m_position.x, this->m_position.y);

    float consumed = total;

    while (true) {
        if (fabsf(length) < 0.00000095367432f) {
            consumed = total;
            break;
        }

        float distance = length;
        float inverse = 1.0f / length;

        C3Vector dir;
        dir.x = step.x * inverse;
        dir.y = step.y * inverse;
        dir.z = step.z * inverse;

        float elapsedFall = fallTime + elapsed;

        int32_t result = this->TryFallingDown(elapsedFall, &step, &distance, &slide, &transport, &landed, &ceiling);

        if (result == 2) {
            int32_t skip = (int32_t)((total - elapsed) * 1000.0f);

            this->m_anchorElapsedMs -= skip;
            this->m_fallTimeMs -= skip;
            //this->SkipTime(skip);

            consumed = total;
            break;
        }

        float stepTime;
        if (result)
            stepTime = this->GetTimeJustFallen(total - elapsed, elapsedFall, &step, &distance, ceiling, length2D);
        else
            stepTime = total - elapsed;

        consumed = total;

        this->m_position.x = this->m_position.x + step.x;
        this->m_position.y = this->m_position.y + step.y;
        this->m_position.z = this->m_position.z + step.z;

        elapsed = elapsed + stepTime;

        if (ceiling) {
            this->m_flags |= MOVEMENTFLAG_FALLING_FAR;
            this->m_fallTimeMs = 0;
            this->m_fallVelocity = 0.0f;
            this->m_fallStartZ = this->m_position.z;

            consumed = elapsed;
            break;
        }

        if (landed) {
            this->StopFalling();
            consumed = elapsed;
            break;
        }

        if (elapsed + 0.00050000002f >= total) {
            consumed = total;
            break;
        }

        anchorDirty = 1;

        float remaining = length - distance;

        if (stepTime > 0.00050000002f) {
            retries = 1;
        } else if (++retries > 5) {
            if (stopped || length2D == 0.0f) {
                this->StopFalling();
                consumed = elapsed;
                break;
            }

            retries = 0;
            steps++;
            stopped = 1;

            this->m_currentSpeed = 0.0f;

            step.x = 0.0f;
            step.y = 0.0f;
            step.z = dir.z * remaining;

            length = fabsf(step.z);
            continue;
        }

        step.x = dir.x * remaining + slide.x;
        step.y = dir.y * remaining + slide.y;
        step.z = remaining * dir.z;

        if (steps) {
            C2Vector moved2D(this->m_position.x, this->m_position.y);

            float offsetX = moved2D.x - startPos.x;
            float offsetY = moved2D.y - startPos.y;

            C2Vector next2D(step.x, step.y);

            float totalX = next2D.x + offsetX;
            float totalY = next2D.y + offsetY;
            float totalSq = totalX * totalX + totalY * totalY;

            if ((startLength2D + 0.00000095367432f) * (startLength2D + 0.00000095367432f) < totalSq && absZ * 1.1866661f * (absZ * 1.1866661f) < totalSq) {
                this->StopFalling();
                consumed = elapsed;
                break;
            }
        }

        step2D = C2Vector(step.x, step.y);
        length2D = sqrtf(step2D.y * step2D.y + step2D.x * step2D.x);

        if (fabsf(length2D) < 0.00000095367432f) {
            this->m_currentSpeed = 0.0f;
        } else {
            float scale2D = 1.0f / length2D;
            step2D.x = step2D.x * scale2D;
            step2D.y = step2D.y * scale2D;

            float speed = (step2D.x * this->m_moveDir2D.x + step2D.y * this->m_moveDir2D.y) * this->m_currentSpeed;
            this->m_currentSpeed = speed;

            if (!(speed > 0.0f) && !(0.0f == speed))
                this->m_currentSpeed = 0.0f;
        }

        steps++;

        this->m_moveDir2D.x = step2D.x;
        this->m_moveDir2D.y = step2D.y;

        length = sqrtf(step.y * step.y + step.x * step.x + step.z * step.z);

        this->m_moveDir.x = this->m_moveDir2D.x;
        this->m_moveDir.y = this->m_moveDir2D.y;
        this->m_moveDir.z = 0.0f;
    }

    int32_t consumedMs = (int32_t)(consumed * 1000.0f);

    if (!ceiling)
        this->m_fallTimeMs += consumedMs;

    if (apply) {
        //if (ceiling)
        //    this->FallReset(consumedMs + time);

        uint32_t flags = this->m_flags;

        if ((flags & MOVEMENTFLAG_FALLING) != 0) {
            if ((flags & (MOVEMENTFLAG_FALLING_FAR | MOVEMENTFLAG_FALLING_SLOW)) == 0) {
                if (0.0f == this->m_fallVelocity) {
                    if (this->m_fallTimeMs >= 500)
                        this->m_flags = flags | MOVEMENTFLAG_FALLING_FAR;
                } else if (this->m_fallStartZ - 0.11111111f >= this->m_position.z) {
                    this->m_flags = flags | MOVEMENTFLAG_FALLING_FAR;
                }
            }

            uint32_t current = this->m_flags;

            if ((current & MOVEMENTFLAG_FALLING) != 0 && 0.0f != this->m_fallVelocity && (current & MOVEMASK_TRANSLATE) != 0) {
                this->m_currentSpeed = savedSpeed;
                this->m_moveDir2D.x = savedDir2D.x;
                this->m_moveDir2D.y = savedDir2D.y;
                this->m_moveDir.x = savedDir.x;
                this->m_moveDir.y = savedDir.y;
                this->m_moveDir.z = savedDir.z;
            }

            //if (transport)
            //    this->SetTransport(transport);
        } else {
            //if (transport)
            //    this->SetTransport(transport);
            if (this->HandlePendingActions())
                return consumedMs;
        }
    }

    if (anchorDirty)
        this->UpdateAnchors(0);

    return consumedMs;
}

// OFFSET: 0x75E040
bool CMovement_C::CheckFallImpactThreshold(float elapsedFall, float remaining, C3Vector* step, int32_t force) {
    if (force)
        return true;

    if (this->m_fallVelocity < 0.0f) {
        float toPeak = this->TimeToJumpPeak();

        if (remaining + elapsedFall < toPeak)
            return true;
        if (elapsedFall > toPeak)
            return false;

        float reach = (toPeak - elapsedFall) * this->m_currentSpeed;
        if (step->y * step->y + step->x * step->x < reach * reach)
            return true;
    }

    return false;
}

// OFFSET: 0x75EB00
float CMovement_C::GetTimeJustFallen(float remaining, float elapsedFall, C3Vector* step, float* distance, int32_t force, float length2D) {
    int32_t upward = this->CheckFallImpactThreshold(elapsedFall, remaining, step, force);

    float horizontalTime = 0.0f;
    if (length2D > 0.00000095367432f) {
        C2Vector step2D(step->x, step->y);
        horizontalTime = sqrtf(step2D.y * step2D.y + step2D.x * step2D.x) / length2D * remaining;
    }

    float drop = this->m_fallStartZ - this->m_position.z - step->z;
    float impactTime = this->CalcTimeFallen(drop, upward);

    if (elapsedFall >= impactTime) {
        step->x = 0.0f;
        step->y = 0.0f;
        step->z = 0.0f;
        *distance = 0.0f;

        return 0.0f;
    }

    float toImpact = impactTime - elapsedFall;
    if (remaining < toImpact)
        return remaining;

    if (horizontalTime > toImpact) {
        float scale = toImpact / horizontalTime;

        step->x = step->x * scale;
        step->y = step->y * scale;
        *distance = sqrtf(step->x * step->x + step->y * step->y + step->z * step->z);
    }

    return toImpact;
}

// OFFSET: 0x75D2D0
bool CMovement_C::IsFacetSteepBothWays(int32_t facet) {
    float z = CMovement_C::s_moveFacets.facets[facet].plane.n.z;

    if (z > 0.64278764f)
        return false;

    return z >= 0.0f || -z <= 0.64278764f;
}

// OFFSET: 0x75D740
bool CMovement_C::ComputeSteepSurfacePushNormalImpl(C4Plane* plane) {
    float shoulder = this->m_collisionRadius * 1.849399f;

    C3Vector points[5];
    points[0] = { this->m_position.x, this->m_position.y, this->m_position.z };
    points[1] = { this->m_collisionRadius + this->m_position.x, this->m_collisionRadius + this->m_position.y, this->m_position.z + shoulder };
    points[2] = { this->m_position.x - this->m_collisionRadius, this->m_collisionRadius + this->m_position.y, this->m_position.z + shoulder };
    points[3] = { this->m_collisionRadius + this->m_position.x, this->m_position.y - this->m_collisionRadius, this->m_position.z + shoulder };
    points[4] = { this->m_position.x - this->m_collisionRadius, this->m_position.y - this->m_collisionRadius, shoulder + this->m_position.z };

    for (int32_t i = 0; i < 5; i++) {
        if (fabsf(points[i].x * plane->n.x + points[i].z * plane->n.z + points[i].y * plane->n.y + plane->d) < 0.0013888889f)
            return true;
    }

    return false;
}

// OFFSET: 0x75D890
bool CMovement_C::PlaneIntersectsVolume(C4Plane* plane) {
    static const uint8_t edges[32] = {
        0, 1, 0, 2, 0, 3, 0, 4,
        1, 2, 2, 3, 3, 4, 4, 1,
        1, 5, 2, 6, 3, 7, 4, 8,
        5, 6, 6, 7, 7, 8, 8, 5
    };

    float shoulder = this->m_collisionRadius * 1.849399f;

    C3Vector verts[9];
    verts[0] = { this->m_position.x, this->m_position.y, this->m_position.z };
    verts[1] = { this->m_collisionRadius + this->m_position.x, this->m_collisionRadius + this->m_position.y, this->m_position.z + shoulder };
    verts[2] = { this->m_position.x - this->m_collisionRadius, this->m_collisionRadius + this->m_position.y, this->m_position.z + shoulder };
    verts[3] = { this->m_collisionRadius + this->m_position.x, this->m_position.y - this->m_collisionRadius, this->m_position.z + shoulder };
    verts[4] = { this->m_position.x - this->m_collisionRadius, this->m_position.y - this->m_collisionRadius, shoulder + this->m_position.z };
    verts[5] = { this->m_collisionRadius + this->m_position.x, this->m_collisionRadius + this->m_position.y, this->m_collisionHeight + this->m_position.z };
    verts[6] = { this->m_position.x - this->m_collisionRadius, this->m_collisionRadius + this->m_position.y, this->m_collisionHeight + this->m_position.z };
    verts[7] = { this->m_collisionRadius + this->m_position.x, this->m_position.y - this->m_collisionRadius, this->m_collisionHeight + this->m_position.z };
    verts[8] = { this->m_position.x - this->m_collisionRadius, this->m_position.y - this->m_collisionRadius, this->m_collisionHeight + this->m_position.z };

    for (int32_t i = 0; i < 32; i += 2) {
        C3Vector* a = &verts[edges[i]];
        C3Vector* b = &verts[edges[i + 1]];

        float da = a->y * plane->n.y + a->z * plane->n.z + a->x * plane->n.x + plane->d;
        float db = b->y * plane->n.y + b->z * plane->n.z + b->x * plane->n.x + plane->d;

        if (fabsf(da) * fabsf(db) < 0.0f)
            return true;
    }

    return false;
}

// OFFSET: 0x75D680
void CMovement_C::SolveThreePlaneIntersection(C4Plane* first, C4Plane* second, C4Plane* third, C3Vector* out) {
    C33Matrix m;
    m.a0 = third->n.x;
    m.a1 = third->n.y;
    m.a2 = third->n.z;
    m.b0 = first->n.x;
    m.b1 = first->n.y;
    m.b2 = first->n.z;
    m.c0 = second->n.x;
    m.c1 = second->n.y;
    m.c2 = second->n.z;

    float d0 = -third->d;
    float d1 = -first->d;
    float d2 = -second->d;

    C33Matrix inverse = m.Inverse(m.Determinant());

    out->x = inverse.a0 * d0 + inverse.a1 * d1 + inverse.a2 * d2;
    out->y = inverse.b2 * d2 + inverse.b1 * d1 + inverse.b0 * d0;
    out->z = d1 * inverse.c1 + d2 * inverse.c2 + d0 * inverse.c0;
}

// OFFSET: 0x75D4B0
void CMovement_C::ComputeSteepSurfacePushNormalHelper(int32_t facet, C3Vector* edge, C3Vector* origin, C3Vector* out) {
    static const int32_t next[3] = { 1, 2, 0 };

    float best = 3.4028235e38f;

    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;

    CFacet* f = &CMovement_C::s_moveFacets.facets[facet];

    for (int32_t i = 0; i < 3; i++) {
        C3Vector* from = &f->v[i];
        C3Vector* to = &f->v[next[i]];

        float dx = from->x - to->x;
        float dy = from->y - to->y;
        float dz = from->z - to->z;

        float length = sqrtf(dz * dz + dy * dy + dx * dx);
        if (fabsf(length) < 0.00000023841858f)
            continue;

        float inverse = 1.0f / length;
        C3Vector dir = { dx * inverse, dy * inverse, dz * inverse };

        float error;

        if (fabsf(edge->z * dir.z + edge->y * dir.y + edge->x * dir.x - 1.0f) >= 0.00000095367432f) {
            float cx = edge->z * dir.y - edge->y * dir.z;
            float cy = dir.z * edge->x - edge->z * dir.x;
            float cz = dir.x * edge->y - dir.y * edge->x;

            float d = -(origin->z * cz + origin->y * cy + origin->x * cx);
            float at = cx * from->x + cy * from->y + cz * from->z + d;

            error = at * at;
        } else {
            float along = (from->x - origin->x) * edge->x + (from->y - origin->y) * edge->y + (from->z - origin->z) * edge->z;

            float ex = from->x - (edge->x * along + origin->x);
            float ey = from->y - (edge->y * along + origin->y);
            float ez = from->z - (along * edge->z + origin->z);

            error = ez * ez + ey * ey + ex * ex;
        }

        if (best > error) {
            best = error;
            *out = dir;
        }
    }
}

// OFFSET: 0x75E760
C3Vector CMovement_C::GetSteepSurfacePushNormal(int32_t facet, C4Plane* planes, int32_t planeCount) {
    C3Vector push;

    if (planeCount == 1) {
        if (this->PlaneIntersectsVolume(&CMovement_C::s_moveFacets.facets[facet].plane)) {
            push.x = -planes[0].n.x;
            push.y = -planes[0].n.y;
            push.z = -planes[0].n.z;

            return push;
        }

        push = CMovement_C::s_moveFacets.facets[facet].plane.n;
        return push;
    }

    if (planeCount != 2) {
        push = CMovement_C::s_moveFacets.facets[facet].plane.n;
        return push;
    }

    float cx = planes[1].n.z * planes[0].n.y - planes[0].n.z * planes[1].n.y;
    float cy = planes[1].n.x * planes[0].n.z - planes[0].n.x * planes[1].n.z;
    float cz = planes[0].n.x * planes[1].n.y - planes[1].n.x * planes[0].n.y;

    float inverse = 1.0f / sqrtf(cy * cy + cx * cx + cz * cz);

    C3Vector edge;
    edge.x = cx * inverse;
    edge.y = cy * inverse;
    edge.z = cz * inverse;

    CFacet* f = &CMovement_C::s_moveFacets.facets[facet];

    if (fabsf(edge.z) < 0.00000095367432f) {
        push = f->plane.n;
        return push;
    }

    if (!(edge.y * f->plane.n.y + edge.z * f->plane.n.z + edge.x * f->plane.n.x != 0.0f)) {
        push = f->plane.n;
        return push;
    }

    if (fabsf(edge.z - 1.0f) >= 0.00000095367432f && this->ComputeSteepSurfacePushNormalImpl(&f->plane)) {
        push = f->plane.n;
        return push;
    }

    C3Vector origin;
    this->SolveThreePlaneIntersection(&planes[0], &planes[1], &f->plane, &origin);

    C3Vector fit;
    this->ComputeSteepSurfacePushNormalHelper(facet, &edge, &origin, &fit);

    C3Vector cross = C3Vector::Cross(edge, fit);

    float length = sqrtf(cross.y * cross.y + cross.z * cross.z + cross.x * cross.x);

    if (CMath::fequal(length, 0.0f)) {
        push.x = -planes[0].n.x;
        push.y = -planes[0].n.y;
        push.z = -planes[0].n.z;

        return push;
    }

    cross /= length;

    if (planes[0].n.z * cross.z + planes[0].n.x * cross.x + cross.y * planes[0].n.y <= 0.0f) {
        push = cross;
    } else {
        push.x = -cross.x;
        push.y = -cross.y;
        push.z = -cross.z;
    }

    return push;
}

// OFFSET: 0x75DB00
C3Vector CMovement_C::PushOffObstacleEdge(C3Vector* point, C3Vector* dir, int32_t facet) {
    static const int32_t next[3] = { 1, 2, 0 };

    CFacet* f = &CMovement_C::s_moveFacets.facets[facet];

    C3Vector best;
    float bestDistance = 3.4028235e38f;

    for (int32_t i = 0; i < 3; i++) {
        C3Vector* from = &f->v[i];
        C3Vector* to = &f->v[next[i]];

        C3Vector edge;
        edge.x = to->x - from->x;
        edge.y = to->y - from->y;
        edge.z = to->z - from->z;

        float lengthSq = edge.y * edge.y + edge.z * edge.z + edge.x * edge.x;
        if (lengthSq > 0.00000023841858f) {
            float inverse = 1.0f / sqrtf(lengthSq);
            edge.x = edge.x * inverse;
            edge.y = edge.y * inverse;
            edge.z = edge.z * inverse;
        }

        float along = (point->z - from->z) * edge.z + (point->y - from->y) * edge.y + (point->x - from->x) * edge.x;

        float ex = point->x - (along * edge.x + from->x);
        float ey = point->y - (along * edge.y + from->y);
        float ez = point->z - (along * edge.z + from->z);

        float distance = ex * ex + ey * ey + ez * ez;

        if (distance < bestDistance) {
            bestDistance = distance;
            best = edge;
        }
    }

    C3Vector push;
    push.x = f->plane.n.z * best.y - f->plane.n.y * best.z;
    push.y = best.z * f->plane.n.x - f->plane.n.z * best.x;
    push.z = best.x * f->plane.n.y - best.y * f->plane.n.x;

    if (dir->z * push.z + dir->y * push.y + push.x * dir->x < 0.0f) {
        push.x = -push.x;
        push.y = -push.y;
        push.z = -push.z;
    }

    return push;
}

// OFFSET: 0x75E9C0
C2Vector CMovement_C::CalcFallObstaclePush(C3Vector* dir, float moved, float requested, int32_t facet, C4Plane* planes, int32_t planeCount) {
    C3Vector normal;

    if (this->IsFacetSteepBothWays(facet)) {
        normal = this->GetSteepSurfacePushNormal(facet, planes, planeCount);
    } else {
        C3Vector at;
        at.x = dir->x * moved + this->m_position.x;
        at.y = dir->y * moved + this->m_position.y;
        at.z = moved * dir->z + this->m_position.z;

        normal = this->PushOffObstacleEdge(&at, dir, facet);
    }

    C2Vector flat(normal.x, normal.y);

    float lengthSq = flat.y * flat.y + flat.x * flat.x;
    if (lengthSq > 0.00000023841858f) {
        float inverse = 1.0f / sqrtf(lengthSq);
        flat.x = flat.x * inverse;
        flat.y = flat.y * inverse;
    }

    float depth = (-normal.x * dir->x + -normal.y * dir->y + -normal.z * dir->z) * (requested - moved);
    float along = normal.x * flat.x + normal.y * flat.y + normal.z * 0.0f;

    float slide = depth;
    if (fabsf(along) >= 0.00000023841858f)
        slide = depth / along;

    C2Vector push;
    push.x = flat.x * (slide + 0.001f);
    push.y = flat.y * (slide + 0.001f);

    return push;
}

// OFFSET: 0x75B480
void CMovement_C::SaveMoveState(MoveState* out) {
    out->anchorPos = this->m_anchorPos;
    out->anchorFacing = this->m_anchorFacing;
    out->anchorPitch = this->m_anchorPitch;
    out->anchorElapsedMs = this->m_anchorElapsedMs;
    out->fallTimeMs = this->m_fallTimeMs;
    out->moveDir = this->m_moveDir;
    out->moveDir2D = this->m_moveDir2D;
    out->flags = this->m_flags;
    out->currentSpeed = this->m_currentSpeed;
}

// OFFSET: 0x75B4F0
void CMovement_C::RestoreMoveState(MoveState* in) {
    this->m_anchorPos = in->anchorPos;
    this->m_anchorFacing = in->anchorFacing;
    this->m_anchorPitch = in->anchorPitch;
    this->m_anchorElapsedMs = in->anchorElapsedMs;
    this->m_fallTimeMs = in->fallTimeMs;
    this->m_moveDir = in->moveDir;
    this->m_moveDir2D = in->moveDir2D;
    this->m_flags = in->flags;
    this->m_currentSpeed = in->currentSpeed;
}

// OFFSET: 0x7619C0
bool CMovement_C::WillPassObstacle(C2Vector* dir2D, C3Vector* startPos, float height) {
    uint32_t durationMs = (uint32_t)(sqrtf((height + height) * 0.051837362f) * 1000.0f);

    MoveState saved;
    this->SaveMoveState(&saved);

    this->StartFalling(0.0f);

    float drop = this->RelDistanceFallen(durationMs);

    float dx = this->m_moveDir2D.x;
    float dy = this->m_moveDir2D.y;
    float dz = -drop;

    for (uint32_t elapsed = 0; elapsed < durationMs;) {
        if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0)
            break;

        elapsed += this->FallDown(0, durationMs - elapsed, dx, dy, dz, 0);
    }

    this->RestoreMoveState(&saved);

    C2Vector from = { startPos->x, startPos->y };
    C2Vector to = { this->m_position.x, this->m_position.y };

    float ex = to.x - from.x;
    float ey = to.y - from.y;
    float length = sqrtf(ex * ex + ey * ey);

    if (length < this->m_collisionRadius)
        return false;

    float inverse = 1.0f / length;

    return ex * inverse * dir2D->x + ey * inverse * dir2D->y > 0.98480773f;
}

// OFFSET: 0x761B00
bool CMovement_C::AttemptStepUp(C2Vector* dir2D, C3Vector n) {
    C3Vector savedPos = this->m_position;

    C4Plane planes[7];
    for (int32_t i = 0; i < 7; i++) {
        planes[i].n = { 0.0f, 0.0f, 1.0f };
        planes[i].d = 0.0f;
    }

    C2Vector dir = *dir2D;

    float distance = this->m_collisionRadius + 0.0013888889f;
    if (distance < this->GetStepUpHeight() * 1.1917536f)
        distance = this->GetStepUpHeight() * 1.1917536f;

    uint32_t facetIndex;
    uint32_t planeCount;
    float moved;

    if (n.z <= 0.64278764f && n.z >= 0.0f) {
        float inverse = 1.0f / sqrtf(n.x * n.x + n.y * n.y);

        dir.x = -(n.x * inverse);
        dir.y = -(n.y * inverse);

        C3Vector back = { dir.x, dir.y, 0.0f };

        if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &back, distance, &facetIndex, planes, &planeCount, &moved, nullptr))
            return false;

        if (facetIndex == CMovement_C::s_moveFacets.facets.Count() || CMovement_C::s_moveFacets.facets[facetIndex].plane.n != n)
            dir = *dir2D;
    }

    C3Vector up = { 0.0f, 0.0f, 1.0f };
    float upMoved;

    if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &up, this->GetRemainingStepUpHeight(), &facetIndex, planes, &planeCount, &upMoved, nullptr))
        return false;

    float lift = upMoved;
    if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) != 0)
        lift = this->m_position.z - this->m_stepUpStartZ + upMoved;

    if (fabsf(lift) < 0.00000023841858f) {
        this->m_flags &= ~MOVEMENTFLAG_SPLINE_ELEVATION;
        return true;
    }

    this->m_position.z += upMoved;

    C3Vector along = { dir.x, dir.y, 0.0f };

    if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &along, distance, &facetIndex, planes, &planeCount, &moved, nullptr))
        return false;

    this->m_position.x += dir.x * moved;
    this->m_position.y += dir.y * moved;

    if (CMath::fnotequal(moved, distance)) {
        int32_t detached;

        if (this->UseWalkableRedirection(facetIndex, &detached)) {
            float remaining = distance - moved;

            C3Vector pushNormal = { 0.0f, 0.0f, 0.0f };
            int32_t boxPush = this->GetBoxPushNormal(planes, planeCount, &pushNormal);

            C3Vector flat = { dir.x, dir.y, 0.0f };
            C3Vector push = this->CalcRunWalkWalkableObstaclePush(&flat, &remaining, CMovement_C::s_moveFacets.facets[facetIndex].plane.n, boxPush, &pushNormal);

            C3Vector total;
            total.x = dir.x * remaining + push.x;
            total.y = remaining * dir.y + push.y;
            total.z = push.z;

            float length = sqrtf(total.x * total.x + total.y * total.y + total.z * total.z);

            if (CMath::fnotequal(length, 0.0f)) {
                total /= length;

                float advanced;
                if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &total, length, &facetIndex, planes, &planeCount, &advanced, nullptr))
                    return false;

                total *= advanced;

                C2Vector flat2 = { total.x, total.y };
                moved += sqrtf(flat2.x * flat2.x + flat2.y * flat2.y);

                this->m_position += total;
                upMoved += total.z;
            }
        }
    }

    bool stepped = true;

    if (facetIndex != CMovement_C::s_moveFacets.facets.Count() && this->m_collisionRadius >= moved) {
        stepped = false;
    } else {
        C3Vector down = { 0.0f, 0.0f, -1.0f };
        float dropped;

        if (!this->DistanceToMove(&CMovement_C::s_moveFacets, &down, upMoved, &facetIndex, planes, &planeCount, &dropped, nullptr))
            return false;

        this->m_position.z -= dropped;

        if (facetIndex != CMovement_C::s_moveFacets.facets.Count() && this->IsSurfaceTooSteep(facetIndex) && !this->WillPassObstacle(&dir, &savedPos, upMoved - dropped))
            stepped = false;
    }

    this->m_position = savedPos;

    if (!stepped) {
        this->m_flags &= ~MOVEMENTFLAG_SPLINE_ELEVATION;
        return true;
    }

    if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) == 0)
        this->m_stepUpStartZ = savedPos.z;

    this->m_flags |= MOVEMENTFLAG_SPLINE_ELEVATION;
    return true;
}
