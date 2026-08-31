#include "world/map/CMapObjGroup.hpp"
#include "async/AsyncFile.hpp"
#include "world/map/CMapObj.hpp"
#include <bsp/CAaBspQuery.hpp>
#include <bsp/BspQuery.hpp>
#include <world/CWorld.hpp>
#include "world/map/CMapObjDef.hpp"
#include "world/World.hpp"
#include <tempest/facet/CFacet.hpp>
#include "world/map/CMap.hpp"
#include <world/daynight/DayNight.hpp>
#include <world/daynight/DNInfo.hpp>
#include <gx/RenderState.hpp>

VBBList CMapObjGroup::vertexVBList;
VBBList CMapObjGroup::indexVBList;

// OFFSET: 0x7D82E0
void CMapObjGroup::Create() {
    this->parent->mapObjGroupList.LinkToTail(this);

    SIffChunk* headerChunk = reinterpret_cast<SIffChunk*>(static_cast<char*>(this->filePtr) + 12);
    auto header = headerChunk->Data<SMOGroupHeader>();

    this->groupName = &this->parent->groupNameList[header->groupName];
    this->flags = header->flags;
    if (!this->parent->skybox)
        this->flags &= 0xFFFBFFFF;
    this->bbox = header->boundingBox;
    this->portalStart = header->portalStart;
    this->portalCount = header->portalCount;
    this->transparencyBatchesCount = header->transBatchCount;
    this->intBatchCount = header->intBatchCount;
    this->extBatchCount = header->extBatchCount;
    this->fogs = header->fogIds;
    this->wmoGroupId = header->uniqueID;

    //if ((parent->header->flags & 4) != 0) {
    //    LiquidType = CMapObjGroup::GetLiquidType(this, v4->groupLiquid);
    //} else {
    //    groupLiquid = v4->groupLiquid;
    //    if (groupLiquid == 15)
    //        v11 = 0;
    //    else
    //        v11 = groupLiquid + 1;
    //    LiquidType = CMapObjGroup::GetLiquidType(this, v11);
    //}
    //this->liquidType = LiquidType;
    this->CreateDataPointers(reinterpret_cast<SIffChunk*>(static_cast<char*>(this->filePtr) + 0x58));
    for (int32_t i = 0; i < this->batchListCount; i++) {
        this->parent->CreateMaterial(this->batchList[i].texture);
    }
    this->unkLoadedFlag = (this->unkLoadedFlag & ~0x3) | 1;
    if ((this->parent->header->flags & 1) != 0)
        this->unkLoadedFlag |= 2;
    //result = (unsigned __int8*)SStrCmpN(this->groupName, "antiportal", 0x7FFFFFFFu);
    //if (!result)
    //    result = (unsigned __int8*)CMapObjGroup::HandleAntiportal(this);
    this->unkLoadedFlag |= 4u;
    //batchListCount = this->batchListCount;
    //v17 = 0;
    //if (batchListCount) {
    //    this->unkIndexMin1 = -1;
    //    this->unkIndexMax1 = 0;
    //    this->unkIndexMin2 = -1;
    //    this->unkIndexMax2 = 0;
    //    p_vertexEnd = &this->batchList->vertexEnd;
    //    while (!v23->materialList[*((unsigned __int8*)p_vertexEnd + 3)].blendMode) {
    //        v19 = *(p_vertexEnd - 2) + *((_DWORD*)p_vertexEnd - 2) - 1;
    //        if (this->unkIndexMax2 < *p_vertexEnd)
    //            this->unkIndexMax2 = *p_vertexEnd;
    //        v20 = *(p_vertexEnd - 1);
    //        if (this->unkIndexMin2 > v20)
    //            this->unkIndexMin2 = v20;
    //        v21 = *((_DWORD*)p_vertexEnd - 2);
    //        if (this->unkIndexMin1 > v21)
    //            this->unkIndexMin1 = v21;
    //        if (this->unkIndexMax1 < v19)
    //            this->unkIndexMax1 = v19;
    //        ++v17;
    //        p_vertexEnd += 12;
    //        if (v17 >= batchListCount)
    //            goto LABEL_31;
    //    }
    //    this->unkLoadedFlag &= ~4u;
//LABEL_31:
    //    v22 = 0;
    //    result = &this->batchList->texture;
    //    while (v23->materialList[*result].shader != 6) {
    //        ++v22;
    //        result += 24;
    //        if (v22 >= this->batchListCount)
    //            return result;
    //    }
    //    this->unkLoadedFlag |= 8u;
    //}
}

// OFFSET: 0x7D7F50
void CMapObjGroup::CreateDataPointers(SIffChunk* polyListChunk) {
    this->polyList = polyListChunk->Data<SMOPoly>();
    this->polyListSize = polyListChunk->size / sizeof(SMOPoly);

    SIffChunk* indicesChunk = polyListChunk->Next();
    this->indices = indicesChunk->Data<uint16_t>();
    this->indicesCount = indicesChunk->size / sizeof(uint16_t);

    SIffChunk* vertexListChunk = indicesChunk->Next();
    this->vertexList = vertexListChunk->Data<C3Vector>();
    this->vertexListCount = vertexListChunk->size / sizeof(C3Vector);

    SIffChunk* normalListChunk = vertexListChunk->Next();
    this->normalList = normalListChunk->Data<C3Vector>();
    this->normalListCount = normalListChunk->size / sizeof(C3Vector);

    SIffChunk* textureVertexListChunk = normalListChunk->Next();
    this->textureVertexList = textureVertexListChunk->Data<C2Vector>();
    this->textureVertexListCount = textureVertexListChunk->size / sizeof(C2Vector);

    SIffChunk* batchListChunk = textureVertexListChunk->Next();
    this->batchList = batchListChunk->Data<SMOBatch>();
    this->batchListCount = batchListChunk->size / sizeof(SMOBatch);

    this->CreateOptionalDataPointers(batchListChunk->Next());
}

// OFFSET: 0x7D7C30
void CMapObjGroup::CreateOptionalDataPointers(SIffChunk* dataChunk) {
    if ((this->flags & 0x200) != 0) {
        //this->unk_104 = &a2->data;
        //this->unk_174 = a2->size >> 1;
        dataChunk = dataChunk->Next();
    }
    if ((this->flags & 0x800) != 0) {
        this->doodadRefList = dataChunk->Data<uint16_t>();
        this->doodadRefListCount = dataChunk->size / sizeof(uint16_t);
        dataChunk = dataChunk->Next();
    }
    if ((this->flags & 1) != 0) {
        CAaBspNode* nodes = dataChunk->Data<CAaBspNode>();
        uint32_t nodesSize = dataChunk->size / sizeof(CAaBspNode);
        dataChunk = dataChunk->Next();
        uint16_t* indices = dataChunk->Data<uint16_t>();
        uint32_t indicesCount = dataChunk->size / sizeof(uint16_t);
        dataChunk = dataChunk->Next();
        this->CAaBspNodePtr1.Set(nodes, nodesSize, indices, indicesCount, this->bbox);
    }
    if ((this->flags & 0x400) != 0) {
        //v13 = &v2[2] + v2->size + *(&v2[1].token + v2->size) + *(&v2[1].data + v2->size + *(&v2[1].token + v2->size));
        //v2 = &v13[*(v13 + 1) + 8];

        dataChunk = dataChunk->Next();
        dataChunk = dataChunk->Next();
        dataChunk = dataChunk->Next();
        dataChunk = dataChunk->Next();
    }
    if ((this->flags & 4) != 0) {
        parent = this->parent;
        this->colorVertexList = dataChunk->Data<CImVector>();
        this->colorVertexListSize = dataChunk->size / sizeof(CImVector);
        dataChunk = dataChunk->Next();
        if ((parent->header->flags & 8) == 0)
            this->FixColorVertexAlpha();
    }
    if ((this->flags & 0x1000) != 0) {
        //data = v2->data;
        //v18 = &v2->data;
        //this->liquidVerts.x = data;
        //v19 = v18[1];
        //v18 += 2;
        //this->liquidVerts.y = v19;
        //this->liquidTiles.x = *v18;
        //v20 = v18[1];
        //v18 += 2;
        //this->liquidTiles.y = v20;
        //LODWORD(this->liquidCorner.x) = *v18;
        //LODWORD(this->liquidCorner.y) = v18[1];
        //v21 = this->liquidVerts.x * this->liquidVerts.y;
        //LODWORD(this->liquidCorner.z) = v18[2];
        //LOWORD(v19) = *(v18 + 6);
        //v22 = this->liquidTiles.x * this->liquidTiles.y;
        //v18 = (v18 + 14);
        //this->liquidVertexList = v18;
        //v23 = &v18[2 * v21];
        //this->liquidTileList = v23;
        //v2 = &v23[v22];
        //v24 = this->liquidType == 0;
        //LOWORD(this->luquidMaterialId) = v19;
        //if (v24) {
        //    v25 = WMOGroup::sub_7C8D80(this);
        //    this->liquidType = CMapObjGroup::GetLiquidType(this, v25);
        //}
        //bn_CMapObjGroup_AllocVertArray(&this->unk_1C, this->liquidVerts.x * this->liquidVerts.y);
        //bn_CMapObjGroup_GenLiquidVerts(this);
    }
    if ((this->flags & 0x20000) != 0) {
        //v26 = v2;
        //v27 = &v2->data;
        //this->unk_E8 = v27;
        //this->unk_15C = v26->size >> 1;
        //v2 = (v27 + v26->size + 8);
        //this->unk_100 = v2;
        //if (byte_CE049C) {
        //    v28 = 0;
        //    if (this->batchListCount) {
        //        v29 = 0;
        //        do {
        //            this->batchList[v29].indexStart = *(this->unk_100 + 8 * v28);
        //            this->batchList[v29++].indexCount = *(this->unk_100 + 8 * v28++ + 4);
        //        } while (v28 < this->batchListCount);
        //    }
        //}
    }
    if ((this->flags & 0x2000000) != 0) {
        //v31 = v2;
        //v32 = &v2->data;
        //this->unk_F8 = v32;
        //this->unk_16C = v31->size >> 3;
        dataChunk = dataChunk->Next();
    }
    if ((this->flags & 0x1000000) != 0) {
        this->colorVertexListExtra = dataChunk->Data<CImVector>();
        this->colorVertexListExtraSize = dataChunk->size / sizeof(CImVector);
    }
}

// OFFSET: 0x7CBCB0
void CMapObjGroup::AllocVB() {
    if (!this->vertsBlock) {
        uint32_t size = 36;
        if ((this->unkLoadedFlag & 0x8) != 0)
            size = 48;
        CMapObjGroup::vertexVBList.AllocVBB(&this->vertsBlock, size, this->vertexListCount);
    }
    if (!CShaderEffect::s_enableShaders && (this->parent->header->flags & 2) != 0) {
        if (this->colorVertexList) {
            if (this->transparencyBatchesCount) {
                if (!this->transparencyVertsBlock) {
                    //v6 = this->batchList[transparencyBatchesCount - 1].vertexEnd + 1;
                    //v5 = bn_GxVertexSize(4);
                    //VBBList::AllocVBB(&CMapObjGroup::vertexVBList, &this->transparencyVertsBlock, v5, v6);
                }
            }
        }
    }
    if (!this->indicesBlock)
        CMapObjGroup::indexVBList.AllocVBB(&this->indicesBlock, sizeof(uint16_t), this->indicesCount);
}

// OFFSET: 0x7C9D80
void CMapObjGroup::SetIndexVB() {
    CGxBuf* buffer;
    if (this->indicesBlock)
        buffer = this->indicesBlock->buffer;
    else
        buffer = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), this->indicesCount);

    if (!buffer->unk1C || !buffer->unk1D)
        this->UploadIndexBuffer(buffer);
    GxPrimIndexPtr(buffer);
}

// OFFSET: 0x7C8B90
void CMapObjGroup::UploadIndexBuffer(CGxBuf* buf) {
    char* buffer = g_theGxDevicePtr->BufLock(buf);
    memcpy(buffer, this->indices, sizeof(int16_t) * this->indicesCount);
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk1C = 1;
}

// OFFSET: 0x7C9CB0
void CMapObjGroup::SetVertexVB() {
    EGxVertexBufferFormat format = GxVBF_PNCT;
    uint32_t size = sizeof(CGxVertexPNCT);
    if ((this->unkLoadedFlag & 0x8) != 0) {
        format = GxVBF_PNC2T2;
        size = sizeof(CGxVertexPNC2T2);
    }

    CGxBuf* buf;
    if (this->vertsBlock)
        buf = this->vertsBlock->buffer;
    else
        buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, size, this->vertexListCount);

    if (!buf->unk1C || !buf->unk1D)
        this->FillVertexVB(buf, format);
    GxPrimVertexPtr(buf, format);
}

// OFFSET: 0x7C8560
void CMapObjGroup::FillVertexVB(CGxBuf* buf, EGxVertexBufferFormat format) {
    CImVector defaultColor = { 127, 127, 127, 255 };
    const CImVector zeroColor = { 0, 0, 0, 0 };
    const C2Vector zeroUV = { 0.0f, 0.0f };

    if ((this->parent->header->flags & 2) != 0) {
        defaultColor.b = 0;
        defaultColor.g = 0;
        defaultColor.r = 0;
    }

    if (format == GxVBF_PNCT) {
        CGxVertexPNCT* buffer = (CGxVertexPNCT*)g_theGxDevicePtr->BufLock(buf);
        for (int32_t i = 0; i < this->vertexListCount; i++) {
            buffer->position = this->vertexList[i];
            buffer->normal = this->normalList[i];
            CImVector color = this->colorVertexList ? this->colorVertexList[i] : defaultColor;
            if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                buffer->color.b = color.r;
                buffer->color.g = color.g;
                buffer->color.r = color.b;
                buffer->color.a = color.a;
            } else {
                buffer->color = color;
            }
            buffer->texture = this->textureVertexList[i];

            buffer++;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk1C = 1;
    } else if (format == GxVBF_PNC2T2) {
        CGxVertexPNC2T2* buffer = (CGxVertexPNC2T2*)g_theGxDevicePtr->BufLock(buf);
        for (int32_t i = 0; i < this->vertexListCount; i++) {
            buffer->position = this->vertexList[i];
            buffer->normal = this->normalList[i];
            CImVector color = this->colorVertexList ? this->colorVertexList[i] : defaultColor;
            if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                buffer->color[0].b = color.r;
                buffer->color[0].g = color.g;
                buffer->color[0].r = color.b;
                buffer->color[0].a = color.a;
            } else {
                buffer->color[0] = color;
            }
            color = this->colorVertexListExtra ? this->colorVertexListExtra[i] : zeroColor;
            if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                buffer->color[1].b = color.r;
                buffer->color[1].g = color.g;
                buffer->color[1].r = color.b;
                buffer->color[1].a = color.a;
            } else {
                buffer->color[1] = color;
            }
            buffer->texture[0] = this->textureVertexList[i];
            buffer->texture[1] = this->textureVertexListExtra ? this->textureVertexListExtra[i] : zeroUV;

            buffer++;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk1C = 1;
    }
}

// OFFSET: 0x7D7380
void CMapObjGroup::FixColorVertexAlpha() {
    uint32_t beginSecondFixup = 0;
    if (this->transparencyBatchesCount) {
        beginSecondFixup = this->batchList[this->transparencyBatchesCount - 1].vertexEnd + 1;
    }

    for (uint32_t i = 0; i < this->colorVertexListSize; i++) {
        CImVector& c = this->colorVertexList[i];

        if (i >= beginSecondFixup) {
            uint32_t a = c.a;
            uint32_t r = (c.r + ((a * c.r) >> 6)) >> 1;
            uint32_t g = (c.g + ((a * c.g) >> 6)) >> 1;
            uint32_t b = (c.b + ((a * c.b) >> 6)) >> 1;
            c.r = r > 255 ? 255 : r;
            c.g = g > 255 ? 255 : g;
            c.b = b > 255 ? 255 : b;
            c.a = 255;
        } else {
            c.r >>= 1;
            c.g >>= 1;
            c.b >>= 1;
        }
    }
}

// OFFSET: 0x7CB0C0
bool CMapObjGroup::GetTris(C3Segment& seg, float* dist, uint32_t a4, uint16_t faceIgnoreFlags, uint32_t a6, CMapObjDef* mapObjDef) {
    auto v16 = World::TriData::nBatches;

    BspQuery_Segment v14;
    BuildTriQuery(&v14, this->polyList, this->vertexList, this->indices, &seg, dist, faceIgnoreFlags, this->parent->materialList);
    CAaBsp_Query_Segment<BspQuery_Segment> v15 = {};
    v15.aaBsp = &this->CAaBspNodePtr1;
    v15.f = &v14;
    v15.GetFaceIndices(0, seg, this->CAaBspNodePtr1.aaBox);
    this->GetTrisFromQuery(a6, &v14, mapObjDef, 0);
    // if ((a4 & 0x30000) != 0 && (this->flags & 0x1000) != 0)
    //     CMapObjGroup::VectorIntersectLiquid((int)this, *(float*)&seg, dist, a4, a6, a7);
    auto v11 = World::TriData::nBatches != v16;
    v14.ClearTestFaces();
    return v11;
}

// OFFSET: 0x7CB7B0
bool CMapObjGroup::GetTris(CAaBox& box, uint32_t a4, uint16_t faceIgnoreFlags, uint32_t a6, CMapObjDef* mapObjDef) {
    auto v16 = World::TriData::nBatches;

    int32_t statusFlags = 0;

    BspQuery_Volume<CAaBox> v14;
    v14.overflowFlags = &statusFlags;
    v14.faces = this->polyList;
    v14.vertexList = this->vertexList;
    v14.indices = this->indices;
    v14.volume = &box;
    v14.faceIgnoreFlags = faceIgnoreFlags | BSPQUERY_FACE_TESTED;

    CAaBsp_Query_AaBox<BspQuery_Volume<CAaBox>> v15 = {};
    v15.aaBsp = &this->CAaBspNodePtr1;
    v15.f = &v14;
    v15.GetFaceIndices(0, box, this->CAaBspNodePtr1.aaBox);

    this->GetTrisFromQuery(a6, &v14, mapObjDef, statusFlags);

    // if ((a4 & 0x30000) != 0)
    //     this->GetLiquidTris(&box, a4, a6, mapObjDef);

    auto v11 = World::TriData::nBatches != v16;
    v14.ClearTestFaces();
    return v11;
}

// OFFSET: 0x7C7AE0
void CMapObjGroup::GetTrisFromQuery(uint32_t a2, BspQuery* a3, CMapObjDef* mapObjDef, uint32_t statusFlags) {
    if (CWorld::s_enables & 0x200000) {
        for (uint32_t i = 0; i < BspQuery::testFaceSub; i++) {
            uint32_t face = BspQuery::testFaces[i];
            uint32_t base = 3 * face;
    
            C3Vector v2, v1, v0;
            v2 = mapObjDef->mat.TransformPoint(this->vertexList[this->indices[base + 2]]);
            v1 = mapObjDef->mat.TransformPoint(this->vertexList[this->indices[base + 1]]);
            v0 = mapObjDef->mat.TransformPoint(this->vertexList[this->indices[base + 0]]);
    
            CFacet facet(v0, v1, v2);
            C44Matrix mat;
            CImVector color = { 0x00, 0x00, 0xFF, 0x7F };
            CMap::TestQueryAdd(facet, color, &mat);
        }
    
        for (uint32_t i = 0; i < BspQuery::hitFaceSub; i++) {
            uint32_t face = BspQuery::hitFaces[i];
            uint32_t base = 3 * face;
    
            C3Vector v2, v1, v0;
            v2 = mapObjDef->mat.TransformPoint(this->vertexList[this->indices[base + 2]]);
            v1 = mapObjDef->mat.TransformPoint(this->vertexList[this->indices[base + 1]]);
            v0 = mapObjDef->mat.TransformPoint(this->vertexList[this->indices[base + 0]]);
    
            CFacet facet(v0, v1, v2);
            C44Matrix mat;
            CImVector color = { 0x00, 0xFF, 0x00, 0x7F };
            CMap::TestQueryAdd(facet, color, &mat);
        }
    }

    World::TriData::statusFlags |= statusFlags;

    uint32_t nFaces = BspQuery::hitFaceSub;

    if (nFaces == 0)
        return;

    if (nFaces > 0x2000)
        return;

    World::TriData::Batch* batch = World::TriData::AllocBatch(3 * nFaces, nFaces);

    if (!batch)
        return;

    uint16_t* indexSlice;
    uint16_t* faceSlice;

    if (World::TriData::indexCursor + 3 * nFaces < 0xC000) {
        indexSlice = &World::TriData::indexPool[World::TriData::indexCursor];
        World::TriData::indexCursor += 3 * nFaces;
    } else {
        World::TriData::statusFlags |= 1;
        indexSlice = 0;
    }

    if (World::TriData::faceIndexCursor + nFaces < 0x4000) {
        faceSlice = &World::TriData::faceIndexPool[World::TriData::faceIndexCursor];
        World::TriData::faceIndexCursor += nFaces;
    } else {
        World::TriData::statusFlags |= 1;
        faceSlice = 0;
    }

    batch->matrix = &mapObjDef->mat;
    batch->vertexList = this->vertexList;
    batch->normalList = this->normalList;
    batch->def = mapObjDef;

    batch->faceCount = (uint16_t)nFaces;
    batch->indexCount = (uint16_t)(3 * nFaces);

    for (uint32_t i = 0; i < BspQuery::hitFaceSub; i++) {
        uint16_t face = BspQuery::hitFaces[i];

        faceSlice[i] = face;

        uint32_t base = 3 * face;

        for (int k = 0; k < 3; k++) {
            uint16_t index = this->indices[base + k];

            indexSlice[3 * i + k] = index;

            if (index < batch->minVertexIndex)
                batch->minVertexIndex = index;

            if (index > batch->maxVertexIndex)
                batch->maxVertexIndex = index;
        }
    }

    batch->indices = indexSlice;
    batch->faceIndices = faceSlice;
}

// OFFSET: 0x7A8B10
void CMapObjGroup::SetLighting(uint32_t mode) {
    if (CMapObj::s_lightingMode == mode)
        return;
    CMapObj::s_lightingMode = mode;

    DayNight::DNInfo* dayNight = DayNight::GetInfo();
    C3Vector ambient = { 0.0f, 0.0f, 0.0f };
    C3Vector diffuse = { 0.0f, 0.0f, 0.0f };

    switch (mode) {
    case 1: {
        const CImVector& d = dayNight->m_light1.m_diffuse;
        const CImVector& a = dayNight->m_light1.m_ambient;

        diffuse = { d.r / 255.0f, d.g / 255.0f, d.b / 255.0f };
        ambient = { a.r / 255.0f, a.g / 255.0f, a.b / 255.0f };
        break;
    }

    case 2: {
        const CImVector& d = dayNight->m_light1.m_mid0;
        const CImVector& a = dayNight->m_light1.m_mid1;

        diffuse = { d.r / 255.0f, d.g / 255.0f, d.b / 255.0f };
        ambient = { a.r / 255.0f, a.g / 255.0f, a.b / 255.0f };
        break;
    }

    case 3: {
        const CImVector& a = this->parent->argb_color;

        diffuse = { 0.0f, 0.0f, 0.0f };
        ambient = { a.r / 255.0f, a.g / 255.0f, a.b / 255.0f };
        break;
    }

    default:
        break;
    }

    CM2Light* light = &CMap::s_mapLight->m_light;

    if (CShaderEffect::s_enableShaders) {
        if (mode) {
            C44Matrix xform;
            g_theGxDevicePtr->XformView(xform);
            C3Vector dir = xform.TransformDirection(light->m_dir);

            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 9, &C4Vector(ambient), 1);
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 10, &C4Vector(ambient), 1);
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 11, &C4Vector(diffuse), 1);
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 12, &C4Vector(dir), 1);
        } else {
            static bool s_unlitInit = false;
            static C4Vector s_unlitDiffuse;

            if (!s_unlitInit) {
                s_unlitInit = true;
                s_unlitDiffuse = { 0.0f, 0.0f, 0.0f, 0.5f };
            }

            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 11, &s_unlitDiffuse, 1);
        }
    } else if (mode) {
        GxRsSet(GxRs_Lighting, 1);

        C3Vector savedAmb = light->m_ambColor;
        C3Vector savedDir = light->m_dirColor;

        light->m_dirColor = diffuse;
        light->m_ambColor = ambient;

        light->ApplyGxLight(0);

        light->m_dirColor = savedDir;
        light->m_ambColor = savedAmb;
    } else {
        GxRsSet(GxRs_Lighting, 0);
    }
}

// OFFSET: 0x7D8570
void CMapObjGroup::AsyncPostloadCallback(void* arg) {
    CMapObjGroup* mapObjGroup = static_cast<CMapObjGroup*>(arg);

    AsyncFileReadDestroyObject(mapObjGroup->asyncObjPtr);
    mapObjGroup->asyncObjPtr = nullptr;

    //perv = a1->perv;
    //if (perv) {
    //    next = a1->next;
    //    if (((unsigned __int8)next & 1) == 0 && next)
    //        v4 = (int32_t*)((char*)&next->objectIndex + (char*)p_perv - (char*)perv->vertsBlock);
    //    else
    //        v4 = (_DWORD*)((unsigned int)next & 0xFFFFFFFE);
    //    *v4 = perv;
    //    (*p_perv)->vertsBlock = (VBBList_Block*)a1->next;
    //    *p_perv = 0;
    //    a1->next = 0;
    //}

    mapObjGroup->Create();
}

// OFFSET: 0x7CB990
void CMapObjGroup::Initialize() {
    CMapObjGroup::vertexVBList.singlePool = 0;
    CMapObjGroup::vertexVBList.target = GxPoolTarget_Vertex;
    CMapObjGroup::vertexVBList.usage = GxPoolUsage_Dynamic;
    CMapObjGroup::vertexVBList.pool = 0;
    CMapObjGroup::indexVBList.singlePool = 0;
    CMapObjGroup::indexVBList.target = GxPoolTarget_Index;
    CMapObjGroup::indexVBList.usage = GxPoolUsage_Dynamic;
    CMapObjGroup::indexVBList.pool = 0;
}
