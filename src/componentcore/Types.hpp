#ifndef COMPONENT_CORE_TYPES_HPP
#define COMPONENT_CORE_TYPES_HPP

#include <cstdint>
#include "gx/Texture.hpp"

class CharSectionsRec;

struct CharacterSection {
    HTEXTURE layerTex[7];
    uint32_t layerItemDisplayId[7];
    uint32_t layerMask;
};

struct CharacterBaseVariation {
    HTEXTURE m_texture[3];
};

struct CharColorList {
    int numColors;
    CharSectionsRec** color;
};

struct CharVariationList {
    int numVariations;
    CharColorList* variation;
};

struct st_race {
    CharVariationList m_variation[5];
};

#endif // COMPONENT_CORE_TYPES_HPP
