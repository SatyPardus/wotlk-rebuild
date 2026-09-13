#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include <cstdint>
#include "world/CWorld.hpp"
#include <tempest/facet/CFacet.hpp>
#include <storm/Array.hpp>
#include "clientobject/WGUID.hpp"

#define NDCCLIP_MAX 32

class CMapObjDef;
class CFrustum;
class CMapEntity;
class CM2Model;

extern uint32_t s_newZoneID;
extern C3Vector s_newPosition;
extern float s_newFacing;
extern const char* s_newMapname;

int32_t LoadNewWorld(const void* eventData);

namespace World {


    struct CLIPINFO {
        float d[6];
        uint32_t outcode;
        uint32_t pad;

        void Set(const C3Vector& ndc);
    };

    struct CLIPPOLY {
        C3Vector** verts;
        CLIPINFO** infos;
        uint32_t count;
    };

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

    class FacetData {
        public:
        TSGrowableArray<CFacet> facets;
        TSGrowableArray<uint64_t> facetIds;
    };

    bool IsValidPosition(float x, float y, float z, float a4);

    bool NDCXform(CFrustum* frustum, C44Matrix* out, bool includeTranslation);
    bool NDCClip(C3Vector* verts, uint32_t count, C3Vector*** outVerts, uint32_t* outCount);
    bool GetFacets(CFrustum* frustum, FacetData* facets, uint32_t flags, uint32_t* a4);
    bool GetFacets(CAaBox* a1, CAaBox* a2, FacetData* a3, uint32_t a4, uint32_t* a5);
    void AddAaBoxFacets(CAaBox* box, FacetData* facets);
    uint32_t TriDataToFacetData(void* unused, FacetData* facets, WGUID guid);
    int32_t GetFlightBoundsLower(const C3Vector& pos, float* height);
    bool Intersect(C3Vector* start, C3Vector* end, C3Vector* hitPoint, float* distance, uint32_t flags, void* hitInfo);
    void ObjectSetModel(CMapEntity* entity, CM2Model* model);

}

#endif
