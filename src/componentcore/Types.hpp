#ifndef COMPONENT_CORE_TYPES_HPP
#define COMPONENT_CORE_TYPES_HPP

#include <cstdint>
#include "gx/Texture.hpp"

class CharSectionsRec;

enum CHAR_CUSTOMIZATION_TYPE {
    CHAR_CUSTOMIZATION_SKIN = 0,
    CHAR_CUSTOMIZATION_FACE = 1,
    CHAR_CUSTOMIZATION_HAIR_STYLE = 2,
    CHAR_CUSTOMIZATION_HAIR_COLOR = 3,
    CHAR_CUSTOMIZATION_FACIAL_FEATURE = 4
};

enum COMPONENT_CONTEXT {
    DEFAULT_CONTEXT = 0,
    CONTEXT_1,
    CONTEXT_2,
    CONTEXT_3
};

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
