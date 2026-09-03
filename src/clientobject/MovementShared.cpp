#include "clientobject/MovementShared.hpp"
#include <common/time/Time.hpp>
#include <util/Byte.hpp>
#include "clientobject/Types.hpp"
#include "clientobject/Movement.hpp"
#include <tempest/math/CMath.hpp>

CMovementShared::CMovementShared(WGUID* transportGuid, C3Vector& position, float facing, WGUID* guid)
    : CPassenger(transportGuid, position, guid) {
    this->m_groundNormal.x = 0.0;
    this->m_groundNormal.y = 0.0;
    this->m_groundNormal.z = 1.0;
    this->m_flags = 0;
    LOWORD(this->m_flags2) = 0;
    BYTE2(this->m_flags2) = -1;
    this->m_anchorPos.x = position.x;
    this->m_anchorPos.y = position.y;
    this->m_anchorFacing = facing;
    this->m_anchorPos.z = position.z;
    this->m_anchorPitch = 0.0;
    this->m_moveDir.x = 0.0;
    this->m_moveDir.y = 0.0;
    this->m_moveDir.z = 0.0;
    this->m_moveDir2D.x = 0.0;
    this->m_moveDir2D.y = 0.0;
    this->m_fallTimeMs = 0;
    this->m_pitchSin = 0.0;
    this->m_pitchCos = 1.0;
    LOBYTE(this->unk_002C) |= 1u;
    this->m_fallStartZ = position.z;
    this->m_spline = 0;
    this->m_currentSpeed = 0.0;
    this->m_walkSpeed = 0.0;
    this->m_runSpeed = 0.0;
    this->m_runBackSpeed = 0.0;
    this->m_swimSpeed = 0.0;
    this->m_swimBackSpeed = 0.0;
    this->m_flightSpeed = 0.0;
    this->m_flightBackSpeed = 0.0;
    this->m_turnRate = 0.0;
    this->m_pitchRate = 0.0;
    this->m_fallVelocity = 0.0;
    this->m_facing = facing;
    this->m_statusTimeMs = OsGetAsyncTimeMs();
    this->m_hoverHeight = 1.0;
}

// OFFSET: 0x4F5240
bool CMovementShared::IsOnSpline() {
    return this->m_spline && (this->m_spline->flags & SPLINE_FLAG_NO_SPLINE) == 0;
}

// OFFSET: 0x4F5260
bool CMovementShared::IsOnFlyingSpline() {
    if (!this->m_spline)
        return false;

    return (this->m_spline->flags & SPLINE_FLAG_NO_SPLINE) == 0 && (this->m_spline->flags & SPLINE_FLAG_FLYING) != 0;
}

// OFFSET: 0x6E9A70
bool CMovementShared::IsOnFallingSpline() {
    if (!this->m_spline)
        return false;

    const uint32_t flags = this->m_spline->flags;
    return (this->m_spline->flags & SPLINE_FLAG_NO_SPLINE) == 0 && (this->m_spline->flags & SPLINE_FLAG_FALLING) != 0;
}

// OFFSET: 0x75EDA0
bool CMovementShared::IsSplineFlyer_FlyingSwimming() {
    bool fallingSpline = false;

    if (this->m_spline) {
        uint32_t flags = this->m_spline->flags;

        if ((flags & SPLINE_FLAG_NO_SPLINE) == 0 && (flags & SPLINE_FLAG_FALLING) != 0)
            fallingSpline = true;
        else if ((flags & SPLINE_FLAG_NO_SPLINE) == 0 && (flags & SPLINE_FLAG_FLYING) != 0)
            return true;
    }

    if (!fallingSpline) {
        if ((this->m_flags2 & MOVEMENTFLAG2_UNK3) != 0)
            return true;
        if ((this->m_flags & MOVEMENTFLAG_DISABLE_GRAVITY) != 0)
            return true;
    }

    return (this->m_flags & MOVEMASK_SWIM_FLY) != 0;
}

// OFFSET: 0x6EABA0
bool CMovementShared::IsFallingSwimmingFlying() {
    bool fallingSpline = false;

    if (this->m_spline) {
        uint32_t flags = this->m_spline->flags;

        if ((flags & SPLINE_FLAG_NO_SPLINE) == 0 && (flags & SPLINE_FLAG_FALLING) != 0)
            fallingSpline = true;
        else if ((flags & SPLINE_FLAG_NO_SPLINE) == 0 && (flags & SPLINE_FLAG_FLYING) != 0)
            return true;
    }

    if (!fallingSpline) {
        if ((this->m_flags2 & MOVEMENTFLAG2_UNK3) != 0)
            return true;
        if ((this->m_flags & MOVEMENTFLAG_DISABLE_GRAVITY) != 0)
            return true;
    }

    return (this->m_flags & MOVEMASK_AIRBORNE) != 0;
}

// OFFSET: 0x6E9AD0
bool CMovementShared::IsGravityDisabled() {
    if (this->m_spline) {
        if ((this->m_spline->flags & SPLINE_FLAG_NO_SPLINE) == 0 && (this->m_spline->flags & SPLINE_FLAG_FALLING) != 0)
            return 0;
        if ((this->m_spline->flags & SPLINE_FLAG_NO_SPLINE) == 0 && (this->m_spline->flags & SPLINE_FLAG_FLYING) != 0)
            return 1;
    }
    return (this->m_flags2 & MOVEMENTFLAG2_UNK3) != 0 || (this->m_flags & MOVEMENTFLAG_DISABLE_GRAVITY) != 0;
}

// OFFSET: 0x75EE00
bool CMovementShared::CanCollideWhileFlying() {
    if (this->IsSplineFlyer_FlyingSwimming())
        return true;

    if ((this->m_flags2 & MOVEMENTFLAG2_UNK8) != 0) {
        CMoveSpline* spline = this->m_spline;
        if (spline && (spline->flags & SPLINE_FLAG_NO_SPLINE) == 0 && (spline->flags & SPLINE_FLAG_PARABOLIC) != 0)
            return true;
    }

    return this->IsOnFallingSpline();
}

// OFFSET: 0x987E30
void CMovementShared::CalcDirection() {
    this->m_moveDir2D = C2Vector(cos(this->m_anchorFacing), sin(this->m_anchorFacing));
    if ((this->m_flags & 0x2200000) != 0 && fabs(this->m_anchorPitch) >= 0.00000095367432) {
        this->m_pitchCos = cos(this->m_anchorPitch);
        this->m_pitchSin = sin(this->m_anchorPitch);
        this->m_moveDir.x = this->m_pitchCos * this->m_moveDir2D.x;
        this->m_moveDir.y = this->m_pitchCos * this->m_moveDir2D.y;
        this->m_moveDir.z = this->m_pitchSin;
    } else {
        this->m_moveDir.x = this->m_moveDir2D.x;
        this->m_moveDir.y = this->m_moveDir2D.y;
        this->m_pitchSin = 0.0;
        this->m_moveDir.z = 0.0;
        this->m_pitchCos = 1.0;
    }
}

// OFFSET: 0x987EF0
void CMovementShared::CalcDirection(bool ignoreFalling) {
    if ((this->m_flags & MOVEMENTFLAG_FALLING) != 0 && !ignoreFalling)
        return;

    this->CalcDirection();

    if ((this->m_flags & MOVEMASK_FWDBACK) != 0 && (this->m_flags & MOVEMASK_STRAFE) != 0) {
        C2Vector fwd2D = this->m_moveDir2D;
        C3Vector fwd3D = this->m_moveDir;

        if ((this->m_flags & MOVEMENTFLAG_BACKWARD) != 0) {
            fwd2D = -fwd2D;
            fwd3D = -fwd3D;
        }

        const float swap = this->m_moveDir2D.x;
        this->m_moveDir2D.x = this->m_moveDir2D.y;
        this->m_moveDir2D.y = swap;

        if ((this->m_flags & MOVEMENTFLAG_STRAFE_LEFT) != 0)
            this->m_moveDir2D.x = -this->m_moveDir2D.x;
        else
            this->m_moveDir2D.y = -this->m_moveDir2D.y;

        this->m_moveDir.x = this->m_moveDir2D.x + fwd3D.x;
        this->m_moveDir.y = this->m_moveDir2D.y + fwd3D.y;
        this->m_moveDir.z = fwd3D.z;

        this->m_moveDir2D += fwd2D;

        this->m_moveDir *= 0.70710677f;
        this->m_moveDir2D *= 0.70710677f;
    } else if ((this->m_flags & MOVEMASK_STRAFE) != 0) {
        const float swap = this->m_moveDir2D.x;
        this->m_moveDir2D.x = this->m_moveDir2D.y;
        this->m_moveDir2D.y = swap;

        if ((this->m_flags & MOVEMENTFLAG_STRAFE_LEFT) != 0)
            this->m_moveDir2D.x = -this->m_moveDir2D.x;
        else
            this->m_moveDir2D.y = -this->m_moveDir2D.y;

        this->m_moveDir.x = this->m_moveDir2D.x;
        this->m_moveDir.y = this->m_moveDir2D.y;
        this->m_moveDir.z = 0.0f;
        return;
    } else if ((this->m_flags & MOVEMENTFLAG_BACKWARD) != 0) {
        this->m_moveDir = -this->m_moveDir;
        this->m_moveDir2D = -this->m_moveDir2D;
    }
}

// OFFSET: 0x986F00
float CMovementShared::CalcFallStartElevation(float elapsed, int32_t slowFall, float velocity) {
    float terminal = slowFall ? 7.0f : 60.148003f;

    float v = velocity;
    if (velocity > terminal)
        v = terminal;

    if (19.291105f * elapsed + v > terminal) {
        float rampTime = (terminal - v) * 0.051837362f;
        return terminal * (elapsed - rampTime) + (v + 9.6455526f * rampTime) * rampTime;
    }

    return elapsed * (v + elapsed * 9.6455526f);
}

// OFFSET: 0x986F70
float CMovementShared::CalcFallStartElevation(int32_t time) {
    return this->CalcFallStartElevation(time * 0.001f, this->m_flags & MOVEMENTFLAG_FALLING_SLOW, this->m_fallVelocity) * this->m_position.z;
}

// OFFSET: 0x9880C0
void CMovementShared::CalcCurrentSpeed(bool ignoreFalling) {
    if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0 || ignoreFalling)
        this->m_currentSpeed = this->GetBaseSpeed(ignoreFalling);
}

// OFFSET: 0x988280
float CMovementShared::CalcTimeFallen(float distance, int32_t upward) {
    float terminal = 60.148003f;
    if ((this->m_flags & MOVEMENTFLAG_FALLING_SLOW) != 0)
        terminal = 7.0f;

    float velocity = this->m_fallVelocity;
    if (velocity > terminal)
        velocity = terminal;

    if (fabsf(velocity) < 0.00000023841858f)
        return this->TimeToFallDistance(distance, (this->m_flags & MOVEMENTFLAG_FALLING_SLOW) != 0);

    float discriminant = velocity * velocity + 38.582211f * distance;

    float root = 0.0f;
    if (discriminant > 0.0f)
        root = sqrtf(discriminant);

    float rising = (-velocity - root) * 0.051837362f;
    float falling = (root - velocity) * 0.051837362f;
    float toTerminal = 0.051837362f * (terminal - velocity);

    float result;
    if (falling <= toTerminal) {
        result = falling;
    } else {
        result = (distance - (velocity + toTerminal * 9.6455526f) * toTerminal) / terminal + toTerminal;
    }

    if (upward) {
        if (rising >= 0.0f)
            return rising;
        return 0.0f;
    }

    return result;
}

// OFFSET: 0x986FB0
float CMovementShared::GetDistanceFallen() {
    if (this->m_spline && (this->m_spline->flags & 0xA00) != 0) {
        float elapsed = (this->m_spline->m_duration - this->m_spline->m_effectStartTime) * 0.001f;
        float velocity = -(this->m_spline->m_verticalAcceleration * elapsed * 0.5f);
        return this->CalcFallStartElevation(elapsed, 0, velocity);
    }

    float result = this->m_fallStartZ - this->m_position.z;
    if (this->m_fallVelocity < -0.00000023841858f)
        result += this->m_fallVelocity * this->m_fallVelocity * 0.025918681f;
    if (result < 0.0)
        return 0.0;
    return result;
}

// OFFSET: 0x987050
float CMovementShared::RelDistanceFallen(int32_t elapsedMs, float z) {
    uint32_t flags = this->m_flags;
    float velocity = this->m_fallVelocity;

    float ms = (float)elapsedMs;
    if (elapsedMs < 0)
        ms = ms + 4294967300.0f;

    float fallen = CMovementShared::CalcFallStartElevation(ms * 0.001f, flags & MOVEMENTFLAG_FALLING_SLOW, velocity);

    if ((flags & MOVEMENTFLAG_FALLING) != 0)
        return fallen + z - this->m_fallStartZ;

    return fallen;
}

// OFFSET: 0x9870D0
float CMovementShared::RelDistanceFallen(int32_t elapsedMs) {
    return this->RelDistanceFallen(elapsedMs, this->m_position.z);
}

// OFFSET: 0x9881D0
void CMovementShared::UpdateAnchors(bool a2) {
    this->m_anchorFacing = this->m_facing;
    this->m_anchorPos = this->m_position;
    this->m_anchorPitch = this->m_pitch;
    this->m_anchorElapsedMs = 0;
    this->CalcDirection(a2);
}

// OFFSET: 0x987570
float CMovementShared::GetBaseSpeed(bool a2) {
    if ((this->m_flags & 0xC0000F) == 0)
        return 0.0;
    if (!this->IsOnSpline()) {
        if ((this->m_flags & 0x2000000) != 0) {
            if ((this->m_flags & 2) != 0 && this->m_flightSpeed >= this->m_flightBackSpeed)
                return this->m_flightBackSpeed;
            else
                return this->m_flightSpeed;
        } else if ((this->m_flags & 0x200000) != 0) {
            if ((this->m_flags & 2) != 0 && this->m_swimSpeed >= this->m_swimBackSpeed)
                return this->m_swimBackSpeed;
            else
                return this->m_swimSpeed;
        } else {
            if ((this->m_flags & 0x100) != 0 || a2) {
                if (this->m_runSpeed > this->m_walkSpeed)
                    return this->m_walkSpeed;
            } else if ((this->m_flags & 2) != 0 && this->m_runSpeed >= this->m_runBackSpeed) {
                return this->m_runBackSpeed;
            }
            return this->m_runSpeed;
        }
    } else {
        if (!m_spline->m_duration)
            return 0.0f;
        return this->m_spline->spline.m_length / m_spline->m_duration * 1000.0f;
    }
}

// OFFSET: 0x986DE0
float CMovementShared::GetModifiedSplineDuration() {
    return this->m_spline->m_duration * this->m_spline->m_durationMod;
}

// OFFSET: 0x986E80
float CMovementShared::TimeToJumpPeak() {
    if ((this->m_flags & MOVEMENTFLAG_FALLING) != 0)
        return this->m_fallVelocity / -19.291103f;
    else
        return 0.0;
}

// OFFSET: 0x988220
float CMovementShared::TimeToFallDistance(float distance, bool slowFall) {
    float terminal = slowFall ? 7.0f : 60.148003f;
    float rampDistance = 0.051837362f * terminal * terminal * 0.5f;

    if (distance >= rampDistance)
        return (distance - rampDistance) / terminal + 0.051837362f * terminal;
    if (distance <= 0.0f)
        return 0.0f;

    return sqrtf(distance * 0.10367472f);
}

// OFFSET: 0x988A20
bool CMovementShared::StartMove(bool a2, bool a3) {
    this->m_flags &= 0xFFFCBFFF;

    bool v5 = false;
    bool proceed;
    if (a3 || (this->m_flags & MOVEMENTFLAG_FALLING) == 0) {
        proceed = true;
    } else if ((this->m_flags & 0xF) == 0) {
        v5 = true;
        proceed = true;
    } else {
        proceed = false;
    }

    if (proceed) {
        if (a2)
            this->m_flags = (this->m_flags & 0xFFFFFFFC) | MOVEMENTFLAG_FORWARD;
        else
            this->m_flags = (this->m_flags & 0xFFFFFFFC) | MOVEMENTFLAG_BACKWARD;

        this->m_anchorFacing = this->m_facing;
        this->m_anchorPitch = this->m_pitch;
        this->m_anchorPos = this->m_position;
        this->m_anchorElapsedMs = 0;

        this->CalcDirection(v5);

        if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0 || v5)
            this->m_currentSpeed = this->GetBaseSpeed(v5);
        return true;
    }

    if (a2) {
        if ((this->m_flags & MOVEMENTFLAG_FORWARD) == 0) {
            this->m_flags = this->m_flags | 0x10000;
            return false;
        }
    } else if ((this->m_flags & MOVEMENTFLAG_BACKWARD) == 0) {
        this->m_flags = this->m_flags | 0x20000;
    }
    return false;
}

// OFFSET: 0x98C8D0
bool CMovementShared::StopMove() {
    if ((this->m_flags & 3) != 0) {
        if ((this->m_flags & 0x4000000) != 0) {
            this->m_flags = this->m_flags & 0xFBFFFFFF;
            if (!this->IsFallingSwimmingFlying())
                this->StartFalling(0.0f);
        }
        if ((this->m_flags & 0x1000) != 0) {
            this->m_flags = this->m_flags & 0xFFFCBFFF | 0x4000;
            return 0;
        } else {
            this->ForceStopMove(1);
            return 1;
        }
    } else {
        if ((m_flags & 0x30000) != 0)
            this->m_flags = m_flags & 0xFFFCFFFF;
        return 0;
    }
}

// OFFSET: 0x98C8A0
bool CMovementShared::StopMove(uint32_t flags) {
    if ((flags & (MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_FORWARD)) == 0 || (this->m_flags & MOVEMENTFLAG_FALLING) != 0)
        return 0;
    this->ForceStopMove(1);
    return 1;
}

// OFFSET: 0x98BD10
void CMovementShared::ForceStopMove(bool a2) {
    this->m_flags &= 0xFFFFBFFC;

    if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0)
        this->m_currentSpeed = this->GetBaseSpeed(0);

    this->UpdateAnchors(false);

    CMoveSpline* spline = this->m_spline;
    if (spline && (spline->flags & SPLINE_FLAG_NO_SPLINE) == 0) {
        spline->flags |= SPLINE_FLAG_NO_SPLINE;

        if (a2) {
            CMoveSpline* s = this->m_spline;
            if (s && (s->flags & 0x2000) != 0 && !this->IsFallingSwimmingFlying()) {
                this->StartFalling(0.0f);
            }
        }
    }
}

// OFFSET: 0x988B00
bool CMovementShared::StartStrafe(bool a2) {
    this->m_flags &= 0xFFF37FFF;
    if ((this->m_flags2 & 1) != 0)
        return 0;

    bool v5 = false;
    bool proceed;
    if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0) {
        proceed = true; // v5 stays false
    } else if ((this->m_flags & 0xF) == 0) {
        v5 = true;
        proceed = true;
    } else {
        proceed = false;
    }

    if (proceed) {
        if (a2)
            this->m_flags = (this->m_flags & 0xFFFFFFF3) | MOVEMENTFLAG_STRAFE_LEFT;
        else
            this->m_flags = (this->m_flags & 0xFFFFFFF3) | MOVEMENTFLAG_STRAFE_RIGHT;

        this->UpdateAnchors(v5);

        if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0 || v5)
            this->m_currentSpeed = this->GetBaseSpeed(v5);
        return true;
    }

    if (a2) {
        if ((this->m_flags & MOVEMENTFLAG_STRAFE_LEFT) == 0) {
            this->m_flags = this->m_flags | 0x40000;
            return false;
        }
    } else if ((this->m_flags & MOVEMENTFLAG_STRAFE_RIGHT) == 0) {
        this->m_flags = this->m_flags | 0x80000;
    }
    return false;
}

// OFFSET: 0x98BF80
bool CMovementShared::StopStrafe() {
    if ((this->m_flags & MOVEMASK_STRAFE) != 0) {
        if ((this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) != 0) {
            this->m_flags = this->m_flags & 0xFBFFFFFF;
            if (!this->IsFallingSwimmingFlying())
                this->StartFalling(0.0f);
        }
        if ((this->m_flags & 0x1000) != 0) {
            this->m_flags = this->m_flags & 0xFFF37FFF | 0x8000;
            return 0;
        } else {
            this->ForceStopStrafe();
            return 1;
        }
    } else {
        if ((this->m_flags & 0xC0000) != 0)
            this->m_flags = this->m_flags & 0xFFF3FFFF;
        return 0;
    }
}

// OFFSET: 0x988DC0
bool CMovementShared::StopStrafe(uint32_t flags) {
    if ((flags & 0xC) == 0 || (this->m_flags & 0x1000) != 0)
        return 0;
    this->ForceStopStrafe();
    return 1;
}

// OFFSET: 0x988BA0
void CMovementShared::ForceStopStrafe() {
    this->m_flags &= 0xFFFF7FF3;
    if ((this->m_flags & 0x1000) == 0)
        this->m_currentSpeed = this->GetBaseSpeed(0);

    this->UpdateAnchors(false);
}

// OFFSET: 0x9898E0
bool CMovementShared::StartAscensionDescension(bool a2) {
    m_flags = this->m_flags;
    if ((this->m_flags & 0x2200000) == 0 || (this->m_flags2 & 1) != 0)
        return 0;
    if (a2)
        this->m_flags = this->m_flags | 0x400000;
    else
        this->m_flags = this->m_flags | 0x800000;
    this->UpdateAnchors(0);
    if ((this->m_flags & 0x1000) == 0)
        this->m_currentSpeed = this->GetBaseSpeed(0);
    return 1;
}

// OFFSET: 0x989940
bool CMovementShared::StopAscensionDescension() {
    if ((this->m_flags & 0x2200000) == 0)
        return 0;
    this->m_flags = this->m_flags & 0xFF3FFFFF;
    if ((this->m_flags & 0x1000) == 0)
        this->m_currentSpeed = this->GetBaseSpeed(0);

    this->UpdateAnchors(false);
    return 1;
}

// Offset: 0x988DF0
bool CMovementShared::StartTurn(bool a2) {
    if (a2)
        this->m_flags = this->m_flags & 0xFFFFFFCF | MOVEMENTFLAG_LEFT;
    else
        this->m_flags = this->m_flags & 0xFFFFFFCF | MOVEMENTFLAG_RIGHT;
    this->m_flags2 &= ~MOVEMENTFLAG2_INTERPOLATED_TURNING;

    this->UpdateAnchors(false);
    return 1;
}

// OFFSET: 0x989010
bool CMovementShared::StopTurn() {
    if ((this->m_flags & 0x30) == 0)
        return 0;
    this->m_flags = this->m_flags & 0xFFFFFFCF;

    this->UpdateAnchors(false);
    return 1;
}

// OFFSET: 0x988370
bool CMovementShared::StartFalling(float velocity) {
    if ((this->m_flags & MOVEMASK_ROOTED) != 0)
        return false;

    if (this->m_spline && (this->m_spline->flags & (SPLINE_FLAG_FALLING | SPLINE_FLAG_NO_SPLINE)) != 0)
        return false;

    this->UpdateAnchors(0);

    uint32_t flags = this->m_flags & 0xF91FEFFF | MOVEMENTFLAG_FALLING;
    this->m_flags = flags;
    if ((this->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING) == 0)
        this->m_flags = flags & 0xFFFFFF3F;

    this->m_fallTimeMs = 0;
    this->m_fallStartZ = this->m_position.z;
    this->m_fallVelocity = velocity;

    return true;
}

// OFFSET: 0x98B710
bool CMovementShared::TryStartFalling() {
    if (this->IsFallingSwimmingFlying())
        return 0;
    return this->StartFalling(0.0);
}

// OFFSET: 0x988490
void CMovementShared::StopFalling() {
    bool wasFalling = (this->m_flags & MOVEMENTFLAG_FALLING);
    if ((this->m_flags & MOVEMENTFLAG_FALLING) != 0)
        this->m_flags = this->m_flags & 0xFFFFCFFF;
    if ((this->m_flags & MOVEMENTFLAG_PENDING_ROOT) != 0) {
        this->m_flags = this->m_flags & 0xFF203700 | MOVEMENTFLAG_ROOT;
    } else if (!wasFalling) {
        return;
    }

    this->UpdateAnchors(false);

    if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0)
        this->m_currentSpeed = this->GetBaseSpeed(0);
}

void CMovementShared::StopFallingAlwaysAnchor() {
    bool wasFalling = (this->m_flags & MOVEMENTFLAG_FALLING);
    if ((this->m_flags & MOVEMENTFLAG_FALLING) != 0)
        this->m_flags = this->m_flags & 0xFFFFCFFF;
    if ((this->m_flags & MOVEMENTFLAG_PENDING_ROOT) != 0) {
        this->m_flags = this->m_flags & 0xFF203700 | MOVEMENTFLAG_ROOT;
    } else if (!wasFalling) {
        return;
    }

    this->UpdateAnchors(false);

    if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0)
        this->m_currentSpeed = this->GetBaseSpeed(0);
}

// OFFSET: 0x98C240
void CMovementShared::StopFlying() {
    this->m_flags &= 0xFD3FFF3F;
    this->m_anchorPitch = 0.0;
    this->m_pitch = 0.0;
    if (!this->IsFallingSwimmingFlying())
        this->StartFalling(0.0);
    if ((this->m_flags & 0x1000) == 0)
        this->m_currentSpeed = this->GetBaseSpeed(0);

    this->UpdateAnchors(false);
}

// OFFSET: 0x9883F0
bool CMovementShared::Jump(bool a2) {
    if (a2) {
        if ((this->m_flags & MOVEMENTFLAG_HOVER) != 0 && (this->m_flags & MOVEMENTFLAG_SWIMMING) == 0)
            return 0;
    }
    if (this->m_spline) {
        if ((this->m_spline->flags & SPLINE_FLAG_NO_SPLINE) == 0 && (this->m_spline->flags & SPLINE_FLAG_FLYING) != 0)
            return 0;
    }
    if ((this->m_flags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ROOT)) != 0)
        return 0;
    if ((this->m_flags2 & MOVEMENTFLAG2_UNK3) == 0) {
        if (this->IsGravityDisabled())
            return 0;
    }
    if ((this->m_flags2 & 2) != 0)
        return 0;

    float velocity = -7.9555473f;
    if ((this->m_flags & MOVEMENTFLAG_SWIMMING) != 0)
        velocity = -9.0967484f;
    this->StartFalling(velocity);
    return 1;
}

// OFFSET: 0x98B5B0
void CMovementShared::ToggleMovementFlag2_0x40(bool active) {
    if (active)
        this->m_flags2 |= MOVEMENTFLAG2_UNK7;
    else
        this->m_flags2 &= ~MOVEMENTFLAG2_UNK7;
}

// OFFSET: 0x98B570
void CMovementShared::ToggleMovementFlag2_0x80(bool active) {
    if (active)
        this->m_flags2 |= MOVEMENTFLAG2_UNK8;
    else
        this->m_flags2 &= ~MOVEMENTFLAG2_UNK8;
}

// OFFSET: 0x98B590
void CMovementShared::ToggleMovementFlag2_0x100(bool active) {
    if (active)
        this->m_flags2 |= MOVEMENTFLAG2_UNK9;
    else
        this->m_flags2 &= ~MOVEMENTFLAG2_UNK9;
}

// OFFSET: 0x9870F0
void CMovementShared::AddSpline(C3Vector* dest) {
    if (!this->m_spline)
        this->m_spline = MovementNewSpline();
    this->m_spline->flags = 0;
    this->m_spline->m_finalDestination = *dest;
}

// OFFSET: 0x98B730
void CMovementShared::RemoveSpline() {
    this->m_flags2 &= ~0x80u;
    if (this->m_spline) {
        MovementDelSpline(this->m_spline);
        this->m_spline = nullptr;
        if ((this->m_flags & 0x1000) == 0)
            this->m_currentSpeed = this->GetBaseSpeed(0);
    }
}

// OFFSET: 0x98C770
void CMovementShared::OnSpline(int32_t timePassed, C3Vector* points, uint32_t pointCount, int32_t duration, uint32_t flags, uint32_t id) {
    this->m_spline->flags = flags;
    this->m_spline->m_timePassed = timePassed;
    this->m_spline->start = 0;
    this->m_spline->m_duration = duration;
    this->m_spline->spline.m_splineMode = (m_spline->flags & Mask_CatmullRom) != 0;
    this->m_spline->spline.SetPoints(points, pointCount);
    this->m_spline->m_id = id;
    this->m_spline->m_durationModNext = 1.0;
    this->m_spline->m_durationMod = 1.0;
    this->StartMove((this->m_spline->flags & 0x8000000) == 0, 1);
    if ((this->m_flags & 0x800) != 0 && (this->m_spline->flags & 0xA00) != 0)
        this->m_flags = m_flags & 0xFFEFF7FF | 0x100000;
    if ((this->m_spline->flags & 0x200) != 0) {
        if ((this->m_flags & 0x2000000) != 0)
            this->StopFlying();
        this->StartFalling(0.0);
    }
    this->m_spline->m_effectStartTime = 0;
    this->m_spline->m_verticalAcceleration = 0.0;
}

// OFFSET: 0x987140
void CMovementShared::GetMoveStatus(NETMESSAGE msgId, int32_t time, CClientMoveUpdate* moveUpdate) {
    moveUpdate->status.m_gameTime = time;
    moveUpdate->status.m_moveExtraFlags = this->m_flags2;
    if (this->m_transportGuid != 0 || msgId == CMSG_MOVE_CHNG_TRANSPORT) {
        //moveUpdate->status.m_transportGuid = this->m_transportGuid;
        //moveUpdate->status.m_transportTime = MovementGetGlobals()->m_transportTime;
        //if (MovementGetGlobals()->ukn78) {
        //    moveUpdate->status.m_moveExtraFlags |= 0x400u;
        //    moveUpdate->status.m_transportTime2 = MovementGetGlobals()->m_transportTime2;
        //    MovementGetGlobals()->ukn78 = 0;
        //} else {
        //    moveUpdate->status.m_transportTime2 = 0;
        //}
        //moveUpdate->status.m_moveFlags |= 0x200u;
        //moveUpdate->status.m_transportSeat = BYTE2(this->m_flags2);
        SErrDisplayAppFatal("Not implemented yet");
    } else {
        moveUpdate->status.m_moveFlags &= ~0x200u;
        moveUpdate->status.m_transportGuid.guid_low = 0;
        moveUpdate->status.m_transportGuid.guid_high = 0;
        moveUpdate->status.m_transportTime = 0.0;
        moveUpdate->status.m_transportTime2 = 0;
    }
    if (this->IsOnSpline())
        moveUpdate->status.m_moveFlags &= ~0x8000000u;
    else
        moveUpdate->status.m_moveFlags |= 0x8000000u;
    moveUpdate->status.m_transportPosition = this->m_position;
    moveUpdate->status.m_transportFacing = this->m_facing;
    C3Vector pos;
    this->GetPosition(&pos, &this->m_position);
    moveUpdate->status.m_position = pos;
    moveUpdate->status.m_facing = this->GetFacing(this->m_facing);
    moveUpdate->status.m_pitch = this->m_pitch;
    moveUpdate->status.m_fallTime = this->m_fallTimeMs;
    if ((moveUpdate->status.m_moveFlags & 0x1000) != 0) {
        moveUpdate->status.m_zSpeed = this->m_fallVelocity;
        moveUpdate->status.m_sinAngle = this->m_moveDir2D.x;
        moveUpdate->status.m_cosAngle = this->m_moveDir2D.y;
        moveUpdate->status.m_xySpeed = this->m_currentSpeed;
    }
    if ((this->m_flags & 0x4000000) != 0)
        moveUpdate->status.m_splineElevation = this->m_stepUpStartZ;
}

// OFFSET: 0x988920
void CMovementShared::UpdateBaseStatus(CMovementStatus* status) {
    this->m_position = status->m_position;
    this->m_anchorPos = status->m_position;

    this->m_facing = status->m_facing;
    this->m_anchorFacing = status->m_facing;

    this->m_pitch = status->m_pitch;
    this->m_anchorPitch = status->m_pitch;

    this->m_anchorElapsedMs = 0;
    this->m_stepUpStartZ = status->m_splineElevation;

    if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0)
        this->m_currentSpeed = this->GetBaseSpeed(0);
}

// OFFSET: 0x988990
void CMovementShared::UpdateFallState(CMovementStatus* status) {
    this->m_fallTimeMs = status->m_fallTime;
    this->m_fallVelocity = status->m_zSpeed;

    float elapsed = status->m_fallTime * 0.001f;

    this->m_fallStartZ = this->CalcFallStartElevation(elapsed, this->m_flags & 0x20000000, status->m_zSpeed) + this->m_position.z;

    this->m_moveDir2D.x = status->m_sinAngle;
    this->m_moveDir2D.y = status->m_cosAngle;

    this->m_moveDir.x = status->m_sinAngle;
    this->m_moveDir.y = status->m_cosAngle;
    this->m_moveDir.z = 0.0f;

    this->m_currentSpeed = status->m_xySpeed;
}

// OFFSET: 0x987D00
int32_t CMovementShared::PlotUnitMovement(int32_t time, C3Vector* out) {
    return this->PlotUnitMovement(time, out, &this->m_facing, &this->m_pitch);
}

// OFFSET: 0x987B50
int32_t CMovementShared::PlotUnitMovement(int32_t time, C3Vector* out, float* outFacing, float* outPitch) {
    if (!time)
        return this->m_flags & 0xC0100F;

    float delta = time * 0.001f;
    uint32_t shape = 0;

    if ((this->m_flags & MOVEMASK_TURN) != 0) {
        if (outFacing)
            *outFacing = this->PlotFacing(delta);

        if ((this->m_flags & MOVEMENTFLAG_FALLING) == 0)
            shape = 2;
    }

    if ((this->m_flags & MOVEMASK_PITCH) != 0) {
        if (outPitch)
            *outPitch = this->PlotPitch(delta);

        if ((this->m_flags & MOVEMASK_VERTICAL) != 0)
            shape |= 0x10;
        else
            shape |= 8;
    } else if ((this->m_flags & MOVEMASK_VERTICAL) != 0) {
        shape |= 0x10;
    }

    if ((this->m_flags & MOVEMASK_FWDBACK) != 0)
        shape |= 1;
    if ((this->m_flags & MOVEMASK_STRAFE) != 0)
        shape |= 4;

    switch (shape) {
    case 1:
    case 4:
    case 5:
    case 12:
        this->PlotStraight(delta, out);
        break;
    case 3:
    case 6:
    case 7:
    case 14:
    case 19:
    case 22:
    case 23:
        this->PlotHorzCircularPosition(delta, out);
        break;
    case 9:
    case 13:
        this->PlotVertCircularPosition(delta, out);
        break;
    case 11:
    case 15:
        this->PlotSpiralPosition(delta, out);
        break;
    case 16:
    case 18: {
        float v = delta * this->m_currentSpeed;
        if ((this->m_flags & MOVEMENTFLAG_ASCENDING) == 0)
            v = -v;
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = v;
        break;
    }
    case 17:
    case 20:
    case 21:
        this->PlotAscendDescend(delta, out);
        break;
    }

    return this->m_flags & 0xC0100F;
}

// OFFSET: 0x987820
void CMovementShared::PlotSpiralPosition(float dt, C3Vector* out) {
    float turn = 0.0f;
    if ((this->m_flags & MOVEMENTFLAG_LEFT) != 0)
        turn = this->m_turnRate;
    else if ((this->m_flags & MOVEMENTFLAG_RIGHT) != 0)
        turn = -this->m_turnRate;

    if ((this->m_flags & 0xC0100F) != 0 && (this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_TURNING) == 0)
        turn *= 0.75f;

    float pitch = 0.0f;
    if ((this->m_flags & MOVEMENTFLAG_PITCH_UP) != 0)
        pitch = this->m_pitchRate;
    else if ((this->m_flags & MOVEMENTFLAG_PITCH_DOWN) != 0)
        pitch = -this->m_pitchRate;

    if ((this->m_flags & 0xC0100F) != 0 && (this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_PITCHING) == 0)
        pitch *= 0.75f;

    const float turnRadius = this->m_currentSpeed * 0.70710677f / turn;
    const float pitchRadius = this->m_currentSpeed * 0.70710677f / pitch;

    const float turnAngle = turn * dt;
    const float pitchAngle = dt * pitch;

    const float along = sinf(turnAngle) * turnRadius;
    const float across = turnRadius - cosf(turnAngle) * turnRadius;

    out->x = this->m_moveDir2D.x * along - this->m_moveDir2D.y * across;
    out->y = along * this->m_moveDir2D.y + across * this->m_moveDir2D.x;
    out->z = (pitchRadius - cosf(pitchAngle) * pitchRadius) * this->m_pitchCos + pitchRadius * sinf(pitchAngle) * this->m_pitchSin;
}

// OFFSET: 0x987950
void CMovementShared::PlotVertCircularPosition(float dt, C3Vector* out) {
    float pitch = 0.0f;
    if ((this->m_flags & MOVEMENTFLAG_PITCH_UP) != 0)
        pitch = this->m_pitchRate;
    else if ((this->m_flags & MOVEMENTFLAG_PITCH_DOWN) != 0)
        pitch = -this->m_pitchRate;

    if ((this->m_flags & 0xC0100F) != 0 && (this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_PITCHING) == 0)
        pitch *= 0.75f;

    const float radius = this->m_currentSpeed / pitch;
    const float angle = pitch * dt;

    const float along = sinf(angle) * radius;
    const float across = radius - cosf(angle) * radius;

    const float horiz = along * this->m_pitchCos - across * this->m_pitchSin;

    out->x = horiz * this->m_moveDir2D.x;
    out->y = horiz * this->m_moveDir2D.y;
    out->z = across * this->m_pitchCos + along * this->m_pitchSin;
}

// OFFSET: 0x987A00
void CMovementShared::PlotHorzCircularPosition(float dt, C3Vector* out) {
    float turn = 0.0f;
    if ((this->m_flags & MOVEMENTFLAG_LEFT) != 0)
        turn = this->m_turnRate;
    else if ((this->m_flags & MOVEMENTFLAG_RIGHT) != 0)
        turn = -this->m_turnRate;

    if ((this->m_flags & 0xC0100F) != 0 && (this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_TURNING) == 0)
        turn *= 0.75f;

    const float radius = this->m_currentSpeed / turn;
    const float angle = turn * dt;

    const float along = sinf(angle) * radius;
    const float across = radius - cosf(angle) * radius;

    const float x = this->m_moveDir2D.x * along - this->m_moveDir2D.y * across;
    const float y = along * this->m_moveDir2D.y + across * this->m_moveDir2D.x;

    const uint32_t f = this->m_flags;
    if ((this->m_flags & MOVEMENTFLAG_ASCENDING) != 0) {
        out->x = x * 0.70710677f;
        out->y = y * 0.70710677f;
        out->z = 0.70710677f * (this->m_currentSpeed * dt);
    } else if ((this->m_flags & MOVEMENTFLAG_DESCENDING) != 0) {
        out->x = x * 0.70710677f;
        out->y = y * 0.70710677f;
        out->z = this->m_currentSpeed * dt * -0.70710677f;
    } else if ((this->m_flags & MOVEMASK_FWDBACK) == 0)
    {
        out->x = x;
        out->y = y;
        out->z = 0.0f;
    } else
    {
        out->x = x * this->m_pitchCos;
        out->y = y * this->m_pitchCos;
        out->z = this->m_pitchSin * this->m_currentSpeed * dt;
    }
}

// OFFSET: 0x987700
void CMovementShared::PlotAscendDescend(float dt, C3Vector* out) {
    const float k = 0.70710677f;
    const float vert = (this->m_flags & MOVEMENTFLAG_ASCENDING) != 0 ? k : -k;

    out->x = this->m_moveDir2D.x * k * dt * this->m_currentSpeed;
    out->y = this->m_moveDir2D.y * k * dt * this->m_currentSpeed;
    out->z = this->m_currentSpeed * (vert * dt);
}

// OFFSET. 0x9876B0
void CMovementShared::PlotStraight(float dt, C3Vector* out) {
    out->x = this->m_moveDir.x * dt * this->m_currentSpeed;
    out->y = this->m_moveDir.y * dt * this->m_currentSpeed;
    out->z = this->m_moveDir.z * dt * this->m_currentSpeed;
}

// OFFSET: 0x9877D0
float CMovementShared::PlotPitch(float dt) {
    float rate = 0.0f;
    if ((this->m_flags & MOVEMENTFLAG_PITCH_UP) != 0)
        rate = this->m_pitchRate;
    else if ((this->m_flags & MOVEMENTFLAG_PITCH_DOWN) != 0)
        rate = -this->m_pitchRate;

    if ((this->m_flags & 0xC0100F) != 0 && (this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_PITCHING) == 0)
        rate *= 0.75f;

    return fmodf(rate * dt + this->m_anchorPitch, 6.2831855f);
}

// OFFSET: 0x987770
float CMovementShared::PlotFacing(float dt) {
    float rate = 0.0f;
    if ((this->m_flags & MOVEMENTFLAG_LEFT) != 0)
        rate = this->m_turnRate;
    else if ((this->m_flags & MOVEMENTFLAG_RIGHT) != 0)
        rate = -this->m_turnRate;

    if ((this->m_flags & 0xC0100F) != 0 && (this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_TURNING) == 0)
        rate *= 0.75f;

    const float result = fmodf(rate * dt + this->m_anchorFacing, 6.2831855f);
    return result < 0.0f ? result + 6.2831855f : result;
}

// OFFSET: 0x98C940
void CMovementShared::ConvertCurrentSplineToLoopingSpline(int32_t timePassed) {
    CMoveSpline* spline = this->m_spline;
    if (!spline)
        return;

    uint32_t pointCount = spline->spline.m_pointCount;
    if (pointCount < 4)
        return;

    C3Vector* points = static_cast<C3Vector*>(alloca(sizeof(C3Vector) * pointCount));
    if (!points)
        return;

    this->m_spline = MovementNewSpline();
    if (!this->m_spline) {
        this->m_spline = spline;
        return;
    }

    spline->spline.GetPoints(points, pointCount);
    points[1] = points[pointCount - 3];
    this->OnSpline(timePassed, &points[1], pointCount - 1, spline->m_duration, spline->flags, spline->m_id);
    MovementDelSpline(spline);
}

// OFFSET: 0x987D20
float CMovementShared::PlotSplineElevation(uint32_t elapsedMs, float z) {
    if (!elapsedMs)
        return z;

    CMoveSpline* spline = this->m_spline;
    if (elapsedMs >= spline->m_duration)
        return z;

    uint32_t flags = spline->flags;

    if ((flags & SPLINE_FLAG_PARABOLIC) != 0) {
        if (elapsedMs <= spline->m_effectStartTime)
            return z;

        float effectStart = spline->m_effectStartTime * 0.001f;
        float elapsed = elapsedMs * 0.001f;

        return (this->m_spline->m_duration * 0.001f - effectStart) * this->m_spline->m_verticalAcceleration * 0.5f * (elapsed - effectStart) - 0.5f * ((elapsed - effectStart) * (this->m_spline->m_verticalAcceleration * (elapsed - effectStart))) + z;
    }

    if ((flags & SPLINE_FLAG_FALLING) == 0)
        return z;

    float elevation = this->m_fallStartZ - this->CalcFallStartElevation(elapsedMs * 0.001f, 0, 0.0f);
    if (elevation <= spline->m_finalDestination.z)
        return spline->m_finalDestination.z;

    return elevation;
}

// OFFSET: 0x98CA00
bool CMovementShared::PlotUnitSplineMovement(int32_t time, C3Vector* out) {
    *out = this->m_position;

    if ((this->m_flags & MOVEMASK_FWDBACK) == 0)
        return this->m_flags & MOVEMASK_MOVING_FALL;

    CMoveSpline* spline = this->m_spline;
    if ((spline->flags & Frozen) != 0)
        return 1;

    spline->start = time - spline->m_timePassed;
    spline = this->m_spline;

    uint32_t durationMs = CMath::fuint_n(spline->m_duration * spline->m_durationMod);
    float t;

    if (!durationMs) {
        spline->flags |= SPLINE_FLAG_DONE;
        t = 1.0f;
    } else if (static_cast<int32_t>(spline->start) < 0) {
        t = 0.0f;
    } else if (spline->start < durationMs) {
        t = static_cast<int32_t>(spline->start) / static_cast<float>(durationMs);
    } else if ((spline->flags & Cyclic) != 0) {
        spline->start -= durationMs;
        this->m_spline->m_timePassed = time - this->m_spline->start;

        if ((this->m_spline->flags & Enter_Cycle) != 0) {
            this->ConvertCurrentSplineToLoopingSpline(this->m_spline->m_timePassed);
            this->m_spline->flags &= ~Enter_Cycle;
        }

        this->m_spline->m_durationMod = this->m_spline->m_durationModNext;
        this->m_spline->m_durationModNext = 1.0f;

        durationMs = CMath::fuint_n(this->GetModifiedSplineDuration());
        if (!durationMs)
            t = 1.0f;
        else
            t = static_cast<int32_t>(this->m_spline->start) / static_cast<float>(durationMs);
    } else {
        spline->flags |= SPLINE_FLAG_DONE;
        t = 1.0f;
    }

    C44Matrix frame(this->m_moveDir.x, this->m_moveDir.y, this->m_moveDir.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    this->m_spline->spline.Frame(t, &frame, 1);

    spline = this->m_spline;

    if ((spline->flags & (OrientationFixed | SPLINE_FLAG_FALLING)) == 0) {
        if (frame.a0 * frame.a0 + frame.a1 * frame.a1 > 0.001849f)
            this->m_facing = atan2f(frame.a1, frame.a0);
    }

    if ((spline->flags & Backward) != 0)
        this->m_facing = this->m_facing - CMath::PI;

    if (this->m_facing < 0.0f)
        this->m_facing = CMath::TWO_PI + this->m_facing;

    this->m_moveDir.x = frame.a0;
    this->m_moveDir.y = frame.a1;
    this->m_moveDir.z = frame.a2;

    if ((this->m_flags & MOVEMASK_SWIM_FLY) != 0) {
        this->m_pitch = asinf(frame.a2);
    } else if ((spline->flags & SPLINE_FLAG_FLYING) != 0) {
        C3Vector ahead = { 0.0f, 0.0f, 0.0f };

        float lookAhead = 1.0f;
        if (durationMs)
            lookAhead = (static_cast<int32_t>(spline->start) + 1000) / static_cast<float>(durationMs);

        if ((spline->flags & Cyclic) != 0 && lookAhead > 1.0f)
            lookAhead = lookAhead - 1.0f;

        if (lookAhead < 0.0f)
            lookAhead = 0.0f;
        else if (lookAhead > 1.0f)
            lookAhead = 1.0f;

        this->m_spline->spline.Pos(lookAhead, &ahead, 1);

        C2Vector dir(this->m_moveDir.x, this->m_moveDir.y);

        C3Vector position;
        this->GetPosition(&position, &this->m_position);

        C3Vector delta = { ahead.x - position.x, ahead.y - position.y, ahead.z - position.z };
        C2Vector toAhead(delta.x, delta.y);

        dir.Normalize();
        toAhead.Normalize();

        float dot = dir.y * toAhead.y + toAhead.x * dir.x;
        if (dot < -1.0f)
            dot = -1.0f;
        else if (dot > 1.0f)
            dot = 1.0f;

        float angle;
        if (toAhead.y * dir.x - dir.y * toAhead.x < 0.0f)
            angle = acosf(dot);
        else
            angle = -acosf(dot);

        angle = angle + angle;
        if (angle < -1.5707964f)
            angle = -1.5707964f;
        else if (angle > 1.5707964f)
            angle = 1.5707964f;

        C33Matrix roll = C33Matrix::Rotation(angle, this->m_moveDir, false);
        this->m_groundNormal = *frame.Row2AsVec3() * roll;
    }

    *out = *frame.Row3AsVec3();

    spline = this->m_spline;
    uint32_t flags = spline->flags;

    if ((flags & Animation) != 0) {
        if ((this->m_flags2 & MOVEMENTFLAG2_UNK9) == 0 && spline->start > spline->m_effectStartTime)
            this->m_flags2 |= MOVEMENTFLAG2_UNK9;
    } else if ((flags & SPLINE_FLAG_PARABOLIC) != 0) {
        if (spline->start > spline->m_effectStartTime && (this->m_flags2 & MOVEMENTFLAG2_UNK8) == 0) {
            this->m_fallStartZ = this->m_position.z;
            this->m_flags2 |= MOVEMENTFLAG2_UNK8;
        }

        out->z = this->PlotSplineElevation(this->m_spline->start, out->z);
        if (out->z > this->m_fallStartZ)
            this->m_fallStartZ = this->m_position.z;
    } else if ((flags & SPLINE_FLAG_FALLING) != 0) {
        float z = this->PlotSplineElevation(spline->start, out->z);
        out->z = z;

        if (CMath::fequal(z, this->m_spline->m_finalDestination.z)) {
            int32_t fallTimeMs = static_cast<int32_t>(this->TimeToFallDistance(this->m_fallStartZ - z, false) * 1000.0f);

            if (static_cast<int32_t>(this->m_spline->start) > fallTimeMs)
                this->m_spline->start = fallTimeMs;

            this->m_spline->flags |= SPLINE_FLAG_DONE;
        }
    }

    return 1;
}
