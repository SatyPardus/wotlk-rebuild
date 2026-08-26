#include "world/daynight/LightQueue.hpp"
#include <bc/memory/Storm.hpp>

namespace DayNight {

// OFFSET: 0x7F0D40
LightQueue::LightQueue(uint32_t initial, uint32_t chunk) {
    this->m_vtable = nullptr;
    this->m_alloc = (void*)-1;
    this->m_allocBytes = 0;
    this->m_data = nullptr;
    this->m_dataBytes = 0;

    this->Resize(8 * initial, false);

    this->m_chunk = chunk;
    this->m_capacity = initial;
    this->m_count = 0;

    if (initial >= 1) {
        ++this->m_count;
        return;
    }

    if (chunk) {
        uint32_t grow = chunk;

        if (grow <= 1 - initial) {
            grow = 1 - initial;
        }

        if (this->Resize(8 * (grow + initial), true)) {
            this->m_capacity += grow;
            ++this->m_count;
        }
    }
}

// OFFSET: 0x984670  (CMemBlock::Free)
LightQueue::~LightQueue() {
    if (this->m_alloc) {
        if (this->m_alloc != (void*)-1) {
            SMemFree(this->m_alloc, ".\\cmemblock.cpp", 364, 0);
        }

        this->m_dataBytes = 0;
        this->m_allocBytes = 0;
        this->m_data = nullptr;
        this->m_alloc = nullptr;
    }
}

// OFFSET: 0x9847D0  (CMemBlock::Resize)
bool LightQueue::Resize(uint32_t bytes, bool noClear) {
    if (bytes != this->m_dataBytes) {
        const uint32_t lead = this->m_allocBytes - this->m_dataBytes;
        const uint32_t total = lead + bytes;

        void* raw = this->m_alloc == (void*)-1
                        ? SMemAlloc(total, ".\\cmemblock.cpp", 364, 0)
                        : SMemReAlloc(this->m_alloc, total, ".\\cmemblock.cpp", 364, 0);

        this->m_alloc = raw;
        this->m_data = (LightQE*)((char*)raw + lead);

        if (bytes > this->m_dataBytes) {
            memset((char*)this->m_data + this->m_dataBytes, 0, bytes - this->m_dataBytes);
        }

        this->m_allocBytes = total;
        this->m_dataBytes = bytes;
    }

    if (!noClear) {
        memset(this->m_data, 0, this->m_dataBytes);
    }

    return true;
}

// OFFSET: 0x7F0DC0
void LightQueue::Insert(float key, LightRec** light) {
    if (this->m_count + 1 > this->m_capacity) {
        if (this->m_chunk) {
            uint32_t grow = this->m_chunk;
            const uint32_t needed = this->m_count - this->m_capacity + 1;

            if (grow <= needed) {
                grow = needed;
            }

            if (this->Resize(8 * (grow + this->m_capacity), true)) {
                this->m_capacity += grow;
                ++this->m_count;
            }
        }
    } else {
        ++this->m_count;
    }

    LightQE entry;
    entry.m_key = key;
    entry.m_light = light;

    uint32_t index = this->m_count - 1;

    if (this->m_count != 1 && index != 1) {
        uint32_t parent;

        do {
            parent = index >> 1;

            if (!LightQE::HasHigherPriority(entry, this->m_data[parent])) {
                break;
            }

            this->m_data[index] = this->m_data[parent];
            index >>= 1;
        } while (parent > 1);
    }

    this->m_data[index] = entry;
}

// OFFSET: 0x7A0C70
bool LightQueue::Erase(uint32_t index, uint32_t count) {
    if (index >= this->m_count) {
        return false;
    }

    uint32_t n = count;

    if (index + count > this->m_count) {
        n = this->m_count - index;
    }

    const uint32_t tail = this->m_count - index - n;

    if (tail) {
        memmove(&this->m_data[index], &this->m_data[index + n], 8 * tail);
    }

    this->m_count -= n;

    return true;
}

// OFFSET: 0x7F1280
void LightQueue::Pop(LightQE* out) {
    *out = this->m_data[1];

    const uint32_t count = this->m_count;
    const LightQE last = this->m_data[count - 1];

    if (count) {
        this->Erase(count - 1, 1);
    }

    if (this->m_count >= 2) {
        const uint32_t limit = this->m_count - 1;
        const uint32_t half = limit >> 1;

        uint32_t index = 1;

        if (half) {
            while (1) {
                uint32_t child = 2 * index;

                if (child < limit && LightQE::HasHigherPriority(this->m_data[child + 1], this->m_data[child])) {
                    ++child;
                }

                if (LightQE::HasHigherPriority(last, this->m_data[child])) {
                    break;
                }

                const bool more = child <= half;
                this->m_data[index] = this->m_data[child];
                index = child;

                if (!more) {
                    break;
                }
            }
        }

        this->m_data[index] = last;
    }
}

} // namespace DayNight
