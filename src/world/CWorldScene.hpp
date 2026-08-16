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

struct SPortalExt {
    uint32_t flags;
    CRect rect;
    uint32_t stamp;
    uint32_t pad;
};


class CWorldScene {
    public:
    static CM2Scene* s_m2Scene;
    static HTEXTURE s_defaultTexture;
    static HTEXTURE s_defaultBlendTexture;

    static int32_t frustumIndex;
    static CFrustum frustumStack[32];
    static CFrustum s_clipFrustum;
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

    static C3Vector s_camPosLocal;
    static C3Vector s_camTargetLocal;
    static C4Plane s_camPlaneLocal;
    static C44Matrix s_viewProj;
    static C44Matrix s_modelView;
    static C44Matrix s_modelViewProj;
    static C44Matrix s_mapObjToWorld;
    static bool s_cullStateValid;

    static uint32_t s_interiorPass;
    static uint32_t s_portalStamp;
    static uint32_t s_maxPortalDepth;
    static CMapObjDef* s_curMapObjDef;
    static CMapObjDef* s_stampedMapObjDef;
    static CMapObjDef* s_viewerMapObjDef;
    static CMapObjDef* s_viewerMovedMapObjDef;
    static TSGrowableArray<uint16_t> s_viewerMapObjGroups;
    static TSGrowableArray<uint16_t> s_viewerMovedMapObjGroups;

    static SPortalExt s_portalExt[2048];
    static TSGrowableArray<CPortalView> s_pendingPortalViews;
    static TSGrowableArray<CRect> s_coveredRects;
    static int32_t s_curGroupIsInterior;

    static char s_debugMapName[260];
    static char s_debugMapChunk[64];

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
    static bool FrustumCull(CAaSphere* box);
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
    static void SetupMapObjDefCull(CMapObj* mapObj, C44Matrix& a2, C44Matrix& a3, C3Vector& a4, C3Vector& a5);
    static void RenderThruPortalsExterior(CMapObj* mapObj, C44Matrix& mat, C44Matrix& invMat, C3Vector& worldPos, C3Vector& camTarget, CRect& a6, int32_t a7);
    static void RenderInterior(CMapObj* mapObj, C44Matrix& mat, C44Matrix& invMat, C3Vector& worldPos, C3Vector& camTarget, TSGrowableArray<uint16_t>* groups);
    static void RenderThruPortals(CMapObj* mapObj, uint32_t groupNum, uint32_t fromGroup, CRect& ndcRect, uint32_t depth, int32_t interior);
    static void PushPortalView(CPortalView* portalView);
    static void TransformPortal(CMapObj* mapObj, SMOPortal* portal, SPortalExt* portalExt);
    static void ClassifyPortalPlane(CMapObj* mapObj, SMOPortal* portal, SPortalExt* portalExt);
    static uint32_t TransformAndClipVerts(CMapObj* mapObj, uint32_t a2, C3Vector* verts, uint32_t vertCount, C3Vector& a5, C3Vector*& clippedVerts, uint32_t& clippedCount);
    static void ClipVerts(C3Vector* verts, uint32_t count, C3Vector** outVerts, uint32_t* outCount);
    static void CalcScreenRectFromVerts(CRect& rect, C3Vector* clippedVerts, uint32_t clippedCount);
    static void LocateViewer3();
    static void RenderCollisionDebug();
    static void AddViewerGroup(TSGrowableArray<uint16_t>* group, uint16_t val);
    static void RenderMapObjWithCallback(CMapObjDef* mapObjDef, TSGrowableArray<uint16_t>* groups);
    static void AddExteriorPortalView(CMapObj* mapObj, SMOPortal* portal, SMOPortalRef* ref, SPortalExt* ext, uint32_t destIsExterior);
    static void AddInteriorPortalView(CMapObj* mapObj, SMOPortal* portal, SMOPortalRef* ref, SPortalExt* ext, TSGrowableArray<CPortalView>* a5);
    static void MergeIntoFrustumRect(CPortalView* portalView);
};

#endif // WORLD_C_WORLDSCENE_HPP
