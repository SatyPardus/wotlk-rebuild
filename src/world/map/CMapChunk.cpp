#include "world/map/CMapChunk.hpp"
#include "world/map/CMap.hpp"

C3Vector CMapChunk::vertexList[145];
int32_t CMapChunk::cornerVertexIndex[4] = { 0, 8, 0x88, 0x90 };
int32_t CMapChunk::farCornerIndex;
float CMapChunk::s_geoToTex;

// OFFSET: 0x7C5C50
CMapChunk::CMapChunk() {
    this->aIndex.x = 0;
    this->aIndex.y = 0;
    this->sOffset.x = 0;
    this->sOffset.y = 0;
    this->cOffset.x = 0;
    this->cOffset.y = 0;
    this->sphere.r = 0.0;
    this->sphere.c.x = 0.0;
    this->sphere.c.y = 0.0;
    this->sphere.c.z = 0.0;
    this->bbox.b.x = 0.0;
    this->bbox.b.y = 0.0;
    this->unk_B4 = 0;
    this->bbox.b.z = 0.0;
    this->unk_B8 = 0;
    this->bbox.t.x = 0.0;
    this->sortListLink.m_prevlink = nullptr;
    this->bbox.t.y = 0.0;
    this->sortListLink.m_next = nullptr;
    this->bbox.t.z = 0.0;
    this->doodadDefLinkList.m_terminator.m_next = 0;
    this->bottomRight.x = 0.0;
    this->bottomRight.y = 0.0;
    this->bottomRight.z = 0.0;
    this->topLeft.x = 0.0;
    this->topLeft.y = 0.0;
    this->topLeft.z = 0.0;
    this->topLeftCoords.x = 0.0;
    this->topLeftCoords.y = 0.0;
    this->topLeftCoords.z = 0.0;
    this->bbox2.b.x = 0.0;
    this->bbox2.b.y = 0.0;
    this->bbox2.b.z = 0.0;
    this->bbox2.t.x = 0.0;
    this->bbox2.t.y = 0.0;
    this->bbox2.t.z = 0.0;
    //this->doodadDefLinkList.m_terminator.m_prevLink = &this->doodadDefLinkList.m_terminator;
    //this->doodadDefLinkList.m_linkoffset = 12;
    //this->doodadDefLinkList.m_terminator.m_next = (CMapChunkDoodadDefLink*)((unsigned int)&this->doodadDefLinkList.m_terminator | 1);
    //this->mapObjDefLinkList.m_terminator.m_next = 0;
    //this->mapObjDefLinkList.m_linkoffset = 12;
    //this->mapObjDefLinkList.m_terminator.m_prevLink = &this->mapObjDefLinkList.m_terminator;
    //this->mapObjDefLinkList.m_terminator.m_next = (CMapChunkMapObjDefLink*)((unsigned int)&this->mapObjDefLinkList.m_terminator | 1);
    //this->TSExplicitList__ptr2_E4 = 0;
    //this->TSExplicitList__ptr_E0 = &this->TSExplicitList__ptr_E0;
    //this->TSExplicitList__ptr2_E4 = (void*)((unsigned int)&this->TSExplicitList__ptr_E0 | 1);
    //this->TSExplicitList__m_linkoffset_DC = 12;
    //this->TSExplicitList__ptr2_F0 = 0;
    //this->TSExplicitList__ptr_EC = &this->TSExplicitList__ptr_EC;
    //this->TSExplicitList__ptr2_F0 = (void*)((unsigned int)&this->TSExplicitList__ptr_EC | 1);
    //this->TSExplicitList__m_linkoffset_E8 = 12;
    //this->TSExplicitList__ptr2_FC = 0;
    //this->TSExplicitList__ptr_F8 = &this->TSExplicitList__ptr_F8;
    //this->TSExplicitList__ptr2_FC = (void*)((unsigned int)&this->TSExplicitList__ptr_F8 | 1);
    //this->TSExplicitList__m_linkoffset_F4 = 0;
    //this->liquidChunkLinkList.m_terminator.m_next = 0;
    //this->liquidChunkLinkList.m_terminator.m_prevLink = &this->liquidChunkLinkList.m_terminator;
    //this->liquidChunkLinkList.m_linkoffset = 112;
    //this->liquidChunkLinkList.m_terminator.m_next = (CChunkLiquid*)((unsigned int)&this->liquidChunkLinkList.m_terminator | 1);
    //sub_95DA10(&this->unk_140);
    //sub_95DA10(&this->unk_14C);
    this->type |= 4u;
    this->distToCamera = 0.0;
    this->unk_C |= 1u;
    this->detailDoodadInst = nullptr;
    this->renderChunk = nullptr;
    this->chunkHeaderPtr = nullptr;
    this->header = nullptr;
    this->lowQualityTexMap = nullptr;
    this->predTexture = nullptr;
    this->height = nullptr;
    this->vertexShading = nullptr;
    this->normals = nullptr;
    this->shadowMap = nullptr;
    this->layers = nullptr;
    this->additionalShadowmap = nullptr;
    this->MCRF_ptr = nullptr;
    this->liquid = nullptr;
    this->soundEmitters = nullptr;
    this->bLoaded = 0;
}

// OFFSET: 0x7C3D90
void CMapChunk::Initialize() {
    CMapRenderChunk::Initialize();
    CMapChunk::InitializeVertexGrid();
    CMapChunk::s_geoToTex = -1.0 / CMapChunk::vertexList[1].y;
}

// OFFSET: 7C3C60
void CMapChunk::InitializeVertexGrid() {
    const float OUTER_STEP = 100.0f / 24.0f;
    const float INNER_OFFSET = OUTER_STEP / 2.0f;

    int vertIndex = 0;

    for (int row = 0; row < 9; row++) {
        float outerX = (float)row * -OUTER_STEP;

        for (int col = 0; col < 9; col++) {
            CMapChunk::vertexList[vertIndex].x = outerX;
            CMapChunk::vertexList[vertIndex].y = (float)col * -OUTER_STEP;
            CMapChunk::vertexList[vertIndex].z = 0.0f;
            vertIndex++;
        }

        if (row < 8) {
            float innerX = outerX - INNER_OFFSET;

            for (int col = 0; col < 8; col++) {
                CMapChunk::vertexList[vertIndex].x = innerX;
                CMapChunk::vertexList[vertIndex].y = ((float)col * -OUTER_STEP) - INNER_OFFSET;
                CMapChunk::vertexList[vertIndex].z = 0.0f;
                vertIndex++;
            }
        }
    }
}

// OFFSET: 0x7C64B0
void CMapChunk::Create(SIffChunk* headerChunk, bool a3) {
    this->chunkHeaderPtr = headerChunk;
    this->ProcessIffChunks(a3);
    this->areaId = this->header->areaid;
    this->topLeftCoords.x = 17066.666f - 33.333332f * this->cOffset.y;
    this->topLeftCoords.y = 17066.666f - this->cOffset.x * 33.333332f;
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
    this->CreateRefs(area, this->MCRF_ptr, this->header->nDoodadRefs, this->header->nMapObjRefs);
    area->mapChunks[16 * this->aIndex.y + this->aIndex.x] = this;
    this->unk_C |= 0x80u;
}

// OFFSET: 0x
void CMapChunk::CreateRefs(CMapArea* area, uint32_t* mcrfPtr, uint32_t doodadRefs, uint32_t mapObjRefs) {
    C3Vector center = { 17066.666f, 17066.666f, 0.0f };
    //v5 = area;
    //v7 = 0;
    //mcrfMapObjPtr = &mcrfPtr[doodadRefs];
    //for (i = 0; i < mapObjRefs; ++i) {
    //    v8 = &area->mapObjDef[mcrfMapObjPtr[v7]];
    //    if ((v8->flags & 1) != 0) {
    //        if (v8->uniqueId == 5535469 && !sub_7BDDF0((TSHashTable_CMapObjDef_HASHKEY_NONE*)&CMap::mapObjDefHash, 0x5476EDu, (int)&unk_CE04A3)) {
    //            m_next = stru_AEEDE0.m_terminator.m_next;
    //            if (((int)stru_AEEDE0.m_terminator.m_next & 1) != 0 || !stru_AEEDE0.m_terminator.m_next)
    //                m_next = 0;
    //            while (((unsigned __int8)m_next & 1) == 0 && m_next) {
    //                if (m_next[7] == v8->uniqueId)
    //                    goto LABEL_20;
    //                m_next = *(_DWORD**)((char*)m_next + stru_AEEDE0.m_linkoffset + 4);
    //            }
    //            v10 = -v8->extents.max.z;
    //            v35 = 0;
    //            x = v8->extents.max.x;
    //            v36 = 0;
    //            v12 = v8->extents.min.y + center.z;
    //            v13 = -v8->extents.min.z;
    //            v42 = -v8->extents.min.x;
    //            y = v8->extents.max.y;
    //            v14 = center.x + v13;
    //            v15 = center.y + v42;
    //            v16 = center.z + y;
    //            v17 = center.y - x + v15;
    //            v46 = v17;
    //            v18 = v17;
    //            v19 = v12 + v16;
    //            v47 = v19;
    //            v44 = (v10 + center.x + v14) * 0.5;
    //            v45 = v46 * 0.5;
    //            v48 = v44;
    //            v37 = v44;
    //            uniqueId = v8->uniqueId;
    //            v49 = v18 * 0.5;
    //            v38 = v49;
    //            v50 = 0.5 * v19;
    //            v39 = v50;
    //            v20 = sqrt((v16 - v47 * 0.5) * (v16 - v47 * 0.5) + (v15 - v45) * (v15 - v45) + (v14 - v44) * (v14 - v44));
    //            v40 = v20;
    //            if (v20 < 0.001) {
    //                nameId = v8->nameId;
    //                wmoFilenamesOffsets = area->wmoFilenamesOffsets;
    //                v40 = 50.0;
    //                SysMsgPrintf_0(
    //                    2,
    //                    2,
    //                    "Destructible building WMO(%s) has invalid geobox",
    //                    &area->wmoFileNames[wmoFilenamesOffsets[nameId]]);
    //            }
    //            sub_77F290(v34);
    //            if (v35) {
    //                if ((v36 & 1) == 0 && v36) {
    //                    *(int*)((char*)&v35 + v36 - *(_DWORD*)(v35 + 4)) = v35;
    //                    *(_DWORD*)(v35 + 4) = v36;
    //                } else {
    //                    *(_DWORD*)(v36 & 0xFFFFFFFE) = v35;
    //                    *(_DWORD*)(v35 + 4) = v36;
    //                }
    //            }
    //        }
    //    } else {
    //        MapObjDef_0 = (CMapBaseObj*)CMap::CreateMapObjDef_0(
    //            (int)&area->wmoFileNames[area->wmoFilenamesOffsets[v8->nameId]],
    //            (int)v8,
    //            &center.x,
    //            1);
    //        v24 = CMap::AllocBaseObjLink(MapObjDef_0);
    //        v24->ref = this;
    //        HashTable::AddEntry((TSList*)&this->mapObjDefLinkList, (char*)v24);
    //    }
//LABEL_20:
    //    v7 = i + 1;
    //}
    for (int32_t i = 0; i < doodadRefs; i++) {
        CMapDoodadDef* mapDoodadDef = CMap::CreateDoodadDef(&area->m2FileNames[area->modelFilenamesOffsets[area->doodadDef[mcrfPtr[i]].nameId]], &area->doodadDef[mcrfPtr[i]], &center);
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapDoodadDef);
        link->ref = this;
        this->doodadDefLinkList.LinkToTail(link);
        mapDoodadDef->unk_08C = 1.0f;
        mapDoodadDef->unk_C |= 4;
        if ((mapDoodadDef->unk_C & 0x80) != 0) {
            //sub_7B4FA0(v25, this);
        }
    }
    //i = 0;
    //if (doodadRefs) {
    //    while (1) {
    //        v25 = CMap::CreateDoodadDef(
    //            &area->m2FileNames[area->modelFilenamesOffsets[area->doodadDef[mcrfPtr[i]].nameId]],
    //            &area->doodadDef[mcrfPtr[i]],
    //            &center);
    //        v26 = CMap::AllocBaseObjLink((CMapBaseObj*)v25);
    //        v26->ref = this;
    //        m_linkoffset = this->doodadDefLinkList.m_linkoffset;
    //        v28 = *(uint32_t*)((char*)&v26->objectIndex + m_linkoffset);
    //        v29 = (TSLink_CMapChunkDoodadDefLink*)((char*)v26 + m_linkoffset);
    //        if (v28) {
    //            v30 = v29->m_next;
    //            if (((unsigned __int8)v30 & 1) == 0 && v30) {
    //                v31 = (uint32_t*)((char*)&v30->objectIndex + (_DWORD)v29 - *(_DWORD*)(v28 + 4));
    //            } else {
    //                v31 = (_DWORD*)((unsigned int)v30 & 0xFFFFFFFE);
    //            }
    //            *v31 = v28;
    //            v29->m_prevLink->m_next = v29->m_next;
    //            v29->m_prevLink = 0;
    //            v29->m_next = 0;
    //        }
    //        m_prevLink = this->doodadDefLinkList.m_terminator.m_prevLink;
    //        v29->m_prevLink = m_prevLink;
    //        v29->m_next = m_prevLink->m_next;
    //        m_prevLink->m_next = (CMapChunkDoodadDefLink*)v26;
    //        this->doodadDefLinkList.m_terminator.m_prevLink = v29;
    //        v25->unk_08C = 1.0;
    //        v25->unk_C |= 4u;
    //        if ((v25->unk_C & 0x80u) != 0)
    //            sub_7B4FA0(v25, this);
    //        if (++i >= doodadRefs)
    //            break;
    //    }
    //}
}

// OFFSET: 0x7C3A10
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

// OFFSET: 0x7C5220
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

    this->sphere.c.x = (this->bbox.b.x + this->bbox.t.x) * 0.5f;
    this->sphere.c.y = (this->bbox.b.y + this->bbox.t.y) * 0.5f;
    this->sphere.c.z = (this->bbox.b.z + this->bbox.t.z) * 0.5f;

    this->bbox2 = this->bbox;

    float ex = this->bbox.t.x - this->sphere.c.x;
    float ey = this->bbox.t.y - this->sphere.c.y;
    float ez = this->bbox.t.z - this->sphere.c.z;
    this->sphere.r = sqrt(ex * ex + ey * ey + ez * ez);
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
    this->renderChunk = CMap::AllocRenderChunk();
    this->renderChunk->AddBatch(this, nullptr, &this->topLeftCoords, 0);
    this->bLoaded = 1;
    //}
}

// OFFSET: 0x7C51B0
void CMapChunk::CreateIndices(char* buf, CGxBatch* batch) {
    uint16_t baseVertex = batch->m_maxIndex != 0 ? batch->m_maxIndex + 1 : 0;
    int16_t indicesWritten = this->CreateIndices(buf, baseVertex);

    uint16_t minVertex = batch->m_minIndex;
    if (minVertex >= baseVertex)
        minVertex = baseVertex;
    batch->m_minIndex = minVertex;

    uint16_t maxVertex = CMapRenderChunk::s_maxVertexOffset + baseVertex;
    if (batch->m_maxIndex > maxVertex)
        maxVertex = batch->m_maxIndex;
    batch->m_maxIndex = maxVertex;

    batch->m_count += indicesWritten;
}

// OFFSET: 0x7C3B60
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
