#include "world/map/CFrustum.hpp"

// OFFSET: 0x601650
CFrustum::CFrustum() {
    for (int32_t i = 0; i < 6; i++) {
        this->planes[i].n.x = 0.0f;
        this->planes[i].n.y = 0.0f;
        this->planes[i].n.z = 1.0f;
        this->planes[i].d = 0.0f;
    }

    for (int32_t i = 0; i < 8; i++) {
        this->corners[i].x = 0.0f;
        this->corners[i].y = 0.0f;
        this->corners[i].z = 0.0f;
    }

    this->lookPos.x = 0.0f;
    this->lookPos.y = 0.0f;
    this->lookPos.z = 0.0f;
    this->lookAt.x = 0.0f;
    this->lookAt.y = 0.0f;
    this->lookAt.z = 0.0f;
    this->lookUp.x = 0.0f;
    this->lookUp.y = 0.0f;
    this->lookUp.z = 0.0f;
}

// OFFSET: 0x983FE0
CFrustum::CFrustum(C3Vector* corners)
    : CFrustum() {
    for (int32_t i = 0; i < 8; i++) {
        this->corners[i] = corners[i];
    }

    this->CalcPlanesFromCorners();
}

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
            return WorldCull_outside;
    }

    return WorldCull_notOutside;
}

// OFFSET: 0x983D70
void CFrustum::Cull(C3Vector& a2, uint8_t* a3) {
    *a3 = 0;
    if (this->planes[0].n.z * a2.z + this->planes[0].n.y * a2.y + a2.x * this->planes[0].n.x + this->planes[0].d < -0.019444443)
        *a3 = 1;
    if (this->planes[1].n.z * a2.z + this->planes[1].n.y * a2.y + this->planes[1].n.x * a2.x + this->planes[1].d < -0.019444443)
        *a3 |= 2u;
    if (this->planes[2].n.z * a2.z + this->planes[2].n.y * a2.y + this->planes[2].n.x * a2.x + this->planes[2].d < -0.019444443)
        *a3 |= 4u;
    if (this->planes[3].n.z * a2.z + this->planes[3].n.y * a2.y + this->planes[3].n.x * a2.x + this->planes[3].d < -0.019444443)
        *a3 |= 8u;
    if (this->planes[4].n.z * a2.z + this->planes[4].n.y * a2.y + this->planes[4].n.x * a2.x + this->planes[4].d < -0.019444443)
        *a3 |= 0x10u;
    if (this->planes[5].n.z * a2.z + this->planes[5].n.y * a2.y + this->planes[5].n.x * a2.x + this->planes[5].d < -0.019444443)
        *a3 |= 0x20u;
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

// OFFSET: 0x983AE0
void CFrustum::Translate(const C3Vector& offset) {
    for (int32_t i = 0; i < 8; i++) {
        this->corners[i].x = this->corners[i].x + offset.x;
        this->corners[i].y = this->corners[i].y + offset.y;
        this->corners[i].z = this->corners[i].z + offset.z;
    }

    for (int32_t i = 0; i < 6; i++) {
        this->planes[i].d = this->planes[i].d - (this->planes[i].n.z * offset.z + this->planes[i].n.y * offset.y + this->planes[i].n.x * offset.x);
    }

    this->lookPos.x = this->lookPos.x + offset.x;
    this->lookPos.y = this->lookPos.y + offset.y;
    this->lookPos.z = this->lookPos.z + offset.z;

    this->lookAt.x = this->lookAt.x + offset.x;
    this->lookAt.y = this->lookAt.y + offset.y;
    this->lookAt.z = this->lookAt.z + offset.z;
}
