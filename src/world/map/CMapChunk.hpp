#ifndef WORLD_MAP_C_MAP_CHUNK_HPP
#define WORLD_MAP_C_MAP_CHUNK_HPP

#include "async/CAsyncObject.hpp"
#include "util/SFile.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/CMapRenderChunk.hpp"
#include "world/map/CMapArea.hpp"
#include "world/map/Types.hpp"

class CMapChunk : public CMapBaseObj {
    public:
    C2iVector aIndex;
    C2iVector sOffset;
    C2iVector cOffset;
    C3Vector center;
    float radius;
    CAaBox bbox;
    C3Vector bottomRight;
    C3Vector topLeft;
    C3Vector topLeftCoords;
    float distToCamera;
    CAaBox bbox2;
    void* detailDoodadInst;
    CMapRenderChunk* renderChunk;
    int32_t unk_AC;
    int32_t areaId;
    int32_t unk_B4;
    int32_t unk_B8;
    int32_t unk_BC;
    int32_t unk_C0;
    STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) doodadDefLinkList;
    STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) mapObjDefLinkList;
    int32_t TSExplicitList__m_linkoffset_DC;
    void* TSExplicitList__ptr_E0;
    void* TSExplicitList__ptr2_E4;
    int32_t TSExplicitList__m_linkoffset_E8;
    void* TSExplicitList__ptr_EC;
    void* TSExplicitList__ptr2_F0;
    int32_t TSExplicitList__m_linkoffset_F4;
    void* TSExplicitList__ptr_F8;
    void* TSExplicitList__ptr2_FC;
    STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) liquidChunkLinkList;
    SIffChunk* chunkHeaderPtr;
    SMChunk* header;
    uint8_t* lowQualityTexMap;
    uint8_t* predTexture;
    float* vertices;
    uint32_t* vertexShading;
    int8_t* normals;
    uint8_t* shadowMap;
    SMLayer* layers;
    uint8_t* additionalShadowmap;
    uint8_t* MCRF_ptr;
    SMLiquidChunk* liquid;
    CWSoundEmitter* soundEmitters;
    int32_t unk_140;
    int32_t unk_144;
    int32_t unk_148;
    int32_t unk_14C;
    int32_t unk_150;
    int32_t unk_154;

    void Create(SIffChunk* headerChunk, bool a3);
    void ProcessIffChunks(bool a3);
};

#endif
