#ifndef WORLD_MAP_C_MAP_OBJ_GROUP_HPP
#define WORLD_MAP_C_MAP_OBJ_GROUP_HPP

#include "storm/list/TSLink.hpp"
#include "tempest/Plane.hpp"
#include "world/map/Types.hpp"
#include <tempest/Vector.hpp>
#include <tempest/Box.hpp>
#include "world/map/CMapBaseObj.hpp"
#include "async/CAsyncObject.hpp"
#include "world/map/VBBList.hpp"
#include "bsp/AaBsp.hpp"
#include "tempest/segment/C3Segment.hpp"
#include <bsp/BspQuery.hpp>

class CMapObj;
class CMapObjDef;
class CFrustum;

class CMapObjGroup : public CMapBaseObj {
    public:
    static VBBList vertexVBList;
    static VBBList indexVBList;

    VBBList_Block* vertsBlock;
    VBBList_Block* transparencyVertsBlock;
    VBBList_Block* indicesBlock;
    VBBList_Block* liquidVertsBlock;
    VBBList_Block* liquidIndicesBlock;
    float timer;
    void* unk_1C;
    int32_t unk_20;
    int32_t unk_24;
    int32_t unk_28;
    int16_t unk_2C[2];
    int32_t flags;
    CAaBox bbox;
    float distToCamera;
    int32_t portalStart;
    int32_t portalCount;
    uint8_t* fogs;
    uint16_t transparencyBatchesCount;
    uint16_t intBatchCount;
    uint16_t extBatchCount;
    CAaBsp CAaBspNodePtr1;
    int32_t unkFlags;
    int32_t unk_D4;
    int32_t minimapTag;
    char* groupName;
    SMOPoly* polyList;
    uint16_t* indices;
    int32_t unk_E8;
    C3Vector* vertexList;
    C3Vector* normalList;
    C2Vector* textureVertexList;
    C2Vector* textureVertexListExtra;
    SMOBatch* batchList;
    int32_t unk_100;
    int32_t unk_104;
    uint16_t* doodadRefList;
    CImVector* colorVertexList;
    CImVector* colorVertexListExtra;
    int32_t unk_114;
    C2iVector liquidVerts;
    C2iVector liquidTiles;
    C3Vector liquidCorner;
    int32_t luquidMaterialId;
    SMOLiquidVert* liquidVertexList;
    SMOLTile* liquidTileList;
    float liquidHeight;
    int32_t unk_144;
    int32_t liquidType;
    int32_t unkFlag;
    int32_t unk_150;
    int32_t polyListSize;
    int32_t indicesCount;
    int32_t unk_15C;
    int32_t vertexListCount;
    int32_t normalListCount;
    int32_t textureVertexListCount;
    int32_t unk_16C;
    int32_t batchListCount;
    int32_t unk_174;
    int32_t doodadRefListCount;
    int32_t colorVertexListSize;
    int32_t colorVertexListExtraSize;
    int32_t wmoGroupId;
    void* filePtr;
    int32_t fileSize;
    CMapObj* parent;
    int32_t unk_194;
    CAsyncObject* asyncObjPtr;
    int32_t unkLoadedFlag;
    int32_t unkIndexMin1;
    int32_t unkIndexMax1;
    uint16_t unkIndexMin2;
    uint16_t unkIndexMax2;
    int32_t TSExplicitList__m_linkoffset;
    void* TSExplicitList__ptr;
    void* TSExplicitList__ptr2;
    TSLink<CMapObjGroup> groupLink;

    void Create();
    void CreateDataPointers(SIffChunk* dataChunk);
    void CreateOptionalDataPointers(SIffChunk* dataChunk);

    void AllocVB();
    void SetIndexVB();
    void UploadIndexBuffer(CGxBuf* buf);
    void SetVertexVB();
    void FillVertexVB(CGxBuf* buf, EGxVertexBufferFormat format);
    void FixColorVertexAlpha();
    bool GetTris(C3Segment& seg, float* dist, uint32_t a4, uint16_t faceIgnoreFlags, uint32_t a6, CMapObjDef* mapObjDef);
    bool GetTris(CAaBox& box, uint32_t a4, uint16_t faceIgnoreFlags, uint32_t a6, CMapObjDef* mapObjDef);
    bool GetTris(CFrustum* frustum, uint32_t flags, uint16_t faceIgnoreFlags, uint32_t a5, CMapObjDef* mapObjDef);
    void GetTrisFromQuery(uint32_t a2, BspQuery* a3, CMapObjDef* mapObjDef, uint32_t a5);
    void SetLighting(uint32_t mode);
    bool Intersect(C3Segment& seg, float* dist, uint32_t flags, uint16_t faceIgnoreFlags, int32_t* hitIndex);

    static void AsyncPostloadCallback(void* arg);
    static void Initialize();
};

#endif
