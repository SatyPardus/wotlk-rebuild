#include "clientobject/Unit_C.hpp"
#include "db/Db.hpp"
#include <util/Unimplemented.hpp>
#include <gameui/CGWorldFrame.hpp>
#include "gameui/camera/CGCamera.hpp"
#include "gameui/CGGameUI.hpp"
#include <util/Animation.hpp>

// OFFSET: 0x73AC30
void CGUnit_C::UpdateBaseAnimation(uint8_t a2, uint32_t a3) {
    if (this->m_worldModel && !this->m_worldModel->IsLoaded(0, 0))
        return;

    ANIMATION_ID animId = this->ChooseAnimation(a3, a2, nullptr);
    if (animId == ANIM_COUNT) {
        //m_vehiclePassenger = this->m_vehiclePassenger;
        //if (m_vehiclePassenger && *(m_vehiclePassenger + 20))
        //    CGUnit_C::PlayBaseAnimation(this, 0, a2);
        return;
    }

    if (animId == this->m_pendingAnimId)
        this->m_awaitingEndAnimId = animId;

    this->PlayBaseAnimation(animId, a2);

    if ((this->m_animationState & 0x40) != 0) {
        if (this->GetCurrentTorsoAnimId() == 135) {
            //auto camera = CGWorldFrame::GetActiveCamera();
            //if (camera->m_targetGUID == this->m_obj->m_guid)
            //    camera->ResetCamera(this->m_obj->m_guid);
        }
        this->unk_09F8 = this->GetClientStandState();
    }
}

// OFFSET: 0x7385C0
void CGUnit_C::PlayBaseAnimation(ANIMATION_ID animId, uint8_t flags) {
    uint32_t modelFlags = this->m_modelFlags;
    if ((modelFlags & 0x40000) == 0 || (modelFlags & 0x20000) != 0 || !this->GetObjectModel() || !this->m_worldModel)
        return;

    CM2Model* objectModel = this->GetObjectModel();
    if (!objectModel->IsLoaded(0, 0) || !this->m_worldModel->IsLoaded(0, 0)) {
        if (animId != ANIM_NONE)
            this->m_pendingAnimId = animId;
        return;
    }
    this->m_pendingAnimId = ANIM_NONE;

    //####TESTING
    this->m_worldModel->SetBoneSequence(-1, animId, -1, 0, 1.0f, 1, 1);
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x724500
ANIMATION_ID CGUnit_C::ChooseAnimation(uint32_t a2, uint8_t a3, bool* a4) {
    ANIMATION_ID animId = ANIM_COUNT;
    if (!this->ChooseDeathAnim(a2, &animId, a3) && !this->Uses_A30_Flag_0x40000000(a2, &animId))
    {
        //m_vehiclePassenger = this->m_vehiclePassenger;
        //if ( !m_vehiclePassenger || !CVehiclePassenger_C::ChooseSeatAnim(m_vehiclePassenger, a2, &animId) )
        //{
            //m_vehicle = this->m_vehicle;
            //if (m_vehicle && m_vehicle[3] && m_vehicle->sub_756F40())
            //{
            //    CGUnit_C::ChooseSpellVisualKitAnim(this, a2, &animId, a3);
            //    return animId;
            //}
            if (!this->ChooseSubmergeAnim(a2, &animId)
              && !this->ChooseFallAnim(&animId)
              && !this->ChooseMovementAnim(a2, &animId)
              && !this->ChooseLootAnim(a2, &animId)
              && !this->ChooseSpellVisualKitAnim(a2, &animId, a3)
              && !this->ChooseCombatAnim(a2, &animId, a3)
              && !this->ChooseShuffleAnim(a2, &animId)
              && !this->ChooseRangedLoadAnim(a2, &animId)
              && !this->ChooseStandStateAnim(a2, &animId)
              && !this->ChooseEmoteAnim(a2, &animId))
            {
                //if (!this->m_vehiclePassenger || !this->m_vehiclePassenger->ChooseSeatExitAnim(a2, &animId))
                //{
                    if (this->m_pendingAnimId != ANIM_NONE)
                        return this->m_pendingAnimId;
                    if ( a4 )
                        *a4 = 1;
                    if (this->m_awaitingEndAnimId == -1)
                        this->ChooseDefaultAnim(&animId, 0);
                //}
            }
        //}
    }
    return animId;
}

// OFFSET: 0x717260
ANIMATION_ID CGUnit_C::GetCurrentTorsoAnimId() {
    if (!this->m_worldModel || !this->m_worldModel->IsLoaded(0, 0))
        return ANIM_NONE;

    if (this->m_torsoKeyBone == -1)
        return (ANIMATION_ID)this->m_worldModel->GetBoneSequenceId(-1);
    ANIMATION_ID result = (ANIMATION_ID)this->m_worldModel->GetBoneSequenceId(this->m_torsoKeyBone);
    if (result == ANIM_NONE)
        return (ANIMATION_ID)this->m_worldModel->GetBoneSequenceId(-1);
    return result;
}

// OFFSET: 0x716FD0
bool CGUnit_C::Uses_A30_Flag_0x40000000(uint32_t a2, ANIMATION_ID* animId) {
    if ((this->m_animationState & 0x40) == 0 || this->data98C || this->unk_09F8 != 10) {
        return (a2 & 0xFFFFF800) == 0;
    }

    if (this->unk_0A30 & 0x40000000) {
        return false;
    }

    if (!this->m_worldModel->HasSequence(ANIM_BIRTH)) {
        return (a2 & 0xFFFFFFFC) == 0;
    }

    *animId = ANIM_BIRTH;

    return true;
}

// OFFSET: 0x724060
bool CGUnit_C::ChooseDeathAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags) {
    WHOA_UNIMPLEMENTED(false);
}

// OFFSET: 0x71DFF0
bool CGUnit_C::ChooseSubmergeAnim(uint32_t a2, ANIMATION_ID* animId) {
    if (this->m_animationState & 0x400008) {
        return true;
    }

    if ((this->m_animationState & 0x40) == 0 || this->data98C) {
        return (a2 & 0xFFFFFFFC) == 0;
    }

    ANIMATION_ID chosen;

    if (this->GetClientStandState() == 9) {
        chosen = this->unk_09F8 == 9 ? ANIM_SUBMERGED : ANIM_SUBMERGE;
    } else {
        if (this->unk_09F8 != 9) {
            return (a2 & 0xFFFFFFFC) == 0;
        }

        chosen = this->m_worldModel->HasSequence(ANIM_EMERGE) ? ANIM_EMERGE : ANIM_BIRTH;
    }

    if (!this->m_worldModel->HasSequence(chosen)) {
        return (a2 & 0xFFFFFFFC) == 0;
    }

    if (this->GetCurrentTorsoAnimId() != chosen && (a2 & 2)) {
        *animId = chosen;
    }

    return true;
}

// OFFSET: 0x724200
bool CGUnit_C::ChooseFallAnim(ANIMATION_ID* animId) {
    if (!this->movementData.IsFalling())
        return false;

    uint32_t sequenceId = -1;
    if (this->m_worldModel && this->m_worldModel->IsLoaded(0, 0))
        sequenceId = this->m_worldModel->GetBoneSequenceId(-1);

    if (IsAirboneAnim(sequenceId))
        *animId = ANIM_COUNT;
    else
        *animId = ANIM_FALL;

    return true;
}

// OFFSET: 0x717050
bool CGUnit_C::ChooseMovementAnim(uint32_t a2, ANIMATION_ID* animId) {
    if ((this->movementData.m_flags & (MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_FORWARD)) == 0)
        return (a2 & 0xFFFFFFF8) == 0;
    if ((this->m_animationState & 0x70) == 0 || (a2 & 4) == 0 || (this->m_animationState & 0x800000) != 0 || (this->m_animationState & 0x2000000) != 0)
        return 1;
    if ((this->movementData.m_flags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING)) == 0) {
        if (this->movementData.IsOnFlyingSpline()) {
            *animId = ANIM_FLY;
            return 1;
        }
        if ((this->movementData.m_flags & MOVEMENTFLAG_BACKWARD) != 0) {
            *animId = ANIM_WALKBACKWARDS;
            return 1;
        }
        if ((this->m_unit->UNIT_FIELD_BYTES_1 & 0x20000) != 0) {
            *animId = ANIM_STEALTH_WALK;
            return 1;
        }
        auto speed = this->movementData.GetBaseSpeed(0);
        if (speed >= 11.0) {
            *animId = ANIM_SPRINT;
            return 1;
        }
        if (this->movementData.m_walkSpeed + this->movementData.m_walkSpeed < speed) {
            *animId = ANIM_RUN;
            return 1;
        }
        *animId = ANIM_WALK;
        return 1;
    }
    if ((this->movementData.m_flags & (MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_STRAFE_LEFT)) != 0) {
        *animId = ((this->movementData.m_flags & MOVEMENTFLAG_STRAFE_LEFT) != 0) ? ANIM_SWIM_LEFT : ANIM_SWIM_RIGHT;
        return 1;
    } else {
        *animId = (this->movementData.m_flags & MOVEMENTFLAG_BACKWARD) != 0 ? ANIM_SWIM_BACKWARDS : ANIM_SWIM;
        return true;
    }
    return false;
}

// OFFSET: 0x724280
bool CGUnit_C::ChooseLootAnim(uint32_t a2, ANIMATION_ID* animId) {
    if (this->data98C || (this->m_animationState & 0x40) == 0 || !this->ShouldKneelForLoot())
        return (a2 & 0xFFFFFFF0) == 0;
    auto torsoAnim = this->GetCurrentTorsoAnimId();
    if (!this->IsLooting()) {
        if (torsoAnim != ANIM_LOOT_HOLD)
            return (a2 & 0xFFFFFFF0) == 0;
        if ((a2 & 8) != 0)
            *animId = ANIM_LOOT_UP;
        return 1;
    }
    if ((a2 & 8) == 0 || torsoAnim == ANIM_LOOT_HOLD || torsoAnim == ANIM_LOOT)
        return 1;
    *animId = ANIM_LOOT;
    return 1;
}

bool CGUnit_C::ChooseSpellVisualKitAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseCombatAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags) {
    WHOA_UNIMPLEMENTED(false);
}

// OFFSET: 0x71E180
bool CGUnit_C::ChooseShuffleAnim(uint32_t a2, ANIMATION_ID* animId) {
    if (!this->CanShuffle())
        return (a2 & 0xFFFFFF80) == 0;
    if ((this->m_animationState & 0x70) != 0 && (a2 & 0x40) != 0) {
        if ((this->movementData.m_flags & MOVEMENTFLAG_LEFT) == 0 && (this->m_animationState & 0x800) == 0) {
            *animId = ANIM_SHUFFLE_RIGHT;
            return 1;
        }
        *animId = ANIM_SHUFFLE_LEFT;
    }
    return 1;
}

// OFFSET: 0x714F90
bool CGUnit_C::ChooseRangedLoadAnim(uint32_t a2, ANIMATION_ID* animId) {
    if (this->m_sheatheState != 2) {
        return (a2 & 0xFFFFFF00) == 0;
    }

    if ((this->m_animationState & 0x200) == 0) {
        return (a2 & 0xFFFFFF00) == 0;
    }

    if ((a2 & 0x80) == 0 || (this->m_animationState & 0x4000)) {
        return true;
    }

    *animId = ANIM_READY_UNARMED;

    CGUnitVirtualItem* item = this->GetVirtualItem(2, 0);

    if (!item || item->classID != 2) {
        return true;
    }

    switch (item->subclassID) {
    case 2:
        *animId = ANIM_LOAD_BOW;
        break;
    case 3:
    case 18:
        *animId = ANIM_LOAD_RIFLE;
        break;
    case 16:
        *animId = ANIM_LOAD_THROWN;
        break;
    case 19:
        *animId = ANIM_HOLD_THROWN;
        break;
    }

    return true;
}

// OFFSET: 0x71E1F0
bool CGUnit_C::ChooseStandStateAnim(uint32_t a2, ANIMATION_ID* animId) {
    if ((this->m_animationState & 0x40) == 0 || this->data98C) {
        return (a2 & 0xFFFFFE00) == 0;
    }

    ANIMATION_ID anim;

    switch (this->GetClientStandState()) {
    case 0:
        switch (this->unk_09F8) {
        case 1:
            anim = ANIM_SIT_GROUND_UP;
            break;
        case 3:
            anim = ANIM_SLEEP_UP;
            break;
        case 8:
            anim = ANIM_KNEEL_END;
            break;
        default:
            return (a2 & 0xFFFFFE00) == 0;
        }
        break;

    case 1:
        if (!this->unk_09F8 || this->GetCurrentTorsoAnimId() == ANIM_SIT_GROUND_DOWN) {
            anim = ANIM_SIT_GROUND_DOWN;
        } else {
            anim = ANIM_SIT_GROUND;
        }
        break;

    case 3:
        anim = (ANIMATION_ID)(ANIM_SLEEP_DOWN + (this->unk_09F8 != 0));
        if (anim == ANIM_COUNT) {
            return true;
        }
        break;

    case 4:
        anim = ANIM_SIT_CHAIR_LOW;
        break;

    case 5:
        anim = ANIM_SIT_CHAIR_MED;
        break;

    case 6:
        anim = ANIM_SIT_CHAIR_HIGH;
        break;

    case 7:
        if (this->unk_09F8 == 7) {
            return true;
        }

        if (this->m_passenger->m_flags & MOVEMENTFLAG_SWIMMING) {
            anim = ANIM_DROWNED;
        } else {
            anim = /*this->HasAnim466() ? ANIM_DEATH_END_HOLD :*/ ANIM_DEAD;
        }
        break;

    case 8:
        anim = (ANIMATION_ID)(ANIM_KNEEL_START + (this->unk_09F8 != 0));
        if (anim == ANIM_COUNT) {
            return true;
        }
        break;

    default:
        return (a2 & 0xFFFFFE00) == 0;
    }

    if (a2 & 0x100) {
        *animId = anim;
    }

    return true;
}

// OFFSET: 0x7171C0
bool CGUnit_C::ChooseEmoteAnim(uint32_t a2, ANIMATION_ID* animId) {
    uint32_t state = this->m_animationState;

    if (((state & 0x40) == 0 || this->data98C) && (state & 0x20) == 0) {
        return (a2 & 0x200) == 0;
    }

    uint32_t emoteState = this->m_unit->UNIT_NPC_EMOTESTATE;

    if (!emoteState) {
        return (a2 & 0x200) == 0;
    }

    EmotesRec* emote = g_emotesDB.GetRecord(emoteState);

    if (!emote) {
        return (a2 & 0x200) == 0;
    }

    if (CGGameUI::m_interactTarget == this->m_obj->m_guid && (emote->m_emoteFlags & 0x2000)) {
        return (a2 & 0x200) == 0;
    }

    if (a2 & 0x200) {
        *animId = (ANIMATION_ID)emote->m_animID;
    }

    return true;
}

// OFFSET: 0x71E340
void CGUnit_C::ChooseDefaultAnim(ANIMATION_ID* animId, bool a3) {
    if (/*(
            !this->m_vehiclePassenger
            || *(this->m_vehiclePassenger + 20) != 3
            || this->m_vehiclePassenger->sub_747B20(*(this->m_vehiclePassenger + 84)) == 506
        )
        &&*/ (this->m_animationState & 0x800004) == 0) {
        auto currentTorsoAnim = this->GetCurrentTorsoAnimId();
        if (a3 || GetAnimBehaviorId(currentTorsoAnim) != 464) {
            if ((this->movementData.m_flags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING)) != 0) {
                *animId = ANIM_SWIM_IDLE;
            } else if ((this->m_unit->UNIT_FIELD_BYTES_1 & 0x20000) != 0) {
                *animId = ANIM_STEALTH_STAND;
            } else {
                *animId = this->m_passenger->IsSplineFlyer_NotHovering() ? ANIM_HOVER : ANIM_STAND;
            }
        } else {
            *animId = currentTorsoAnim;
        }
    }
}

// OFFSET: 0x73D2B0
void CGUnit_C::PlayFallLandAnimation(uint32_t prevFlags, int32_t fellWithSpeed) {
    uint32_t animationState = this->m_animationState;
    uint32_t wasLanding = animationState & 0x1000000;

    this->m_animationState = animationState & 0xFEFFFFFF;

    // auto vehicle = this->m_vehicle;
    // if (vehicle && vehicle->unk_000C && vehicle->Sub7571C0())
    //     return;

    // auto vehiclePassenger = this->m_vehiclePassenger;
    // if (vehiclePassenger && (vehiclePassenger->m_seatState == 4 || vehiclePassenger->m_seatState == 5))
    //     return;

    if ((this->m_animationState & 0x4000000) != 0) {
        this->PlayBaseAnimation(ANIM_DEATH_END, 0);
        return;
    }

    if (fellWithSpeed || (prevFlags & 0x2000) != 0 || wasLanding) {
        if ((this->m_passenger->m_flags & 0x2200000) == 0) {
            uint32_t moveFlags = this->movementData.m_flags;

            if ((moveFlags & 0xF) == 0) {
                //this->PlayUnitSound(12, 1);
                this->PlayBaseAnimation(ANIM_JUMP_END, 0);
                return;
            }

            if ((moveFlags & 2) == 0 && (moveFlags & 0x100) == 0 && !this->IsRunning()) {
                //this->PlayUnitSound(12, 1);
                this->PlayBaseAnimation(ANIM_JUMP_LAND_RUN, 0);
                return;
            }
        }
    } else if (((prevFlags ^ this->m_passenger->m_flags) & 0x40F) == 0) {
        return;
    }

    this->UpdateBaseAnimation(0, -1);
}
