#include "clientobject/MovementShared.hpp"
#include <common/time/Time.hpp>
#include <util/Byte.hpp>

CMovementShared::CMovementShared(WGUID* transportGuid, C3Vector& position, float facing, WGUID* guid)
    : CPassenger(transportGuid, position, guid) {
    this->m_groundNormal.x = 0.0;
    this->m_groundNormal.y = 0.0;
    this->unk_0030 = 0;
    this->unk_0034 = 0;
    this->m_groundNormal.z = 1.0;
    this->m_flags = 0;
    LOWORD(this->m_flags2) = 0;
    BYTE2(this->m_flags2) = -1;
    this->m_anchorPos.x = position.x;
    this->m_anchorPos.y = position.y;
    this->m_anchorFacing = facing;
    this->m_anchorPos.z = position.z;
    this->unk_005C = 0.0;
    this->unk_0064 = 0.0;
    this->unk_0068 = 0.0;
    this->unk_006C = 0.0;
    this->unk_0070 = 0.0;
    this->unk_0074 = 0.0;
    this->m_fallTimeMs = 0;
    this->unk_007C = 0.0;
    this->unk_0078 = 1.0;
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
    this->unk_00C0 = OsGetAsyncTimeMs();
    this->unk_00B4 = 1.0;
}
