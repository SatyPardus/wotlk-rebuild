#ifndef WORLD_C_WORLDSCENE_HPP
#define WORLD_C_WORLDSCENE_HPP

#include <cstdint>
#include <tempest/Vector.hpp>
#include "world/map/CMapChunk.hpp"
#include "world/map/CFrustum.hpp"

#include "gx/Texture.hpp"
#include "world/map/CMapObjDef.hpp"
#include "world/map/CMapObjDefGroup.hpp"
#include "world/map/CPortalView.hpp"

class CM2Scene;
class CMapDoodadDef;

struct CSortEntry {
    STORM_EXPLICIT_LIST(CMapChunk, sortListLink) mapChunkList;
    STORM_EXPLICIT_LIST(CMapObjDefGroup, sortEntryLink) exteriorGroupList;
    //TSList entityList;
    //TSList doodadDefList;
    //TSList liquidList;
    //TSList occluderList;
    //TSList unkList7;
    //TSList holedChunkList;
    //TSList unkList9;
};

struct CSortTable {
    CSortEntry table[64];
    C4Vector unkStuff[64];
    STORM_EXPLICIT_LIST(CMapObjDefGroup, sortEntryLink) pendingExteriorGroupList;
    //TSList detailDoodadInstList;
    STORM_EXPLICIT_LIST(CMapRenderChunk, renderChunkLink) renderChunkLists[24];
    STORM_EXPLICIT_LIST(CMapObjDefGroup, sortTableLink) mapObjDefGroup;
    //TSList interiorLiquidGroupList;
    //TSList visibleEntityList;
    //TSList culledEntityList;
    //TSList detailDoodadBatchList;
    //TSList horizonChunkList;
    //TSList horizonMapObjList;
};


class CWorldScene {
    public:
    static CM2Scene* s_m2Scene;
    static HTEXTURE s_defaultTexture;
    static HTEXTURE s_defaultBlendTexture;

    static int32_t frustumIndex;
    static CFrustum frustumStack[32];
    static CPortalView frustumPortalView;
    static CiRect s_frustumChunkRect;
    static C3Vector s_frustumCorners[8];

    static CSortTable sortTable;

    static C3Vector s_activeWorldView;
    static C3Vector camTarget;
    static C3Vector camVec;
    static C4Plane camPlane;
    static C4Plane camPlaneXY;
    static C44Matrix viewMatrix;
    static C44Matrix projMatrix;
    static CAaBox boundingBox;

    static uint32_t s_chunksRendered;
    static uint32_t s_doodadsRendered;

    static STORM_EXPLICIT_LIST(CFrustum, sceneLink) s_frustumFreeList;

    static void Initialize();
    static void Update(C3Vector* camPos, C3Vector* camTarget);
    static void GetNearestCornerToCamera(CAaBox* box, C3Vector* outCorner);
    static void AddMapChunk(CMapChunk* mapChunk);
    static void AddMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup);
    static void AddMapChunkToRenderList(CMapChunk* mapChunk, C3Vector* pos);
    static void AddMapObjDefGroupToSortTable(uint32_t groupNum, CMapObjDef* mapObjDef);
    static CFrustum* AllocFrustum();
    static bool FrustumCull(CAaBox* box);
    static void FrustumSet(CRect* rect);
    static void FrustumSet(CFrustum* frustum);
    static void FrustumSet(C3Vector* corners, CRect* rect);
    static void FrustumPush(CFrustum* a1, CFrustum* a2);
    static void FrustumPush();
    static void FrustumPop();
    static void FrustumXform(C44Matrix& mat);
    static bool InsideFrustumRect(CiRect* rect);
    static void CullSortTable(CRect* a1);
    static void CullChunks(CSortEntry* entry, int32_t index);
    static void CullDoodads(CSortEntry* entry, uint8_t fadeLevel);
    static void CullDoodadsExterior(STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink)* linkList, uint8_t fadeLevel);
    static void CullMapObjDefGroups(CSortEntry* entry, CRect* a2, uint32_t a3);
    static void CullMapObjDefGroupFromExterior(CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup, CRect* a3, uint32_t a4);
    static void AddDoodadDefModelToModelScene(CMapDoodadDef* a1);
    static void Render(const C3Vector& cameraPos, float time);
    static void RenderChunks();
    static void RenderMapObjDefGroups();
    static void RenderChunksSinglePass();
    static void RenderChunksSolid();
    static void SetWorldProjection(C44Matrix& mat);
};

#endif // WORLD_C_WORLDSCENE_HPP
