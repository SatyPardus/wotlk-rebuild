#ifndef WORLD_MAP_C_FRUSTUM_HPP
#define WORLD_MAP_C_FRUSTUM_HPP

#include "storm/list/TSLink.hpp"
#include "world/map/Types.hpp"
#include <tempest/Vector.hpp>
#include "tempest/Plane.hpp"

class CFrustum {
    public:
    C4Plane planes[6];
    C3Vector corners[8];
    C3Vector lookPos;
    C3Vector lookAt;
    C3Vector lookUp;
    float fovy;
    float aspect;
    float minz;
    float maxz;
    TSLink<CFrustum> sceneLink;

    WorldCullStatus Cull(CAaBox* box);
    WorldCullStatus Cull(CAaSphere* box);
    void CalcPlanesFromCorners(C3Vector* corners);
    void CalcPlanesFromCorners();
    void FrustumPush(CFrustum* other);
};

#endif
