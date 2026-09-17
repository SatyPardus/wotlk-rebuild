#include "model/CM2SequenceLoad.hpp"
#include <async/AsyncFile.hpp>

// OFFSET: 0x83D370
CM2SequenceLoad::~CM2SequenceLoad() {
    this->m_playbacks.Clear();

    if (this->m_asyncObject) {
        AsyncFileReadDestroyObject(this->m_asyncObject);
    }

    this->m_playbacks.UnlinkAll();
}
