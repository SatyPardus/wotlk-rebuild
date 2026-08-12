#include "world/map/VBBList_Block.hpp"

// OFFSET: 0x7C8500
void VBBList_Block::Set(uint32_t itemSize, uint32_t itemCount, uint32_t offset) {
    this->capacity = itemCount * itemSize;
    this->offset = offset;
    auto v5 = offset;
    if (offset % itemSize)
        v5 = offset + itemSize - offset % itemSize;
    this->buffer->m_index = v5;
    g_theGxDevicePtr->BufSizeSet(this->buffer, itemSize, itemCount - 1);
    this->buffer->unk1C = 0;
}
