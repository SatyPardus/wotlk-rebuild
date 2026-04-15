#ifndef WORLD_MAP_C_MAP_AREA_HPP
#define WORLD_MAP_C_MAP_AREA_HPP

#include "world/map/Types.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "async/CAsyncObject.hpp"
#include "util/SFile.hpp"
#include <tempest/Rect.hpp>
#include <storm/Array.hpp>
#include "gx/Texture.hpp"

class CMapChunk;

struct CMapAreaTexture {
    const char* textureName;
    HTEXTURE texture;
};

class CMapArea : public CMapBaseObj {
    public:
    CAaBox bounds;
    C3Vector topLeft2;
    C2iVector index;
    C2iVector tileChunkIndex;

    TSGrowableArray<CMapAreaTexture> textures;
    SMAreaHeader* header;
    SFile* file;
    CAsyncObject* asyncObject;
    STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) chunkLinkList;
    void* fileBuffer;
    int32_t fileSize;
    SMChunkInfo* chunkInfo;
    int32_t unk_8C;
    SMDoodadDef* doodadDef;
    SMMapObjDef* mapObjDef;
    int32_t doodadDefCount;
    int32_t mapObjDefCount;
    char* m2FileNames;
    char* wmoFileNames;
    uint32_t* modelFilenamesOffsets;
    uint32_t* wmoFilenamesOffsets;
    int16_t* flyingBbox;
    int32_t* textureFlags;
    uint8_t* unk_B8;
    CMapChunk* mapChunks[256];

    static void* AllocAsyncLoadBuffer(int32_t fileSize);
    static void AsyncCallback(void* param);

    void Load(const char* fileName);
    void LoadTextures(const char* fileNames, int32_t fileNamesSize);
    void ProcessIffChunks();
    void PrepareChunk(int32_t chunkX, int32_t chunkY);
    void PurgeChunks(CiRect* chunkRect);
    void PurgeChunk(CMapChunk* chunk);
    void Update(bool a1, CiRect* chunkRect);
};

struct CMapAreaEntry {
    CMapArea* area;
    float dist;
};

#endif
