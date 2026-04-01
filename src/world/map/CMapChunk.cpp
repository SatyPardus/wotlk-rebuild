#include "world/map/CMapChunk.hpp"

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
    //this->CreateBounds();
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
        char str[5];
        memcpy(str, &chunk->token, 4);
        str[4] = '\0';

        if (chunk->token == 'MCNR') {
            this->normals = chunk->Data<int8_t>();
            if (a3)
                chunk->size = 448;
        }

        remainingSize -= sizeof(SIffChunk) + chunk->size;
    }
}
