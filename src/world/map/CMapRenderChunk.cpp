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

// OFFSET: 0x7D04A0
void CMapRenderChunk::RenderSetup(int32_t a2) {
    this->unk_0C = 0.0;
    //CMapRenderChunk::AllocLayerTextures(this);
    //if (!a2 || !CMap::enableTerrainShaderVertex) {
    //    v11.M11 = 1.0;
    //    v11.M12 = 0.0;
    //    v11.M13 = 0.0;
    //    v11.M14 = 0.0;
    //    v11.M21 = 0.0;
    //    v11.M23 = 0.0;
    //    v11.M24 = 0.0;
    //    v11.M31 = 0.0;
    //    v11.M32 = 0.0;
    //    v11.M34 = 0.0;
    //    v11.M22 = 1.0;
    //    v11.M33 = 1.0;
    //    v11.M44 = 1.0;
    //    v12 = this->vec1.x - CWorldScene::s_activeWorldView.x;
    //    y = this->vec1.y;
    //    v11.M41 = v12;
    //    v4 = &g_theGxDevicePtr->ukn1[605];
    //    v13 = y - CWorldScene::s_activeWorldView.y;
    //    z = this->vec1.z;
    //    v11.M42 = v13;
    //    v6 = z - CWorldScene::s_activeWorldView.z;
    //    v7 = &g_theGxDevicePtr->ukn1[g_theGxDevicePtr->ukn1[605] + 671];
    //    LOBYTE(g_theGxDevicePtr->ukn1[606]) = 1;
    //    *v7 &= ~1u;
    //    v14 = v6;
    //    v8 = *v4;
    //    v11.M43 = v14;
    //    C44Matrix::Copy((C44Matrix*)&v4[16 * v8 + 2], &v11);
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
