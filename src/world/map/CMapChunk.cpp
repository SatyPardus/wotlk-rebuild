#include "world/map/CMapChunk.hpp"
#include "world/map/CMap.hpp"

// OFFSET: 0x7C3D90
void CMapChunk::Initialize() {
    CMapRenderChunk::Initialize();
    //sub_7C3C60();
    //CMapChunk::s_geoToTex = -1.0 / flt_D254A8;
}

void CMapChunk::Create(SIffChunk* headerChunk, bool a3) {
    this->chunkHeaderPtr = headerChunk;
    this->ProcessIffChunks(a3);
    this->areaId = this->header->areaid;
    this->topLeftCoords.x = 17066.666 - 33.333332 * (double)this->cOffset.y;
    this->topLeftCoords.y = 17066.666 - this->cOffset.x * 33.333332;
    this->topLeftCoords.z = 0.0;
    header = this->header;
    this->topLeftCoords.z = header->position.z;
    this->lowQualityTexMap = header->low_quality_texture_map;
    this->predTexture = (uint8_t*)&header->predTex;
    this->CreateBounds();
    //this->CreateLiquids(a3);
    //this->CreateSoundEmitters(a3);
    this->unk_C = 0;
    if ((this->header->flags & 2) != 0)
        this->unk_C = 64;
    CMapBaseObjLink* link = this->parentLinkList.Head();
    CMapArea* area = (CMapArea*)link->ref;
    //this->CreateRefs(area, this->MCRF_ptr, this->header->nDoodadRefs, this->header->nMapObjRefs);
    area->mapChunks[16 * this->aIndex.y + this->aIndex.x] = this;
    this->unk_C |= 0x80u;
}

void CMapChunk::ProcessIffChunks(bool a3) {
    this->header = this->chunkHeaderPtr->Data<SMChunk>();

    int32_t remainingSize = this->chunkHeaderPtr->size - sizeof(SMChunk);
    for (SIffChunk* chunk = this->chunkHeaderPtr->Next(sizeof(SMChunk)); remainingSize > 0; chunk = chunk->Next()) {
        switch (chunk->token) {
        case 'MCNR':
            this->normals = chunk->Data<int8_t>();
            if (a3)
                chunk->size = 448;
            break;
        case 'MCSH':
            this->shadowMap = chunk->Data<uint8_t>();
            break;
        case 'MCVT':
            this->height = chunk->Data<float>();
            break;
        case 'MCSE':
            if (this->header->ofsSndEmitters)
                this->soundEmitters = chunk->Data<CWSoundEmitter>();
            break;
        case 'MCRF':
            this->MCRF_ptr = chunk->Data<uint32_t>();
            break;
        case 'MCLQ':
            if (this->header->sizeLiquid > 8) {
                this->liquid = chunk->Data<SMLiquidChunk>();
                chunk->size = this->header->sizeLiquid - 8;
            }
            break;
        case 'MCLY':
            this->layers = chunk->Data<SMLayer>();
            break;
        case 'MCAL':
            this->additionalShadowmap = chunk->Data<uint8_t>();
            break;
        case 'MCCV':
            this->vertexShading = chunk->Data<uint32_t>();
            break;
        default:
            
            break;
        }

        remainingSize -= sizeof(SIffChunk) + chunk->size;
    }
}

void CMapChunk::CreateBounds() {
    int32_t tileX = this->cOffset.x;
    int32_t tileY = this->cOffset.y;

    this->bbox.b.x = 17066.666f - 33.333332f * (tileY + 1);
    this->bbox.t.x = 17066.666f - 33.333332f * tileY;
    this->bbox.b.y = 17066.666f - 33.333332f * (tileX + 1);
    this->bbox.t.y = 17066.666f - 33.333332f * tileX;
    this->bbox.b.z = 3.4028235e38f;
    this->bbox.t.z = -3.4028235e38f;

    float* h = this->height;
    int count = 29;
    do {
        for (int i = 0; i < 5; i++) {
            float height = h[i];
            if (this->bbox.b.z > height)
                this->bbox.b.z = height;
            if (this->bbox.t.z < height)
                this->bbox.t.z = height;
        }
        h += 5;
    } while (--count);

    this->bbox.b.z += this->topLeftCoords.z;
    this->bbox.t.z += this->topLeftCoords.z;

    this->center.x = (this->bbox.b.x + this->bbox.t.x) * 0.5f;
    this->center.y = (this->bbox.b.y + this->bbox.t.y) * 0.5f;
    this->center.z = (this->bbox.b.z + this->bbox.t.z) * 0.5f;

    float ex = this->bbox.t.x - this->center.x;
    float ey = this->bbox.t.y - this->center.y;
    float ez = this->bbox.t.z - this->center.z;
    this->radius = sqrt(ex * ex + ey * ey + ez * ez);
}

// OFFSET: 0x7D3FE0
void CMapChunk::RenderPrep() {
    if (!this->renderChunk)
        this->Batch();

    if (this->renderChunk) {
        //    if (CWorld::shadowMipLevel || (v2 = CWorldScene::s_activeWorldView.y - this->center.y,
        //                                   v3 = CWorldScene::s_activeWorldView.z - this->center.z,
        //                                   v4 = CWorldScene::s_activeWorldView.x - this->center.x,
        //                                   v4 * v4 + v3 * v3 + v2 * v2 > 603729.0)) {
        //        this->renderChunk->unk_0A |= 0x10u;
        //    }
        this->renderChunk->RenderPrep();
    }
    //if ((CWorld::enables & Enable_DetailDoodads) != 0 && CWorld::detailDoodadDist > (double)this->distToCamera) {
    //    if (!this->detailDoodadInst)
    //        sub_7D3390(this);
    //    detailDoodadInst = (char*)this->detailDoodadInst;
    //    if (detailDoodadInst)
    //        sub_792FA0(detailDoodadInst);
    //}
}

// OFFSET: 0x7C5440
void CMapChunk::Batch() {
    if (this->bLoaded)
        return;

    //if (CMap::enableChunkBatching) {
    //    m_next = this->parentLinkList.m_terminator.m_next;
    //    if (((unsigned __int8)m_next & 1) != 0 || !m_next)
    //        m_next = 0;
    //    ref = (CMapArea*)m_next->ref;
    //    y = this->aIndex.y;
    //    v6[0] = this->aIndex.x & 0xFFFFFFFE;
    //    v6[1] = y & 0xFFFFFFFE;
    //    CMapArea::BatchChunks(ref, v6);
    //} else {
    //    v5 = CMap::AllocRenderChunk();
    this->renderChunk = CMap::AllocRenderChunk();
    this->renderChunk->AddBatch(this, nullptr, &this->topLeftCoords, 0);
    this->bLoaded = 1;
    //}
}

// OFFSET: 0x7C51B0
void CMapChunk::CreateIndices(char* buf, CMapRenderChunkState* state) {
    uint16_t baseVertex = state->maxVertexIndex != 0 ? state->maxVertexIndex + 1 : 0;
    int16_t indicesWritten = this->CreateIndices(buf, baseVertex);

    uint16_t minVertex = state->minVertexIndex;
    if (minVertex >= baseVertex)
        minVertex = baseVertex;
    state->minVertexIndex = minVertex;

    uint16_t maxVertex = CMapRenderChunk::s_maxVertexOffset + baseVertex;
    if (state->maxVertexIndex > maxVertex)
        maxVertex = state->maxVertexIndex;
    state->maxVertexIndex = maxVertex;

    state->indexCount += indicesWritten;
}

int16_t CMapChunk::CreateIndices(char* buf, int32_t baseVertex) {
    uint16_t* indexDst = reinterpret_cast<uint16_t*>(buf);
    int16_t totalIndices = 0;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // Holes are stored as a 4×4 bitmask (one bit per 2×2 block of cells).
            int holeMaskIndex = (row / 2) * 4 + (col / 2);
            if (this->header->holes & CMap::s_holeMask[holeMaskIndex])
                continue;

            // The five vertices that define this cell.
            // Each outer row has 9 verts, each inner row has 8 — so 17 per row-pair.
            uint16_t TL = baseVertex + col;
            uint16_t TR = baseVertex + col + 1;
            uint16_t C = baseVertex + col + 9;   // inner vertex, midpoint of cell
            uint16_t BL = baseVertex + col + 17; // next outer row
            uint16_t BR = baseVertex + col + 18;

            // Four triangles, all fanning from the center point (CCW winding).
            indexDst[0] = C;
            indexDst[1] = TL;
            indexDst[2] = BL; // left
            indexDst[3] = C;
            indexDst[4] = TR;
            indexDst[5] = TL; // top
            indexDst[6] = C;
            indexDst[7] = BR;
            indexDst[8] = TR; // right
            indexDst[9] = C;
            indexDst[10] = BL;
            indexDst[11] = BR; // bottom
            indexDst += 12;
            totalIndices += 12;
        }

        // Advance past this row-pair (9 outer + 8 inner vertices).
        baseVertex += 17;
    }

    return totalIndices;
}

// OFFSET: 0x7C54C0
void CMapChunk::CreateVertices(char* buf, int32_t bufOffset) {
    //if (CMap::enableTerrainShaderVertex) {
    //    if (CMapRenderChunk::s_gxBufVertexFormat == 1)
    //        CMapChunk::CreateVerticesWorld((CGxVertexPN*)&a2[6 * a3]);
    //    else
    //        CMapChunk::CreateVerticesWorld((CGxVertexPNC*)&a2[7 * a3]);
    //} else if (CMapRenderChunk::s_gxBufVertexFormat == 1) {
        CMapChunk::CreateVerticesLocal(buf);
    //} else {
    //    CMapChunk::CreateVerticesLocal((CGxVertexPNC*)a2);
    //}
}

// OFFSET: 0x7C4960
void CMapChunk::CreateVerticesLocal(char* buf) {
    CGxVertexPN* v = reinterpret_cast<CGxVertexPN*>(buf);

    // ~1/127: scales packed signed-byte normals into [-1, 1] float range
    static const float NORMAL_SCALE = 0.0078740157f;

    // Both expressions simplify to: 33.333332f * 0.125f = ~4.1667 units/step
    const float stepX = (17066.666f - (float)this->cOffset.x * 33.333332f - (17066.666f - (float)(this->cOffset.x + 1) * 33.333332f)) * -0.125f;
    const float stepY = (17066.666f - (float)this->cOffset.y * 33.333332f - (17066.666f - (float)(this->cOffset.y + 1) * 33.333332f)) * -0.125f;

    const float halfStepX = stepX * 0.5f;

    float* height = this->height;
    int8_t* normals = this->normals;

    for (int row = 0; row < 9; row++) {
        // --- Outer grid row: 9 evenly-spaced vertices ---
        const float outerX = (float)row * stepY;

        for (int col = 0; col < 9; col++) {
            v->position.x = outerX;
            v->position.y = (float)col * stepX;
            v->position.z = height[col];
            v->normal.x = normals[col * 3 + 0] * NORMAL_SCALE;
            v->normal.y = normals[col * 3 + 1] * NORMAL_SCALE;
            v->normal.z = normals[col * 3 + 2] * NORMAL_SCALE;
            v++;
        }

        height += 9;
        normals += 27; // 9 vertices * 3 components

        // --- Inner (center-point) grid row: 8 vertices, staggered by half a step ---
        // The last outer row (row == 8) has no corresponding inner row
        if (row < 8) {
            const float innerX = (float)row * stepX + 0.5f * stepY;

            for (int col = 0; col < 8; col++) {
                v->position.x = innerX;
                v->position.y = (float)col * stepY + halfStepX;
                v->position.z = height[col];
                v->normal.x = normals[col * 3 + 0] * NORMAL_SCALE;
                v->normal.y = normals[col * 3 + 1] * NORMAL_SCALE;
                v->normal.z = normals[col * 3 + 2] * NORMAL_SCALE;
                v++;
            }

            height += 8;
            normals += 24; // 8 vertices * 3 components
        }
    }
}
