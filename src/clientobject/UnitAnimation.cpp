#include "clientobject/Unit_C.hpp"
#include "db/Db.hpp"
#include <util/Unimplemented.hpp>
#include <gameui/CGWorldFrame.hpp>
#include "gameui/camera/CGCamera.hpp"

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
    this->m_worldModel->SetBoneSequence(-1, animId, 0, 0, 1.0f, 1, 1);
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

bool CGUnit_C::Uses_A30_Flag_0x40000000(uint32_t a2, ANIMATION_ID* animId) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseDeathAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseSubmergeAnim(uint32_t a2, ANIMATION_ID* animId) {
    WHOA_UNIMPLEMENTED(false);
}

// OFFSET: 0x71DC20
bool IsAirboneAnim(uint32_t sequenceId) {
    auto animationDataRec = g_animationDataDB.GetRecord(sequenceId);
    if (!animationDataRec)
        return false;

    return animationDataRec->m_behaviorID >= 37 && (animationDataRec->m_behaviorID <= 40 || animationDataRec->m_behaviorID == 467);
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

bool CGUnit_C::ChooseLootAnim(uint32_t a2, ANIMATION_ID* animId) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseSpellVisualKitAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseCombatAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseShuffleAnim(uint32_t a2, ANIMATION_ID* animId) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseRangedLoadAnim(uint32_t a2, ANIMATION_ID* animId) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseStandStateAnim(uint32_t a2, ANIMATION_ID* animId) {
    WHOA_UNIMPLEMENTED(false);
}

bool CGUnit_C::ChooseEmoteAnim(uint32_t a2, ANIMATION_ID* animId) {
    WHOA_UNIMPLEMENTED(false);
}

// OFFSET: 0x7176B0
uint32_t GetAnimBehaviorId(ANIMATION_ID animId) {
    auto animationDataRec = g_animationDataDB.GetRecord(animId);
    if (animationDataRec)
        return animationDataRec->m_behaviorID;
    return ANIM_COUNT;
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
