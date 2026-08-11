#include "world/map/CMapObjGroup.hpp"
#include "async/AsyncFile.hpp"
#include "world/map/CMapObj.hpp"

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
    //v12 = 0;
    //v23 = this->parent;
    //if (this->batchListCount) {
    //    v13 = 0;
    //    do {
    //        CMapObj::CreateMaterial(v23, this->batchList[v13].texture);
    //        ++v12;
    //        ++v13;
    //    } while (v12 < this->batchListCount);
    //}
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
