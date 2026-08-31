#include "util/C3Spline.hpp"
#include <tempest/vector/C3Vector.hpp>

CDataStore& operator>>(CDataStore& msg, C3Spline_CatmullRom& spline) {
    uint32_t pointCount = 0;
    msg.Get(pointCount);

    void* points;
    msg.GetDataInSitu(points, sizeof(C3Vector) * pointCount);

    uint8_t splineMode;
    msg.Get(splineMode);
    // TODO spline.splineMode = splineMode;

    if (pointCount && msg.IsValid()) {
        // TODO spline.SetPoints()
    }

    return msg;
}

float C3Spline::ILength() {
    return 0;
}

void C3Spline::IValidateCache() {

}

void C3Spline::IPosArclength(float t, C3Vector* out) {

}

void C3Spline::IPosParametric(float t, C3Vector* out) {

}

void C3Spline::IVelArclength(float t, C3Vector* out) {

}

void C3Spline::IVelParametric(float t, C3Vector* out) {

}

void C3Spline::IFrameArclength(float t, C3Vector* out) {

}

// OFFSET: 0x4C4CD0
void C3Spline::ISetPoints(C3Vector* points, uint32_t count) {
    this->m_pointCount = count;
    for (uint32_t i = 0; i < count; i++) {
        if (i >= 25)
            break;

        this->m_points[i] = points[i];
    }

    if (count <= 25)
        this->m_extraPoints.SetCount(0);
    else
        this->m_extraPoints.Set(count - 25, &points[25]);
}

void C3Spline::IGetPoints(C3Vector* out, uint32_t first, uint32_t count) {

}

// OFFSET: 0x4C3830
void C3Spline::SetPoints(C3Vector* points, uint32_t count) {
    this->ISetPoints(points, count);
    if (this->m_pointCount > 3) {
        this->IValidateCache();
        this->m_length = this->ILength();
    }
}

// OFFSET: 0x4C3D80
void C3Spline::GetPoints(C3Vector* out, uint32_t outCount) {
    uint32_t count = outCount;
    if (count >= this->m_pointCount)
        count = this->m_pointCount;

    for (uint32_t i = 0; i < count; i++) {
        if (i >= 25)
            break;

        out[i] = this->m_points[i];
    }
    if (count >= 25)
        memcpy(&out[25], this->m_extraPoints.Ptr(), (count - 25) * 4);
}

// OFFSET: 0x4C3870
void C3Spline::Pos(float t, C3Vector* out, uint32_t pointCount) {
    if (t > 0.0) {
        if (t < 1.0) {
            if (pointCount) {
                if (pointCount == 1)
                    this->IPosArclength(t, out);
            } else {
                this->IPosParametric(t, out);
            }
        } else {
            m_pointCount = this->m_pointCount;
            C3Vector* v5;
            if (m_pointCount > 25)
                v5 = &this->m_extraPoints.m_data[m_pointCount - 26];
            else
                v5 = v5 = &this->m_points[m_pointCount - 1];
            *out = *v5;
        }
    } else {
        *out = this->m_points[0];
    }
}
