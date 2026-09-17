#ifndef MODEL_C_M2_SEQUENCE_LOAD_HPP
#define MODEL_C_M2_SEQUENCE_LOAD_HPP

#include "model/CM2SequencePlayback.hpp"

class CM2Shared;
class CAsyncObject;

class CM2SequenceLoad {
    public:
    // Member variables
    /* 0x00 */ TSLink<CM2SequenceLoad> m_link;
    /* 0x08 */ CAsyncObject* m_asyncObject;
    /* 0x0C */ CM2Shared* m_shared;
    /* 0x10 */ uint16_t m_sequenceIndex;
    /* 0x12 */ uint16_t m_sequenceBufferIndex;
    /* 0x14 */ STORM_EXPLICIT_LIST(CM2SequencePlayback, m_link) m_playbacks;

    // Member functions
    ~CM2SequenceLoad();
};

#endif
