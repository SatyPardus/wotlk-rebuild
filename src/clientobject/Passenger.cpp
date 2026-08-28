#include "clientobject/Passenger.hpp"
#include <util/Byte.hpp>

CPassenger::CPassenger(WGUID* transportGuid, C3Vector& position, WGUID* guid) {
    this->m_link.m_prevlink = 0;
    this->m_link.m_next = 0;
    this->m_transportGuid = *transportGuid;
    this->m_position.x = position.x;
    this->m_position.y = position.y;
    this->m_facing = 0.0;
    this->m_position.z = position.z;
    this->m_pitch = 0.0;
    this->m_guid2 = guid;
    LOBYTE(this->unk_002C) = 0;
 }

// OFFSET: 0x4F4460
void CPassenger::GetPosition(C3Vector* out, C3Vector* pos) {
     if (this->m_transportGuid) {
        //C44Matrix mat;
        //MovementGetTransportMtxX(__SPAIR64__(guid_high, guid_low), &mat);
        //C44Matrix::TransformPoint(a2, a3, &mat);
     } else {
         *out = *pos;
     }
 }

// OFFSET: 0x4F42A0
float CPassenger::GetFacing(float facing) {
     if (!this->m_transportGuid)
         return facing;

     //v5 = MovementGetTransportFacing(guid_low, guid_high) + a2;
     //return CMath::normalizeangle0to2pi_(v5);
     return 0.0f;
 }
