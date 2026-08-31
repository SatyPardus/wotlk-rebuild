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
    /* 06 */ virtual void IFrameArclength(float t, C3Vector* out);
    /* 07 */ virtual void ISetPoints(C3Vector* points, uint32_t count);
    /* 08 */ virtual void IGetPoints(C3Vector* out, uint32_t first, uint32_t count);

    void SetPoints(C3Vector* points, uint32_t count);
    void GetPoints(C3Vector* out, uint32_t count);
    void Pos(float t, C3Vector* points, uint32_t pointCount);
};

class C3Spline_CatmullRom : public C3Spline {
    public:
    /* 01C0 */ uint32_t m_splineMode;
};

// TODO move this operator>> to util/DataStore.hpp
CDataStore& operator>>(CDataStore& msg, C3Spline_CatmullRom& spline);

#endif
