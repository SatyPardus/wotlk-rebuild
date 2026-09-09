#ifndef MODEL_M2_TYPES_HPP
#define MODEL_M2_TYPES_HPP

#include "M2Data.hpp"

class CM2Model;
class CShaderEffect;
class CParticleEmitter2;

enum M2BLEND {
    M2BLEND_OPAQUE = 0x0,
    M2BLEND_ALPHA_KEY = 0x1,
    M2BLEND_ALPHA = 0x2,
    M2BLEND_NO_ALPHA_ADD = 0x3,
    M2BLEND_ADD = 0x4,
    M2BLEND_MOD = 0x5,
    M2BLEND_MOD_2X = 0x6,
    M2BLEND_COUNT = 0x7,
};

enum M2COMBINER {
    M2COMBINER_OPAQUE = 0x0,
    M2COMBINER_MOD = 0x1,
    M2COMBINER_DECAL = 0x2,
    M2COMBINER_ADD = 0x3,
    M2COMBINER_MOD2X = 0x4,
    M2COMBINER_FADE = 0x5,
    M2COMBINER_MOD2X_NA = 0x6,
    M2COMBINER_ADD_NA = 0x7,
    M2COMBINER_OP_MASK = 0x7,
    M2COMBINER_ENVMAP = 0x8,
    M2COMBINER_STAGE_SHIFT = 0x4,
};

enum M2LIGHTTYPE {
    M2LIGHT_0 = 0,
    M2LIGHT_1 = 1
};

enum M2PASS {
    M2PASS_0 = 0,
    M2PASS_1 = 1,
    M2PASS_2 = 2,
    M2PASS_COUNT = 3
};

struct M2SequenceFallback {
    uint16_t uint0;
    uint16_t uint2;
};

struct M2Element {
    /* 0x00 */ int32_t type;
    /* 0x04 */ CM2Model* model;
    /* 0x08 */ uint32_t flags;
    /* 0x0C */ float alpha;
    /* 0x10 */ float float10;
    /* 0x14 */ float float14;
    /* 0x18 */ union {
        int32_t index;
        CParticleEmitter2* emitter;
    };
    /* 0x1C */ uint32_t doodadRunLength;
    /* 0x20 */ int32_t doodadKey;
    /* 0x24 */ int32_t priorityPlane;
    /* 0x28 */ M2Batch* batch;
    /* 0x2C */ M2SkinSection* skinSection;
    /* 0x30 */ CShaderEffect* effect;
    /* 0x34 */ uint32_t vertexPermute;
    /* 0x38 */ uint32_t pixelPermute;
    /* 0x3C */ uint32_t uint3C;
    /* 0x40 */ uint32_t additiveGroup;
};

struct M2HitRec {
    CM2Model* model;
    float tNear;
    float tFar;
    uint32_t priority;
};

#endif
