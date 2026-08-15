#ifndef WORLD_MAP_C_MAP_OBJ_HPP
#define WORLD_MAP_C_MAP_OBJ_HPP

#include "world/map/Types.hpp"
#include "world/map/CMapHandle.hpp"
#include "storm/Hash.hpp"
#include "async/CAsyncObject.hpp"
#include "world/map/CMapObjGroup.hpp"
#include "world/map/CFrustum.hpp"
#include "tempest/segment/C3Segment.hpp"

typedef void (*RENDER_FUNC)(CMapObj*, CMapObjGroup*, uint32_t);
typedef void (*RENDER_CALLBACK)(uint32_t groupNum, void* param);

class CMapObj : public CMapHandle, public TSHashObject<CMapObj, HASHKEY_STRI> {
    public:
    static TSHashTable<CMapObj, HASHKEY_STRI> mapObjHashtable;
    static uint32_t s_renderMode;
    static RENDER_FUNC s_renderGroupExteriorFunc;
    static RENDER_FUNC s_renderGroupInteriorFunc;
    static RENDER_CALLBACK gRenderCallback;
    static void* gRenderUserParam;
    static CImVector s_lastSidnColor;

    char m_wmoName[260];
    SMOHeader* header;
    char* textureNameList;
    char* groupNameList;
    char* skybox;
    SMOGroupInfo* groupInfo;
    C3Vector* portalVertexList;
    SMOPortal* portalList;
    SMOPortalRef* portalRefList;
    C3Vector* visBlockVertList;
    SMOVisibleBlock* visBlockList;
    SMOLight* lightList;
    SMODoodadSet* doodadSetList;
    char* doodadNameList;
    SMODoodadDef* doodadDefList;
    SMOFog* fogList;
    C4Plane* convexVolumePlanes;
    SMOMaterial* materialList;
    int32_t texturesSize;
    int32_t groupNameSize;
    int32_t groupInfoCount;
    int32_t planeVertCount;
    int32_t portalsCount;
    int32_t portalRefCount;
    int32_t visBlockVertCount;
    int32_t visBlockCount;
    int32_t lightsCount;
    int32_t doodadSetCount;
    int32_t doodadNameSize;
    int32_t doodadDefCount;
    int32_t fogsCount;
    int32_t convexVolumePlaneCount;
    int32_t materialsCount;
    uint32_t argb_color;
    //int32_t unk_1A4;
    CAaBox bbox;
    float distToCamera;
    //int32_t unk_1C4;
    //int32_t unk_1C8;
    void* pWmoData;
    int32_t wmoFileSize;
    int32_t refCount;
    float flushTimer;
    CAsyncObject* asyncObject;
    int32_t isGroupLoaded;
    int16_t unk_1E4;
    int16_t unk_1E6;
    STORM_EXPLICIT_LIST(CMapObjGroup, groupLink) mapObjGroupList;
    int32_t mapObjGroupCount;
    CMapObjGroup* mapObjGroupArray[512];

    bool Read(char* fileName);
    void ReadGroup(uint32_t index, bool preLoad);
    void Load();
    void WaitLoad();
    void WaitLoadGroup(uint32_t index);
    void GetBounds(CAaSphere* sphere);
    void GetBounds(CAaBox* box);
    void GetGroupBounds(CAaSphere* sphere, int32_t index);
    void GetGroupBounds(CAaBox* box, int32_t index);
    CMapObjGroup* GetGroup(int32_t index, bool a3);
    uint32_t GetGroupFlags(int32_t index);
    SMOGroupInfo* GetGroupInfo(int32_t index);
    void RenderGroup(int32_t groupIndex, C44Matrix& matrix, STORM_EXPLICIT_LIST(CFrustum, sceneLink)* frustumList);
    void CreateMaterial(uint8_t texture);
    void CreateMaterials();
    bool TestBounds(C3Vector& start, C3Vector& end);
    bool TestGroupBounds(C3Vector& start, C3Vector& end, uint32_t groupNum);
    bool GroupBoundingBoxIntersectsSphere(C3Vector& pos, uint32_t groupNum, float radius);
    bool VectorIntersectPortal(C3Segment& seg, float* t, int* outGroups, int useSphereTest);

    static void PrepareUpdate();
    static CMapObj* Create(char* fileName);
    static void PostloadCallback(void* arg);
    static void RenderGroupCollidable(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3);
    static void RenderGroupCollidableFaces(CMapObjGroup* mapObjGroup);
    static void ExteriorRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3);
    static void InteriorRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3);
    static void UnifiedRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3);
    static void InvokeGroupRenderCallback(CMapObj* mapObj, uint32_t groupNum);
    static void SetGroupRenderCallback(RENDER_CALLBACK callback, void* param);
    static void SetRenderModeLight();
    static void SetEmissiveColor(CImVector color);
};

#endif
