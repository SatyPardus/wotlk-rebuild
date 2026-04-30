#ifndef WORLD_C_WORLDSCENE_HPP
#define WORLD_C_WORLDSCENE_HPP

#include <cstdint>
#include <tempest/Vector.hpp>
#include "world/map/CMapChunk.hpp"
#include "world/map/CFrustum.hpp"

#include "gx/Texture.hpp"

class CM2Scene;

struct CSortEntry {
    STORM_EXPLICIT_LIST(CMapChunk, sortListLink) mapChunkList;
    //TSList unkList2;
    //TSList unkList3;
    //TSList doodadDefList;
    //TSList unkList5;
    //TSList unkList6;
    //TSList unkList7;
    //TSList unkList8;
    //TSList unkList9;
};

struct CSortTable {
    CSortEntry table[64];
    C4Vector unkStuff[64];
    //TSList unkList1;
    //TSList unkList2;
    STORM_EXPLICIT_LIST(CMapRenderChunk, renderChunkLink) renderChunkLists[24];
    //TSList unkList5;
    //TSList unkList6;
    //TSList unkList7;
    //TSList unkList8;
    //TSList unkList9;
    //TSList unkList10;
    //TSList unkList11;
};


class CWorldScene {
    public:
    static CM2Scene* s_m2Scene;
    static HTEXTURE s_defaultTexture;
    static HTEXTURE s_defaultBlendTexture;

    static int32_t frustumIndex;
    static CFrustum frustumStack[32];
    static CRect frustumRect;
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

    static void Initialize();
    static void Update(C3Vector* camPos, C3Vector* camTarget);
    static void GetNearestCornerToCamera(CAaBox* box, C3Vector* outCorner);
    static void AddMapChunk(CMapChunk* mapChunk);
    static void AddMapChunkToRenderList(CMapChunk* mapChunk, C3Vector* pos);
    static bool FrustumCull(CAaBox* box);
    static void FrustumSet(CRect* rect);
    static bool InsideFrustumRect(CiRect* rect);
    static void CullSortTable(CRect* a1);
    static void CullChunks(CSortEntry* entry, int32_t index);
    static void Render(const C3Vector& cameraPos, float time);
    static void RenderChunks();
    static void RenderChunksSinglePass();
    static void RenderChunksSolid();
};

#endif // WORLD_C_WORLDSCENE_HPP
