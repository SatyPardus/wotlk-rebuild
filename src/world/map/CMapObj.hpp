#ifndef WORLD_MAP_C_MAP_OBJ_HPP
#define WORLD_MAP_C_MAP_OBJ_HPP

#include "world/map/Types.hpp"
#include "world/map/CMapHandle.hpp"
#include "storm/Hash.hpp"
#include "async/CAsyncObject.hpp"
#include "world/map/CMapObjGroup.hpp"

class CMapObj : public CMapHandle, public TSHashObject<CMapObj, HASHKEY_STRI> {
    public:
    static TSHashTable<CMapObj, HASHKEY_STRI> mapObjHashtable;

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
    //int32_t unk_1E4;
    //TSExplicitList_CMapObjGroup mapObjGroupList;
    int32_t mapObjGroupCount;
    CMapObjGroup* mapObjGroupArray[512];

    bool Read(char* fileName);
    void Load();

    static void PrepareUpdate();
    static CMapObj* Create(char* fileName);
    static void PostloadCallback(void* arg);
};

#endif
