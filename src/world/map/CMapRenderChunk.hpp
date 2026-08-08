#ifndef WORLD_MAP_C_MAP_RENDER_CHUNK_HPP
#define WORLD_MAP_C_MAP_RENDER_CHUNK_HPP

#include "async/CAsyncObject.hpp"
#include "util/SFile.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/CMapArea.hpp"
#include "world/map/Types.hpp"
#include <gx/Texture.hpp>
#include <gx/Buffer.hpp>
#include <gx/CGxBatch.hpp>
#include <gx/Shader.hpp>

class CMapChunk;
class CMapRenderChunk;
struct CMapRenderChunkBufBlock;

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
    HTEXTURE layerTexture;
    CMapRenderChunk* owner;
};

struct TextureLayerInfo {
    uint32_t flags;
    uint8_t* alphaData;
};

typedef void(RENDER_LAYER_FUNC)(CMapRenderChunk*);

class CMapRenderChunk {
    public:
    TSLink<CMapRenderChunk> renderChunkLink;
    uint8_t unkFlags;
    uint8_t layersCount;
    int16_t unk_0A;
    float lastUpdateTime;
    CMapChunk* mapChunkPtrs[2];
    C3Vector vec1;
    C3Vector vec2;
    float radius;
    CMapRenderChunkLayer layers[4];
    HTEXTURE terrainBlendTexture;
    HTEXTURE shadowTexture;
    CMapRenderChunkBuf* chunkBuf;
    CGxBatch batch;

    static STORM_EXPLICIT_LIST(CMapRenderChunkBufBlock, bufLink) s_bufList;
    static STORM_EXPLICIT_LIST(CMapRenderChunkBufBlock, blockLink) s_chunkBufBlockFreeList;
    static STORM_EXPLICIT_LIST(CMapRenderChunkBuf, unk_14) s_renderChunkBufFreeList;
    static TSGrowableArray<CMapRenderChunkBufBlock> s_chunkBlockArray;
    static bool s_bPoolsDirty;
    static CGxPool* s_gxVertexPool;
    static CGxPool* s_gxIndexPool;
    static CGxShader* s_currentShaderX[4];
    static EGxVertexBufferFormat s_gxBufVertexFormat;
    static int16_t s_maxVertexCount;
    static int16_t s_maxVertexOffset;
    static int32_t s_pnEstimateVertex;
    static int32_t s_pnEstimateIndex;
    static RENDER_LAYER_FUNC* s_renderLayersFunc;
    static uint16_t s_defaultTex[64 * 64];
    static uint8_t s_defaultShadowRow[64];
    static uint8_t s_defaultAlphaRow[64];

    void AddBatch(CMapChunk* a2, CMapChunk* a3, C3Vector* a4, uint8_t a5);
    void RenderPrep();
    void CreateLayers();
    void CreateLayer(CMapArea* area, SMLayer* layer, bool a4);
    void AllocLayerTextures();
    void AllocShaderTexture();
    void AllocLayerTexture(CMapRenderChunkLayer* layer);
    void AllocShadowTexture();
    void UpdateLoaded();
    void UseStreamingBufs();
    void RenderPrepBufs(CGxBuf* vertexBuf, CGxBuf* indexBuf);
    void RenderSetup(int32_t a2);
    void RenderSolid();
    void RenderSolidVertexPixelShader();
    void SetVertexShader(int32_t a1, int32_t a2);
    void FreeBuf();
    void CreateChunkLayerTex(CMapRenderChunkLayer* layer);
    void CreateShaderTexture();

    void UnpackAlphaShadowBits(uint16_t* outputTexture, int32_t dstOffset, int32_t dstPitch, uint32_t size, TextureLayerInfo* layerInfo, uint32_t alphaSlot, uint8_t* shadowMap, int32_t genFormat, bool doNotFixAlphaMap);
    void UnpackAlphaShadowBitsUnfixed4444Mip1(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo);
    void UnpackAlphaShadowBitsUnfixed4444Mip0(uint16_t* dstBase, int32_t dstOffset, int32_t dstPitch, uint32_t size, TextureLayerInfo* cursors, const uint8_t* shadowMap);
    void UnpackAlphaShadowBitsFixed8888Mip1(uint16_t* outputTexture, uint32_t texSize, uint8_t* shadowMap);
    void UnpackAlphaShadowBitsFixed8888Mip0(uint16_t* outputTexture, uint32_t texSize, uint8_t* shadowMap);
    void UnpackAlphaShadowBitsFixed4444Mip1(uint16_t* dstBase, int32_t dstOffset, int32_t dstPitch, uint32_t size, TextureLayerInfo* cursors, const uint8_t* shadowMap);
    void UnpackAlphaShadowBitsFixed4444Mip0(uint16_t* dstBase, int32_t dstOffset, int32_t dstPitch, uint32_t size, TextureLayerInfo* cursors, const uint8_t* shadowMap);

    void UnpackAlphaBits(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo, uint8_t* shadowMap, int32_t layerMode, bool bigAlpha);
    void UnpackAlphaBitsUnfixed4444Mip1(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo);
    void UnpackAlphaBitsUnfixed4444Mip0(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo);
    void UnpackAlphaBitsFixed8888Mip1(uint16_t* outputTexture, uint32_t texSize, uint8_t* shadowMap);
    void UnpackAlphaBitsFixed8888Mip0(uint16_t* outputTexture, uint32_t texSize, uint8_t* shadowMap);
    void RecreateAlphaBitsFixed8888Mip1(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo, uint8_t* shadowMap);
    void RecreateAlphaBitsFixed8888Mip0(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo, uint8_t* shadowMap);
    void UnpackAlphaBitsFixed4444Mip1(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo);
    void UnpackAlphaBitsFixed4444Mip0(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo);

    static void Initialize();
    static CMapRenderChunkBuf* AllocBuf(int32_t a1, CMapRenderChunk* renderChunk);
    static HTEXTURE AllocTexture(int32_t a1, int32_t a2, void* userArg, TEXTURE_CALLBACK* a4, EGxTexFormat a5, int16_t a6);
    static void UpdateShaderGxTexture(EGxTexCommand cmd, uint32_t w, uint32_t h, uint32_t d, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void UpdateLayerGxTexture(EGxTexCommand cmd, uint32_t w, uint32_t h, uint32_t d, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void UpdateShadowGxTexture(EGxTexCommand cmd, uint32_t w, uint32_t h, uint32_t d, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void UpdatePools();
    static void RenderMultiPassAlpha(CMapRenderChunk* renderChunk);
    static void RenderMultiPassAdditive(CMapRenderChunk* renderChunk);
    static void SetShaders(int32_t a1, int32_t a2);
};

#endif
