#include "world/map/CMapRenderChunk.hpp"
#include "world/map/CMapChunk.hpp"
#include "world/map/CMap.hpp"
#include "gx/Device.hpp"
#include <world/CWorld.hpp>
#include "world/CWorldScene.hpp"
#include "gx/RenderState.hpp"
#include "gx/Draw.hpp"
#include "gx/Transform.hpp"
#include <tempest/matrix/C44Matrix.hpp>

STORM_EXPLICIT_LIST(CMapRenderChunkBufBlock, bufLink) CMapRenderChunk::s_bufList;
STORM_EXPLICIT_LIST(CMapRenderChunkBufBlock, blockLink) CMapRenderChunk::s_chunkBufBlockFreeList;
STORM_EXPLICIT_LIST(CMapRenderChunkBuf, unk_14) CMapRenderChunk::s_renderChunkBufFreeList;
TSGrowableArray<CMapRenderChunkBufBlock> CMapRenderChunk::s_chunkBlockArray;
bool CMapRenderChunk::s_bPoolsDirty;
CGxPool* CMapRenderChunk::s_gxVertexPool;
CGxPool* CMapRenderChunk::s_gxIndexPool;
CGxShader* CMapRenderChunk::s_currentShaderX[4];
EGxVertexBufferFormat CMapRenderChunk::s_gxBufVertexFormat;
int16_t CMapRenderChunk::s_maxVertexCount = 145;
int16_t CMapRenderChunk::s_maxVertexOffset;
int32_t CMapRenderChunk::s_pnEstimateVertex;
int32_t CMapRenderChunk::s_pnEstimateIndex;
uint16_t CMapRenderChunk::s_defaultTex[64 * 64];
uint8_t CMapRenderChunk::s_defaultShadowRow[64];
uint8_t CMapRenderChunk::s_defaultAlphaRow[64];

const uint32_t s_nibbleMask[2] = { 0x0F, 0xF0 };
const uint32_t s_nibbleShift[2] = { 4, 0 };
const uint32_t s_nibbleShiftReverse[2] = { 0, 4 };
const uint32_t s_bitMask[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
const uint32_t s_bitShift[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
const uint8_t s_shadowLut[2] = { 0xFF, 0x00 };


RENDER_LAYER_FUNC* CMapRenderChunk::s_renderLayersFunc;

// OFFSET: 0x7BA340
void CMapRenderChunk::Initialize() {
    CMapRenderChunk::s_bPoolsDirty = 1;
    CMapRenderChunk::s_gxBufVertexFormat = GxVBF_PN;
    CMapRenderChunk::s_maxVertexOffset = CMapRenderChunk::s_maxVertexCount - 1;
    CMapRenderChunk::s_pnEstimateVertex = 0;
    CMapRenderChunk::s_pnEstimateIndex = 0;
    //dword_AEEC60 = 3;
    //dword_AEEC68 = (unsigned __int16)word_AEEC18;
    //dword_AEEC64 = 0;
    //word_AEEC6C = 0;
    CMapRenderChunk::s_gxVertexPool = nullptr;
    CMapRenderChunk::s_gxIndexPool = nullptr;
    CMapRenderChunk::s_chunkBlockArray.SetCount(0);
    memset(&CMapRenderChunk::s_defaultAlphaRow, 0, sizeof(CMapRenderChunk::s_defaultAlphaRow));
    memset(&CMapRenderChunk::s_defaultShadowRow, 0, sizeof(CMapRenderChunk::s_defaultShadowRow));
}

// OFFSET: 0x7B7AF0
void CMapRenderChunk::AddBatch(CMapChunk* a2, CMapChunk* a3, C3Vector* a4, uint8_t a5) {
    this->mapChunkPtrs[0] = a2;
    this->mapChunkPtrs[1] = a3;
    this->vec1 = *a4;
    this->unkFlags = a5;
    //v15.min.x = a2->bbox.min.x;
    //v15.min.y = a2->bbox.min.y;
    //v15.min.z = a2->bbox.min.z;
    //v15.max.x = a2->bbox.max.x;
    //z = a2->bbox.max.z;
    //v15.max.y = a2->bbox.max.y;
    //v15.max.z = z;
    //if (a3)
    //    sub_715130(&v15.min.x, v14, &a3->bbox.min.x);
    //x = v15.max.x;
    //v8 = v15.min.x;
    //y = v15.max.y;
    //v10 = v15.min.y;
    //this->radius = sqrt(
    //                   (v15.max.x - v15.min.x) * (v15.max.x - v15.min.x) + (v15.max.y - v15.min.y) * (v15.max.y - v15.min.y) + (v15.max.z - v15.min.z) * (v15.max.z - v15.min.z)) *
    //               0.5;
    //v11 = v10 + y;
    //v12 = x + v8;
    //v13 = v15.max.z + v15.min.z;
    //v15.max.x = v12 * 0.5;
    //this->vec2.x = v15.max.x;
    //v15.max.y = v11 * 0.5;
    //this->vec2.y = v15.max.y;
    //v15.max.z = v13 * 0.5;
    //this->vec2.z = v15.max.z;
}

// OFFSET: 0x7BA600
void CMapRenderChunk::UpdatePools() {
    if (!CMapRenderChunk::s_bPoolsDirty)
        return;

    //CMap::ClearAllRenderChunks();
    //sub_7BA5A0();
    const float chunkRadius = 1.0f - (int)(CWorld::s_farClip * -0.03f);
    const uint32_t chunkCount = (2 * (uint32_t)(chunkRadius * chunkRadius) + 1) & ~1u;
    CMapRenderChunk::s_pnEstimateVertex = 145 * chunkCount;
    CMapRenderChunk::s_pnEstimateIndex = (768 * 2) * chunkCount;

    const uint32_t vertexStride = (s_gxBufVertexFormat == GxVBF_PNC) ? 28 : 24;

    CMapRenderChunk::s_gxVertexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, GxPoolUsage_Dynamic, CMapRenderChunk::s_pnEstimateVertex * vertexStride, GxPoolHintBit_Unk3, "CMapRenderChunk_vtx");
    CMapRenderChunk::s_gxIndexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Dynamic, CMapRenderChunk::s_pnEstimateIndex * vertexStride, GxPoolHintBit_Unk3, "CMapRenderChunk_idx");

    CMapRenderChunk::s_chunkBlockArray.SetCount(chunkCount >> 1);
    for (uint32_t i = 0; i < CMapRenderChunk::s_chunkBlockArray.m_count; i++) {
        CMapRenderChunkBufBlock* block = &s_chunkBlockArray.m_data[i];

        block->vertexCount = 2 * i;
        block->bufs[0].block = block;
        block->bufs[1].block = block;

        block->blockLink.Unlink();
        CMapRenderChunk::s_chunkBufBlockFreeList.LinkToTail(block);

        block->bufLink.Unlink();
        CMapRenderChunk::s_bufList.LinkToTail(block);
    }
    CMapRenderChunk::s_bPoolsDirty = 0;
}

// OFFSET: 0x7D3F70
void CMapRenderChunk::RenderPrep() {
    if (!this->chunkBuf)
        this->chunkBuf = CMapRenderChunk::AllocBuf(this->unkFlags & 3, this);

    if (this->chunkBuf) {
        CMapRenderChunk::s_bufList.LinkToTail(this->chunkBuf->block);
        this->RenderPrepBufs(this->chunkBuf->vertexBuf, this->chunkBuf->indexBuf);
    }

    if (!this->layersCount)
        this->CreateLayers();
    if ((this->unkFlags & 8) == 0)
        this->UpdateLoaded();
}

// OFFSET: 0x7B9770
void CMapRenderChunk::CreateLayers() {
    const int32_t areaCount = (CMap::header.flags & 4) ? 2 : 1;

    for (int32_t i = 0; i < areaCount; i++) {
        CMapChunk* chunk = this->mapChunkPtrs[i];
        if (!chunk)
            continue;

        CMapBaseObjLink* link = chunk->parentLinkList.Head();
        if (!link || ((uintptr_t)link & 1))
            continue;

        CMapArea* area = (CMapArea*)link->ref;
        bool a4 = (i != 0);

        for (int32_t j = 0; j < chunk->header->nLayers; j++)
            CreateLayer(area, &chunk->layers[j], a4);
    }

    this->unk_0A |= 0x20u;
}

// OFFSET: 0x7B9250
void CMapRenderChunk::CreateLayer(CMapArea* area, SMLayer* layer, bool a4) {
    if (a4 && this->layersCount) {
        for (int32_t i = 0; i < this->layersCount; i++) {
            if (this->layers[i].textureId == layer->textureId)
                return;
        }
    }

    CMapRenderChunkLayer* newLayer = &this->layers[this->layersCount];
    newLayer->flags = layer->flags;
    newLayer->layerIndex = this->layersCount;

    CMapAreaTexture* areaTexture = &area->textures.m_data[layer->textureId];
    if (!areaTexture->texture)
        CMap::LoadTerrainTexture(area, areaTexture, layer->textureId);

    newLayer->texture = areaTexture->texture;
    newLayer->textureId = layer->textureId;
    newLayer->layerTexture = nullptr;
    newLayer->owner = this;

    if ((newLayer->flags & 0x80) != 0)
        this->unk_0A |= 1u;

    if (g_theGxDevicePtr->Caps().m_texTarget[1] && g_theGxDevicePtr->Caps().m_shaderTargets[0] && g_theGxDevicePtr->Caps().m_shaderTargets[4] && (newLayer->flags & 0x400)) {
        this->unk_0A |= 4u;
    }

    ++this->layersCount;
}

// OFFSET: 0x7BA050
void CMapRenderChunk::AllocLayerTextures() {
    if (!this->layersCount) {
        this->unk_0A &= 0xFFCF;
        return;
    }

    if ((this->unk_0A & 0x20) == 0) {
        if ((this->unk_0A & 0x10) != 0) {
            if ((this->unk_0A & 0x8) != 0) {
                this->unk_0A &= 0xFFCF;
                return;
            }
        } else if ((this->unk_0A & 0x8) == 0) {
            this->unk_0A &= 0xFFCF;
            return;
        }
    }

    this->unk_0A &= 0xFFF7;
    if ((this->unk_0A & 0x10) != 0)
        this->unk_0A |= 0x8;

    if (CMap::gTerrainPixelShadersValid || CMap::enableSpecularTerrain) {
        this->AllocShaderTexture();
    } else {
        for (int32_t i = 0; i < this->layersCount; i++) {
            this->AllocLayerTexture(&this->layers[i]);
        }

        if ((CMap::header.flags & 0x4) == 0) {
            //this->AllocShadowTexture(); // TODO implement shadow gx texture callback
        }
    }

    this->unk_0A &= 0xFFCF;
}

// OFFSET: 0x7B9F90
void CMapRenderChunk::AllocShaderTexture() {
    if (this->terrainBlendTexture)
        HandleClose(this->terrainBlendTexture);
    this->terrainBlendTexture = nullptr;

    CMapBaseObjLink* link = this->mapChunkPtrs[0]->parentLinkList.Head();
    CMapArea* area = (CMapArea*)link->ref;
    int32_t v4 = 64 >> area->header->mamp_value;

    if ((this->unk_0A & 0x8) != 0) {
        v4 /= 2;
    }

    int32_t v5 = v4;
    if ((this->unkFlags & 0x1) != 0) {
        v5 *= 2;
    } else if ((this->unkFlags & 0x2) != 0) {
        v4 *= 2;
    }

    HTEXTURE v7;
    if ((CMap::header.flags & 4) != 0)
        v7 = CMapRenderChunk::AllocTexture(v4, v5, this, CMapRenderChunk::UpdateShaderGxTexture, GxTex_Argb8888, 2u);
    else
        v7 = CMapRenderChunk::AllocTexture(v4, v5, this, CMapRenderChunk::UpdateShaderGxTexture, GxTex_Argb4444, 3u);
    this->terrainBlendTexture = v7;
    CGxTex* tex = TextureGetGxTex(v7, 1, nullptr);
    GxTexUpdate(tex, 0, 0, v4, v5, 1);
}

// OFFSET: 0x7B9DE0
void CMapRenderChunk::AllocLayerTexture(CMapRenderChunkLayer* layer) {
    if (layer->layerTexture)
        HandleClose(layer->layerTexture);
    layer->layerTexture = nullptr;

    bool v4 = (CMap::header.flags >> 2) & 1;
    bool v5 = v4 && !layer->layerIndex;
    if ((layer->flags & 0x100) != 0 || v5) {
        CMapBaseObjLink* link = this->mapChunkPtrs[0]->parentLinkList.Head();
        CMapArea* area = (CMapArea*)link->ref;
        int32_t v10 = 64 >> area->header->mamp_value;

        if ((this->unk_0A & 0x8) != 0) {
            v10 /= 2;
        }

        if (v4)
            layer->layerTexture = CMapRenderChunk::AllocTexture(v10, v10, layer, CMapRenderChunk::UpdateLayerGxTexture, GxTex_Argb8888, 2u);
        else
            layer->layerTexture = CMapRenderChunk::AllocTexture(v10, v10, layer, CMapRenderChunk::UpdateLayerGxTexture, GxTex_Argb4444, 3u);
        CGxTex* tex = TextureGetGxTex(layer->layerTexture, 1, nullptr);
        GxTexUpdate(tex, 0, 0, v10, v10, 1);
    }
}

// OFFSET: 0x7B9EE0
void CMapRenderChunk::AllocShadowTexture() {
    if (this->shadowTexture)
        HandleClose(this->shadowTexture);
    this->shadowTexture = nullptr;

    if ((CWorld::s_enables & CWorld::Enables::Enable_Shadow) != 0) {
        if ((this->mapChunkPtrs[0]->header->flags & 1) != 0) {
            CMapBaseObjLink* link = this->mapChunkPtrs[0]->parentLinkList.Head();
            CMapArea* area = (CMapArea*)link->ref;
            int32_t v4 = 64 >> area->header->mamp_value;

            if ((this->unk_0A & 0x8) != 0) {
                v4 /= 2;
            }

            HTEXTURE v6 = CMapRenderChunk::AllocTexture(v4, v4, this, CMapRenderChunk::UpdateShadowGxTexture, GxTex_Argb4444, 3u);
            this->shadowTexture = v6;
            CGxTex* tex = TextureGetGxTex(v6, 1, nullptr);
            GxTexUpdate(tex, 0, 0, v4, v4, 1);
        }
    }
}

// OFFSET: 0x7B73E0
void CMapRenderChunk::UpdateLoaded() {
    if (!this->layersCount) {
        this->unkFlags |= 8;
        return;
    }

    for (int32_t i = 0; i < this->layersCount; i++) {
        if (!TextureGetGxTex(this->layers[i].texture, 0, nullptr))
            return;
    }

    this->unkFlags |= 8;
}

// OFFSET: 0x7D0420
void CMapRenderChunk::UseStreamingBufs() {
    int32_t v2 = 24;
    if (CMapRenderChunk::s_gxBufVertexFormat == GxVBF_PNC)
        v2 = 28;
    int32_t v3 = 145;
    int32_t v4 = 768;
    if ((this->unkFlags & 3) != 0) {
        v3 = 2 * 145;
        v4 = 2 * 768;
    }
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, v2, v3);
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, v4);
    this->RenderPrepBufs(vertexBuf, indexBuf);
    GxPrimVertexPtr(vertexBuf, CMapRenderChunk::s_gxBufVertexFormat);
    GxPrimIndexPtr(indexBuf);
}

// OFFSET: 0x7D02C0
void CMapRenderChunk::RenderPrepBufs(CGxBuf* vertexBuf, CGxBuf* indexBuf) {
    if (!vertexBuf->unk1C || !vertexBuf->unk1D) {
        char* bufData = g_theGxDevicePtr->BufLock(vertexBuf);

        if (this->mapChunkPtrs[0]) {
            this->mapChunkPtrs[0]->CreateVertices(bufData, 0);
        }

        if (this->mapChunkPtrs[1]) {
            this->mapChunkPtrs[1]->CreateVertices(bufData, 145);
        }

        GxBufUnlock(vertexBuf, 0);
    }

    if (!indexBuf->unk1C || !indexBuf->unk1D) {
        this->batch.m_primType = GxPrim_Triangles;
        this->batch.m_start = 0;
        this->batch.m_count = 0;
        this->batch.m_minIndex = -1;
        this->batch.m_maxIndex = 0;

        auto bufData = g_theGxDevicePtr->BufLock(indexBuf);

        if (this->mapChunkPtrs[0]) {
            this->mapChunkPtrs[0]->CreateIndices(bufData, &this->batch);
        }

        if (this->mapChunkPtrs[1]) {
            this->mapChunkPtrs[1]->CreateIndices(&bufData[this->batch.m_count], &this->batch);
        }

        GxBufUnlock(indexBuf, 0);
    }
}

// OFFSET: 0x7B9340
CMapRenderChunkBuf* CMapRenderChunk::AllocBuf(int32_t a1, CMapRenderChunk* renderChunk) {
    const uint32_t vertexStride = (s_gxBufVertexFormat == GxVBF_PNC) ? 28 : 24;

    if ((a1 & 3) != 0) {
        CMapRenderChunkBufBlock* block = CMapRenderChunk::s_chunkBufBlockFreeList.Head();
        if (block) {
            block->blockLink.Unlink();

            CMapRenderChunkBuf* buf = &block->bufs[0];
            buf->renderChunk = renderChunk;
            buf->unkFlags = a1;
            if (buf->vertexBuf) {
                buf->vertexBuf->unk1C = 0;
                buf->indexBuf->unk1C = 0;
            } else {
                buf->vertexBuf = g_theGxDevicePtr->BufCreate(s_gxVertexPool, vertexStride, 290u, 145 * vertexStride * block->vertexCount);
                buf->indexBuf = g_theGxDevicePtr->BufCreate(s_gxIndexPool, 2u, 1536u, 1536 * block->vertexCount);
            }
            return buf;
        }
    } else {
        CMapRenderChunkBuf* buf = CMapRenderChunk::s_renderChunkBufFreeList.Head();
        if (buf) {
            buf->unk_14.Unlink();
            buf->renderChunk = renderChunk;
            buf->unkFlags = a1;
            buf->vertexBuf->unk1C = 0;
            buf->indexBuf->unk1C = 0;
            return buf;
        } else {
            CMapRenderChunkBufBlock* block = CMapRenderChunk::s_chunkBufBlockFreeList.Head();
            if (block) {
                block->blockLink.Unlink();
                if (block->bufs[0].vertexBuf) {
                    //sub_6C42B0(block->bufs[0].vertexBuf);
                    //sub_6C42B0(block->bufs[0].indexBuf);
                    block->bufs[0].vertexBuf = nullptr;
                    block->bufs[0].indexBuf = nullptr;
                }

                for (int i = 0; i < 2; i++) {
                    CMapRenderChunkBuf* entry = &block->bufs[i];
                    uint32_t vertexCount = block->vertexCount + i;

                    entry->vertexBuf = g_theGxDevicePtr->BufCreate(s_gxVertexPool, vertexStride, 145, 145 * vertexStride * vertexCount);
                    entry->indexBuf = g_theGxDevicePtr->BufCreate(s_gxIndexPool, 2u, 768, 1536 * vertexCount);
                    entry->unkFlags = 0;
                    entry->renderChunk = nullptr;

                    if (i == 0)
                        CMapRenderChunk::s_renderChunkBufFreeList.LinkToTail(entry);
                }

                CMapRenderChunkBuf* result = &block->bufs[1];
                result->renderChunk = renderChunk;
                result->unkFlags = a1;
                return result;
            }
        }
    }
    return nullptr;
}

// OFFSET: 0x7B7A70
HTEXTURE CMapRenderChunk::AllocTexture(int32_t a1, int32_t a2, void* userArg, TEXTURE_CALLBACK* a4, EGxTexFormat a5, int16_t a6) {
    EGxTexFormat v6 = a5;
    if ((CMap::header.flags & 4) != 0)
        v6 = (CWorld::terrainAlphaBitDepth != 8) ? GxTex_Argb4444 : GxTex_Argb8888;

    CGxTexFlags texFlags = CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1);
    return TextureCreate(GxTex_2d, a1, a2, 0, v6, v6, texFlags, userArg, a4, "TerrainBlend", 0);
}

// OFFSET: 0x7B9C60
void CMapRenderChunk::UpdateShaderGxTexture(EGxTexCommand cmd, uint32_t w, uint32_t h, uint32_t d, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CMapRenderChunk* renderChunk = static_cast<CMapRenderChunk*>(userArg);

    if (cmd == GxTex_Latch) {
        renderChunk->CreateShaderTexture();
        texels = CMapRenderChunk::s_defaultTex;
        auto v8 = 4 * w;
        if ((CMap::header.flags & 4) == 0)
            v8 = 2 * w;
        texelStrideInBytes = v8;
    }
}

// OFFSET: 0x007B99B0 - Created via AI DOUBLE CHECK!!!
void CMapRenderChunk::CreateShaderTexture() {
    int genFormat = GENFORMAT_4444;
    if (CMap::header.flags & MapHeaderFlag_BigAlpha)
        genFormat = GENFORMAT_8888;

    CMapChunk* const chunk = this->mapChunkPtrs[0];
    const uint32_t nLayers = chunk->header->nLayers;

    TextureLayerInfo cursors[4];
    cursors[0].alphaData = nullptr;
    cursors[1].alphaData = nullptr;
    cursors[2].alphaData = nullptr;
    cursors[3].alphaData = nullptr;

    if (nLayers) {
        SMLayer* layer = chunk->layers;
        uint32_t i = 0;
        do {
            const uint32_t layerFlags = layer->flags;
            if (layerFlags & LayerFlag_UseAlphaMap)
                cursors[i].alphaData = &chunk->additionalShadowmap[layer->offsetInMCAL];
            cursors[i].flags = layerFlags;

            ++i;
            ++layer;
        } while (i < nLayers);
    }

    const uint32_t chunkFlags = chunk->header->flags;

    uint8_t* shadowMap = nullptr;
    if ((chunkFlags & ChunkFlag_HasShadowMap) && (CWorld::s_enables & WorldEnable_TerrainShadows)) {
        shadowMap = chunk->shadowMap;
    }

    CMapBaseObjLink* areaLink = chunk->parentLinkList.Head();

    const uint8_t mampValue = reinterpret_cast<CMapArea*>(areaLink->ref)->header->mamp_value;

    const uint32_t size = 64u >> mampValue;
    int destPitch = static_cast<int>(64u >> mampValue);
    if (this->unkFlags & 0x02)
        destPitch = 2 * static_cast<int>(size);
    if (this->unk_0A & 0x08)
        destPitch >>= 1;

    this->UnpackAlphaShadowBits(CMapRenderChunk::s_defaultTex,
                                0,
                                destPitch,
                                size,
                                cursors,
                                0,
                                shadowMap,
                                genFormat,
                                chunkFlags & ChunkFlag_DoNotFixAlphaMap);
    // Human AI review until here. Continue uwu
    CMapChunk* const chunk2 = this->mapChunkPtrs[1];
    if (!chunk2)
        return;

    SMChunk* const header2 = chunk2->header;
    const uint32_t nLayers2 = header2->nLayers;

    uint32_t alphaSlot = 0;
    cursors[0].alphaData = nullptr;
    cursors[1].alphaData = nullptr;
    cursors[2].alphaData = nullptr;
    cursors[3].alphaData = nullptr;

    if (nLayers2) {
        const uint32_t layersCount = this->layersCount;
        SMLayer* const layers2 = chunk2->layers;

        for (uint32_t i = 0; i < nLayers2; ++i) {
            SMLayer* const srcLayer = &layers2[i];

            if (layersCount == 0)
                continue;

            uint32_t slot = 0;
            while (this->layers[slot].textureId !=
                   static_cast<int32_t>(srcLayer->textureId)) {
                ++slot;
                if (slot >= layersCount)
                    break;
            }
            if (slot >= layersCount)
                continue; // this texture is not used by the render chunk

            const uint32_t layerFlags = srcLayer->flags;
            if (layerFlags & LayerFlag_UseAlphaMap)
                cursors[slot].alphaData =
                    &this->mapChunkPtrs[1]->additionalShadowmap[srcLayer->offsetInMCAL];
            cursors[slot].flags = layerFlags;

            if (i == 0)
                alphaSlot = slot;
        }
    }

    const uint32_t chunk2Flags = header2->flags;

    uint8_t* shadowMap2 = nullptr;
    if ((chunk2Flags & ChunkFlag_HasShadowMap) &&
        (CWorld::s_enables & WorldEnable_TerrainShadows)) {
        shadowMap2 = this->mapChunkPtrs[1]->shadowMap;
    }

    int destOffset = 0;
    const uint8_t flags = this->unkFlags;
    if (flags & 0x01) {
        destOffset = static_cast<int>(size * size);
        if (this->unk_0A & 0x08)
            destOffset >>= 2;
    } else if (flags & 0x02) {
        destOffset = static_cast<int>(size);
        if (this->unk_0A & 0x08)
            destOffset = static_cast<int>(size) >> 1;
    }

    this->UnpackAlphaShadowBits(s_defaultTex,
                                0,
                                destPitch,
                                size,
                                cursors,
                                alphaSlot,
                                shadowMap2,
                                genFormat,
                                chunk2Flags & ChunkFlag_DoNotFixAlphaMap);
}

// OFFSET: 0x7B9BC0
void CMapRenderChunk::UpdateLayerGxTexture(EGxTexCommand cmd, uint32_t w, uint32_t h, uint32_t d, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CMapRenderChunkLayer* layer = static_cast<CMapRenderChunkLayer*>(userArg);

    if (cmd == GxTex_Latch) {
        layer->owner->CreateChunkLayerTex(layer);
        texelStrideInBytes = 4 * w;
        if ((CMap::header.flags & 4) == 0)
            texelStrideInBytes = 2 * w;
        texels = CMapRenderChunk::s_defaultTex;
    }
}

// OFFSET: 0x7B9890
void CMapRenderChunk::CreateChunkLayerTex(CMapRenderChunkLayer* layer) {
    int layerMode = 3;
    if ((CMap::header.flags & 4) != 0)
        layerMode = 2;

    SMLayer* chunkLayer = &this->mapChunkPtrs[0]->layers[layer->layerIndex];
    TextureLayerInfo layerInfo;
    layerInfo.flags = chunkLayer->flags;
    layerInfo.alphaData = nullptr;
    if ((chunkLayer->flags & 0x100) != 0)
        layerInfo.alphaData = &this->mapChunkPtrs[0]->additionalShadowmap[chunkLayer->offsetInMCAL];

    uint8_t* shadowMap = nullptr;
    uint32_t chunkHeaderFlags = this->mapChunkPtrs[0]->header->flags;
    if ((chunkHeaderFlags & 1) != 0 && (CWorld::s_enables & CWorld::Enables::Enable_Shadow) != 0)
        shadowMap = this->mapChunkPtrs[0]->shadowMap;

    CMapBaseObjLink* link = this->mapChunkPtrs[0]->parentLinkList.Head();
    CMapArea* area = (CMapArea*)link->ref;
    unsigned int texSize = 64 >> area->header->mamp_value;

    this->UnpackAlphaBits(CMapRenderChunk::s_defaultTex, texSize, &layerInfo, shadowMap, layerMode, chunkHeaderFlags & 0x8000);
}

// OFFSET: 0x007B7530 - Created via AI DOUBLE CHECK!!!
void FetchAlphaShadowRows(const uint8_t** outShadowRow,
                          const uint8_t** outAlphaRows,
                          TextureLayerInfo* cursors,
                          uint32_t size,
                          const uint8_t** shadowCursor) {
    if (*shadowCursor) {
        *outShadowRow = *shadowCursor;
        *shadowCursor += size >> 3;
    } else {
        *outShadowRow = CMapRenderChunk::s_defaultShadowRow;
    }

    if (cursors[0].alphaData) {
        outAlphaRows[0] = cursors[0].alphaData;
        cursors[0].alphaData += size >> 1;
    } else {
        outAlphaRows[0] = CMapRenderChunk::s_defaultAlphaRow;
    }

    if (cursors[1].alphaData) {
        outAlphaRows[1] = cursors[1].alphaData;
        cursors[1].alphaData += size >> 1;
    } else {
        outAlphaRows[1] = CMapRenderChunk::s_defaultAlphaRow;
    }

    if (cursors[2].alphaData) {
        outAlphaRows[2] = cursors[2].alphaData;
        cursors[2].alphaData += size >> 1;
    } else {
        outAlphaRows[2] = CMapRenderChunk::s_defaultAlphaRow;
    }

    if (cursors[3].alphaData) {
        outAlphaRows[3] = cursors[3].alphaData;
        cursors[3].alphaData += size >> 1;
    } else {
        outAlphaRows[3] = CMapRenderChunk::s_defaultAlphaRow;
    }
}

// OFFSET: 0x007B84A0 - Created via AI DOUBLE CHECK!!!
void EmitRow4444Unfixed(uint16_t* dst, const uint8_t* const* alphaRows, const uint8_t* shadowRow, uint32_t size) {
    uint32_t x = 0;
    const uint32_t lastColumn = size - 1;

    if (size == 1) {
        *dst = static_cast<uint16_t>(reinterpret_cast<uintptr_t>(dst));
        return;
    }

    uint16_t pixel = 0;
    do {
        const uint32_t mask = s_nibbleMask[x & 1];
        const uint32_t shift = s_nibbleShift[x & 1];
        const uint32_t shadowBit = (shadowRow[x >> 3] & s_bitMask[x & 7]) >> s_bitShift[x & 7];

        const uint32_t a1 = static_cast<uint8_t>((alphaRows[1][x >> 1] & mask) << shift);
        const uint32_t a2 = static_cast<uint8_t>((alphaRows[2][x >> 1] & mask) << shift);
        const uint32_t a3 = static_cast<uint8_t>((alphaRows[3][x >> 1] & mask) << shift);

        ++x;

        pixel = static_cast<uint16_t>(((s_shadowLut[shadowBit] & 0xF0) << 8) | (a1 << 4) | a2 | (a3 >> 4));
        *dst++ = pixel;
    } while (x < lastColumn);

    *dst++ = pixel;
}

// OFFSET: 0x7B87F0 - Created via AI DOUBLE CHECK!!!
void CMapRenderChunk::UnpackAlphaShadowBits(uint16_t* outputTexture, int32_t dstOffset, int32_t dstPitch, uint32_t size, TextureLayerInfo* layerInfo, uint32_t alphaSlot, uint8_t* shadowMap, int32_t genFormat, bool doNotFixAlphaMap) {
    bool v1 = (this->unk_0A & 0x8) != 0;

    if (doNotFixAlphaMap) {
        if (genFormat == 3) {
            if (v1)
                this->UnpackAlphaShadowBitsFixed4444Mip1(outputTexture, dstOffset, dstPitch, size, layerInfo, shadowMap);
            else
                this->UnpackAlphaShadowBitsFixed4444Mip0(outputTexture, dstOffset, dstPitch, size, layerInfo, shadowMap);
            return;
        } else if (v1) {
            this->UnpackAlphaShadowBitsFixed8888Mip0(outputTexture, size, shadowMap);
        } else {
            this->UnpackAlphaShadowBitsFixed8888Mip0(outputTexture, size, shadowMap);
        }
    } else {
        if (genFormat != 3)
            SErrDisplayAppFatal("CMapChunk::UnpackAlphaShadowBits(): Bad genformat.");

        if (v1)
            this->UnpackAlphaShadowBitsUnfixed4444Mip1(outputTexture, size, layerInfo);
        else
            this->UnpackAlphaShadowBitsUnfixed4444Mip0(outputTexture, dstOffset, dstPitch, size, layerInfo, shadowMap);
    }
}

// OFFSET: 0x7B8620
void CMapRenderChunk::UnpackAlphaShadowBitsUnfixed4444Mip1(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo) {
    SErrDisplayAppFatal("CMapRenderChunk::UnpackAlphaShadowBitsUnfixed4444Mip1(): Not implemented.");
}

// OFFSET: 0x7B85A0 - Created via AI DOUBLE CHECK!!!
void CMapRenderChunk::UnpackAlphaShadowBitsUnfixed4444Mip0(uint16_t* dstBase, int32_t dstOffset, int32_t dstPitch, uint32_t size, TextureLayerInfo* cursors, const uint8_t* shadowMap) {
    uint16_t* dst = dstBase + dstOffset;
    
    const uint8_t* alphaRows[4] = {};
    const uint8_t* shadowRow = nullptr;
    const uint8_t* lastShadowRow = nullptr;
    
    if (size != 1) {
        uint32_t rowsRemaining = size - 1;
        do {
            FetchAlphaShadowRows(&shadowRow, alphaRows, cursors, size, &shadowMap);
            lastShadowRow = shadowRow;
    
            EmitRow4444Unfixed(dst, alphaRows, lastShadowRow, size);
    
            //dst += dstPitch;
            --rowsRemaining;
        } while (rowsRemaining);
    }
    
    EmitRow4444Unfixed(dst, alphaRows, lastShadowRow, size);
}

// OFFSET: 0x7B7DC0
void CMapRenderChunk::UnpackAlphaShadowBitsFixed8888Mip1(uint16_t* outputTexture, uint32_t texSize, uint8_t* shadowMap) {
    SErrDisplayAppFatal("CMapRenderChunk::UnpackAlphaShadowBitsFixed8888Mip1(): Not implemented.");
}

// OFFSET: 0x7B7C60
void CMapRenderChunk::UnpackAlphaShadowBitsFixed8888Mip0(uint16_t* outputTexture, uint32_t texSize, uint8_t* shadowMap) {
    SErrDisplayAppFatal("CMapRenderChunk::UnpackAlphaShadowBitsFixed8888Mip0(): Not implemented.");
}

// OFFSET: 0x7B8190
void CMapRenderChunk::UnpackAlphaShadowBitsFixed4444Mip1(uint16_t* dstBase, int32_t dstOffset, int32_t dstPitch, uint32_t size, TextureLayerInfo* cursors, const uint8_t* shadowMap) {
    SErrDisplayAppFatal("CMapRenderChunk::UnpackAlphaShadowBitsFixed4444Mip1(): Not implemented.");
}

// OFFSET: 0x7B8070 - Created via AI DOUBLE CHECK!!!
void CMapRenderChunk::UnpackAlphaShadowBitsFixed4444Mip0(uint16_t* dstBase, int32_t dstOffset, int32_t dstPitch, uint32_t size, TextureLayerInfo* cursors, const uint8_t* shadowMap) {
    uint16_t* dst = dstBase + dstOffset;

    if (size == 0)
        return;

    const int rowStride = dstPitch;
    uint32_t rowsRemaining = size;

    do {
        const uint8_t* shadowRow;
        const uint8_t* alphaRows[4];
        FetchAlphaShadowRows(&shadowRow, alphaRows, cursors, size, &shadowMap);

        for (uint32_t x = 0; x < size; ++x) {
            const uint32_t mask = s_nibbleMask[x & 1];
            const uint32_t shift = s_nibbleShift[x & 1];
            const uint32_t shadowBit = (shadowRow[x >> 3] & s_bitMask[x & 7]) >> s_bitShift[x & 7];

            const uint32_t a1 = static_cast<uint8_t>((alphaRows[1][x >> 1] & mask) << shift);
            const uint32_t a2 = static_cast<uint8_t>((alphaRows[2][x >> 1] & mask) << shift);
            const uint32_t a3 = static_cast<uint8_t>((alphaRows[3][x >> 1] & mask) << shift);

            *dst++ = static_cast<uint16_t>(((s_shadowLut[shadowBit] & 0xF0) << 8) | (a1 << 4) | a2 | (a3 >> 4));
        }

        //dst += rowStride;
        --rowsRemaining;
    } while (rowsRemaining);
}

// OFFSET: 0x7B8E20
void CMapRenderChunk::UnpackAlphaBits(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo, uint8_t* shadowMap, int32_t layerMode, bool bigAlpha) {
    bool v1 = (this->unk_0A & 0x8) != 0;

    if (bigAlpha) {
        if (layerMode == 3) {
            if (v1)
                this->UnpackAlphaBitsFixed4444Mip1(outputTexture, texSize, layerInfo);
            else
                this->UnpackAlphaBitsFixed4444Mip0(outputTexture, texSize, layerInfo);
            return;
        }
        if (layerMode == 2) {
            if(layerInfo->alphaData) {
                if (v1)
                    this->RecreateAlphaBitsFixed8888Mip1(outputTexture, texSize, layerInfo, shadowMap);
                else
                    this->RecreateAlphaBitsFixed8888Mip0(outputTexture, texSize, layerInfo, shadowMap);
            } else if (v1) {
                this->UnpackAlphaBitsFixed8888Mip0(outputTexture, texSize, shadowMap);
            } else {
                this->UnpackAlphaBitsFixed8888Mip0(outputTexture, texSize, shadowMap);
            }
            return;
        }
        SErrDisplayAppFatal("CMapChunk::UnpackAlphaBits(): Bad genformat.");
        return;
    }

    if (layerMode != 3)
        SErrDisplayAppFatal("CMapChunk::UnpackAlphaBits(): Bad genformat.");

    if (v1)
        this->UnpackAlphaBitsUnfixed4444Mip1(outputTexture, texSize, layerInfo);
    else
        this->UnpackAlphaBitsUnfixed4444Mip0(outputTexture, texSize, layerInfo);
}

// OFFSET: 0x7B77D0
void CMapRenderChunk::UnpackAlphaBitsUnfixed4444Mip1(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo) {
    SErrDisplayAppFatal("CMapRenderChunk::UnpackAlphaBitsUnfixed4444Mip1(): Not implemented.");
}

// OFFSET: 0x7B76F0
void CMapRenderChunk::UnpackAlphaBitsUnfixed4444Mip0(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo) {
    if (texSize == 1)
        return;

    uint32_t nibbleIndex = 0;
    uint16_t lastPixel = 0x0FFF;

    for (int32_t i = 0; i < texSize - 1; i++) {
        for (int32_t j = 0; j < texSize - 1; j++) {
            uint8_t rawByte = layerInfo->alphaData[nibbleIndex >> 1];
            uint8_t alpha4 = (rawByte & s_nibbleMask[nibbleIndex & 1]) >> s_nibbleShiftReverse[nibbleIndex & 1];
            lastPixel = (uint16_t)((alpha4 << 12) | 0x0FFF);
            *outputTexture++ = lastPixel;
            ++nibbleIndex;
        }

        *outputTexture++ = lastPixel;
        ++nibbleIndex;
    }

    uint32_t finalStart = nibbleIndex - texSize;
    for (int32_t col = 0; col < texSize - 1; col++) {
        unsigned int idx = finalStart + col;
        uint8_t rawByte = layerInfo->alphaData[idx >> 1];
        uint8_t alpha4 = (rawByte & s_nibbleMask[idx & 1]) >> s_nibbleShiftReverse[idx & 1];
        lastPixel = (uint16_t)((alpha4 << 12) | 0x0FFF);
        *outputTexture++ = lastPixel;
    }
    *outputTexture = lastPixel;
}

// OFFSET: 0x7B8C70
void CMapRenderChunk::UnpackAlphaBitsFixed8888Mip1(uint16_t* outputTexture, uint32_t texSize, uint8_t* shadowMap) {
    SErrDisplayAppFatal("CMapRenderChunk::UnpackAlphaBitsFixed8888Mip1(): Not implemented.");
}

// OFFSET: 0x7B8B80
void CMapRenderChunk::UnpackAlphaBitsFixed8888Mip0(uint16_t* outputTexture, uint32_t texSize, uint8_t* shadowMap) {
    SErrDisplayAppFatal("CMapRenderChunk::UnpackAlphaBitsFixed8888Mip0(): Not implemented.");
}

// OFFSET: 0x7B89C0
void CMapRenderChunk::RecreateAlphaBitsFixed8888Mip1(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo, uint8_t* shadowMap) {
    SErrDisplayAppFatal("CMapRenderChunk::RecreateAlphaBitsFixed8888Mip1(): Not implemented.");
}

// OFFSET: 0x7B88D0
void CMapRenderChunk::RecreateAlphaBitsFixed8888Mip0(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo, uint8_t* shadowMap) {
    SErrDisplayAppFatal("CMapRenderChunk::RecreateAlphaBitsFixed8888Mip0(): Not implemented.");
}

// OFFSET: 0x7B7620
void CMapRenderChunk::UnpackAlphaBitsFixed4444Mip1(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo) {
    SErrDisplayAppFatal("CMapRenderChunk::UnpackAlphaBitsFixed4444Mip1(): Not implemented.");
}

// OFFSET: 0x7B75B0
void CMapRenderChunk::UnpackAlphaBitsFixed4444Mip0(uint16_t* outputTexture, uint32_t texSize, TextureLayerInfo* layerInfo) {
    if (!texSize)
        return;

    unsigned int nibbleIndex = 0;

    for (unsigned int row = 0; row < texSize; ++row) {
        for (unsigned int col = 0; col < texSize; ++col) {
            uint8_t rawByte = layerInfo->alphaData[nibbleIndex >> 1];
            uint8_t alpha4 = (rawByte & s_nibbleMask[nibbleIndex & 1]) >> s_nibbleShiftReverse[nibbleIndex & 1];
            *outputTexture++ = (uint16_t)((alpha4 << 12) | 0x0FFF);
            ++nibbleIndex;
        }
    }
}

// OFFSET: 0x7B9C20
void CMapRenderChunk::UpdateShadowGxTexture(EGxTexCommand cmd, uint32_t w, uint32_t h, uint32_t d, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CMapRenderChunk* renderChunk = static_cast<CMapRenderChunk*>(userArg);

    if (cmd == GxTex_Latch) {
        //CMapRenderChunk::CreateShadowTex(a6);
        //*a7 = 2 * a2;
        //*a8 = CMapRenderChunk::s_defaultTex;
    }
}

// OFFSET: 0x7D04A0
void CMapRenderChunk::RenderSetup(int32_t a2) {
    this->lastUpdateTime = 0.0;
    this->AllocLayerTextures();
    //if (!a2 || !CMap::enableTerrainShaderVertex) {
    C44Matrix worldMatrix = C44Matrix();
    worldMatrix.d0 = this->vec1.x - CWorldScene::s_activeWorldView.x;
    worldMatrix.d1 = this->vec1.y - CWorldScene::s_activeWorldView.y;
    worldMatrix.d2 = this->vec1.z - CWorldScene::s_activeWorldView.z;
    g_theGxDevicePtr->XformSet(GxXform_World, worldMatrix);
    //    sub_790440(v10, &this->vec2.x);
    //    CM2Scene::SelectLights(s_m2Scene, v10);
    //    CMapRenderChunk::SelectLights((int)v10);
    //    CM2Lighting::SetupGxLights(v10, &CWorldScene::s_activeWorldView.x);
    //    CM2Lighting::SetupGxFog(v10);
    //}

    if (this->chunkBuf) {
        GxPrimVertexPtr(this->chunkBuf->vertexBuf, CMapRenderChunk::s_gxBufVertexFormat);
        GxPrimIndexPtr(this->chunkBuf->indexBuf);
    } else {
        this->UseStreamingBufs();
    }
}

void BuildTexCoordMatrices(
    C44Matrix* detailMtx,
    C44Matrix* alphaMtx,
    C3Vector* translation,
    float geoToTex) // s_geoToTex ≈ 0.24f
{
    detailMtx->Scale(geoToTex);
    {
        float M11 = detailMtx->a0, M12 = detailMtx->a1,
              M13 = detailMtx->a2, M14 = detailMtx->a3;
        detailMtx->a0 = detailMtx->b0;
        detailMtx->a1 = detailMtx->b1;
        detailMtx->a2 = detailMtx->b2;
        detailMtx->a3 = detailMtx->b3;
        detailMtx->b0 = M11;
        detailMtx->b1 = M12;
        detailMtx->b2 = M13;
        detailMtx->b3 = M14;
    }
    detailMtx->Translate(*translation);

    alphaMtx->Scale(geoToTex * 0.125f);
    {
        float M11 = alphaMtx->a0, M12 = alphaMtx->a1,
              M13 = alphaMtx->a2, M14 = alphaMtx->a3;
        alphaMtx->a0 = alphaMtx->b0;
        alphaMtx->a1 = alphaMtx->b1;
        alphaMtx->a2 = alphaMtx->b2;
        alphaMtx->a3 = alphaMtx->b3;
        alphaMtx->b0 = M11;
        alphaMtx->b1 = M12;
        alphaMtx->b2 = M13;
        alphaMtx->b3 = M14;
    }
    alphaMtx->Translate(*translation);
}

// OFFSET: 0x7D3010
void CMapRenderChunk::RenderSolid() {
    g_theGxDevicePtr->XformSet(GxXform_Tex0, C44Matrix());
    g_theGxDevicePtr->XformSet(GxXform_Tex1, C44Matrix());
    g_theGxDevicePtr->RsSet(GxRs_BlendingMode, 0);
    // if (v8->m_context) {
    //     m_data = v8->m_appRenderStates.m_data;
    //     p_m_data = &v8->m_appRenderStates.m_data;
    //     v12 = CGxDevice::s_alphaRef[m_data[6].m_value.m_data.i[0]];
    //     if (m_data[7].m_value.m_data.i[0] != v12) {
    //         CGxDevice::IRsDirty(v8, GxRs_AlphaRef);
    //         (*p_m_data)[7].m_value.m_data.i[0] = v12;
    //     }
    // }
    CGxTex* defaultTexture = TextureGetGxTex(CWorldScene::s_defaultTexture, 0, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, defaultTexture);
    GxTexSetWrap(defaultTexture, GxTex_Wrap, GxTex_Wrap);
    CGxTex* blendTexture;
    if (CMap::gTerrainPixelShadersValid)
        blendTexture = TextureGetGxTex(CWorldScene::s_defaultBlendTexture, 1, nullptr);
    else
        blendTexture = TextureGetGxTex(CWorldScene::s_defaultTexture, 1, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, blendTexture);
    g_theGxDevicePtr->Draw(&this->batch, 1);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, nullptr);
}

// OFFSET: 0x7D3240
void CMapRenderChunk::RenderSolidVertexPixelShader() {
    g_theGxDevicePtr->RsSet(GxRs_BlendingMode, 0);
    // if (v8->m_context) {
    //     m_data = v8->m_appRenderStates.m_data;
    //     p_m_data = &v8->m_appRenderStates.m_data;
    //     v12 = CGxDevice::s_alphaRef[m_data[6].m_value.m_data.i[0]];
    //     if (m_data[7].m_value.m_data.i[0] != v12) {
    //         CGxDevice::IRsDirty(v8, GxRs_AlphaRef);
    //         (*p_m_data)[7].m_value.m_data.i[0] = v12;
    //     }
    // }
    CGxTex* defaultTexture = TextureGetGxTex(CWorldScene::s_defaultTexture, 1, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, defaultTexture);
    GxTexSetWrap(defaultTexture, GxTex_Wrap, GxTex_Wrap);
    CGxTex* blendTexture = TextureGetGxTex(CWorldScene::s_defaultBlendTexture, 1, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, blendTexture);
    this->SetVertexShader(2, 0);
    g_theGxDevicePtr->Draw(&this->batch, 1);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, nullptr);
}

// OFFSET: 0x7D0050
void CMapRenderChunk::SetVertexShader(int32_t a1, int32_t a2) {
    //v25 = (unsigned __int8)CMap::enableSpecularTerrain;
    //v26 = CMapRenderChunk::s_gxBufVertexFormat == GxVBF_PNC;
    //sub_790440(v24, &a1->vec2.x);
    //CM2Scene::SelectLights(s_m2Scene, v24);
    //CMapRenderChunk::SelectLights((int)v24);
    //v27 = 0.0;
    //v28 = 0.0;
    //v31 = 0;
    //v29 = 0.0;
    //v32 = 0;
    //v3 = &flt_D25278;
    //v30 = 3;
    //do {
    //    if (sub_8349E0(v32, &v27, v3 - 2, v3 + 2)) {
    //        v4 = v28 - CWorldScene::s_activeWorldView.y;
    //        v5 = v29 - CWorldScene::s_activeWorldView.z;
    //        x = v27 - CWorldScene::s_activeWorldView.x;
    //        *(v3 - 6) = x;
    //        y = v4;
    //        *(v3 - 5) = y;
    //        v35 = v5;
    //        *(v3 - 4) = v35;
    //        *(float*)&v36 = 1.0;
    //        v6 = 0.0;
    //        *(v3 - 3) = 1.0;
    //        v31 = 1;
    //    } else {
    //        v6 = 0.0;
    //        *(v3 - 6) = 0.0;
    //        *(v3 - 5) = 0.0;
    //        *(v3 - 4) = 0.0;
    //        *(v3 - 3) = 0.0;
    //        *(v3 - 2) = 0.0;
    //        *(v3 - 1) = 0.0;
    //        *v3 = 0.0;
    //        v3[1] = 0.0;
    //        v3[2] = 1.0;
    //        v3[3] = 0.0;
    //        v3[4] = 0.0;
    //        v3[5] = 0.0;
    //    }
    //    ++v32;
    //    v3 += 12;
    //    --v30;
    //} while (v30);
    //v7 = a1;
    //x = a1->vec1.x;
    //y = a1->vec1.y;
    //z = a1->vec1.z;
    //dword_D25214 = LODWORD(y);
    //v35 = z;
    //*(float*)&v36 = v6;
    //dword_D25210 = LODWORD(x);
    //v9 = CMapChunk::s_geoToTex;
    //v10 = a2 - 1;
    //v11 = -CMapChunk::s_geoToTex;
    //dword_D2521C = v36;
    //x = v11;
    //dword_D25218 = LODWORD(v35);
    //v12 = v11;
    //y = v11;
    //v13 = v11;
    //v14 = v6;
    //v15 = v13;
    //v35 = v14;
    //*(float*)&v36 = v14;
    //v16 = v36;
    //if (a2 != 1) {
    //    dword_D251C0 = LODWORD(x);
    //    dword_D251C4 = LODWORD(y);
    //    dword_D251C8 = LODWORD(v35);
    //    dword_D251CC = v36;
    //    qmemcpy(&unk_D251D0, &dword_D251C0, 4 * ((unsigned int)(16 * v10 - 13) >> 2));
    //    v7 = a1;
    //}
    //unkFlags = v7->unkFlags;
    //x = v12 * 0.125;
    //v18 = v9;
    //v19 = v15 * 0.125;
    //v20 = v18;
    //y = v19;
    //if ((unkFlags & 1) != 0) {
    //    x = v20 * -0.0625;
    //} else if ((unkFlags & 2) != 0) {
    //    y = v20 * -0.0625;
    //}
    //v21 = &dword_D251B0[4 * a2];
    //*(float*)v21 = x;
    //*((float*)v21 + 1) = y;
    //*((float*)v21 + 2) = v35;
    //v21[3] = v16;
    //v22 = sub_873FF0();
    //v23 = CMapRenderChunk::GetVertexShader(v31, v10, v25, v26, a3, v22);
    //CGxDevice::RsSet(g_theGxDevicePtr, GxRs_VertexShader, v23);
    //g_theGxDevicePtr->ShaderConstantsSet(g_theGxDevicePtr, GxSh_Vertex, 0, &stru_D250A0, 37);
}

// OFFSET: 0x7D0D70
void CMapRenderChunk::RenderMultiPassAlpha(CMapRenderChunk* renderChunk) {
    C44Matrix tex0Matrix = C44Matrix();
    C44Matrix tex1Matrix = C44Matrix();
    C3Vector viewVector = {
        CWorldScene::s_activeWorldView.x - renderChunk->vec1.x,
        CWorldScene::s_activeWorldView.y - renderChunk->vec1.y,
        CWorldScene::s_activeWorldView.z - renderChunk->vec1.z
    };
    BuildTexCoordMatrices(&tex0Matrix, &tex1Matrix, &viewVector, -CMapChunk::s_geoToTex);
    g_theGxDevicePtr->XformSet(GxXform_Tex0, tex0Matrix);
    g_theGxDevicePtr->XformSet(GxXform_Tex1, tex1Matrix);

    GxRsSet(GxRs_BlendingMode, 0);
    //if (v6->m_context) {
    //    m_data = v6->m_appRenderStates.m_data;
    //    v9 = CGxDevice::s_alphaRef[m_data[6].m_value.m_data.i[0]];
    //    p_m_data = &v6->m_appRenderStates.m_data;
    //    if (m_data[7].m_value.m_data.i[0] != v9) {
    //        CGxDevice::IRsDirty(v6, GxRs_AlphaRef);
    //        (*p_m_data)[7].m_value.m_data.i[0] = v9;
    //        v6 = g_theGxDevicePtr;
    //    }
    //}

    bool v51 = (renderChunk->unk_0A & 4) != 0;

    for (int layer = 0; layer < renderChunk->layersCount; layer++) {
        CMapRenderChunkLayer* chunkLayer = &renderChunk->layers[layer];

        CGxTex* gxTex = TextureGetGxTex(chunkLayer->texture, 0, 0);
        GxRsSet(GxRs_Texture0, gxTex);
        GxTexSetWrap(gxTex, GxTex_Wrap, GxTex_Wrap);

        if (layer == 0 && v51) {
            GxRsSet(GxRs_TexGen0, 4);
            GxRsSet(GxRs_TextureShader0, 0);
        }

        // TODO enable when lighting is implemented
        //if (chunkLayer->flags & 0x80)
            GxRsSet(GxRs_Lighting, 0);
        
        if (chunkLayer->flags & 0x40) {
            //C3Vector__C3Vector(&a3, (C3Vector*)&World::texVect[v14->flags & 7]);
            //v20 = 1.0 / CMapChunk::s_geoToTex / flt_AF14F8[(LOBYTE(v14->flags) >> 3) & 7];
            //v21 = g_theGxDevicePtr->m_xforms;
            //a3.x = a3.x * v20;
            //a3.y = a3.y * v20;
            //a3.z = v20 * a3.z;
            //v22 = &g_theGxDevicePtr->m_xforms[0].m_flags[g_theGxDevicePtr->m_xforms[0].m_level];
            //g_theGxDevicePtr->m_xforms[0].m_dirty = 1;
            //*v22 &= ~1u;
            //C44Matrix::Translate(&v21->m_mtx[v21->m_level], &a3);
            //v16 = g_theGxDevicePtr;
        }

        if (layer == 1) {
            GxRsSet(GxRs_BlendingMode, 2);
            //if (v16->m_context) {
            //    v24 = v16->m_appRenderStates.m_data;
            //    v25 = CGxDevice::s_alphaRef[v24[6].m_value.m_data.i[0]];
            //    v26 = &v16->m_appRenderStates.m_data;
            //    if (v24[7].m_value.m_data.i[0] != v25) {
            //        CGxDevice::IRsDirty(v16, GxRs_AlphaRef);
            //        (*v26)[7].m_value.m_data.i[0] = v25;
            //        v16 = g_theGxDevicePtr;
            //    }
            //    v11 = v50;
            //}
        }

        if (chunkLayer->layerTexture) {
            CGxTex* layerGxTex = TextureGetGxTex(chunkLayer->layerTexture, 1, 0);
            GxRsSet(GxRs_Texture1, layerGxTex);
        } else {
            GxRsSet(GxRs_Texture1, (CGxTex*)nullptr);
        }

        g_theGxDevicePtr->Draw(&renderChunk->batch, 1);

        if (layer == 0 && v51) {
            GxRsSet(GxRs_TexGen0, 2);
            GxRsSet(GxRs_TextureShader0, 1);
        }

        if (chunkLayer->flags & 0x80)
            GxRsSet(GxRs_Lighting, 1);

        if (chunkLayer->flags & 0x40)
            g_theGxDevicePtr->XformSet(GxXform_World, tex0Matrix);
    }

    if (renderChunk->shadowTexture && (CWorld::s_enables & CWorld::Enables::Enable_Shadow)) {
        //GxRsSet(GxRs_BlendingMode, 2);
        //SyncAlphaRef();
        //
        //GxRsSet(GxRs_Texture0, TextureGetGxTex(CWorld::shadowModTexture, 1, 0));
        //GxRsSet(GxRs_Texture1, TextureGetGxTex(renderChunk->shadowTexture, 1, 0));
        //g_theGxDevicePtr->Draw(&renderChunk->batch, 1);
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, nullptr);
}

// OFFSET: 0x7D0760
void CMapRenderChunk::RenderMultiPassAdditive(CMapRenderChunk* renderChunk) {
    //a1.a0 = 1.0;
    //v1 = arg0;
    //a1.a1 = 0.0;
    //a1.a2 = 0.0;
    //a1.a3 = 0.0;
    //a1.b0 = 0.0;
    //a1.b2 = 0.0;
    //a1.b3 = 0.0;
    //a1.c0 = 0.0;
    //a1.c1 = 0.0;
    //a1.c3 = 0.0;
    //a1.d0 = 0.0;
    //a1.d1 = 0.0;
    //a1.d2 = 0.0;
    //a2.a1 = 0.0;
    //a2.a2 = 0.0;
    //a2.a3 = 0.0;
    //a2.b0 = 0.0;
    //a2.b2 = 0.0;
    //a2.b3 = 0.0;
    //a2.c0 = 0.0;
    //a2.c1 = 0.0;
    //a2.c3 = 0.0;
    //a2.d0 = 0.0;
    //a2.d1 = 0.0;
    //a2.d2 = 0.0;
    //a1.b1 = 1.0;
    //a1.c2 = 1.0;
    //a1.d3 = 1.0;
    //a2.a0 = 1.0;
    //a2.b1 = 1.0;
    //a2.c2 = 1.0;
    //a2.d3 = 1.0;
    //v48.x = CWorldScene::s_activeWorldView.x - arg0->vec1.x;
    //v48.y = CWorldScene::s_activeWorldView.y - arg0->vec1.y;
    //v48.z = CWorldScene::s_activeWorldView.z - arg0->vec1.z;
    //a4 = -CMapChunk::s_geoToTex;
    //sub_7D06B0(&a1, &a2, &v48, a4);
    //m_xforms = g_theGxDevicePtr->m_xforms;
    //v3 = &g_theGxDevicePtr->m_xforms[0].m_flags[g_theGxDevicePtr->m_xforms[0].m_level];
    //g_theGxDevicePtr->m_xforms[0].m_dirty = 1;
    //*v3 &= ~1u;
    //C44Matrix::Copy(&m_xforms->m_mtx[m_xforms->m_level], &a1);
    //v4 = &g_theGxDevicePtr->m_xforms[1];
    //v5 = &g_theGxDevicePtr->m_xforms[1].m_flags[g_theGxDevicePtr->m_xforms[1].m_level];
    //g_theGxDevicePtr->m_xforms[1].m_dirty = 1;
    //*v5 &= ~1u;
    //C44Matrix::Copy(&v4->m_mtx[v4->m_level], &a2);
    //v6 = g_theGxDevicePtr;
    //if (g_theGxDevicePtr->m_context) {
    //    v7 = g_theGxDevicePtr->m_appRenderStates.m_data + 6;
    //    if (v7->m_value.m_data.i[0]) {
    //        CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_BlendingMode);
    //        v7->m_value.m_data.i[0] = 0;
    //        v6 = g_theGxDevicePtr;
    //    }
    //    if (v6->m_context) {
    //        m_data = v6->m_appRenderStates.m_data;
    //        v9 = CGxDevice::s_alphaRef[m_data[6].m_value.m_data.i[0]];
    //        p_m_data = &v6->m_appRenderStates.m_data;
    //        if (m_data[7].m_value.m_data.i[0] != v9) {
    //            CGxDevice::IRsDirty(v6, GxRs_AlphaRef);
    //            (*p_m_data)[7].m_value.m_data.i[0] = v9;
    //            v6 = g_theGxDevicePtr;
    //        }
    //    }
    //}
    //v11 = 0;
    //v51 = (arg0->unk_0A & 4) != 0;
    //v12 = arg0->layersCount == 0;
    //v53 = 0;
    //if (!v12) {
    //    while (1) {
    //        texture = v1->layers[v11].texture;
    //        v14 = &v1->layers[v11];
    //        GxTex = CTexture::GetGxTex(texture, 0, 0);
    //        CGxDevice::RsSet(g_theGxDevicePtr, GxRs_Texture0, GxTex);
    //        GxTexSetWrap(GxTex, 1, 1u);
    //        if (v11 || !v51)
    //            goto LABEL_18;
    //        v16 = g_theGxDevicePtr;
    //        if (g_theGxDevicePtr->m_context) {
    //            v17 = g_theGxDevicePtr->m_appRenderStates.m_data + 53;
    //            if (v17->m_value.m_data.i[0] != 4) {
    //                CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_TexGen0);
    //                v17->m_value.m_data.i[0] = 4;
    //                v16 = g_theGxDevicePtr;
    //            }
    //            if (v16->m_context) {
    //                v18 = v16->m_appRenderStates.m_data + 61;
    //                if (v18->m_value.m_data.i[0])
    //                    break;
    //            }
    //        }
//LABEL_19:
    //        if (SLOBYTE(v14->flags) < 0) {
    //            if (v16->m_context) {
    //                v19 = v16->m_appRenderStates.m_data + 11;
    //                if (v19->m_value.m_data.i[0]) {
    //                    CGxDevice::IRsDirty(v16, GxRs_Lighting);
    //                    v19->m_value.m_data.i[0] = 0;
    //                    v16 = g_theGxDevicePtr;
    //                }
    //            }
    //        }
    //        if ((v14->flags & 0x40) != 0) {
    //            C3Vector__C3Vector(&a3, (C3Vector*)&World::texVect[v14->flags & 7]);
    //            v20 = 1.0 / CMapChunk::s_geoToTex / flt_AF14F8[(LOBYTE(v14->flags) >> 3) & 7];
    //            v21 = g_theGxDevicePtr->m_xforms;
    //            a3.x = a3.x * v20;
    //            a3.y = a3.y * v20;
    //            a3.z = v20 * a3.z;
    //            v22 = &g_theGxDevicePtr->m_xforms[0].m_flags[g_theGxDevicePtr->m_xforms[0].m_level];
    //            g_theGxDevicePtr->m_xforms[0].m_dirty = 1;
    //            *v22 &= ~1u;
    //            C44Matrix::Translate(&v21->m_mtx[v21->m_level], &a3);
    //            v16 = g_theGxDevicePtr;
    //        }
    //        if (v11 == 1) {
    //            if (v16->m_context) {
    //                v23 = v16->m_appRenderStates.m_data + 6;
    //                if (v23->m_value.m_data.i[0] != 10) {
    //                    CGxDevice::IRsDirty(v16, GxRs_BlendingMode);
    //                    v23->m_value.m_data.i[0] = 10;
    //                    v16 = g_theGxDevicePtr;
    //                }
    //                if (v16->m_context) {
    //                    v24 = v16->m_appRenderStates.m_data;
    //                    v25 = CGxDevice::s_alphaRef[v24[6].m_value.m_data.i[0]];
    //                    v26 = &v16->m_appRenderStates.m_data;
    //                    if (v24[7].m_value.m_data.i[0] != v25) {
    //                        CGxDevice::IRsDirty(v16, GxRs_AlphaRef);
    //                        (*v26)[7].m_value.m_data.i[0] = v25;
    //                        v16 = g_theGxDevicePtr;
    //                    }
    //                    v11 = v53;
    //                }
    //            }
    //            v12 = v16->m_context == 0;
    //            v52 = 0;
    //            if (!v12) {
    //                v27 = v16->m_appRenderStates.m_data;
    //                v28 = v27[10].m_value.m_data.i[0];
    //                v29 = v52;
    //                i = v27[10].m_value.m_data.i;
    //                if (v28 != v52) {
    //                    CGxDevice::IRsDirty(v16, GxRs_FogColor);
    //                    *i = v29;
    //                    v16 = g_theGxDevicePtr;
    //                }
    //                v11 = v53;
    //            }
    //        }
    //        layerTexture = (CTexture*)v14->layerTexture;
    //        if (layerTexture) {
    //            v32 = CTexture::GetGxTex(layerTexture, 1, 0);
    //            CGxDevice::RsSet(g_theGxDevicePtr, GxRs_Texture1, v32);
//LABEL_42:
    //            v16 = g_theGxDevicePtr;
    //            goto LABEL_43;
    //        }
    //        if (v16->m_context) {
    //            v33 = v16->m_appRenderStates.m_data + 22;
    //            if (v33->m_value.m_data.i[0]) {
    //                CGxDevice::IRsDirty(v16, GxRs_Texture1);
    //                v33->m_value.m_data.i[0] = 0;
    //                goto LABEL_42;
    //            }
    //        }
//LABEL_43:
    //        v16->Draw(v16, &arg0->batch, 1);
    //        if (v11 || !v51)
    //            goto LABEL_51;
    //        v6 = g_theGxDevicePtr;
    //        if (g_theGxDevicePtr->m_context) {
    //            v34 = g_theGxDevicePtr->m_appRenderStates.m_data + 53;
    //            if (v34->m_value.m_data.i[0] != 2) {
    //                CGxDevice::IRsDirty(g_theGxDevicePtr, GxRs_TexGen0);
    //                v34->m_value.m_data.i[0] = 2;
    //                v6 = g_theGxDevicePtr;
    //            }
    //            if (v6->m_context) {
    //                v35 = v6->m_appRenderStates.m_data + 61;
    //                if (v35->m_value.m_data.i[0] != 1) {
    //                    CGxDevice::IRsDirty(v6, GxRs_TextureShader0);
    //                    v35->m_value.m_data.i[0] = 1;
//LABEL_51:
    //                    v6 = g_theGxDevicePtr;
    //                }
    //            }
    //        }
    //        if (SLOBYTE(v14->flags) < 0) {
    //            if (v6->m_context) {
    //                v36 = v6->m_appRenderStates.m_data + 11;
    //                if (v36->m_value.m_data.i[0] != 1) {
    //                    CGxDevice::IRsDirty(v6, GxRs_Lighting);
    //                    v36->m_value.m_data.i[0] = 1;
    //                    v6 = g_theGxDevicePtr;
    //                }
    //            }
    //        }
    //        if ((v14->flags & 0x40) != 0) {
    //            v6->m_xforms[0].m_dirty = 1;
    //            v37 = v6->m_xforms;
    //            v37->m_flags[v6->m_xforms[0].m_level] &= ~1u;
    //            v38 = v6->m_xforms[0].m_level << 6;
    //            *(float*)((char*)&v37->m_mtx[0].a0 + v38) = a1.a0;
    //            v39 = (float*)((char*)&v6->m_xforms[0].m_mtx[0].a0 + v38);
    //            v39[1] = a1.a1;
    //            v39[2] = a1.a2;
    //            v39[3] = a1.a3;
    //            v39[4] = a1.b0;
    //            v39[5] = a1.b1;
    //            v39[6] = a1.b2;
    //            v39[7] = a1.b3;
    //            v39[8] = a1.c0;
    //            v39[9] = a1.c1;
    //            v39[10] = a1.c2;
    //            v39[11] = a1.c3;
    //            v39[12] = a1.d0;
    //            v39[13] = a1.d1;
    //            v39[14] = a1.d2;
    //            v39[15] = a1.d3;
    //            v6 = g_theGxDevicePtr;
    //        }
    //        layersCount = arg0->layersCount;
    //        v53 = ++v11;
    //        if (v11 >= layersCount)
    //            goto LABEL_59;
    //        v1 = arg0;
    //    }
    //    CGxDevice::IRsDirty(v16, GxRs_TextureShader0);
    //    v18->m_value.m_data.i[0] = 0;
//LABEL_18:
    //    v16 = g_theGxDevicePtr;
    //    goto LABEL_19;
    //}
//LABEL_59:
    //result = dword_D2509C;
    //v42 = dword_D2509C;
    //if (v6->m_context) {
    //    v43 = v6->m_appRenderStates.m_data + 10;
    //    if (v43->m_value.m_data.i[0] != dword_D2509C) {
    //        result = CGxDevice::IRsDirty(v6, GxRs_FogColor);
    //        v43->m_value.m_data.i[0] = v42;
    //        v6 = g_theGxDevicePtr;
    //    }
    //    if (v6->m_context) {
    //        v44 = v6->m_appRenderStates.m_data + 21;
    //        if (v44->m_value.m_data.i[0]) {
    //            result = CGxDevice::IRsDirty(v6, GxRs_Texture0);
    //            v44->m_value.m_data.i[0] = 0;
    //            v6 = g_theGxDevicePtr;
    //        }
    //        if (v6->m_context) {
    //            v45 = v6->m_appRenderStates.m_data + 22;
    //            if (v45->m_value.m_data.i[0]) {
    //                result = CGxDevice::IRsDirty(v6, GxRs_Texture1);
    //                v45->m_value.m_data.i[0] = 0;
    //            }
    //        }
    //    }
    //}
}

// OFFSET: 0x7D3E10
void CMapRenderChunk::SetShaders(int32_t a1, int32_t a2) {
    //dword_D1D094 = 0;
    //dword_D1D090 = 0;
    CMapRenderChunk::s_currentShaderX[0] = nullptr;
    CMapRenderChunk::s_currentShaderX[1] = nullptr;
    CMapRenderChunk::s_currentShaderX[2] = nullptr;
    CMapRenderChunk::s_currentShaderX[3] = nullptr;
    //dword_D1D070 = 0;
    //dword_D1D074 = 0;
    //dword_D1D078 = 0;
    //dword_D1D07C = 0;
    //if (CMap::gTerrainPixelShadersValid) {
    //    auto v2 = (CMap::header.flags >> 2) & 1;
    //    v3 = a1;
    //    v6 = sub_873FF0();
    //    dword_D1D094 = CMap::GetPixelShader(v2, 1, a1);
    //    dword_D1D090 = CMap::GetPixelShader(v2, 0, a1);
    //    v4 = 1;
    //    v8 = CMapRenderChunk::s_currentShaderX;
    //    v7 = 4;
    //    while (1) {
    //        TerrainPixelShader = (CGxShader*)CMap::GetTerrainPixelShader(v2, v4, v6, v3, a2);
    //        if (v4 <= CGxDevice::Caps((char*)g_theGxDevicePtr)->m_numTmus && TerrainPixelShader && CGxShaderPermute::Valid(TerrainPixelShader)) {
    //            *v8 = TerrainPixelShader;
    //        }
    //        ++v8;
    //        ++v4;
    //        if (!--v7)
    //            break;
    //        v3 = a1;
    //    }
    //    if ((CMap::header.flags & 4) != 0) {
    //        if (CMap::enableTerrainShaderVertex)
    //            CMapRenderChunk::s_renderLayersFunc = sub_7D20A0;
    //        else
    //            CMapRenderChunk::s_renderLayersFunc = (int(__cdecl*)(_DWORD))sub_7D13F0;
    //    } else {
    //        CMapRenderChunk::s_renderLayersFunc = (int(__cdecl*)(_DWORD))sub_7D2520;
    //        if (!CMap::enableTerrainShaderVertex)
    //            CMapRenderChunk::s_renderLayersFunc = sub_7D1AD0;
    //    }
    //} else if ((CMap::header.flags & 4) != 0) {
    //    CMapRenderChunk::s_renderLayersFunc = CMapRenderChunk::RenderMultiPassAdditive;
    //} else {
        CMapRenderChunk::s_renderLayersFunc = CMapRenderChunk::RenderMultiPassAlpha;
    //}
}

// OFFSET: 0x7B9830
void CMapRenderChunk::FreeBuf() {
    if (!this->chunkBuf)
        return;

    if ((this->chunkBuf->unkFlags & 3) != 0)
        CMapRenderChunk::s_chunkBufBlockFreeList.LinkToTail(this->chunkBuf->block);
    else
        CMapRenderChunk::s_renderChunkBufFreeList.LinkToTail(this->chunkBuf);
    this->chunkBuf->renderChunk = nullptr;
    this->chunkBuf = nullptr;
}
