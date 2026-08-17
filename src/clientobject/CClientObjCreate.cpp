#include "clientobject/CClientObjCreate.hpp"
#include "util/Unimplemented.hpp"
#include "util/DataStore.hpp"
#include <common/DataStore.hpp>
#include <tempest/vector/C3Vector.hpp>

void CClientObjCreate::Skip(CDataStore* msg) {
    uint16_t flags;
    msg->Get(flags);

    if (flags & 0x20) {
        CClientMoveUpdate::Skip(msg);
    } else if (flags & 0x100) {
        WGUID guid;
        *msg >> guid;

        C3Vector position28;
        *msg >> position28;

        C3Vector position18;
        *msg >> position18;

        float facing34;
        msg->Get(facing34);

        float facing24;
        msg->Get(facing24);
    } else if (flags & 0x40) {
        C3Vector position28;
        *msg >> position28;

        float facing34;
        msg->Get(facing34);
    }

    if (flags & 0x8) {
        uint32_t uint2AC;
        msg->Get(uint2AC);
    }

    if (flags & 0x10) {
        uint32_t uint2B0;
        msg->Get(uint2B0);
    }

    if (flags & 0x4) {
        WGUID guid2B8;
        *msg >> guid2B8;
    }

    if (flags & 0x2) {
        uint32_t uint2C0;
        msg->Get(uint2C0);
    }

    if (flags & 0x80) {
        uint32_t uint2C4;
        msg->Get(uint2C4);

        float float2C8;
        msg->Get(float2C8);
    }

    if (flags & 0x200) {
        uint64_t uint2D4;
        msg->Get(uint2D4);
    }
}

int32_t CClientObjCreate::Get(CDataStore* msg) {
    uint16_t flags;
    msg->Get(flags);

    this->flags = flags;
    
    if (this->flags & 0x20) {
        *msg >> this->m_moveUpdate;
    } else if (this->flags & 0x100) {
        WGUID guid;
        *msg >> guid;
        this->m_moveUpdate.status.m_transportGuid = guid;
    
        *msg >> this->m_moveUpdate.status.m_position;
        *msg >> this->m_moveUpdate.status.m_transportPosition;
    
        msg->Get(this->m_moveUpdate.status.m_facing);
        msg->Get(this->m_moveUpdate.status.m_transportFacing);
    } else if (this->flags & 0x40) {
        *msg >> this->m_moveUpdate.status.m_position;
        msg->Get(this->m_moveUpdate.status.m_facing);

        this->m_moveUpdate.status.m_transportGuid = 0;
        this->m_moveUpdate.status.m_transportPosition = this->m_moveUpdate.status.m_position;
    }
    
    if (this->flags & 0x8) {
        msg->Get(this->unk_02AC);
    } else {
        this->unk_02AC = 0;
    }
    
    if (this->flags & 0x10) {
        msg->Get(this->m_lowGuid);
    } else {
        this->m_lowGuid = 0;
    }
    
    if (this->flags & 0x4) {
        WGUID guid;
        *msg >> guid;
        this->m_targetGuid = guid;
    } else {
        this->m_targetGuid = 0;
    }
    
    if (this->flags & 0x2) {
        msg->Get(this->m_someTransportTime);
    }
    
    if (this->flags & 0x80) {
        msg->Get(this->m_vehicleId);
        msg->Get(this->m_vehicleOrientation);
    }
    
    if (this->flags & 0x200) {
        msg->Get(this->m_packedRotation);
    } else {
        this->m_packedRotation = 0;
    }

    return msg->Size() >= msg->Tell();
}
