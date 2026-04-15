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

    if ((this->unk_0A & 0x20) == 0 && ((this->unk_0A & 0x10) != 0 && (this->unk_0A & 0x8) != 0) || (this->unk_0A & 0x8) == 0) {
        this->unk_0A &= 0xFFCF;
        return;
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
            this->AllocShadowTexture();
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

        HTEXTURE v12;
        if (v4)
            v12 = CMapRenderChunk::AllocTexture(v10, v10, layer, CMapRenderChunk::UpdateLayerGxTexture, GxTex_Argb8888, 2u);
        else
            v12 = CMapRenderChunk::AllocTexture(v10, v10, layer, CMapRenderChunk::UpdateLayerGxTexture, GxTex_Argb4444, 3u);
        layer->layerTexture = v12;
        CGxTex* tex = TextureGetGxTex(v12, 1, nullptr);
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
        // CMapRenderChunk::CreateShaderTexture(renderChunk);
        //*texels = CMapRenderChunk::s_defaultTex;
        // v8 = 4 * w;
        // if ((CMap::header.flags & 4) == 0)
        //     v8 = 2 * w;
        //*texelStrideInBytes = v8;
    }
}

// OFFSET: 0x7B9BC0
void CMapRenderChunk::UpdateLayerGxTexture(EGxTexCommand cmd, uint32_t w, uint32_t h, uint32_t d, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CMapRenderChunkLayer* layer = static_cast<CMapRenderChunkLayer*>(userArg);

    if (cmd == GxTex_Latch) {
        // CMapRenderChunk::CreateChunkLayerTex(a6->owner, (int)a6);
        // v8 = 4 * a2;
        // if ((CMap::header.flags & 4) == 0)
        //     v8 = 2 * a2;
        //*a7 = v8;
        //*(_DWORD*)a8 = CMapRenderChunk::s_defaultTex;
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
    this->unk_0C = 0.0;
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
    CGxTex* defaultTexture = TextureGetGxTex(this->layers[0].texture, 0, nullptr);
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

void sub_7D0D70(CMapRenderChunk* renderChunk) {
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

        //if (chunkLayer->flags & 0x80)
            GxRsSet(GxRs_Lighting, 0);

        // Flag bit 6: apply texture coordinate translation
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
        //GxRsSet(GxRs_Texture0, TextureGetGxTex(texturepointer, 1, 0));
        //GxRsSet(GxRs_Texture1, TextureGetGxTex(renderChunk->shadowTexture, 1, 0));
        //g_theGxDevicePtr->Draw(&renderChunk->batch, 1);
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, nullptr);
}

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
    //    v2 = (CMap::header.flags >> 2) & 1;
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
    //    CMapRenderChunk::s_renderLayersFunc = sub_7D0760;
    //} else {
        CMapRenderChunk::s_renderLayersFunc = sub_7D0D70;
    //}
}
