#ifndef WORLD_DAY_NIGHT_PLANET_HPP
#define WORLD_DAY_NIGHT_PLANET_HPP

#include <cstdint>
#include <storm/Array.hpp>
#include <tempest/Vector.hpp>
#include "gx/Texture.hpp"

namespace DayNight {

class DNPlanet {
    public:
    C3Vector m_position;
    CImVector m_color;
    HTEXTURE m_texture;
    float m_scale;
    float m_baseScale;
    float m_period;

    void Initialize(const char* fileName);
    void Render();
    void ClipGeometry(C3Vector* pos, C2Vector* tex, CImVector* color, uint16_t* indices, uint32_t* outVertexCount, uint32_t* outIndexCount);
    void GenGeometry(C3Vector* pos, C2Vector* tex, CImVector* color, uint16_t* indices, uint32_t* outVertexCount, uint32_t* outIndexCount);
};

} // namespace DayNight

#endif
