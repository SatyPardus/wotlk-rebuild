#ifndef MODEL_C_M2_SEQUENCE_PLAYBACK_HPP
#define MODEL_C_M2_SEQUENCE_PLAYBACK_HPP

#include "model/M2Types.hpp"

class CM2Model;

class CM2SequencePlayback {
    public:
    // Member variables
    /* 0x00 */ TSLink<CM2SequencePlayback> m_link;
    /* 0x08 */ CM2Model* m_model;
    /* 0x0C */ uint16_t m_boneIndex;
    /* 0x0E */ uint16_t m_flags;
    /* 0x10 */ uint32_t m_time;
    /* 0x14 */ float m_speed;
    /* 0x18 */ M2SequenceFallback m_fallback;

    // Member functions
};

#endif
