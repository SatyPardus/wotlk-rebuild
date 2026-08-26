#ifndef WORLD_DAY_NIGHT_LIGHT_QUEUE_HPP
#define WORLD_DAY_NIGHT_LIGHT_QUEUE_HPP

#include <cstdint>
#include "world/daynight/LightQE.hpp"

class LightRec;

namespace DayNight {

    class LightQueue {
        public:
        void* m_vtable;
        void* m_alloc;
        uint32_t m_allocBytes;
        LightQE* m_data;
        uint32_t m_dataBytes;
        uint32_t m_chunk;
        uint32_t m_capacity;
        uint32_t m_count;

        LightQueue(uint32_t initial, uint32_t chunk);
        ~LightQueue();

        bool Resize(uint32_t bytes, bool noClear);
        void Insert(float key, LightRec** light);
        bool Erase(uint32_t index, uint32_t count);
        void Pop(LightQE* out);
    };

} // namespace DayNight

#endif
