#include "world/map/CFrustum.hpp"

// OFFSET: 0x9839E0
WorldCullStatus CFrustum::Cull(CAaBox* box) {
    static constexpr float CULL_EPSILON = -0.019444443f;

    for (int i = 0; i < 6; i++) {
        const C4Plane& plane = this->planes[i];

        float dot = (plane.n.x >= 0.0f ? box->t.x : box->b.x) * plane.n.x
                    + (plane.n.y >= 0.0f ? box->t.y : box->b.y) * plane.n.y
                    + (plane.n.z >= 0.0f ? box->t.z : box->b.z) * plane.n.z
                    + plane.d;

        if (dot < CULL_EPSILON)
            return WorldCull_outside;
    }

    return WorldCull_notOutside;
}

// OFFSET: 0x984240
void CFrustum::CalcPlanesFromCorners(C3Vector* corners) {
    for (int32_t i = 0; i < 8; i++) {
        this->corners[i] = corners[i];
    }
    this->CalcPlanesFromCorners();
}

// OFFSET: 0x984240
void CFrustum::CalcPlanesFromCorners() {
    planes[0].From3Pos(corners[1], corners[5], corners[6]);
    planes[1].From3Pos(corners[0], corners[7], corners[4]);
    planes[2].From3Pos(corners[0], corners[4], corners[5]);
    planes[3].From3Pos(corners[3], corners[6], corners[7]);
    planes[4].From3Pos(corners[5], corners[4], corners[6]);

    planes[5].n.x = -planes[4].n.x;
    planes[5].n.y = -planes[4].n.y;
    planes[5].n.z = -planes[4].n.z;
    planes[5].d = -(planes[5].n.x * corners[2].x + planes[5].n.y * corners[2].y + planes[5].n.z * corners[2].z);
}

// OFFSET: 0x790020
void CFrustum::FrustumPush(CFrustum* other) {
    if (this == other)
        return;

    this->planes[0].n.x = other->planes[0].n.x;
    this->planes[0].n.y = other->planes[0].n.y;
    this->planes[0].n.z = other->planes[0].n.z;
    this->planes[0].d = other->planes[0].d;
    this->planes[1] = other->planes[1];
    this->planes[2] = other->planes[2];
    this->planes[3] = other->planes[3];
    this->planes[4] = other->planes[4];
    this->planes[5] = other->planes[5];
    this->corners[0].x = other->corners[0].x;
    this->corners[0].y = other->corners[0].y;
    this->corners[0].z = other->corners[0].z;
    this->corners[1] = other->corners[1];
    this->corners[2] = other->corners[2];
    this->corners[3] = other->corners[3];
    this->corners[4] = other->corners[4];
    this->corners[5] = other->corners[5];
    this->corners[6] = other->corners[6];
    this->corners[7] = other->corners[7];
    this->lookPos = other->lookPos;
    this->lookAt = other->lookAt;
    this->lookUp = other->lookUp;
    this->fovy = other->fovy;
    this->aspect = other->aspect;
    this->minz = other->minz;
    this->maxz = other->maxz;
}
