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

// OFFSET: 0x7F9480
bool CWorldMath::VectorIntersectAABox2(CAaBox& box, C3Vector& start, C3Vector& end) {
    const float EPSILON = 1e-5f;

    const float* minB = &box.b.x;
    const float* maxB = &box.t.x;
    const float* origin = &start.x;
    const float* target = &end.x;

    float dir[3];
    float maxT[3];

    dir[0] = end.x - start.x;
    dir[1] = end.y - start.y;
    dir[2] = end.z - start.z;
    maxT[0] = maxT[1] = maxT[2] = -1.0f;

    int inside = 1;
    for (int i = 0; i < 3; i++) {
        if (origin[i] < minB[i])
        {
            if (target[i] < minB[i])
                return 0;
            inside = 0;
            if (dir[i] != 0.0f)
                maxT[i] = (minB[i] - origin[i]) / dir[i];
        } else if (origin[i] > maxB[i])
        {
            if (target[i] > maxB[i])
                return 0;
            inside = 0;
            if (dir[i] != 0.0f)
                maxT[i] = (maxB[i] - origin[i]) / dir[i];
        }
    }

    if (inside)
        return 1;

    int whichPlane = (maxT[0] < maxT[1]) ? 1 : 0;
    if (maxT[whichPlane] < maxT[2])
        whichPlane = 2;

    if (maxT[whichPlane] < 0.0f)
        return 0;

    for (int i = 0; i < 3; i++) {
        if (i != whichPlane) {
            float coord = origin[i] + maxT[whichPlane] * dir[i];
            if (coord < minB[i] - EPSILON || coord > maxB[i] + EPSILON)
                return 0;
        }
    }

    return 1;
}

// OFFSET: 0x7A61D0
int32_t CWorldMath::ComputeAaBoxOutcode(CAaBox* box, C3Vector* point) {
    int32_t outcode = 0;

    if (point->x - box->b.x + 0.019444443f < 0.0f)
        outcode |= 0x01;
    if (point->y - box->b.y + 0.019444443f < 0.0f)
        outcode |= 0x02;
    if (point->z - box->b.z + 0.019444443f < 0.0f)
        outcode |= 0x04;
    if (box->t.x - point->x + 0.019444443f < 0.0f)
        outcode |= 0x08;
    if (box->t.y - point->y + 0.019444443f < 0.0f)
        outcode |= 0x10;
    if (box->t.z - point->z + 0.019444443f < 0.0f)
        outcode |= 0x20;

    return outcode;
}
