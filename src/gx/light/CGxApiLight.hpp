#ifndef GX_LIGHT_CGXAPILIGHT_HPP
#define GX_LIGHT_CGXAPILIGHT_HPP

#include "gx/Types.hpp"
#include "tempest/Vector.hpp"
#include <cstdint>
#include "gx/light/CGxLight.hpp"

class CGxApiLight {
    public:
    C4Vector m_dir;
    C3Vector m_ambColor;
    C3Vector m_dirColor;
    C3Vector m_specColor;
    float m_constantAttenuation;
    float m_linearAttenuation;
    float m_quadraticAttenuation;
    int32_t m_enable;
    uint32_t flags;

    CGxApiLight& operator=(const CGxLight& src);
};


#endif
