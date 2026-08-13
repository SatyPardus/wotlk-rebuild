#ifndef GX_LIGHT_CGXLIGHT_HPP
#define GX_LIGHT_CGXLIGHT_HPP

#include "gx/Types.hpp"
#include "tempest/Vector.hpp"
#include <cstdint>

class CGxLight {
    public:
    uint32_t m_flags;
    C3Vector m_dir;
    C3Vector m_ambientColor;
    C3Vector m_dirColor;
    C3Vector m_specularColor;
    float m_constantAttenuation;
    float m_linearAttenuation;
    float m_quadraticAttenuation;

    CGxLight();
};

#endif
