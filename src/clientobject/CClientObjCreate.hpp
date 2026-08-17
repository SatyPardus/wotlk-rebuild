#ifndef OBJECT_CLIENT_C_CLIENT_OBJ_CREATE_HPP
#define OBJECT_CLIENT_C_CLIENT_OBJ_CREATE_HPP

#include <cstdint>
#include "clientobject/CClientMoveUpdate.hpp"
#include "clientobject/WGUID.hpp"

class CDataStore;

struct CClientObjCreate {
    CClientMoveUpdate m_moveUpdate;
    uint32_t unk_02A4;
    uint32_t flags;
    uint32_t unk_02AC;
    uint32_t m_lowGuid;
    uint32_t unk_02B4;
    WGUID m_targetGuid;
    uint32_t m_someTransportTime;
    uint32_t m_vehicleId;
    uint32_t m_vehicleOrientation;
    uint64_t m_packedRotation;

    static void Skip(CDataStore* msg);
    int32_t Get(CDataStore* msg);
};

#endif
