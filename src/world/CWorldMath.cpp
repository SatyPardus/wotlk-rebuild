#include "world/CWorldMath.hpp"
#include <algorithm>

// OFFSET: 0x7F9430
void CWorldMath::TransformAABox(C44Matrix& m, CAaBox& src, CAaBox& dst) {
    dst.b = { m.d0, m.d1, m.d2 };
    dst.t = dst.b;

    for (int axis = 0; axis < 3; axis++) {
        for (int component = 0; component < 3; component++) {
            float e = (&m.a0)[component * 4 + axis];

            float a = e * src.b[component];
            float b = e * src.t[component];

            dst.b[axis] += std::min(a, b);
            dst.t[axis] += std::max(a, b);
        }
    }
}
