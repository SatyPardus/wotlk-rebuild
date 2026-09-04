#ifndef WORLD_C_MAP_HPP
#define WORLD_C_MAP_HPP

#include "world/map/Types.hpp"
#include "world/map/CMapArea.hpp"
#include "world/map/CMapRenderChunk.hpp"
#include "world/map/CMapDoodadDef.hpp"
#include <storm/String.hpp>
#include <storm/Array.hpp>
#include <storm/List.hpp>
#include <tempest/Rect.hpp>
#include <gx/shader/CGxShader.hpp>
#include "world/map/CMapObjDef.hpp"
#include "world/map/CMapObjDefGroup.hpp"
#include "world/map/CMapObj.hpp"
#include "world/map/CMapEntity.hpp"
#include <tempest/facet/CFacet.hpp>
#include "world/map/CMapLight.hpp"
#include <world/World.hpp>

class CMap {
    public:
    static char mapPath[STORM_MAX_PATH];
    static char mapName[STORM_MAX_PATH];
    static char wdtFilename[STORM_MAX_PATH];
    static uint32_t s_holeMask[16];
    static uint32_t s_fanIndices[8];
    static uint32_t version;
    static SMMapHeader header;
    static SMAreaInfo areaInfo[64 * 64];
    static CMapArea *areaTable[64 * 64];
    static STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) mapAreaList;
    static STORM_EXPLICIT_LIST(CMapRenderChunk, renderChunkLink) s_mapRenderChunkFreeList;
    static STORM_EXPLICIT_LIST(CMapRenderChunk, renderChunkLink) s_mapRenderChunkUpdateList;
    static STORM_EXPLICIT_LIST(CMapDoodadDef, doodadDefLink) doodadDefList;
    static STORM_EXPLICIT_LIST(CMapEntity, lameAssLink) entityList;
    static STORM_EXPLICIT_LIST(CMapLight, lameAssLink) lightList;
    static TSHashTable<CMapDoodadDef, uint32_t> doodadDefHashtable;
    static TSHashTable<CMapObjDef, uint32_t> mapObjDefHashtable;
    static int32_t uniqueId;
    static int32_t bDungeon;
    static int32_t counts[11];
    static int32_t freeCounts[11];
    static TSGrowableArray<uint32_t> scCollideList;
    static uint32_t scCollideCnt;
    static uint32_t cCount;
    static bool bPreload;
    static bool bIsStreamingMode;
    static CMapLight* s_mapLight;
    static CiRect gbPrevChunkRect;
    static bool dword_CF08F8;
    static uint32_t mapGetFacetsCount;
    static uint32_t s_queryTag;

    static CGxShader* vertexShader_Terrain[128];
    static CGxShader* pixelShader_Terrain0[3];
    static CGxShader* pixelShader_Terrain0_env;
    static CGxShader* pixelShader_Terrain1[32];
    static CGxShader* pixelShader_Terrain2[32];
    static CGxShader* pixelShader_Terrain3[96];
    static CGxShader* pixelShader_TerrainSM;

    static bool enableVertexShaders;
    static bool enablePixelShaders;
    static bool enableSpecular;
    static bool gTerrainPixelShadersValid;
    static bool enableSpecularTerrain;
    static bool enableTerrainShaderVertex;
    static bool enableChunkBatching;

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

    static TSGrowableArray<CGxVertexPC> debugVertexArray;
    static TSGrowableArray<uint16_t> debugIndexArray;

    static int32_t s_subVertexIndex[5];
    static int32_t s_subTriIndex[4][3];


    static void Initialize();
    static void InitializePCFShaders();
    static void ValidateShaders();
    static CGxShader* GetPixelShader(bool a1, bool a2, bool a3);
    static void MapMemInitialize();
    static void Load(const char* mapName, int32_t zoneID);
    static void LoadWdt();
    static void LoadTex();
    static HTEXTURE LoadTexture(const char* fileName);
    static void LoadTerrainTexture(CMapArea* area, CMapAreaTexture* areaTexture, int32_t textureId);
    static bool SafeOpen(const char* fileName, SFile** file);
    static CMapArea* AllocArea();
    static CMapChunk* AllocMapChunk();
    static CMapRenderChunk* AllocRenderChunk();
    static CMapBaseObjLink* AllocBaseObjLink(CMapBaseObj* baseObj);
    static CMapDoodadDef* AllocDoodadDef();
    static CMapObjDef* AllocMapObjDef();
    static CMapObj* AllocMapObj();
    static CMapEntity* AllocEntity(bool linkToHead);
    static CMapObjGroup* AllocMapObjGroup();
    static CMapObjDefGroup* AllocMapObjDefGroup();
    static CMapLight* AllocLight();
    static CMapDoodadDef* CreateDoodadDef(char* fileName, SMDoodadDef* doodadDef, C3Vector* position);
    static CMapDoodadDef* CreateDoodadDef(uint32_t doodadRef, SMODoodadDef* doodadDef, char* name, uint32_t uniqueId, C44Matrix* mat, uint16_t doodadSet);
    static CMapObjDef* CreateMapObjDef(char* fileName, SMMapObjDef* mapObjectDef, C3Vector* center, bool cached);
    static CMapLight* CreateLight(uint8_t a1, uint8_t a2);
    static void FreeBaseObjLink(CMapBaseObjLink* link);
    static CMapArea* PrepareArea(int32_t areaIndexX, int32_t areaIndexY);
    static void LoadArea(CMapArea* area);
    static void PrepareUpdate(bool a1);
    static void PurgeArea(CMapArea* area);
    static void PurgeMaps();
    static void PreUpdateAreas(bool a1);
    static void UpdateArea(bool a1, CMapArea* area, CiRect* chunkRect, int32_t a4);
    static void PrepareMapObjDefs(bool a1);
    static void PrepareMapObjDef(CMapObjDef* mapObjDef, CMapObj* mapObj);
    static void CreateMapObjDefGroups(CMapObjDef* mapObjDef, CMapObj* mapObj);
    static void PrepareMapDoodadDefs();
    static void ProcessRenderChunkUpdateList();
    static CMapEntity* ObjectCreate(CM2Model* model, MAP_OBJECT_FUNC func, void* funcParam, uint64_t param64, uint32_t param32, uint32_t a7);
    static void ObjectUpdate(CMapEntity* entity, C44Matrix& mat, CAaBox& box, CAaSphere& sphere, C3Vector& vec, bool a6, uint32_t a7);
    static void PrepareEntitys(bool a1);
    static void EnableLight(CMapLight* light);
    static void UpdateLight(CMapLight* light);

    static bool VectorIntersectTerrain(C3Vector* start, C3Vector* end, float* distance, uint32_t flags, CMapChunk** hitChunk);
    static bool VectorIntersectSubChunkList(C3Vector* start, C3Vector* end, float* distance, uint32_t flags, CMapChunk** hitChunk);
    static void VectorIntersectSY(CiRect& rect);
    static void VectorIntersectSX(CiRect& rect);
    static void VectorIntersectDY(C3Vector& a1, C3Vector& a2, CiRect& rect);
    static void VectorIntersectDX(C3Vector& a1, C3Vector& a2, CiRect& rect);

    static bool LocateViewerMapObjs(C3Vector& start, C3Vector& end, float dist, CMapObjDef** outDefs, uint32_t* outGroups);
    static void TestQueryAdd(CFacet& facet, CImVector& color, C44Matrix* mat);
    static bool GetFacets(CAaBox* a1, CAaBox* a2, World::FacetData* a3, uint32_t a4, uint32_t* a5);
    static bool GetMapObjFacets(CAaBox* a1, CAaBox* box, World::FacetData* facets, uint32_t flags, uint32_t* statusOut);
    static CFacet* BuildImpassableFacets(World::FacetData* facets, C3Vector* up, C3Vector* edge, C3Vector* normal, C3Vector* origin);
    static void CreateImpassableFacets(CMapChunk* chunk, CAaBox* box, World::FacetData* facets, uint32_t flags);
    static bool GetChunkFacets(int32_t chunkX, int32_t chunkY, CiRect* subRect, CAaBox* a4, CAaBox* box, World::FacetData* facets, uint32_t flags);
    static bool CreateFlightBoundsFacets(int32_t areaX, int32_t areaY, CAaBox* box, World::FacetData* facets);
    static void AppendMapObjFacets(CMapDoodadDef* def, CAaBox* box, World::FacetData* facets);
    static bool GetDoodadDefFacets(STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink)* linkList, CAaBox* box, World::FacetData* facets, uint32_t flags);
    static int32_t GetFlightBounds(const C3Vector& pos, float* height, int32_t which);
};

#endif
