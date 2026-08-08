#include "world/CWorldMath.hpp"
#include <algorithm>

// OFFSET: 0x7F9430
void CWorldMath::TransformAABox(C44Matrix& m, CAaBox& src, CAaBox& dst) {
    dst.b = { m.d0, m.d1, m.d2 };
    dst.t = dst.b;

    for (int axis = 0; axis < 3; axis++) {
        const float* row = &m.a0 + axis * 4;

        for (int component = 0; component < 3; component++) {
            float a = row[component] * src.b[component];
            float b = row[component] * src.t[component];

            dst.b[axis] += std::min(a, b);
            dst.t[axis] += std::max(a, b);
        }
    }
}
