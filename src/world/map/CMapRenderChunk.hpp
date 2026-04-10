#ifndef WORLD_MAP_C_MAP_RENDER_CHUNK_HPP
#define WORLD_MAP_C_MAP_RENDER_CHUNK_HPP

#include "async/CAsyncObject.hpp"
#include "util/SFile.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/CMapArea.hpp"
#include "world/map/Types.hpp"
#include <gx/Texture.hpp>
#include <gx/Buffer.hpp>

class CMapChunk;
class CMapRenderChunk;
struct CMapRenderChunkBufBlock;

struct CMapRenderChunkState {
    int32_t unk_00;
    int32_t unk_04;
    int32_t indexCount;
    int16_t minVertexIndex;
    int16_t maxVertexIndex;
};

struct CMapRenderChunkBuf {
    CGxBuf* vertexBuf;
    CGxBuf* indexBuf;
    CMapRenderChunk* renderChunk;
    CMapRenderChunkBufBlock* block;
    uint32_t unkFlags;
    TSLink<CMapRenderChunkBuf> unk_14;
};

struct CMapRenderChunkBufBlock {
    uint32_t vertexCount;
    CMapRenderChunkBuf bufs[2];
    TSLink<CMapRenderChunkBufBlock> blockLink;
    TSLink<CMapRenderChunkBufBlock> bufLink;
};

struct CMapRenderChunkLayer {
    int16_t flags;
    int16_t layerIndex;
    HTEXTURE texture;
    int32_t textureId;
    int32_t unkValue;
    CMapRenderChunk* owner;
};

class CMapRenderChunk {
    public:
    TSLink<CMapRenderChunk> renderChunkLink;
    uint8_t unkFlags;
    uint8_t layersCount;
    int16_t unk_0A;
    float unk_0C;
    CMapChunk* mapChunkPtrs[2];
    C3Vector vec1;
    C3Vector vec2;
    float radius;
    CMapRenderChunkLayer layers[4];
    CTexture* terrainBlendTexture;
    CTexture* shadowTexture;
    CMapRenderChunkBuf* chunkBuf;
    CMapRenderChunkState state;

    static STORM_EXPLICIT_LIST(CMapRenderChunkBufBlock, blockLink) s_bufList;
    static STORM_EXPLICIT_LIST(CMapRenderChunkBufBlock, bufLink) s_chunkBufBlockFreeList;
    static STORM_EXPLICIT_LIST(CMapRenderChunkBuf, unk_14) s_renderChunkBufFreeList;
    static TSGrowableArray<CMapRenderChunkBufBlock> s_chunkBlockArray;
    static bool s_bPoolsDirty;
    static CGxPool* s_gxVertexPool;
    static CGxPool* s_gxIndexPool;
    static EGxVertexBufferFormat s_gxBufVertexFormat;
    static int16_t s_maxVertexCount;
    static int16_t s_maxVertexOffset;
    static int32_t s_pnEstimateVertex;
    static int32_t s_pnEstimateIndex;

    void Initialize();
    void UpdatePools();
    void RenderPrep();
    void CreateLayers();
    void CreateLayer(CMapArea* area, SMLayer* layer, bool a4);
    void UpdateLoaded();
    void UseStreamingBufs();
    void RenderPrepBufs(CGxBuf* vertexBuf, CGxBuf* indexBuf);

    static CMapRenderChunkBuf* AllocBuf(int32_t a1, CMapRenderChunk* renderChunk);
};

#endif
