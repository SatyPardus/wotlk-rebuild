#ifndef MODEL_C_PARTICLE_EMITTER_2_HPP
#define MODEL_C_PARTICLE_EMITTER_2_HPP

#include <tempest/Vector.hpp>
#include <tempest/Box.hpp>
#include <tempest/Range.hpp>
#include <tempest/Matrix.hpp>
#include "model/M2Animate.hpp"
#include <util/C3Spline.hpp>
#include <tempest/Random.hpp>
#include <storm/queue/TSHeap.hpp>

class CM2Model;
class CM2Scene;
class CGxPool;
class CGxBuf;

struct CParticleMaterial {
    int32_t blend;
    uint32_t flags;
};

struct CParticle2 {
    float m_age;
    C3Vector m_position;
    C3Vector m_velocity;
    int16_t m_lifeSeed;
    uint16_t m_randomSeed;
};

struct CParticle2Model {
    CParticle2 base;
    C4Quaternion m_orientation;
    C3Vector m_angularVelocity;
    CM2Model* m_model = nullptr;
};

struct ParticleVertexWriter {
    float* position;
    float* normal;
    uint32_t* color;
    float* texCoord;
    uint32_t positionStride;
    uint32_t normalStride;
    uint32_t colorStride;
    uint32_t texCoordStride;
    uint32_t count;
};

struct CParticleSortEntry {
    float m_key;
    CParticle2* m_particle;

    static bool HasHigherPriority(const CParticleSortEntry& a, const CParticleSortEntry& b) {
        return a.m_key >= b.m_key;
    }
};

struct CParticleSimpleKeys {
    /* 0x00 */ CImVector m_color;
    /* 0x04 */ int32_t m_redDelta;
    /* 0x08 */ int32_t m_greenDelta;
    /* 0x0C */ int32_t m_blueDelta;
    /* 0x10 */ int32_t m_alphaDelta;
    /* 0x14 */ float m_scaleBase;
    /* 0x18 */ float m_scaleDelta;
    /* 0x1C */ int32_t m_headCellBase;
    /* 0x20 */ int32_t m_headCellDelta;
    /* 0x24 */ int32_t m_tailCellBase;
    /* 0x28 */ int32_t m_tailCellDelta;

    void Constructor();
};

class CParticleEmitter2 {
    public:
    static float s_viewDist;
    static CGxPool* s_indexPool;
    static CGxBuf*  s_indexBuf;
    static int32_t  s_initCount;
    static C44Matrix s_particleXform;
    static C3Vector s_particleViewAxis;
    static C33Matrix s_particleBasis;
    static C3Vector s_zeroNormal;
    static TSHeap<CParticleSortEntry> g_particleSortBuffer;
    static bool g_particleRenderEnable;

    static void Init();
    static void Destroy();

    /* 0x004 */ int32_t   m_refCount = 0;
    /* 0x008 */ float     m_emitAccumulator = 0;
    /* 0x00C */ int32_t   m_textureCellShift = 0;
    /* 0x010 */ float     m_textureCellWidth = 0;
    /* 0x014 */ float     m_textureCellHeight = 0;
    /* 0x018 */ int32_t   m_priorityPlane = 0;
    /* 0x01C */ uint32_t  m_renderedParticles = 0;
    /* 0x020 */ int32_t   m_emitterType = 0;
    /* 0x024 */ CRndSeed  m_random{0};
    /* 0x02C */ TSGrowableArray<CParticle2> m_particles;
    /* 0x03C */ TSGrowableArray<CParticle2Model> m_particleModels;
    /* 0x04C */ TSGrowableArray<uint32_t> m_liveList;
    /* 0x05C */ TSGrowableArray<uint32_t> m_freeList;
    /* 0x06C */ uint32_t  m_childEmitterCount = 0;
    /* 0x070 */ CParticleEmitter2* m_childEmitters[4];
    /* 0x080 */ CM2Model* m_model = nullptr;
    /* 0x084 */ CM2Scene* m_scene = nullptr;
    /* 0x088 */ CM2Model* m_childModel = nullptr;
    /* 0x08C */ uint32_t  m_vertsPerParticle = 0;
    /* 0x090 */ uint32_t  m_indicesPerParticle = 0;
    /* 0x094 */
    /* 0x098 */ uint32_t  m_hasModel = 0;
    /* 0x09C */ float     m_emissionRate = 0;
    /* 0x0A0 */ float     m_emissionRateVariation = 0;
    /* 0x0A4 */ float     m_life = 0;
    /* 0x0A8 */ float     m_lifeVariation = 0;
    /* 0x0AC */ float     m_tailLength = 0;
    /* 0x0B0 */ float     m_speed = 0;
    /* 0x0B4 */ float     m_gravity = 0;
    /* 0x0B8 */ float     m_variation = 0;
    /* 0x0BC */ float     m_zsource = 0;
    /* 0x0C0 */ float     m_initialSpin = 0;
    /* 0x0C4 */ float     m_initialSpinVariation = 0;
    /* 0x0C8 */ float     m_spin = 0;
    /* 0x0CC */ float     m_spinVariation = 0;
    /* 0x0D0 */ int32_t   m_materialBlend = 0;
    /* 0x0D4 */ uint32_t  m_materialFlags = 0;
    /* 0x0D8 */ M2PartTrack<C3Vector>* m_colorTrack = nullptr;
    /* 0x0DC */ M2PartTrack<fixed16>*  m_alphaTrack = nullptr;
    /* 0x0E0 */ M2PartTrack<C2Vector>* m_scaleTrack = nullptr;
    /* 0x0E4 */ float     m_scaleVariationX = 0.0f;
    /* 0x0E8 */ float     m_scaleVariationY = 0.0f;
    /* 0x0EC */ M2PartTrack<uint16_t>* m_headCellTrack = nullptr;
    /* 0x0F0 */ M2PartTrack<uint16_t>* m_tailCellTrack = nullptr;
    /* 0x0F4 */ C3Vector  m_replacementColors[3];
    /* 0x118 */ float     m_simpleMidTime = 0;
    /* 0x118 */ CParticleSimpleKeys* m_simpleKeys;
    /* 0x120 */ uint32_t  m_textureCols = 0;
    /* 0x124 */ uint32_t  m_textureRows = 0;
    /* 0x128 */ HTEXTURE  m_texture = nullptr;
    /* 0x12C */
    /* 0x130 */ float     m_alphaScale = 0;
    /* 0x134 */ uint32_t  m_flags = 0;
    /* 0x138 */ uint32_t  m_batchFlags = 0;
    /* 0x13C */ float     m_twinkleFPS = 0;
    /* 0x140 */ float     m_twinkleOnOff = 0;
    /* 0x144 */ float     m_twinkleScaleBase = 0;
    /* 0x148 */ float     m_twinkleScaleSpan = 0;
    /* 0x14C */ float     m_ivelScale = 0;
    /* 0x150 */ float     m_tumbleBaseX = 0;
    /* 0x154 */ float     m_tumbleSpanX = 0;
    /* 0x158 */ float     m_tumbleBaseY = 0;
    /* 0x15C */ float     m_tumbleSpanY = 0;
    /* 0x160 */ float     m_tumbleBaseZ = 0;
    /* 0x164 */ float     m_tumbleSpanZ = 0;
    /* 0x168 */ float     m_drag = 0;
    /* 0x16C */ float     m_windVectorX = 0;
    /* 0x170 */ float     m_windVectorY = 0;
    /* 0x174 */ float     m_windVectorZ = 0;
    /* 0x178 */ float     m_windTime = 0;
    /* 0x17C */ float     m_followIntercept = 0;
    /* 0x180 */ float     m_followSlope = 0;
    /* 0x184 */ C44Matrix m_xform;
    /* 0x1C4 */ C3Vector  m_cameraPos;
    /* 0x1D0 */ C3Vector  m_parentPosition;
    /* 0x1DC */ float     m_ivelTimer = 0;
    /* 0x1E0 */ C3Vector  m_parentVelocity;
    /* 0x1EC */ float     m_xformScale = 0;
    /* 0x1F0 */
    /* 0x1F4 */ C3Vector  m_followOffset;
    /* 0x200 */ C3Vector  m_followOffsetStep;
    /* 0x20C */ C3Vector  m_billboardAxis;
    /* 0x218 */ CAaBox    m_worldBounds;

    /* 00 */ virtual void Sync();
    /* 01 */ virtual void CreateParticle(CParticle2Model* particle, float dt, C44Matrix* xform);
    /* 02 */ virtual void CreateParticle(CParticle2* particle, float dt, C44Matrix* xform);
    /* 03 */ virtual void Unused();
    /* 04 */ virtual void Clone();
    /* 05 */ virtual ~CParticleEmitter2();
    /* 06 */ virtual void SetWidth(float value);
    /* 07 */ virtual void SetHeight(float value);
    /* 08 */ virtual void SetLatitude(float value);
    /* 09 */ virtual void SetLongitude(float value);
    /* 10 */ virtual void SetEmissionRate(float rate);

    CParticleEmitter2();
    CAaBox* WorldBounds();
    void SetZsource(float zsource);
    void SetChooseRandomTexture(int32_t enabled);
    void AddRef();
    void DecRef();
    void SetTwinkleScale(const CRange& scale);
    void SetModel(CM2Scene* scene, const char* fileName);
    void CreateChildEmittersFromModel(CM2Scene* scene, const char* fileName);
    void SetMaterial(const CParticleMaterial* material, HTEXTURE texture);
    void SetFollowParams(float speed1, float scale1, float speed2, float scale2);
    void SetTextureDimensions(uint32_t rows, uint32_t cols);
    void GetReplacementColors(CImVector* color0, CImVector* color1, CImVector* color2);
    void SetParticleColors(const CImVector* color0, const CImVector* color1, const CImVector* color2);
    void DetermineIfSimple();
    void SetParticleStyle(int32_t head, int32_t tail, float tailLength, int32_t style);
    void EmitNewParticles(float dt, C44Matrix* xform);
    void RecycleParticleSlot(float dt, C44Matrix* xform);
    bool HasLiveParticles();
    uint32_t GetNumParticleModels();
    CM2Model* GetParticleModelInternal(uint32_t* index);
    void Update(float dt, C44Matrix* xform, C3Vector* cameraPos, C44Matrix* frameOfReference);
    void UpdateXform(C44Matrix* xform, const C3Vector* cameraPos, C44Matrix* frameOfReference);
    void InternalUpdate(float dt, int32_t noEmit);
    void StepUpdate(float dt, int32_t noEmit);
    int32_t UpdateLiveParticle(float dt, CParticle2* particle, uint32_t liveIndex);
    int32_t MoveParticle(CParticle2* particle, float dt);
    int32_t MoveParticle(CParticle2Model* particle, float dt);
    void DestroyParticle(CParticle2* particle);
    float CalcVelocity();
    void ProjectParticle(CParticle2* particle);
    void SyncAllocation(uint32_t count);
    void SyncReserve(uint32_t count, uint32_t used, uint32_t spare);
    void Render(C44Matrix* xform, void* vertexData, uint8_t enabled);
    void RenderParticles(C44Matrix* xform, void* vertexData);
    void RenderParticlesPrep(C44Matrix* xform, C44Matrix* view);
    void FillOutParticleVertex(void* base, EGxVertexBufferFormat format, ParticleVertexWriter* writer);
    uint32_t BuildParticleVertices(ParticleVertexWriter* writer, uint32_t count, C44Matrix* inverseView);
    void RenderIndices(CGxBuf* buf);
    void RenderParticleVertices(CGxBuf* vertexBuf, EGxVertexBufferFormat format, uint32_t vertexCount, uint32_t indexCount);
    void RandomizeSpin(CParticle2* particle, float* outSpin, float* outSpinRate);
    void InterpolateAllTracksSimple(const float* age, CImVector* color, C2Vector* scale, uint32_t* headCell, uint32_t* tailCell);
    void InterpolateAllTracks(CParticle2* particle, CImVector* color, C2Vector* scale, uint32_t* headCell, uint32_t* tailCell);
    CImVector InterpolateColorTrack(float t);
    int32_t BuildVertex(CParticle2* particle, ParticleVertexWriter* writer);
};

class CPlaneParticleEmitter : public CParticleEmitter2 {
    public:
    /* 0x234 */ float m_width;
    /* 0x238 */ float m_height;
    /* 0x23C */ float m_latitude;
    /* 0x240 */ float m_longitude;

    CPlaneParticleEmitter();
    void SetWidth(float value) override;
    void SetHeight(float value) override;
    void SetLatitude(float value) override;
    void SetLongitude(float value) override;
    void CreateParticle(CParticle2* particle, float dt, C44Matrix* xform) override;
};

class CSphereParticleEmitter : public CParticleEmitter2 {
    public:
    /* 0x234 */ float m_minRadius;
    /* 0x238 */ float m_maxRadius;
    /* 0x23C */ float m_radiusSpan;
    /* 0x240 */ float m_latitude;
    /* 0x244 */ float m_longitude;

    CSphereParticleEmitter();
    void SetWidth(float value) override;
    void SetHeight(float value) override;
    void SetLatitude(float value) override;
    void SetLongitude(float value) override;
    void CreateParticle(CParticle2* particle, float dt, C44Matrix* xform) override;
};

class CSplineParticleEmitter : public CParticleEmitter2 {
    public:
    /* 0x234 */ float m_ratePerUnit;
    /* 0x238 */ float m_width;
    /* 0x23C */ float m_splineLength;
    /* 0x240 */ float m_latitude;
    /* 0x244 */ float m_longitude;
    /* 0x248 */ uint32_t m_spawnAtEnd;
    /* 0x24C */ C3Spline_CatmullRom m_spline;

    CSplineParticleEmitter();
    void SetSpline(C3Vector* points, uint32_t count);
    void SetWidth(float value) override;
    void SetHeight(float value) override;
    void SetLatitude(float value) override;
    void SetLongitude(float value) override;
    void SetEmissionRate(float value) override;
    void CreateParticle(CParticle2* particle, float dt, C44Matrix* xform) override;
};

class ParticleSystemManager {
    public:
    static float g_particleDensity;
    static bool (*s_projectCallback)(C3Vector*, float*, void*);
    static void* s_projectParam;
    static ParticleSystemManager* s_instance;

    static ParticleSystemManager* GetInstance();
    static float GetScaler();
    static void SetScaler(float scaler);
};

#endif
