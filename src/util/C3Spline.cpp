#include "util/C3Spline.hpp"
#include <tempest/vector/C3Vector.hpp>

static const C44Matrix s_catmullRomBasis = {
    -0.5f, 1.0f, -0.5f, 0.0f,
    1.5f, -2.5f, 0.0f, 1.0f,
    -1.5f, 2.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f
};

static const float s_catmullRomDer1Basis[16] = {
    -1.5f, 2.0f, -0.5f, 0.0f,
    4.5f, -5.0f, 0.0f, 0.0f,
    -4.5f, 4.0f, 0.5f, 0.0f,
    1.5f, -1.0f, 0.0f, 1.0f
};

// OFFSET: 0x4F4AE0
CDataStore& operator>>(CDataStore& msg, C3Spline_CatmullRom& spline) {
    uint32_t pointCount = 0;
    msg.Get(pointCount);

    void* points;
    msg.GetDataInSitu(points, sizeof(C3Vector) * pointCount);

    uint8_t splineMode;
    msg.Get(splineMode);
    spline.m_splineMode = splineMode;

    if (pointCount && msg.IsValid())
        spline.SetPoints(static_cast<C3Vector*>(points), pointCount);

    return msg;
}

// OFFSET: 0x6EBE80
C3Spline& C3Spline::operator=(const C3Spline& source) {
    this->m_length = source.m_length;

    for (uint32_t i = 0; i < 25; i++) {
        this->m_points[i].x = source.m_points[i].x;
        this->m_points[i].y = source.m_points[i].y;
        this->m_points[i].z = source.m_points[i].z;
    }

    if (&this->m_extraPoints != &source.m_extraPoints)
        this->m_extraPoints.Set(source.m_extraPoints.m_count, source.m_extraPoints.m_data);
    this->m_extraPoints.m_chunk = source.m_extraPoints.m_chunk;
    this->m_pointCount = source.m_pointCount;

    for (uint32_t i = 0; i < 25; i++)
        this->m_segLength[i] = source.m_segLength[i];

    if (&this->m_extraSegLength != &source.m_extraSegLength)
        this->m_extraSegLength.Set(source.m_extraSegLength.m_count, source.m_extraSegLength.m_data);
    this->m_extraSegLength.m_chunk = source.m_extraSegLength.m_chunk;
    this->m_segCount = source.m_segCount;

    return *this;
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

void C3Spline::IFrameArclength(float t, C44Matrix* out) {

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

// OFFSET: 0x946080
uint32_t C3Spline::IGetPoints(float t, C3Vector* out, uint32_t maxCount) {
    return 0;
}

// OFFSET: 0x4096D0
C3Spline::C3Spline() {
    for (uint32_t i = 0; i < 25; i++) {
        this->m_points[i].x = 0.0f;
        this->m_points[i].y = 0.0f;
        this->m_points[i].z = 0.0f;
    }

    this->m_pointCount = 0;
    this->m_segCount = 0;
    this->m_extraPoints.Constructor();
    this->m_extraSegLength.Constructor();
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

// OFFSET: 0x4C3920
void C3Spline::Vel(float t, C3Vector* out, uint32_t mode) {
    auto clamped = 0.0f;
    if (t >= 0.0f) {
        clamped = t;
        if (t >= 1.0)
            clamped = 1.0;
    }
    if (mode) {
        if (mode == 1)
            this->IVelArclength(clamped, out);
    } else {
        this->IVelParametric(clamped, out);
    }
}

// OFFSET: 0x4C3980
void C3Spline::Frame(float t, C44Matrix* out, uint32_t a4) {
    auto v4 = 0.0f;
    if (t >= 0.0f) {
        v4 = t;
        if (t >= 1.0)
            v4 = 1.0;
    }
    if (a4 == 1) {
        this->IFrameArclength(v4, out);
    }
}

// OFFSET: 0x4C36F0
C3Vector* C3Spline::GetVectorAtIndex(uint32_t index) {
    if (index >= 25)
        return &this->m_extraPoints.m_data[index - 25];

    return &this->m_points[index];
}

// OFFSET: 0x4C39D0
C3Vector* C3Spline::Point(uint32_t segment, float t, const C44Matrix& basis, C3Vector* out) {
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;

    const float* row = &basis.a0;

    for (uint32_t i = 0; i < 4; i++) {
        float weight = ((row[0] * t + row[1]) * t + row[2]) * t + row[3];
        C3Vector* point = this->GetVectorAtIndex(segment + i);

        out->x = weight * point->x + out->x;
        out->y = point->y * weight + out->y;
        out->z = weight * point->z + out->z;

        row += 4;
    }

    return out;
}

// OFFSET: 0x4C3B10
float C3Spline::SegLength(uint32_t segment, const C44Matrix& basis) {
    float t = 0.05f;
    float length = 0.0f;

    C3Vector previous = { 0.0f, 0.0f, 0.0f };
    C3Vector current = { 0.0f, 0.0f, 0.0f };

    this->Point(segment, 0.0f, basis, &previous);

    for (uint32_t i = 20; i; i--) {
        this->Point(segment, t, basis, &current);

        float dx = current.x - previous.x;
        float dy = current.y - previous.y;
        float dz = current.z - previous.z;

        previous = current;

        length = sqrtf(dx * dx + dy * dy + dz * dz) + length;
        t = t + 0.05f;
    }

    return length;
}

// OFFSET: 0x4C3BD0
void C3Spline::ArclengthSegT(float t, const C44Matrix& basis, int32_t segCount, uint32_t* outSegment, float* outT) {
    if (segCount <= 1) {
        *outSegment = 0;
        *outT = t;
        return;
    }

    float target = this->m_length * t;
    float length = 0.0f;
    *outSegment = 0;

    int32_t last = segCount - 1;
    if (last) {
        while (true) {
            uint32_t segment = *outSegment;
            float segLength = segment >= 25 ? this->m_extraSegLength[segment - 25] : this->m_segLength[segment];

            if (segLength + length > target)
                break;

            length = segLength + length;
            *outSegment = segment + 1;

            if (segment + 1 >= last)
                break;
        }
    }

    uint32_t segment = *outSegment;
    float segLength = segment >= 25 ? this->m_extraSegLength[segment - 25] : this->m_segLength[segment];

    *outT = (target - length) / segLength;
}

// OFFSET: 0x4C3C80
float C3Spline::SumCachedSegLengths(uint32_t count) {
    float length = 0.0f;

    for (uint32_t i = 0; i < count; i++)
        length = length + (i >= 25 ? this->m_extraSegLength[i - 25] : this->m_segLength[i]);

    return length;
}

// OFFSET: 0x4C4140
void C3Spline::ParametricSegT(float t, uint32_t* outSegment, float* outT) {
    float segCount = static_cast<float>(this->m_pointCount - 3);
    uint32_t segment = static_cast<uint32_t>(lrintf(segCount * t - 0.5f));

    *outSegment = segment;
    *outT = (t - 1.0f / segCount * static_cast<float>(segment)) * segCount;
}

// OFFSET: 0x409740
C3Spline_CatmullRom::C3Spline_CatmullRom() {
    this->m_splineMode = 1;
}

// OFFSET: 0x409760
uint32_t C3Spline_CatmullRom::GetSplineMode() {
    return this->m_splineMode;
}

// OFFSET: 0x4C3A70
C3Vector* C3Spline_CatmullRom::EvaluateDer1(uint32_t segment, float t, const float* basis, C3Vector* out) {
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;

    for (uint32_t i = 0; i < 4; i++) {
        float weight = (basis[0] * t + basis[1]) * t + basis[2];
        C3Vector* point = this->GetVectorAtIndex(segment + i);

        out->x = weight * point->x + out->x;
        out->y = point->y * weight + out->y;
        out->z = weight * point->z + out->z;

        basis += 4;
    }

    return out;
}

// OFFSET: 0x4C3FD0
C3Vector* C3Spline_CatmullRom::Evaluate(uint32_t segment, float t, C3Vector* out) {
    if (this->m_splineMode)
        return this->Point(segment, t, s_catmullRomBasis, out);

    C3Vector* a = this->GetVectorAtIndex(segment + 1);
    C3Vector* b = this->GetVectorAtIndex(segment + 2);

    C3Vector value;
    value.x = (b->x - a->x) * t + a->x;
    value.y = (b->y - a->y) * t + a->y;
    value.z = (b->z - a->z) * t + a->z;

    *out = value;
    return out;
}

// OFFSET: 0x4C40B0
float C3Spline_CatmullRom::SegLength(uint32_t segment) {
    if (this->m_splineMode)
        return this->C3Spline::SegLength(segment, s_catmullRomBasis);

    C3Vector* a = this->GetVectorAtIndex(segment + 1);
    C3Vector* b = this->GetVectorAtIndex(segment + 2);

    float dy = b->y - a->y;
    float dz = b->z - a->z;
    float dx = b->x - a->x;

    return sqrtf(dx * dx + dz * dz + dy * dy);
}

// OFFSET: 0x4C41B0
float C3Spline_CatmullRom::ILength() {
    return this->SumCachedSegLengths(this->m_pointCount - 3);
}

// OFFSET: 0x4C41C0
void C3Spline_CatmullRom::IValidateCache() {
    uint32_t segCount = this->m_pointCount - 3;
    if (this->m_pointCount == 3)
        return;

    for (uint32_t i = 0; i < segCount; i++) {
        float length = this->SegLength(i);

        if (i >= 25)
            this->m_extraSegLength[i - 25] = length;
        else
            this->m_segLength[i] = length;
    }
}

// OFFSET: 0x4C4230
void C3Spline_CatmullRom::IPosArclength(float t, C3Vector* out) {
    uint32_t segment;
    float segT;

    this->ArclengthSegT(t, s_catmullRomBasis, this->m_pointCount - 3, &segment, &segT);
    this->Evaluate(segment, segT, out);
}

// OFFSET: 0x4C4280
void C3Spline_CatmullRom::IPosParametric(float t, C3Vector* out) {
    uint32_t segment;
    float segT;

    this->ParametricSegT(t, &segment, &segT);
    this->Evaluate(segment, segT, out);
}

// OFFSET: 0x4C42C0
void C3Spline_CatmullRom::IVelArclength(float t, C3Vector* out) {
    float clamped = 0.0f;
    if (t >= 0.0f) {
        clamped = t;
        if (t >= 1.0f)
            clamped = 1.0f;
    }

    uint32_t segment;
    float segT;

    this->ArclengthSegT(clamped, s_catmullRomBasis, this->m_pointCount - 3, &segment, &segT);
    this->EvaluateDer1(segment, segT, s_catmullRomDer1Basis, out);
}

// OFFSET: 0x4C4340
void C3Spline_CatmullRom::IVelParametric(float t, C3Vector* out) {
    float clamped = 0.0f;
    if (t >= 0.0f) {
        clamped = t;
        if (t >= 1.0f)
            clamped = 1.0f;
    }

    uint32_t segment;
    float segT;

    this->ParametricSegT(clamped, &segment, &segT);
    this->EvaluateDer1(segment, segT, s_catmullRomDer1Basis, out);
}

// OFFSET: 0x4C43B0
void C3Spline_CatmullRom::IFrameArclength(float t, C44Matrix* out) {
    uint32_t segment;
    float segT;

    this->ArclengthSegT(t, s_catmullRomBasis, this->m_pointCount - 3, &segment, &segT);
    this->Evaluate(segment, segT, reinterpret_cast<C3Vector*>(&out->d0));

    C3Vector* a = this->GetVectorAtIndex(segment + 1);
    C3Vector* b = this->GetVectorAtIndex(segment + 2);

    C3Vector chord;
    chord.x = b->x - a->x;
    chord.y = b->y - a->y;
    chord.z = b->z - a->z;

    float chordMag = chord.z * chord.z + chord.y * chord.y + chord.x * chord.x;
    if (chordMag > 0.00000023841858f) {
        float scale = 1.0f / sqrtf(chordMag);
        chord.x = chord.x * scale;
        chord.y = chord.y * scale;
        chord.z = scale * chord.z;
    }

    if (this->m_splineMode) {
        C3Vector velocity;
        velocity.x = 0.0f;
        velocity.y = 0.0f;
        velocity.z = 0.0f;

        this->EvaluateDer1(segment, segT, s_catmullRomDer1Basis, &velocity);

        float velocityMag = velocity.z * velocity.z + velocity.y * velocity.y + velocity.x * velocity.x;
        if (velocityMag > 0.000099999997f) {
            float scale = 1.0f / sqrtf(velocityMag);
            velocity.x = velocity.x * scale;
            velocity.y = velocity.y * scale;
            velocity.z = scale * velocity.z;

            if (velocity.z * chord.z + velocity.y * chord.y + velocity.x * chord.x >= 0.5f) {
                out->a0 = velocity.x;
                out->a1 = velocity.y;
                out->a2 = velocity.z;
            } else {
                out->a0 = chord.x;
                out->a1 = chord.y;
                out->a2 = chord.z;
            }
        }
    } else {
        out->a0 = chord.x;
        out->a1 = chord.y;
        out->a2 = chord.z;
    }

    out->b0 = -out->a1;
    out->b1 = out->a0;
    out->b2 = 0.0f;

    float sideMag = out->b1 * out->b1 + out->b0 * out->b0;
    if (sideMag > 0.00000023841858f) {
        float scale = 1.0f / sqrtf(sideMag);
        out->b0 = scale * out->b0;
        out->b1 = scale * out->b1;
    }

    out->c0 = -(out->a2 * out->b1);
    out->c1 = out->a2 * out->b0;
    out->c2 = out->a0 * out->b1 - out->a1 * out->b0;
}

// OFFSET: 0x4C4600
uint32_t C3Spline_CatmullRom::IGetPoints(float t, C3Vector* out, uint32_t maxCount) {
    uint32_t pointCount = this->m_pointCount;

    uint32_t segment;
    float segT;
    this->ArclengthSegT(t, s_catmullRomBasis, pointCount - 3, &segment, &segT);

    uint32_t count = pointCount - segment - 3;
    if (!out)
        return count;

    if (count >= maxCount)
        count = maxCount;

    if (!count)
        return count;

    for (uint32_t i = 0; i < count; i++)
        out[i] = *this->GetVectorAtIndex(segment + 2 + i);

    return count;
}

// OFFSET: 0x4C4DA0
void C3Spline_CatmullRom::ISetPoints(C3Vector* points, uint32_t count) {
    if (count) {
        this->m_segCount = count - 3;
        if (this->m_segCount > 25)
            this->m_extraSegLength.SetCount(this->m_segCount - 25);
    } else {
        this->m_segCount = 0;
        this->m_extraSegLength.SetCount(0);
    }

    this->C3Spline::ISetPoints(points, count);
}
