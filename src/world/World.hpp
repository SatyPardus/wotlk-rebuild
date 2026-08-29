#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include <cstdint>
#include "world/CWorld.hpp"

class CMapObjDef;

extern uint32_t s_newZoneID;
extern C3Vector s_newPosition;
extern float s_newFacing;
extern const char* s_newMapname;

int32_t LoadNewWorld(const void* eventData);

namespace World {

    bool IsValidPosition(float x, float y, float z, float a4);

    namespace TriData {
        struct Batch {
            C44Matrix* matrix;
            C3Vector* vertexList;
            C3Vector* normalList;
            uint32_t unk_08;
            uint16_t* indices;
            uint16_t* faceIndices;
            uint16_t indexCount;
            uint16_t faceCount;
            uint16_t minVertexIndex;
            uint16_t maxVertexIndex;
            CMapObjDef* def;
        };

        extern uint16_t faceIndexPool[0x4000];
        extern uint16_t indexPool[0xC000];
        extern uint32_t indexCursor;
        extern uint32_t faceIndexCursor;
        extern uint32_t statusFlags;
        extern uint32_t nBatches;
        extern Batch batches[32];

        Batch* AllocBatch(uint32_t indexCount, uint32_t faceCount);
    }
}

#endif
