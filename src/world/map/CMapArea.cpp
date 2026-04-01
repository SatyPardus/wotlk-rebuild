#include "world/map/CMapArea.hpp"
#include "world/map/CMapChunk.hpp"
#include "world/map/CMap.hpp"
#include "world/World.hpp"
#include "async/AsyncFile.hpp"

// OFFSET: 0x7BFE40
void* CMapArea::AllocAsyncLoadBuffer(int32_t fileSize) {
    return SMemAlloc(fileSize, __FILE__, __LINE__, 0);
}

// OFFSET: 0x7D7150
void CMapArea::Load(const char* fileName) {
    CMap::SafeOpen(fileName, &this->file);
    this->fileSize = SFile::GetFileSize(this->file, 0);
    this->fileBuffer = CMapArea::AllocAsyncLoadBuffer(this->fileSize);
    this->asyncObject = AsyncFileReadAllocObject();
    this->asyncObject->file = this->file;
    this->asyncObject->buffer = this->fileBuffer;
    this->asyncObject->size = this->fileSize;
    this->asyncObject->userArg = this;
    this->asyncObject->userPostloadCallback = CMapArea::AsyncCallback;
    AsyncFileReadObject(this->asyncObject, 0);
}

// OFFSET: 0x7D7020
void CMapArea::AsyncCallback(void* param) {
    CMapArea* area = static_cast<CMapArea*>(param);

    area->ProcessIffChunks();
    AsyncFileReadDestroyObject(area->asyncObject);
    area->asyncObject = nullptr;
    area->file = nullptr;
}

// OFFSET: 0x7D6D20
void CMapArea::LoadTextures(const char* fileNames, int32_t fileNamesSize) {
    if (fileNamesSize <= 0)
        return;

    //v4 = 0;
    //p_textures = &this->textures;
    //do {
    //    v6 = p_textures->m_count + 1;
    //    if (v6 > p_textures->m_alloc) {
    //        m_chunk = p_textures->m_chunk;
    //        if (!m_chunk) {
    //            m_chunk = p_textures->m_count + 1;
    //            if (v6 >= 0x20) {
    //                p_textures->m_chunk = 32;
    //                m_chunk = 32;
    //            } else {
    //                for (i = (p_textures->m_count + 1) & p_textures->m_count; i; i &= i - 1)
    //                    m_chunk = i;
    //                if (!m_chunk)
    //                    m_chunk = 1;
    //            }
    //        }
    //        if (v6 % m_chunk)
    //            v9 = v6 + m_chunk - v6 % m_chunk;
    //        else
    //            v9 = v6;
    //        sub_7C30B0(&p_textures->m_alloc, v9);
    //    }
    //    m_count = p_textures->m_count;
    //    v11 = a2;
    //    v12 = &p_textures->m_data[m_count];
    //    p_textures->m_count = m_count + 1;
    //    v12->textureName = &a2[v4];
    //    v12->texture = 0;
    //    if (!IsStreamingMode) {
    //        CMap::LoadTerrainTexture(
    //            this,
    //            (int)&this->textures.m_data[this->textures.m_count - 1],
    //            this->textures.m_count - 1);
    //        v11 = a2;
    //    }
    //    if (a2[v4]) {
    //        do
    //            ++v4;
    //        while (v11[v4]);
    //    }
    //    ++v4;
    //} while (v4 < fileNamesSize);
}

// OFFSET: 0x7D6EF0
void CMapArea::ProcessIffChunks() {
    SIffChunk* versionChunk = reinterpret_cast<SIffChunk*>(this->fileBuffer);

    SIffChunk* headerChunk = versionChunk->Next();
    this->header = headerChunk->Data<SMAreaHeader>();

    SIffChunk* chunkInfoChunk = headerChunk->Next(this->header->mcin);
    this->chunkInfo = chunkInfoChunk->Data<SMChunkInfo>();

    SIffChunk* doodadDefChunk = headerChunk->Next(this->header->mddf);
    this->doodadDef = doodadDefChunk->Data<SMDoodadDef>();
    this->doodadDefCount = doodadDefChunk->size / sizeof(SMDoodadDef);

    SIffChunk* mapObjDefChunk = headerChunk->Next(this->header->modf);
    this->mapObjDef = mapObjDefChunk->Data<SMMapObjDef>();
    this->mapObjDefCount = mapObjDefChunk->size / sizeof(SMMapObjDef);

    SIffChunk* m2FileNamesChunk = headerChunk->Next(this->header->mmdx);
    this->m2FileNames = m2FileNamesChunk->Data<char>();

    SIffChunk* wmoFileNamesChunk = headerChunk->Next(this->header->mwmo);
    this->wmoFileNames = wmoFileNamesChunk->Data<char>();

    SIffChunk* modelFilenamesOffsetsChunk = headerChunk->Next(this->header->mmid);
    this->modelFilenamesOffsets = modelFilenamesOffsetsChunk->Data<uint32_t>();

    SIffChunk* wmoFilenamesOffsetsChunk = headerChunk->Next(this->header->mwid);
    this->wmoFilenamesOffsets = wmoFilenamesOffsetsChunk->Data<uint32_t>();

    if ((this->header->flags & 0x1) != 0) {
        SIffChunk* flyingBboxChunk = headerChunk->Next(this->header->mfbo);
        this->flyingBbox = flyingBboxChunk->Data<int16_t>();
    }

    SIffChunk* mh2oChunk;
    if (this->header->mh2o && (mh2oChunk = headerChunk->Next(this->header->mh2o)) != nullptr && mh2oChunk->size) {
        //v6 = SMemAlloc(8, ".\\MapArea.cpp", 553, 0);
        //if (v6)
        //    v7 = (uint8_t*)sub_8A3050(v6, 0);
        //else
        //    v7 = 0;
        //header = this->header;
        //this->unk_B8 = v7;
        //sub_7D4F10(v7, (int)&v2->mtex + header->mh2o);
    }

    if (this->header->mtxf) {
        SIffChunk* textureFlagsChunk = headerChunk->Next(this->header->mtxf);
        this->textureFlags = textureFlagsChunk->Data<int32_t>();
    }

    SIffChunk* texturesChunk = headerChunk->Next(this->header->mtex);
    this->LoadTextures(texturesChunk->Data<char>(), texturesChunk->size);
}

// OFFSET: 0x7D6B30
void CMapArea::PrepareChunk(int32_t chunkX, int32_t chunkY) {
    int32_t chunkIndex = 16 * chunkY + chunkX;
    SMChunkInfo chunkInfo = this->chunkInfo[chunkIndex];

    bool v10 = (chunkInfo.flags & 1) == 0;
    chunkInfo.flags |= 1;

    CMapChunk* chunk = CMap::AllocMapChunk();
    CMapBaseObjLink* link = CMap::AllocBaseObjLink(chunk);

    link->ref = this;
    this->chunkLinkList.LinkToTail(link);

    chunk->aIndex.x = chunkX;
    chunk->unk_C = 0;
    chunk->aIndex.y = chunkY;
    chunk->cOffset.x = chunkX + this->tileChunkIndex.x;
    chunk->cOffset.y = chunkY + this->tileChunkIndex.y;
    chunk->sOffset.x = 8 * (chunkX + this->tileChunkIndex.x);
    chunk->sOffset.y = 8 * (chunkY + this->tileChunkIndex.y);
    chunk->Create((SIffChunk*)((uint8_t*)this->fileBuffer + chunkInfo.offset), v10);
}

// OFFSET: 0x7D6A90
void CMapArea::PurgeChunks(CiRect* chunkRect) {

}

// OFFSET: 0x7C35D0
void CMapArea::PurgeChunk(CMapChunk* chunk) {

}

// OFFSET: 0x7D6BF0
void CMapArea::Update(bool a1, CiRect* chunkRect) {
    for (int32_t chunkY = chunkRect->minY; chunkY <= chunkRect->maxY; chunkY++) {
        int32_t localY = chunkY & 0xF;
        for (int32_t chunkX = chunkRect->minX; chunkX <= chunkRect->maxX; chunkX++) {
            int32_t localX = chunkX & 0xF;
            int32_t chunkIndex = 16 * localY + localX;

            CMapChunk* chunk = this->mapChunks[chunkIndex];

            if (chunkX < CWorld::s_chunkRectLow.minX ||
                chunkX > CWorld::s_chunkRectLow.maxX ||
                chunkY < CWorld::s_chunkRectLow.minY ||
                chunkY > CWorld::s_chunkRectLow.maxY) {
                if (chunk) {
                    this->mapChunks[chunkIndex] = nullptr;

                    CMapBaseObjLink* link = chunk->parentLinkList.Head();
                    CMap::FreeBaseObjLink(link);
                    CMapArea::PurgeChunk(chunk);
                }
                continue;
            }

            if (!chunk) {
                this->PrepareChunk(localX, localY);
                chunk = this->mapChunks[chunkIndex];
            }

            //if (a1 && sub_7D6690(chunkRect)) {
            //    sub_7C3E70(chunk);
            //    sub_7C5B20(chunk);
            //}
        }
    }
}
