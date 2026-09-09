#ifndef MODEL_C_M2_SCENE_HPP
#define MODEL_C_M2_SCENE_HPP

#include "model/M2Model.hpp"
#include "model/M2Types.hpp"
#include <cstdint>
#include <storm/Array.hpp>
#include <tempest/Matrix.hpp>

class CM2Cache;
class CM2Light;
class CM2Lighting;
class CM2Model;
class CMapBaseObj;
class CParticleEmitter2;

typedef void (*M2ProjectTextureCallback)(const CAaBox&, const CImVector&, int32_t, void*, int32_t);
typedef int32_t (*M2ProjectPositionCallback)(const C3Vector&, float&, void*);

int32_t GxBlendToM2Blend(int32_t gxBlend);

class CM2Scene {
    public:
    // Static variables
    static uint32_t s_optFlags;

    // Static functions
    static void AnimateThread(void* arg);
    static void ComputeElementShaders(M2Element* element);
    static int32_t SortOpaque(uint32_t a, uint32_t b, const void* userArg);
    static int32_t SortOpaqueGeoBatches(M2Element* elementA, M2Element* elementB);
    static int32_t SortOpaqueParticles(M2Element* elementA, M2Element* elementB);
    static int32_t SortOpaqueRibbons(M2Element* elementA, M2Element* elementB);
    static int32_t SortTransparent(uint32_t a, uint32_t b, const void* userArg);
    static int32_t SortHitNear(uint32_t a, uint32_t b, const void* userArg);
    static int32_t SortAdditiveParticles(uint32_t a, uint32_t b, const void* userArg);

    // Member variables
    /* 0000 */ int32_t m_refCount = 1;
    /* 0004 */ CM2Cache* m_cache;
    /* 0008 */ CM2Model* m_modelList = nullptr;
    /* 000C */ uint32_t m_time = 0;
    /* 0010 */ uint32_t m_timeDelta = 0;
    /* 0014 */ uint32_t m_frameStamp = 0;
    /* 0018 */ // apparently unused
    /* 001C */ uint32_t m_flags = 0;
    /* 0020 */ CM2Light* m_lightList = nullptr;
    /* 0024 */ CM2Light** m_lightGrid = nullptr;
    /* 0028 */ CM2Model* m_animateList = nullptr;
    /* 002C */ CM2Model* m_drawList = nullptr;
    /* 0030 */ CM2Model* m_particleList = nullptr;
    /* 0034 */ TSGrowableArray<M2Element> m_elements;
    /* 0044 */ TSGrowableArray<uint32_t> m_doodadElements;
    /* 0054 */ TSGrowableArray<uint32_t> m_passElements[3];
    /* 0084 */ C44Matrix m_view;
    /* 00C4 */ C44Matrix m_viewInv;
    /* 0104 */ M2ProjectTextureCallback m_projectTextureCallback = nullptr;
    /* 0108 */ void* m_projectTextureParam = nullptr;
    /* 010C */ M2ProjectPositionCallback m_projectPositionCallback = nullptr;
    /* 0110 */ void* m_projectPositionParam = nullptr;
    /* 0114 */ CM2Model* m_hitTestList = nullptr;
    /* 0118 */ M2HitRec* m_hitRecs = nullptr;
    /* 011C */ uint32_t* m_hitOrder = nullptr;
    /* 0120 */ uint32_t m_hitCapacity = 0;
    /* 0124 */ C3Vector* m_hitVerts = nullptr;
    /* 0128 */ uint32_t m_hitVertCapacity = 0;
    /* 012C */ M2HitRec m_lastHit;
    /* 013C */ CMapBaseObj* m_lastHitOwner = nullptr;
    /* 0140 */ uint32_t m_liquidTypeId = 0;
    /* 0144 */ uint32_t m_passMask = 0;


    // Member functions
    CM2Scene(CM2Cache* cache)
        : m_cache(cache), m_passMask(0xFFFFFFFF)
        {};
    void AdvanceTime(uint32_t a2);
    bool Animate(const C3Vector& cameraPos);
    void SortAdditiveParticleElements(int32_t pass);
    void QueueParticleElement(CParticleEmitter2* emitter, CM2Model* model, float depth, float alpha, int32_t aboveWater, uint32_t* elementIndex, uint32_t* particleCount);
    CM2Model* CreateModel(const char* file, uint32_t a3);
    void Draw(M2PASS pass);
    void SelectLights(CM2Lighting* lighting);
    void Release();
    CM2Model* DuplicateModel(CM2Model* a2, uint32_t a3);
    void AllocateSpaceForHitList();
    int32_t ComputeRayDirAndLen(const C3Vector& start, const C3Vector& end, float dist, float* len, C3Vector* dir);
    void BeginHitTest();
    CMapBaseObj* EndHitTest(const C3Vector& start, const C3Vector& end, float* dist, int32_t allowSecondPass);
    CMapBaseObj* EndHitTestCollisionWorld(const C3Vector& start, const C3Vector& end, float* dist);
    M2HitRec* HitTestCollision(CM2Model* model, uint32_t pass, const C3Vector& dir, float dirDotStart, const C3Vector& start, M2HitRec* rec, float* t, M2HitRec* best);
    M2HitRec* HitTestGeometry(CM2Model* model, uint32_t pass, const C3Vector& dir, float dirDotStart, const C3Vector& start, M2HitRec* rec, float* t, M2HitRec* best);
    M2HitRec* IntersectHitTestTriangles(const uint16_t* begin, const uint16_t* end, uint32_t vertexStart, const C3Vector& start, uint32_t pass, M2HitRec* rec, float* t, M2HitRec* best);
    uint32_t SphereTestModels(const C3Vector& start, const C3Vector& dir, float len, int32_t requireCurrentFrame);
    void TransformHitTestBone(CM2Model* model, M2SkinProfile* skin, M2SkinSection* section, uint32_t pass, const C3Vector& dir, float dirDotStart);
    void TransformHitTestBoneVariant(CM2Model* model, M2SkinProfile* skin, M2SkinSection* section, uint32_t pass, const C3Vector& dir, float dirDotStart);
    void TransformHitTestVertices(CM2Model* model, M2SkinProfile* skin, M2SkinSection* section, uint32_t pass, const C3Vector& dir, float dirDotStart);
};

#endif
