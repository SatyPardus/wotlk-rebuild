#ifndef WORLD_MAP_C_MAP_CHUNK_HPP
#define WORLD_MAP_C_MAP_CHUNK_HPP

#include "async/CAsyncObject.hpp"
#include "util/SFile.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/CMapRenderChunk.hpp"
#include "world/map/CMapArea.hpp"
#include "world/map/Types.hpp"
#include <gx/buffer/Types.hpp>

class CMapChunk : public CMapBaseObj {
    public:
    C2iVector aIndex;
    C2iVector sOffset;
    C2iVector cOffset;
    CAaSphere sphere;
    CAaBox bbox;
    C3Vector bottomRight;
    C3Vector topLeft;
    C3Vector topLeftCoords;
    float distToCamera;
    CAaBox bbox2;
    void* detailDoodadInst;
    CMapRenderChunk* renderChunk;
    int32_t bLoaded;
    int32_t areaId;
    int32_t unk_B4;
    int32_t unk_B8;
    TSLink<CMapChunk> sortListLink;
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
    float* height;
    uint32_t* vertexShading;
    int8_t* normals;
    uint8_t* shadowMap;
    SMLayer* layers;
    uint8_t* additionalShadowmap;
    uint32_t* MCRF_ptr;
    SMLiquidChunk* liquid;
    CWSoundEmitter* soundEmitters;
    int32_t unk_140;
    int32_t unk_144;
    int32_t unk_148;
    int32_t unk_14C;
    int32_t unk_150;
    int32_t unk_154;

    static C3Vector vertexList[145];
    static int32_t cornerVertexIndex[4];
    static int32_t farCornerIndex;
    static float s_geoToTex;

    void Create(SIffChunk* headerChunk, bool a3);
    void ProcessIffChunks(bool a3);
    void CreateBounds();
    void RenderPrep();
    void Batch();
    void CreateIndices(char* buf, CGxBatch* batch);
    int16_t CreateIndices(char* buf, int32_t offset);
    void CreateVertices(char* buf, int32_t bufOffset);
    void CreateVerticesLocal(char* buf);

    CMapChunk();
    static void Initialize();
    static void InitializeVertexGrid();
};

#endif
