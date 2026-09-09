#ifndef MODEL_M2_MODEL_HPP
#define MODEL_M2_MODEL_HPP

#include "gx/Camera.hpp"
#include "model/CM2Light.hpp"
#include <cstdint>
#include <tempest/Quaternion.hpp>
#include <tempest/Vector.hpp>

template<class T>
class M2Track;

template<class T>
struct M2ModelTrack {
    uint32_t currentKey = 0;
    uint32_t secondaryKey = 0;
    T currentValue;
};

struct M2ModelAttachment {
    M2ModelTrack<uint8_t> visibilityTrack;
};

struct M2ModelBoneSeq {
    /* 0x000 */ uint32_t m_currentTime = 0;
    /* 0x004 */ uint16_t m_animIndex = -1;
    /* 0x006 */ uint16_t m_sourceBoneIndex = -1;
    /* 0x008 */ uint16_t m_sequenceIndex = -1;
    /* 0x00A */ uint8_t m_finished = 1;
    /* 0x00B */ uint8_t m_pickRandomVariation = 1;
    /* 0x00C */ uint32_t m_startTime = 0;
    /* 0x010 */ uint32_t m_endTime = 0;
    /* 0x014 */ float m_speed = 1.0f;
    /* 0x018 */ float m_invSpeed = 1.0f;
    /* 0x01C */ uint32_t m_startOffset = 0;
    /* 0x020 */ uint32_t m_repeatCount = 0;
};

struct M2ModelBone {
    /* 0x000 */ M2ModelTrack<C3Vector> translationTrack;
    /* 0x014 */ M2ModelTrack<C4Quaternion> rotationTrack;
    /* 0x02C */ M2ModelTrack<C3Vector> scaleTrack;
    /* 0x040 */ M2ModelBoneSeq sequence;
    /* 0x064 */ M2ModelBoneSeq secondarySequence;
    /* 0x088 */ C44Matrix* m_proceduralTransform = nullptr;
    /* 0x08C */ uint32_t m_flags = 0;
    /* 0x090 */ uint32_t m_sequenceId = 0xFFFFFFFF;
    /* 0x094 */ uint16_t m_variationIndex = 0;
    /* 0x096 */ uint16_t m_animListNext = 0;
    /* 0x098 */ uint16_t* m_animListPrev = nullptr;
    /* 0x09C */ uint32_t m_blendEndTime = 0;
    /* 0x0A0 */ float m_invBlendDuration = 0.0f;
    /* 0x0A4 */ float m_blendWeightMax = 1.0f;
    /* 0x0A8 */ float m_blendFactor = 0.0f;
};

struct M2ModelRibbon {
    M2ModelTrack<C3Vector> colorTrack;
    M2ModelTrack<float> alphaTrack;
    M2ModelTrack<float> heightAboveTrack;
    M2ModelTrack<float> heightBelowTrack;
    M2ModelTrack<uint16_t> textureSlotTrack;
    M2ModelTrack<uint8_t> visibilityTrack;
};

struct M2ModelParticle {
    M2ModelTrack<float> speedTrack;
    M2ModelTrack<float> variationTrack;
    M2ModelTrack<float> latitudeTrack;
    M2ModelTrack<float> longitudeTrack;
    M2ModelTrack<float> gravityTrack;
    M2ModelTrack<float> lifeTrack;
    M2ModelTrack<float> emissionRateTrack;
    M2ModelTrack<float> widthTrack;
    M2ModelTrack<float> lengthTrack;
    M2ModelTrack<float> zsourceTrack;
    M2ModelTrack<uint8_t> visibilityTrack;
    uint8_t m_emitting = 1;
    uint8_t m_active = 1;
    uint8_t m_burstFired = 0;
    uint8_t pad_87 = 0;
};

struct M2ModelCamera {
    M2ModelTrack<C3Vector> positionTrack;
    M2ModelTrack<C3Vector> targetTrack;
    M2ModelTrack<float> rollTrack;
    HCAMERA m_camera = nullptr;
};

struct M2ModelColor {
    M2ModelTrack<C3Vector> colorTrack;
    M2ModelTrack<float> alphaTrack;
};

struct M2ModelLight {
    M2ModelTrack<C3Vector> ambientColorTrack;
    M2ModelTrack<float> ambientIntensityTrack;
    M2ModelTrack<C3Vector> diffuseColorTrack;
    M2ModelTrack<float> diffuseIntensityTrack;
    M2ModelTrack<uint8_t> visibilityTrack;
    uint32_t uint64 = 1;
    CM2Light light;
};

struct M2ModelTextureTransform {
    M2ModelTrack<C3Vector> translation;
    M2ModelTrack<C4Quaternion> rotation;
    M2ModelTrack<C3Vector> scaling;
};

struct M2ModelTextureWeight {
    M2ModelTrack<float> weightTrack;
};

#endif
