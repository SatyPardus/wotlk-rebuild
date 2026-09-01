#include "clientobject/CPlayerMoveEvent.hpp"
#include "clientobject/CMovementStatus.hpp"

// OFFSET: 0x6E9050
void CPlayerMoveEvent::FromMoveStatus(CMovementStatus* update) {
    this->m_transportGuid = update->m_transportGuid;
    this->m_moveFlags = update->m_moveFlags;
    this->m_moveExtraFlags = update->m_moveExtraFlags;
    if (update->m_transportGuid) {
        this->m_position = update->m_transportPosition;
        this->m_facing = update->m_transportFacing;
        this->m_seat = update->m_transportSeat;
        this->m_pitch = update->m_pitch;
        this->unk_0050 = 1;
    } else {
        this->m_position = update->m_position;
        this->m_facing = update->m_facing;
        this->unk_0050 = 1;
        this->m_pitch = update->m_pitch;
    }
    this->m_fallTime = update->m_fallTime;
}
