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

WorldCullStatus CFrustum::Cull(CAaSphere* sphere) {
    for (int i = 0; i < 6; ++i) {
        const C4Plane& p = this->planes[i];

        float dist = p.n.x * sphere->c.x + p.n.y * sphere->c.y + p.n.z * sphere->c.z + p.d;

        if (dist < -sphere->r)
            return WorldCull_notOutside;
    }

    return WorldCull_outside;
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

// OFFSET: 0x983F40
void CFrustum::Transform(C44Matrix& mat) {
    for (int32_t i = 0; i < 8; i++) {
        this->corners[i] = this->corners[i] * mat;
    }
    this->CalcPlanesFromCorners();
    this->lookPos = this->lookPos * mat;
    this->lookAt = this->lookAt * mat;
}
