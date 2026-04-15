#include "world/map/CWorldOcclusion.hpp"

// OFFSET: 0x7CD910
void CWorldOcclusion::ClearVolumes() {
    //TSGrowableArray_C4Plane__SetCount(&CWorldOcclusion::planeArray, 0);
    //TSGrowableArray_CClipVolume__SetCount(&CWorldOcclusion::clipVolumeArray, 0);
}

// OFFSET: 0x7CCE00
uint32_t CWorldOcclusion::QueryVolumes(CAaSphere* sphere) {
    return 0;
}

// OFFSET: 0x78FDC0
uint32_t CWorldOcclusion::QueryBuffer(CAaBox* box, uint8_t flags) {
    return 0;
}
