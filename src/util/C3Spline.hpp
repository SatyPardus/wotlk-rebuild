#ifndef UTIL_C3_SPLINE_HPP
#define UTIL_C3_SPLINE_HPP

#include <common/DataStore.hpp>
#include <tempest/Vector.hpp>
#include <storm/Array.hpp>

// TODO move these classes to typhoon
class C3Spline {
    public:
    /* 0004 */ float m_length;
    /* 0008 */ C3Vector m_points[25];
    /* 0134 */ TSGrowableArray<C3Vector> m_extraPoints;
    /* 0144 */ uint32_t m_pointCount;
    /* 0148 */ float m_segLength[25];
    /* 01AC */ TSGrowableArray<float> m_extraSegLength;
    /* 01BC */ uint32_t m_segCount;

    /* 00 */ virtual float ILength();
    /* 01 */ virtual void IValidateCache();
    /* 02 */ virtual void IPosArclength(float t, C3Vector* out);
    /* 03 */ virtual void IPosParametric(float t, C3Vector* out);
    /* 04 */ virtual void IVelArclength(float t, C3Vector* out);
    /* 05 */ virtual void IVelParametric(float t, C3Vector* out);
    /* 06 */ virtual void IFrameArclength(float t, C44Matrix* out);
    /* 07 */ virtual void ISetPoints(C3Vector* points, uint32_t count);
    /* 08 */ virtual uint32_t IGetPoints(float t, C3Vector* out, uint32_t maxCount);

    C3Spline();
    void SetPoints(C3Vector* points, uint32_t count);
    void GetPoints(C3Vector* out, uint32_t count);
    void Pos(float t, C3Vector* points, uint32_t pointCount);
    void Frame(float t, C44Matrix* out, uint32_t a4);
    C3Vector* GetVectorAtIndex(uint32_t index);
    C3Vector* Point(uint32_t segment, float t, const C44Matrix& basis, C3Vector* out);
    float SegLength(uint32_t segment, const C44Matrix& basis); 
    void ArclengthSegT(float t, const C44Matrix& basis, int32_t segCount, uint32_t* outSegment, float* outT);
    float SumCachedSegLengths(uint32_t count);
    void ParametricSegT(float t, uint32_t* outSegment, float* outT);

    C3Spline& operator=(const C3Spline& source);
};

class C3Spline_CatmullRom : public C3Spline {
    public:
    /* 01C0 */ uint32_t m_splineMode;

    /* 00 */ float ILength() override;
    /* 01 */ void IValidateCache() override;
    /* 02 */ void IPosArclength(float t, C3Vector* out) override;
    /* 03 */ void IPosParametric(float t, C3Vector* out) override;
    /* 04 */ void IVelArclength(float t, C3Vector* out) override;
    /* 05 */ void IVelParametric(float t, C3Vector* out) override;
    /* 06 */ void IFrameArclength(float t, C44Matrix* out) override;
    /* 07 */ void ISetPoints(C3Vector* points, uint32_t count) override;
    /* 08 */ uint32_t IGetPoints(float t, C3Vector* out, uint32_t maxCount) override;

    C3Spline_CatmullRom();
    uint32_t GetSplineMode();
    C3Vector* EvaluateDer1(uint32_t segment, float t, const float* basis, C3Vector* out);
    C3Vector* Evaluate(uint32_t segment, float t, C3Vector* out);
    float SegLength(uint32_t segment);
};

// TODO move this operator>> to util/DataStore.hpp
CDataStore& operator>>(CDataStore& msg, C3Spline_CatmullRom& spline);

#endif
