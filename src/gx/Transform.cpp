#include "gx/Transform.hpp"
#include "gx/Device.hpp"
#include "gx/Types.hpp"
#include <algorithm>
#include <cmath>
#include <storm/Error.hpp>
#include <tempest/Math.hpp>
#include <tempest/Matrix.hpp>
#include <tempest/Vector.hpp>

void GxXformPop(EGxXform xf) {
    g_theGxDevicePtr->XformPop(xf);
}

void GxXformProjection(C44Matrix& matrix) {
    g_theGxDevicePtr->XformProjection(matrix);
}

void GxXformProjNative(C44Matrix& matrix) {
    g_theGxDevicePtr->XformProjNative(matrix);
}

void GxXformProjNativeTranspose(C44Matrix& matrix) {
    C44Matrix p;
    GxXformProjNative(p);

    matrix = p.Transpose();
}

void GxXformPush(EGxXform xf) {
    g_theGxDevicePtr->XformPush(xf);
}

// 1-liner for Push/Set
void GxXformPush(EGxXform xf, const C44Matrix& matrix) {
    g_theGxDevicePtr->XformPush(xf, matrix);
}

void GxXformSet(EGxXform xf, const C44Matrix& matrix) {
    g_theGxDevicePtr->XformSet(xf, matrix);
}

void GxXformSetProjection(const C44Matrix& matrix) {
    g_theGxDevicePtr->XformSetProjection(matrix);
}

void GxXformSetView(const C44Matrix& matrix) {
    g_theGxDevicePtr->XformSetView(matrix);
}

void GxXformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    minX = std::max(minX, 0.0f);
    maxX = std::min(maxX, 1.0f);
    minY = std::max(minY, 0.0f);
    maxY = std::min(maxY, 1.0f);

    STORM_ASSERT(minX < maxX);
    STORM_ASSERT(minY < maxY);
    STORM_ASSERT(minZ <= maxZ);

    g_theGxDevicePtr->XformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

void GxXformView(C44Matrix& matrix) {
    g_theGxDevicePtr->XformView(matrix);
}

void GxXformViewport(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) {
    g_theGxDevicePtr->XformViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

void GxXformViewProjNativeTranspose(C44Matrix& matrix) {
    C44Matrix v;
    GxXformView(v);

    C44Matrix p;
    GxXformProjNative(p);

    matrix = (v * p).Transpose();
}

void GxuXformCreateLookAtSgCompat(const C3Vector& eye, const C3Vector& center, const C3Vector& up, C44Matrix& dst) {
    C3Vector v16 = {
        center.x - eye.x,
        center.y - eye.y,
        center.z - eye.z
    };

    if (v16.SquaredMag() >= 0.01f && up.SquaredMag() >= 0.01f) {
        v16.Normalize();

        C3Vector v15 = {
            up.z * v16.y - up.y * v16.z,
            up.x * v16.z - up.z * v16.x,
            up.y * v16.x - up.x * v16.y
        };

        v15.Normalize();

        C3Vector v14 = {
            v16.z * v15.y - v16.y * v15.z,
            v16.x * v15.z - v16.z * v15.x,
            v16.y * v15.x - v16.x * v15.y
        };

        v14.Normalize();

        dst.a0 = v15.x;
        dst.a1 = v14.x;
        dst.a2 = v16.x;

        dst.b0 = v15.y;
        dst.b1 = v14.y;
        dst.b2 = v16.y;

        dst.c0 = v15.z;
        dst.c1 = v14.z;
        dst.c2 = v16.z;

        C3Vector move = { -eye.x, -eye.y, -eye.z };
        dst.Translate(move);
    }
}

void GxuXformCreateOrtho(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, C44Matrix& dst) {
    STORM_ASSERT(minX != maxX);
    STORM_ASSERT(minY != maxY);
    STORM_ASSERT(minZ < maxZ);

    double v10 = maxX - minX;
    double v11 = maxY - minY;
    double v12 = maxZ - minZ;

    dst.a0 = 2.0 / v10;
    dst.a1 = 0.0f;
    dst.a2 = 0.0f;
    dst.a3 = 0.0f;

    dst.b0 = 0.0f;
    dst.b1 = 2.0 / v11;
    dst.b2 = 0.0f;
    dst.b3 = 0.0f;

    dst.c0 = 0.0f;
    dst.c1 = 0.0f;
    dst.c2 = 2.0 / v12;
    dst.c3 = 0.0f;

    dst.d0 = -((minX + maxX) / v10);
    dst.d1 = -((minY + maxY) / v11);
    dst.d2 = -((minZ + maxZ) / v12);
    dst.d3 = 1.0f;
}

void GxuXformCreateOrthoDepth(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, C44Matrix& dst) {
    STORM_ASSERT(minX != maxX);
    STORM_ASSERT(minY != maxY);
    STORM_ASSERT(minZ < maxZ);

    int32_t stereoEnabled = g_theGxDevicePtr->StereoEnabled();

    dst.a0 = 2.0 / (maxX - minX);
    dst.a1 = 0.0f;
    dst.a2 = 0.0f;
    dst.a3 = 0.0f;

    dst.b0 = 0.0f;
    dst.b1 = 2.0 / (maxY - minY);
    dst.b2 = 0.0f;
    dst.b3 = 0.0f;

    dst.c0 = 0.0f;
    dst.c1 = 0.0f;
    dst.c2 = stereoEnabled ? 1.00008f : 2.0 / (maxZ - minZ);
    dst.c3 = stereoEnabled ? 1.0f : 0.0f;

    dst.d0 = -((maxX + minX) / (maxX - minX));
    dst.d1 = -((maxY + minY) / (maxY - minY));
    dst.d2 = stereoEnabled ? -0.40001601f : -((maxZ + minZ) / (maxZ - minZ));
    dst.d3 = stereoEnabled ? 0.0f : 1.0f;
}

void GxuXformCreateProjection_Exact(float fovyInRadians, float aspect, float minZ, float maxZ, C44Matrix& dst) {
    STORM_ASSERT(fovyInRadians > 0.0f && fovyInRadians < CMath::PI);
    STORM_ASSERT(aspect > 0.0f);
    STORM_ASSERT(minZ < maxZ);

    float v8 = fovyInRadians * 0.5;
    float v9 = tan(v8);
    float v10 = v9 * minZ;
    float v11 = v10 * aspect;
    float v7 = maxZ - minZ;

    dst.a0 = minZ / v11;
    dst.b0 = 0.0f;
    dst.c0 = 0.0f;
    dst.d0 = 0.0f;

    dst.a1 = 0.0f;
    dst.b1 = minZ / v10;
    dst.c1 = 0.0f;
    dst.d1 = 0.0f;

    dst.a2 = 0.0f;
    dst.b2 = 0.0f;
    dst.c2 = (minZ + maxZ) / v7;
    dst.d2 = minZ * (maxZ * -2.0f) / v7;

    dst.a3 = 0.0f;
    dst.b3 = 0.0f;
    dst.c3 = 1.0f;
    dst.d3 = 0.0f;
}

void GxuXformCreateProjection_SG(float fov, float aspect, float minZ, float maxZ, C44Matrix& dst) {
    float v6 = aspect * aspect + 1.0f;
    float v7 = sqrt(v6);
    float v8 = 1.0f / v7;
    float v9 = v8 * fov;

    GxuXformCreateProjection_Exact(v9, aspect, minZ, maxZ, dst);
}

void GxuXformCalcFrustumCorners(C44Matrix* viewMatrix, C44Matrix* projMatrix, C3Vector corners[8]) {
    C44Matrix invProj = projMatrix->Inverse();
    C44Matrix invView = viewMatrix->Inverse();
    C44Matrix invViewProj = invProj * invView;

    auto Unproject = [&](float x, float y, float z, float w) -> C3Vector {
        C4Vector ndc = { x, y, z, w };
        return C3Vector(ndc * invViewProj);

    };

    bool isOrthographic = fabsf(projMatrix->d3 - 1.0f) < 0.00000023841858f;

    // TODO cleaned up with claude, double check for hallucinations
    if (isOrthographic) {
        // Standard NDC unit cube corners, w=1
        corners[0] = Unproject(-1, -1, -1, 1); // near bottom-left
        corners[1] = Unproject(-1, 1, -1, 1);  // near top-left
        corners[2] = Unproject(1, 1, -1, 1);   // near top-right
        corners[3] = Unproject(1, -1, -1, 1);  // near bottom-right
        corners[4] = Unproject(-1, -1, 1, 1);  // far  bottom-left
        corners[5] = Unproject(-1, 1, 1, 1);   // far  top-left
        corners[6] = Unproject(1, 1, 1, 1);    // far  top-right
        corners[7] = Unproject(1, -1, 1, 1);   // far  bottom-right
    } else {
        // Perspective: derive NDC z values from the projection matrix coefficients
        // M43 and M33 encode the near/far clip planes.
        // Standard GL-style perspective: M43 = -(2*far*near)/(far-near), M33 = -(far+near)/(far-near)
        float mz = -projMatrix->d2;
        float nearZ = mz / (projMatrix->c2 + 1.0f); // NDC z at near plane
        float farZ = mz / (projMatrix->c2 - 1.0f);  // NDC z at far plane

        float n = nearZ;   // positive near NDC depth
        float nn = -nearZ; // negative near NDC depth (left/bottom side)
        float f = farZ;    // positive far NDC depth
        float nf = -farZ;  // negative far NDC depth

        // Near plane corners (w = n)
        corners[0] = Unproject(nn, nn, nn, n); // near bottom-left
        corners[1] = Unproject(nn, n, nn, n);  // near top-left
        corners[2] = Unproject(n, n, nn, n);   // near top-right
        corners[3] = Unproject(n, nn, nn, n);  // near bottom-right

        // Far plane corners (w = f)
        corners[4] = Unproject(nf, nf, f, f); // far bottom-left
        corners[5] = Unproject(nf, f, f, f);  // far top-left
        corners[6] = Unproject(f, f, f, f);   // far top-right
        corners[7] = Unproject(f, nf, f, f);  // far bottom-right
    }
}
