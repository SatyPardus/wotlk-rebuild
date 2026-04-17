#include "world/map/CFrustum.hpp"

// OFFSET: 0x9839E0
WorldCullStatus CFrustum::Cull(CAaBox* box) {
    static constexpr float CULL_EPSILON = -0.019444443f;

    /*for (int i = 0; i < 6; i++) {
        const C4Plane& plane = this->planes[i];

        float dot = (plane.n.x >= 0.0f ? box->t.x : box->b.x) * plane.n.x
                    + (plane.n.y >= 0.0f ? box->t.y : box->b.y) * plane.n.y
                    + (plane.n.z >= 0.0f ? box->t.z : box->b.z) * plane.n.z
                    + plane.d;

        if (dot < CULL_EPSILON)
            return WorldCull_outside;
    }*/

    return WorldCull_notOutside;
}
