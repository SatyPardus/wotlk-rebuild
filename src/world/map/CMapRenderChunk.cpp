#include "world/map/CMapRenderChunk.hpp"
#include "world/map/CMapChunk.hpp"
#include "world/map/CMap.hpp"
#include "gx/Device.hpp"
#include <world/CWorld.hpp>

STORM_EXPLICIT_LIST(CMapRenderChunkBufBlock, blockLink) CMapRenderChunk::s_bufList;
STORM_EXPLICIT_LIST(CMapRenderChunkBufBlock, bufLink) CMapRenderChunk::s_chunkBufBlockFreeList;
STORM_EXPLICIT_LIST(CMapRenderChunkBuf, unk_14) CMapRenderChunk::s_renderChunkBufFreeList;
TSGrowableArray<CMapRenderChunkBufBlock> CMapRenderChunk::s_chunkBlockArray;
bool CMapRenderChunk::s_bPoolsDirty;
CGxPool* CMapRenderChunk::s_gxVertexPool;
CGxPool* CMapRenderChunk::s_gxIndexPool;
EGxVertexBufferFormat CMapRenderChunk::s_gxBufVertexFormat;
int16_t CMapRenderChunk::s_maxVertexCount = 145;
int16_t CMapRenderChunk::s_maxVertexOffset;
int32_t CMapRenderChunk::s_pnEstimateVertex;
int32_t CMapRenderChunk::s_pnEstimateIndex;

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
    //memset(&unk_D1D018, 0, 0x40u);
    //memset(&unk_D1CFD8, 0, 0x40u);
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
    newLayer->unkValue = 0;
    newLayer->owner = this;

    if ((newLayer->flags & 0x80) != 0)
        this->unk_0A |= 1u;

    if (g_theGxDevicePtr->Caps().m_texTarget[1] && g_theGxDevicePtr->Caps().m_shaderTargets[0] && g_theGxDevicePtr->Caps().m_shaderTargets[4] && (newLayer->flags & 0x400)) {
        this->unk_0A |= 4u;
    }

    ++this->layersCount;
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
    if (CMapRenderChunk::s_gxBufVertexFormat == 2)
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
        auto bufData = g_theGxDevicePtr->BufLock(vertexBuf);

        if (this->mapChunkPtrs[0]) {
            this->mapChunkPtrs[0]->CreateVertices(bufData, 0);
        }

        if (this->mapChunkPtrs[1]) {
            this->mapChunkPtrs[1]->CreateVertices(bufData, 145);
        }

        g_theGxDevicePtr->BufUnlock(vertexBuf, 0);

        vertexBuf->unk1C = 1;
    }

    if (!indexBuf->unk1C || !indexBuf->unk1D) {
        this->state.unk_00 = 3;
        this->state.unk_04 = 0;
        this->state.indexCount = 0;
        this->state.minVertexIndex = -1;
        this->state.maxVertexIndex = 0;

        auto bufData = g_theGxDevicePtr->BufLock(indexBuf);

        if (this->mapChunkPtrs[0]) {
            this->mapChunkPtrs[0]->CreateIndices(bufData, &this->state);
        }

        if (this->mapChunkPtrs[1]) {
            this->mapChunkPtrs[1]->CreateIndices(&bufData[this->state.indexCount], &this->state);
        }

        g_theGxDevicePtr->BufUnlock(vertexBuf, 0);

        indexBuf->unk1C = 1;
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
