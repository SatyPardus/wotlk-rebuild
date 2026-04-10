#ifndef WORLD_C_MAP_HPP
#define WORLD_C_MAP_HPP

#include "world/map/Types.hpp"
#include "world/map/CMapArea.hpp"
#include <storm/String.hpp>
#include <storm/Array.hpp>
#include <storm/List.hpp>
#include <tempest/Rect.hpp>
#include <gx/shader/CGxShader.hpp>

class CMap {
    public:
    static char mapPath[STORM_MAX_PATH];
    static char mapName[STORM_MAX_PATH];
    static char wdtFilename[STORM_MAX_PATH];
    static uint32_t s_holeMask[16];
    static uint32_t version;
    static SMMapHeader header;
    static SMAreaInfo areaInfo[64 * 64];
    static CMapArea *areaTable[64 * 64];
    static STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) mapAreaList;
    static int32_t uniqueId;
    static int32_t bDungeon;
    static int32_t counts[11];
    static int32_t freeCounts[11];
    static TSGrowableArray<uint32_t> scCollideList;
    static uint32_t scCollideCnt;
    static uint32_t cCount;
    static bool bPreload;
    static bool bIsStreamingMode;

    static CGxShader* vertexShader_Terrain[128];
    static CGxShader* pixelShader_Terrain0[3];
    static CGxShader* pixelShader_Terrain0_env;
    static CGxShader* pixelShader_Terrain1[32];
    static CGxShader* pixelShader_Terrain2[32];
    static CGxShader* pixelShader_Terrain3[96];
    static CGxShader* pixelShader_TerrainSM;

    static uint32_t* lightHeap;
    static uint32_t* cacheLightHeap;
    static uint32_t* mapObjGroupHeap;
    static uint32_t* mapObjHeap;
    static uint32_t* baseObjLinkHeap;
    static uint32_t* areaHeap;
    static uint32_t* areaMedHeap;
    static uint32_t* areaLowHeap;
    static uint32_t* chunkHeap;
    static uint32_t* doodadDefHeap;
    static uint32_t* entityHeap;
    static uint32_t* mapObjDefGroupHeap;
    static uint32_t* mapObjDefHeap;
    static uint32_t* chunkLiquidHeap;


    static void Initialize();
    static void InitializePCFShaders();
    static void MapMemInitialize();
    static void Load(const char* mapName, int32_t zoneID);
    static void LoadWdt();
    static void LoadTextureBlob();
    static void LoadTerrainTexture(CMapArea* area, CMapAreaTexture* areaTexture, int32_t textureId);
    static bool SafeOpen(const char* fileName, SFile** file);
    static CMapArea* AllocArea();
    static CMapChunk* AllocMapChunk();
    static CMapBaseObjLink* AllocBaseObjLink(CMapBaseObj* baseObj);
    static void FreeBaseObjLink(CMapBaseObjLink* link);
    static CMapArea* PrepareArea(int32_t areaIndexX, int32_t areaIndexY);
    static void LoadArea(CMapArea* area);
    static void PrepareUpdate(bool a1);
    static void PurgeArea(CMapArea* area);
    static void PurgeMaps();
    static void PreUpdateAreas(bool a1);
    static void UpdateArea(bool a1, CMapArea* area, CiRect* chunkRect, int32_t a4);
    static void PrepareMapObjDefs(bool a1);
    static void PrepareMapDoodadDefs();
};

#endif
