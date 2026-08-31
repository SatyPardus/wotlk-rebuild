#include "util/DataStore.hpp"

CDataStore& operator>>(CDataStore& msg, C3Vector& vector) {
    msg.Get(vector.x);
    msg.Get(vector.y);
    msg.Get(vector.z);

    return msg;
}

// OFFSET: 0x7152B0
C3Vector* ReadPackedVector3(CDataStore* msg, C3Vector* base, C3Vector* out) {
    uint32_t packed;
    msg->Get(packed);

    int32_t dx = (int32_t)((packed & 0x7FF) << 21) >> 21;
    int32_t dy = (int32_t)(((packed >> 11) & 0x7FF) << 21) >> 21;
    int32_t dz = (int32_t)((packed >> 22) << 22) >> 22;

    out->x = base->x - dx * 0.25f;
    out->y = base->y - dy * 0.25f;
    out->z = base->z - dz * 0.25f;

    return out;
}
