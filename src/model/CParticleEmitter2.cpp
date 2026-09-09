#include "model/CParticleEmitter2.hpp"
#include "model/CM2Scene.hpp"
#include "model/CM2Model.hpp"
#include "model/CM2Shared.hpp"
#include <cmath>
#include <util/Unimplemented.hpp>
#include <gx/Device.hpp>
#include <gx/Draw.hpp>
#include <gx/Buffer.hpp>

float CParticleEmitter2::s_viewDist;
CGxPool* CParticleEmitter2::s_indexPool;
CGxBuf* CParticleEmitter2::s_indexBuf;
int32_t CParticleEmitter2::s_initCount;
C44Matrix CParticleEmitter2::s_particleXform;
C3Vector CParticleEmitter2::s_particleViewAxis;
C33Matrix CParticleEmitter2::s_particleBasis;
C3Vector CParticleEmitter2::s_zeroNormal = { 0.0f, 0.0f, 0.0f };
TSHeap<CParticleSortEntry> CParticleEmitter2::g_particleSortBuffer = { 32, 32 };
bool CParticleEmitter2::g_particleRenderEnable = 1;

static const C2Vector s_cornerOffset[4] = { { -1.0f, 1.0f }, { -1.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, -1.0f } };
static const C2Vector s_cornerUV[4] = { { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } };
static float s_twinkleTable[128];

// OFFSET: 0x978B70
void FlushDenormals(C3Vector* v) {
    if (v->x != 0.0f && fabs(v->x) < 0.0000000099999999f) {
        v->x = 0.0f;
    }

    if (v->y != 0.0f && fabs(v->y) < 0.0000000099999999f) {
        v->y = 0.0f;
    }

    if (v->z != 0.0f && fabs(v->z) < 0.0000000099999999f) {
        v->z = 0.0f;
    }
}

// OFFSET: 0x978AD0
int16_t PackFloatToInt16(float a2) {
    if (a2 < 1.0) {
        if (a2 > -1.0)
            return (a2 * 32767.0 + 0.5);
        else
            return -32767;
    } else {
        return 0x7FFF;
    }
}

// OFFSET: 0x4C1680
void RandomUnitVector(C3Vector* out, CRndSeed* seed) {
    uint32_t zRaw = CRandom::uint32(*seed);
    uint32_t zBits = (zRaw & 0x7FFFFF) | 0x3F800000;
    float zUnit = *reinterpret_cast<float*>(&zBits);
    float z = static_cast<int32_t>(zRaw) >= 0 ? zUnit - 2.0f : 2.0f - zUnit;

    uint32_t angleBits = (CRandom::uint32(*seed) & 0x7FFFFF) | 0x3F800000;
    float angle = (*reinterpret_cast<float*>(&angleBits) - 1.0f) * 6.2831855f;

    float radius = sqrt(1.0f - (z * z));

    out->x = cos(angle) * radius;
    out->y = radius * sin(angle);
    out->z = z;
}

// OFFSET: 0x979330
template <typename T1>
uint32_t M2PartTrackSearch(const M2PartTrack<T1>* track, float t) {
    uint32_t high = track->times.Count() - 1;

    if (track->times.Count() == 1) {
        return 0;
    }

    uint32_t low = 0;

    while (true) {
        uint32_t mid = (high + low) >> 1;

        if (static_cast<float>(track->times[mid].n) * 0.0000305185094f <= t) {
            low = mid + 1;

            if (low >= track->times.Count() - 1 || static_cast<float>(track->times[mid + 1].n) * 0.0000305185094f > t) {
                return mid;
            }
        } else {
            high = mid - 1;
        }

        if (low >= high) {
            return low;
        }
    }
}

// OFFSET: 0x9793B0
template<typename T1>
float M2PartTrackFindKeyPair(const M2PartTrack<T1>* track, float t, uint32_t* low, uint32_t* high) {
    if (track->times.Count() == 2) {
        *low = 0;
        *high = 1;
        return t;
    }

    if (track->times.Count() == 3) {
        float middle = static_cast<float>(track->times[1].n) * 0.0000305185094f;

        if (t >= middle) {
            *low = 1;
            *high = 2;
            return (t - middle) / (1.0f - middle);
        }

        *low = 0;
        *high = 1;
        return t / middle;
    }

    uint32_t index = M2PartTrackSearch(track, t);

    *low = index;
    *high = index + 1;

    float lowTime = static_cast<float>(track->times[*low].n) * 0.0000305185094f;

    return (t - lowTime) / (0.0000305185094f * static_cast<float>(track->times[*high].n - track->times[*low].n));
}

// OFFSET: 0x979480
void M2PartTrackSample(C2Vector* out, const M2PartTrack<C2Vector>* track, float t) {
    if (track->values.Count() == 1) {
        *out = track->values[0];
        return;
    }

    uint32_t low;
    uint32_t high;
    float ratio = M2PartTrackFindKeyPair(track, t, &low, &high);

    out->x = ((track->values[high].x - track->values[low].x) * ratio) + track->values[low].x;
    out->y = (ratio * (track->values[high].y - track->values[low].y)) + track->values[low].y;
}

// OFFSET: 0x9794F0
float M2PartTrackSampleFixed16(const M2PartTrack<fixed16>* track, float t) {
    if (track->values.Count() == 1) {
        return static_cast<float>(track->values[0].n) * 0.0000305185094f;
    }

    uint32_t low;
    uint32_t high;
    float ratio = M2PartTrackFindKeyPair(track, t, &low, &high);

    float lowValue = static_cast<float>(track->values[low].n) * 0.0000305185094f;
    float highValue = static_cast<float>(track->values[high].n) * 0.0000305185094f;

    return ((highValue - lowValue) * ratio) + lowValue;
}

// OFFSET: 0x979560
uint32_t M2PartTrackSampleUint16(const M2PartTrack<uint16_t>* track, float t) {
    if (track->values.Count() == 1) {
        return track->values[0];
    }

    uint32_t low;
    uint32_t high;
    float ratio = M2PartTrackFindKeyPair(track, t, &low, &high);

    uint32_t lowValue = track->values[low];
    int32_t delta = track->values[high] - lowValue;

    return static_cast<uint32_t>((ratio * delta) + lowValue);
}

// OFFSET: 0x979170
void CParticleEmitter2::Init() {
    if (CParticleEmitter2::s_initCount == 0) {
        CParticleEmitter2::s_indexPool = g_theGxDevicePtr->PoolCreate(static_cast<EGxPoolTarget>(1), static_cast<EGxPoolUsage>(0), 0x3FFF0, static_cast<EGxPoolHintBits>(2), "CParticleEmitter2_idx");
        CParticleEmitter2::s_indexBuf = g_theGxDevicePtr->BufCreate(CParticleEmitter2::s_indexPool, 2, 0x1FFF8, 0);
    }

    CParticleEmitter2::s_initCount++;
}

// OFFSET: 0x9791E0
void CParticleEmitter2::Destroy() {
    CParticleEmitter2::s_initCount--;

    if (CParticleEmitter2::s_initCount != 0) {
        return;
    }

    CGxBuf* buf = CParticleEmitter2::s_indexBuf;

    if (buf) {
        //if (g_theGxDevicePtr->BufStream(buf->m_pool->m_target, 0, 0) != buf) {
        //    g_theGxDevicePtr->BufDestroy(&buf);
        //}
    }

    if (CParticleEmitter2::s_indexPool) {
        //g_theGxDevicePtr->PoolDestroy(CParticleEmitter2::s_indexPool);
    }
}

// OFFSET: 0x97B9C0
void CParticleSimpleKeys::Constructor() {
    this->m_color.value = 0;
    this->m_redDelta = 0;
    this->m_greenDelta = 0;
    this->m_blueDelta = 0;
    this->m_alphaDelta = 0;
}

// OFFSET: 0x97E150
CParticleEmitter2::CParticleEmitter2()
    : m_random(0) {
    this->m_emitAccumulator = 0.0f;
    this->m_refCount = 1;
    this->m_textureCellShift = 0;
    this->m_textureCellWidth = 1.0f;
    this->m_textureCellHeight = 1.0f;
    this->m_renderedParticles = 0;
    this->m_emitterType = 0;

    this->m_childEmitterCount = 4;

    this->m_model = nullptr;
    this->m_childModel = nullptr;
    this->m_vertsPerParticle = 0;
    this->m_indicesPerParticle = 0;

    this->m_hasModel = 0;
    this->m_emissionRate = 0.0f;
    this->m_emissionRateVariation = 0.0f;
    this->m_life = 0.0f;
    this->m_lifeVariation = 0.0f;
    this->m_tailLength = 1.0f;
    this->m_speed = 0.0f;
    this->m_gravity = 0.0f;
    this->m_variation = 0.1f;
    this->m_zsource = 0.0f;
    this->m_initialSpin = 0.0f;
    this->m_initialSpinVariation = 0.0f;
    this->m_spin = 0.0f;
    this->m_spinVariation = 0.0f;
    this->m_materialBlend = 0;
    this->m_materialFlags = 7;
    this->m_colorTrack = nullptr;
    this->m_alphaTrack = nullptr;
    this->m_scaleTrack = nullptr;
    this->m_scaleVariationX = 0.0f;
    this->m_scaleVariationY = 0.0f;
    this->m_headCellTrack = nullptr;
    this->m_tailCellTrack = nullptr;
    this->m_replacementColors[0].x = 0.0f;
    this->m_replacementColors[0].y = 0.0f;
    this->m_replacementColors[0].z = 0.0f;
    this->m_replacementColors[1].x = 0.0f;
    this->m_replacementColors[1].y = 0.0f;
    this->m_replacementColors[1].z = 0.0f;
    this->m_replacementColors[2].x = 0.0f;
    this->m_replacementColors[2].y = 0.0f;
    this->m_replacementColors[2].z = 0.0f;
    this->m_simpleMidTime = 0.0f;
    this->m_simpleKeys = nullptr;

    this->m_alphaScale = 1.0f;
    this->m_texture = nullptr;
    this->m_twinkleFPS = 10.0f;
    this->m_flags = 6;
    this->m_textureCols = 1;
    this->m_twinkleOnOff = 1.0f;
    this->m_textureRows = 1;
    this->m_twinkleScaleBase = 1.0f;
    this->m_twinkleScaleSpan = 0.0f;
    this->m_ivelScale = 1.0f;
    this->m_tumbleBaseX = 0.0f;
    this->m_tumbleSpanX = 0.0f;
    this->m_tumbleBaseY = 0.0f;
    this->m_tumbleSpanY = 0.0f;
    this->m_tumbleBaseZ = 0.0f;
    this->m_tumbleSpanZ = 0.0f;
    this->m_drag = 0.0f;
    this->m_windVectorX = 0.0f;
    this->m_windVectorY = 0.0f;
    this->m_windVectorZ = 0.0f;
    this->m_windTime = 0.0f;
    this->m_followIntercept = 0.0f;
    this->m_followSlope = 0.0f;

    this->m_xform = C44Matrix();

    this->m_cameraPos.x = 0.0f;
    this->m_cameraPos.y = 0.0f;
    this->m_cameraPos.z = 0.0f;
    this->m_parentPosition.x = 0.0f;
    this->m_parentPosition.y = 0.0f;
    this->m_parentPosition.z = 0.0f;
    this->m_ivelTimer = 0.0f;
    this->m_parentVelocity.x = 0.0f;
    this->m_parentVelocity.y = 0.0f;
    this->m_parentVelocity.z = 0.0f;
    this->m_followOffset.x = 0.0f;
    this->m_followOffset.y = 0.0f;
    this->m_followOffset.z = 0.0f;
    this->m_followOffsetStep.x = 0.0f;
    this->m_followOffsetStep.y = 0.0f;
    this->m_followOffsetStep.z = 0.0f;
    this->m_billboardAxis.x = 0.0f;
    this->m_billboardAxis.y = 0.0f;
    this->m_billboardAxis.z = 0.0f;

    this->m_worldBounds.b.x = FLT_MAX;
    this->m_worldBounds.b.y = FLT_MAX;
    this->m_worldBounds.b.z = FLT_MAX;
    this->m_worldBounds.t.x = -FLT_MAX;
    this->m_worldBounds.t.y = -FLT_MAX;
    this->m_worldBounds.t.z = -FLT_MAX;

    uint32_t seed = (rand() << 16) | (rand() & 0xFFFF);
    this->m_random.SetSeed(seed);

    this->m_childEmitterCount = 0;
}

// OFFSET: 0x97EDF0
void CParticleEmitter2::Sync() {
    uint32_t reserve = static_cast<uint32_t>((this->m_lifeVariation + this->m_life) * (this->m_emissionRateVariation + this->m_emissionRate) * 1.14999998f);

    this->SyncAllocation(reserve);

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        CParticleEmitter2* child = this->m_childEmitters[i];
        uint32_t childReserve = static_cast<uint32_t>((child->m_lifeVariation + child->m_life) * (child->m_emissionRateVariation + child->m_emissionRate) * 1.14999998f);

        childReserve = childReserve * reserve;

        if (childReserve > 0x1000) {
            childReserve = 0x1000;
        }

        child->SyncAllocation(childReserve);
    }
}

// OFFSET: 0x979870
void CParticleEmitter2::CreateParticle(CParticle2* particle, float dt, C44Matrix* xform) {
    uint32_t ageBits = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
    float age = (*reinterpret_cast<float*>(&ageBits) - 1.0f) * dt;

    uint32_t seedRaw = CRandom::uint32(this->m_random);
    uint32_t seedBits = (seedRaw & 0x7FFFFF) | 0x3F800000;
    float seedUnit = *reinterpret_cast<float*>(&seedBits);
    float seedSigned = static_cast<int32_t>(seedRaw) >= 0 ? seedUnit - 2.0f : 2.0f - seedUnit;

    particle->m_lifeSeed = PackFloatToInt16(seedSigned);
    particle->m_age = age < 0.0f ? 0.0f : age;
    particle->m_randomSeed = static_cast<uint16_t>(CRandom::uint32(this->m_random));

    particle->m_position.x = 0.0f;
    particle->m_position.y = 0.0f;
    particle->m_position.z = 0.0f;

    if ((this->m_flags & 0x200) == 0) {
        particle->m_position = particle->m_position  * *xform;

        if ((this->m_flags & 0x40000) != 0) {
            this->ProjectParticle(particle);
        }
    }

    float speed = this->CalcVelocity();
    C3Vector direction;
    RandomUnitVector(&direction, &this->m_random);

    particle->m_velocity.x = (direction.x * speed) + this->m_parentVelocity.x;
    particle->m_velocity.y = (direction.y * speed) + this->m_parentVelocity.y;
    particle->m_velocity.z = (direction.z * speed) + this->m_parentVelocity.z;

    FlushDenormals(&particle->m_velocity);
}

// OFFSET: 0x9799C0
void CParticleEmitter2::CreateParticle(CParticle2Model* particle, float dt, C44Matrix* xform) {
    this->CreateParticle(&particle->base, dt, xform);

    if ((this->m_flags & 0x200) == 0) {
        float basis[9];
        basis[0] = xform->a0;
        basis[1] = xform->a1;
        basis[2] = xform->a2;
        basis[3] = xform->b0;
        basis[4] = xform->b1;
        basis[5] = xform->b2;
        basis[6] = xform->c0;
        basis[7] = xform->c1;
        basis[8] = xform->c2;

        float lengthSq = (basis[1] * basis[1]) + (basis[2] * basis[2]) + (basis[0] * basis[0]);

        if (lengthSq > 2.384185791015625e-07f) {
            float invLength = 1.0f / sqrt(lengthSq);

            basis[0] = basis[0] * invLength;
            basis[1] = basis[1] * invLength;
            basis[2] = basis[2] * invLength;
        }

        particle->m_orientation.FromBasis(basis);
        particle->m_orientation.Normalize();
    }

    uint32_t bitsZ = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
    uint32_t bitsY = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
    uint32_t bitsX = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;

    float unitZ = *reinterpret_cast<float*>(&bitsZ);
    float unitY = *reinterpret_cast<float*>(&bitsY);
    float unitX = *reinterpret_cast<float*>(&bitsX);

    particle->m_angularVelocity.x = ((unitX - 1.0f) * this->m_tumbleSpanX) + this->m_tumbleBaseX;
    particle->m_angularVelocity.y = unitY * this->m_tumbleSpanY;
    particle->m_angularVelocity.z = unitZ * this->m_tumbleSpanZ;

    if ((this->m_flags & 0x10000) != 0) {
        float signZ = (CRandom::uint32(this->m_random) & 1) ? 1.0f : -1.0f;
        float signY = (CRandom::uint32(this->m_random) & 1) ? 1.0f : -1.0f;
        float signX = (CRandom::uint32(this->m_random) & 1) ? 1.0f : -1.0f;

        particle->m_angularVelocity.x = signX * particle->m_angularVelocity.x;
        particle->m_angularVelocity.y = particle->m_angularVelocity.y * signY;
        particle->m_angularVelocity.z = particle->m_angularVelocity.z * signZ;
    }

    if (!particle->m_model) {
        particle->m_model = this->m_scene->CreateModel(this->m_model->m_shared->m_filePath, 0);
    }
}

void CParticleEmitter2::Unused() {
    WHOA_UNIMPLEMENTED();
}

void CParticleEmitter2::Clone() {
    WHOA_UNIMPLEMENTED();
}

CParticleEmitter2::~CParticleEmitter2() {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: none
void CParticleEmitter2::SetWidth(float width) {
    
}

// OFFSET: none
void CParticleEmitter2::SetHeight(float height) {
    
}

// OFFSET: none
void CParticleEmitter2::SetLatitude(float latitude) {
    
}

// OFFSET: none
void CParticleEmitter2::SetLongitude(float longitude) {
    
}

// OFFSET: 0x97BD80
void CParticleEmitter2::SetEmissionRate(float rate) {
    if (rate >= 0.0f) {
        this->m_emissionRate = rate;
    } else {
        this->m_emissionRate = 0.0f;
    }
}

// OFFSET: 0x978BE0
CAaBox* CParticleEmitter2::WorldBounds() {
    return &this->m_worldBounds;
}

// OFFSET: 0x978DA0
void CParticleEmitter2::SetZsource(float zsource) {
    this->m_zsource = zsource;

    if (fabs(zsource) <= 0.001f) {
        this->m_zsource = 0.0f;
    }
}

// OFFSET: 0x978E30
void CParticleEmitter2::SetChooseRandomTexture(int32_t enabled) {
    if (this->m_textureRows * this->m_textureCols > 1 && enabled) {
        this->m_flags |= 0x100000;
    } else {
        this->m_flags &= ~0x100000u;
    }
}

// OFFSET: 0x978E70
void CParticleEmitter2::AddRef() {
    this->m_refCount++;
}

// OFFSET: 0x978E80
void CParticleEmitter2::DecRef() {
    this->m_refCount--;

    if (this->m_refCount == 0) {
        delete this;
    }
}

// OFFSET: 0x97AC00
void CParticleEmitter2::SetTwinkleScale(const CRange& scale) {
    this->m_twinkleScaleBase = scale.l;
    this->m_twinkleScaleSpan = scale.h - scale.l;
}

// OFFSET: 0x978B30
void CParticleEmitter2::SetModel(CM2Scene* scene, const char* fileName) {
    if (scene && fileName && fileName[0]) {
        this->m_scene = scene;
        this->m_model = this->m_scene->CreateModel(fileName, 0);
        if (this->m_model)
            this->m_hasModel = 1;
    }
}

// OFFSET: 0x97AE00
void RecursiveEmitterModelLoaded(CM2Model* model, void* arg) {
    if (!model || !arg) {
        return;
    }

    CParticleEmitter2* parent = static_cast<CParticleEmitter2*>(arg);

    if ((model->f_flags & 0x1) == 0) {
        model->WaitForLoad(nullptr);
    }

    uint32_t count = 4;

    if (model->m_shared->m_data->particles.Count() < 4) {
        if ((model->f_flags & 0x1) == 0) {
            model->WaitForLoad(nullptr);
        }

        count = model->m_shared->m_data->particles.Count();
    }

    for (uint32_t i = 0; i < count; i++) {
        if ((model->f_flags & 0x1) == 0) {
            model->WaitForLoad(nullptr);
        }

        CParticleEmitter2* child = model->m_particleEmitters[i];

        parent->m_childEmitters[parent->m_childEmitterCount] = child;
        parent->m_childEmitterCount++;

        child->m_flags |= 0x1;
    }
}

// OFFSET: 0x97AEB0
void CParticleEmitter2::CreateChildEmittersFromModel(CM2Scene* scene, const char* fileName) {
    if (!scene) {
        return;
    }

    this->m_childModel = scene->CreateModel(fileName, 0);

    if (this->m_childModel) {
        this->m_childModel->SetLoadedCallback(&RecursiveEmitterModelLoaded, this);
    }
}

// OFFSET: 0x978BF0
void CParticleEmitter2::SetMaterial(const CParticleMaterial* material, HTEXTURE texture) {
    if (this->m_texture) {
        HandleClose(this->m_texture);
    }

    this->m_texture = HandleDuplicate(texture);
    this->m_materialBlend = material->blend;
    this->m_materialFlags = material->flags;
}

// OFFSET: 0x978DD0
void CParticleEmitter2::SetFollowParams(float speed1, float scale1, float speed2, float scale2) {
    if (fabs(speed2 - speed1) < 0.00000023841858f) {
        this->m_followSlope = 0.0f;
        this->m_followIntercept = 0.0f;
        return;
    }

    this->m_followSlope = (scale2 - scale1) / (speed2 - speed1);
    this->m_followIntercept = scale1 - (speed1 * this->m_followSlope);
}

// OFFSET: 0x978C70
void CParticleEmitter2::SetTextureDimensions(uint32_t rows, uint32_t cols) {
    if ((rows & (rows - 1)) || (cols & (cols - 1)) || !rows || !cols) {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return;
    }

    this->m_textureRows = rows;
    this->m_textureCols = cols;

    int32_t shift = -1;

    for (uint32_t v = cols; v; v >>= 1) {
        shift++;
    }

    this->m_textureCellShift = shift;
    this->m_textureCellWidth = 1.0f / static_cast<float>(this->m_textureCols);
    this->m_textureCellHeight = 1.0f / static_cast<float>(rows);
}

// OFFSET: 0x97AB10
void CParticleEmitter2::GetReplacementColors(CImVector* color0, CImVector* color1, CImVector* color2) {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x97A990
void CParticleEmitter2::SetParticleColors(const CImVector* color0, const CImVector* color1, const CImVector* color2) {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x97D370
void CParticleEmitter2::DetermineIfSimple() {
    WHOA_UNIMPLEMENTED();
}

// OFFSET: 0x978D00
void CParticleEmitter2::SetParticleStyle(int32_t head, int32_t tail, float tailLength, int32_t style) {
    if (head) {
        this->m_flags |= 0x4;
    } else {
        this->m_flags &= ~0x4u;
    }
    if (tail) {
        this->m_flags |= 0x8;
    } else {
        this->m_flags &= ~0x8u;
    }
    if (style) {
        this->m_flags |= 0x20000;
    } else {
        this->m_flags &= ~0x20000u;
    }

    this->m_vertsPerParticle = 0;
    this->m_indicesPerParticle = 0;
    this->m_tailLength = tailLength;

    if (this->m_flags & 0x4) {
        this->m_vertsPerParticle = 4;
        this->m_indicesPerParticle = 6;
    }
    if (this->m_flags & 0x8) {
        this->m_vertsPerParticle += 4;
        this->m_indicesPerParticle += 6;
    }
}

// OFFSET: 0x97D8C0
void CParticleEmitter2::EmitNewParticles(float dt, C44Matrix* xform) {
    float densityScale = 1.0f;

    if ((this->m_flags & 0x400000) == 0) {
        densityScale = 1.0f - ((CParticleEmitter2::s_viewDist - 50.0f) * 0.02f);

        if (densityScale < 0.25f) {
            densityScale = 0.25f;
        } else if (densityScale > 1.0f) {
            densityScale = 1.0f;
        }
    }

    uint32_t rateBits = CRandom::uint32(this->m_random);
    uint32_t rateMantissa = (rateBits & 0x7FFFFF) | 0x3F800000;
    float rateUnit = *reinterpret_cast<float*>(&rateMantissa);
    float rateSigned = static_cast<int32_t>(rateBits) < 0 ? 2.0f - rateUnit : rateUnit - 2.0f;

    float rate = this->m_emissionRate + (rateSigned * this->m_emissionRateVariation);
    rate = rate * ParticleSystemManager::GetScaler() * densityScale;

    if ((this->m_flags & 0x40) != 0 && (this->m_flags & 0x2) != 0) {
        int32_t burst = static_cast<int32_t>(rate);

        while (this->m_freeList.Count()) {
            if (burst-- == 0) {
                break;
            }

            this->RecycleParticleSlot(0.0f, xform);
        }

        this->m_flags &= ~0x40u;
    }

    if ((this->m_flags & 0x3) != 0x3) {
        return;
    }

    this->m_emitAccumulator += rate * dt;

    uint32_t emitted = 0;

    if ((this->m_flags & 0x2000) != 0) {
        C3Vector origin;
        origin.x = xform->d0;
        origin.y = xform->d1;
        origin.z = xform->d2;

        C3Vector travel;
        travel.x = origin.x - this->m_parentPosition.x;
        travel.y = origin.y - this->m_parentPosition.y;
        travel.z = origin.z - this->m_parentPosition.z;

        int32_t count = static_cast<int32_t>(this->m_emitAccumulator + 0.5f);

        while (count > 0 && this->m_freeList.Count()) {
            uint32_t stepBits = CRandom::uint32(this->m_random);
            uint32_t stepMantissa = (stepBits & 0x7FFFFF) | 0x3F800000;
            float t = *reinterpret_cast<float*>(&stepMantissa) - 1.0f;

            xform->d0 = this->m_parentPosition.x + (travel.x * t);
            xform->d1 = this->m_parentPosition.y + (travel.y * t);
            xform->d2 = this->m_parentPosition.z + (travel.z * t);

            this->RecycleParticleSlot(dt, xform);

            emitted++;
            count--;
        }

        xform->d0 = origin.x;
        xform->d1 = origin.y;
        xform->d2 = origin.z;
    } else {
        int32_t count = static_cast<int32_t>(this->m_emitAccumulator + 0.5f);

        while (count > 0 && this->m_freeList.Count()) {
            this->RecycleParticleSlot(dt, xform);

            emitted++;
            count--;
        }
    }

    this->m_emitAccumulator -= static_cast<float>(emitted);
}

// OFFSET: 0x97D820
void CParticleEmitter2::RecycleParticleSlot(float dt, C44Matrix* xform) {
    uint32_t index = this->m_freeList.Pop();
    this->m_liveList.Add(&index);
    if (this->m_hasModel)
        return this->CreateParticle(&this->m_particleModels[index], dt, xform);
    return this->CreateParticle(&this->m_particles[index], dt, xform);
}

// OFFSET: 0x97B9E0
bool CParticleEmitter2::HasLiveParticles() {
    if (this->m_liveList.Count()) {
        return 1;
    }

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        if (this->m_childEmitters[i]->HasLiveParticles()) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x97BA30
uint32_t CParticleEmitter2::GetNumParticleModels() {
    uint32_t count = 0;

    if (this->m_hasModel == 1) {
        count = this->m_liveList.Count();
    }

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        count += this->m_childEmitters[i]->GetNumParticleModels();
    }

    return count;
}

// OFFSET: 0x97BA70
CM2Model* CParticleEmitter2::GetParticleModelInternal(uint32_t* index) {
    if (this->m_hasModel == 1) {
        if (*index < this->m_liveList.Count()) {
            uint32_t slot = this->m_liveList[*index];
            return this->m_particleModels[slot].m_model;
        }

        *index -= this->m_liveList.Count();
    }

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        CM2Model* model = this->m_childEmitters[i]->GetParticleModelInternal(index);

        if (model) {
            return model;
        }
    }

    return nullptr;
}

// OFFSET: 0x97EB10
void CParticleEmitter2::Update(float dt, C44Matrix* xform, C3Vector* cameraPos, C44Matrix* frameOfReference) {
    float dx = xform->d0 - cameraPos->x;
    float dy = xform->d1 - cameraPos->y;
    float dz = xform->d2 - cameraPos->z;
    float distSq = (dz * dz) + (dy * dy) + (dx * dx);

    CParticleEmitter2::s_viewDist = distSq > 0.0f ? sqrt(distSq) : 0.0f;

    this->m_parentPosition.x = this->m_xform.d0;
    this->m_parentPosition.y = this->m_xform.d1;
    this->m_parentPosition.z = this->m_xform.d2;

    this->UpdateXform(xform, cameraPos, frameOfReference);

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        this->m_childEmitters[i]->UpdateXform(xform, cameraPos, frameOfReference);
    }

    if (fabs(dt) < 2.384185791015625e-07f) {
        this->m_flags |= 0x100;
        return;
    }

    if ((this->m_flags & 0x80000) != 0) {
        this->m_followOffset.x = this->m_xform.d0 - this->m_parentPosition.x;
        this->m_followOffset.y = this->m_xform.d1 - this->m_parentPosition.y;
        this->m_followOffset.z = this->m_xform.d2 - this->m_parentPosition.z;

        float lengthSq = (this->m_followOffset.z * this->m_followOffset.z) + (this->m_followOffset.y * this->m_followOffset.y) + (this->m_followOffset.x * this->m_followOffset.x);
        float speed = 0.0f;

        if (lengthSq > 0.0f) {
            speed = sqrt(lengthSq) / dt;
        }

        float scale = (speed * this->m_followSlope) + this->m_followIntercept;

        if (scale < 0.0f) {
            scale = 0.0f;
        } else if (scale >= 1.0f) {
            scale = 1.0f;
        }

        this->m_followOffset.x = this->m_followOffset.x * scale;
        this->m_followOffset.y = this->m_followOffset.y * scale;
        this->m_followOffset.z = this->m_followOffset.z * scale;
    }

    if ((this->m_flags & 0x800) != 0) {
        this->m_ivelTimer += dt;

        if (this->m_ivelTimer > 0.0333333351f) {
            float elapsed = this->m_ivelTimer * 29.9999981f;
            this->m_ivelTimer = 0.0f;

            if (this->m_liveList.Count() == 0) {
                this->m_parentVelocity.x = 0.0f;
                this->m_parentVelocity.y = 0.0f;
                this->m_parentVelocity.z = 0.0f;
            } else {
                this->m_parentVelocity.x = this->m_xform.d0 - this->m_parentPosition.x;
                this->m_parentVelocity.y = this->m_xform.d1 - this->m_parentPosition.y;
                this->m_parentVelocity.z = this->m_xform.d2 - this->m_parentPosition.z;

                float factor = (1.0f / elapsed) * this->m_ivelScale;

                this->m_parentVelocity.x = this->m_parentVelocity.x * factor;
                this->m_parentVelocity.y = this->m_parentVelocity.y * factor;
                this->m_parentVelocity.z = this->m_parentVelocity.z * factor;
            }
        }
    }

    this->InternalUpdate(dt, 0);

    this->m_flags |= 0x80;

    if (this->m_hasModel == 1) {
        //this->PlaceParticleModels(frameOfReference);
    }
}

// OFFSET: 0x97AC20
void CParticleEmitter2::UpdateXform(C44Matrix* xform, const C3Vector* cameraPos, C44Matrix* frameOfReference) {
    this->m_cameraPos.x = cameraPos->x;
    this->m_cameraPos.y = cameraPos->y;
    this->m_cameraPos.z = cameraPos->z;

    if (!frameOfReference || (this->m_flags & 0x200) != 0) {
        this->m_xform = *xform;
    } else {
        this->m_xform = *xform * frameOfReference->AffineInverse();
    }

    this->m_xformScale = sqrt((xform->a2 * xform->a2) + (xform->a1 * xform->a1) + (xform->a0 * xform->a0));
}

// OFFSET: 0x97ACB0
void CParticleEmitter2::InternalUpdate(float dt, int32_t noEmit) {
    if (dt < 0.0f || dt <= 0.1f) {
        float step = dt < 0.0f ? 0.0f : dt;

        this->m_followOffsetStep.x = this->m_followOffset.x;
        this->m_followOffsetStep.y = this->m_followOffset.y;
        this->m_followOffsetStep.z = this->m_followOffset.z;

        this->StepUpdate(step, noEmit);
        return;
    }

    float steps = floor(dt * 10.0f);
    float remainder = dt - (steps * 0.1f);
    float lifeSteps = floor(this->m_life * 10.0f);

    if (lifeSteps < steps) {
        steps = lifeSteps;
    }

    int32_t extraSteps = lrintf(steps - 0.5f);
    float scale = 1.0f / static_cast<float>(static_cast<uint32_t>(extraSteps + 1));

    this->m_followOffsetStep.x = this->m_followOffset.x * scale;
    this->m_followOffsetStep.y = this->m_followOffset.y * scale;
    this->m_followOffsetStep.z = this->m_followOffset.z * scale;

    for (int32_t i = extraSteps; i > 0; i--) {
        this->StepUpdate(0.1f, noEmit);
    }

    this->StepUpdate(remainder, noEmit);
}

// OFFSET: 0x979BB0
int32_t CParticleEmitter2::MoveParticle(CParticle2* particle, float dt) {
    if (this->m_windTime > particle->m_age) {
        particle->m_velocity.x += this->m_windVectorX * dt;
        particle->m_velocity.y += this->m_windVectorY * dt;
        particle->m_velocity.z += this->m_windVectorZ * dt;

        FlushDenormals(&particle->m_velocity);
    }

    if ((this->m_flags & 0x80000) != 0 && (dt + dt) < particle->m_age) {
        particle->m_position.x += this->m_followOffsetStep.x;
        particle->m_position.y += this->m_followOffsetStep.y;
        particle->m_position.z += this->m_followOffsetStep.z;
    }

    float stepX = particle->m_velocity.x * dt;
    float stepY = particle->m_velocity.y * dt;
    float stepZ = particle->m_velocity.z * dt;

    particle->m_position.x += stepX;
    particle->m_position.y += stepY;
    particle->m_position.z += stepZ - (this->m_gravity * dt * dt * 0.5f);
    particle->m_velocity.z -= this->m_gravity * dt;

    if (this->m_drag != 0.0f) {
        float drag = this->m_drag * dt;

        if (drag > 1.0f) {
            drag = 1.0f;
        }

        particle->m_velocity.x -= particle->m_velocity.x * drag;
        particle->m_velocity.y -= particle->m_velocity.y * drag;
        particle->m_velocity.z -= particle->m_velocity.z * drag;
    }

    FlushDenormals(&particle->m_velocity);

    if ((this->m_flags & 0x1000) != 0) {
        float dot;

        if ((this->m_flags & 0x200) != 0) {
            dot = (particle->m_position.z * stepZ) + (particle->m_position.y * stepY) + (particle->m_position.x * stepX);
        } else {
            dot = ((particle->m_position.z - this->m_xform.d2) * stepZ) + ((particle->m_position.y - this->m_xform.d1) * stepY) + ((particle->m_position.x - this->m_xform.d0) * stepX);
        }

        if (dot > 0.0f) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x97BDB0
int32_t CParticleEmitter2::MoveParticle(CParticle2Model* particle, float dt) {
    float length = sqrt((particle->m_angularVelocity.x * particle->m_angularVelocity.x) + (particle->m_angularVelocity.y * particle->m_angularVelocity.y) + (particle->m_angularVelocity.z * particle->m_angularVelocity.z));

    if (length > 0.0000999999975f) {
        float halfAngle = length * dt * 0.5f;
        float scale = sin(halfAngle) / length;

        C4Quaternion delta;
        delta.x = particle->m_angularVelocity.x * scale;
        delta.y = particle->m_angularVelocity.y * scale;
        delta.z = particle->m_angularVelocity.z * scale;
        delta.w = cos(halfAngle);

        particle->m_orientation = particle->m_orientation * delta;
    }

    return this->MoveParticle(&particle->base, dt);
}

// OFFSET: 0x97DB80
int32_t CParticleEmitter2::UpdateLiveParticle(float dt, CParticle2* particle, uint32_t liveIndex) {
    C3Vector startPosition = particle->m_position;
    int32_t alive;

    if (this->m_hasModel) {
        alive = this->MoveParticle(reinterpret_cast<CParticle2Model*>(particle), dt);
    } else {
        alive = this->MoveParticle(particle, dt);
    }

    if (!alive) {
        this->DestroyParticle(particle);

        uint32_t slot = this->m_liveList[liveIndex];
        this->m_freeList.Add(&slot);

        this->m_liveList[liveIndex] = this->m_liveList[this->m_liveList.Count() - 1];
        this->m_liveList.Pop();

        return 0;
    }

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        CParticleEmitter2* child = this->m_childEmitters[i];

        float savedX = this->m_xform.d0;
        float savedY = this->m_xform.d1;
        float savedZ = this->m_xform.d2;

        this->m_xform.d0 = particle->m_position.x;
        this->m_xform.d1 = particle->m_position.y;
        this->m_xform.d2 = particle->m_position.z;

        if ((child->m_flags & 0x800) != 0) {
            child->m_parentVelocity = particle->m_velocity;
        }

        if ((child->m_flags & 0x2000) != 0) {
            child->m_parentPosition = startPosition;
        }

        child->EmitNewParticles(dt, &this->m_xform);

        this->m_xform.d0 = savedX;
        this->m_xform.d1 = savedY;
        this->m_xform.d2 = savedZ;
    }

    return 1;
}

// OFFSET: 0x97DD20
void CParticleEmitter2::StepUpdate(float dt, int32_t noEmit) {
    if (dt <= 0.0f) {
        return;
    }

    if ((this->m_flags & 0x3) == 0x3 || ((this->m_flags & 0x40) != 0 && (this->m_flags & 0x2) != 0)) {
        this->Sync();
    }

    if (!noEmit) {
        this->EmitNewParticles(dt, &this->m_xform);
    }

    uint32_t i = 0;

    while (i < this->m_liveList.Count()) {
        uint32_t slot = this->m_liveList[i];
        CParticle2* particle;

        if (this->m_hasModel) {
            particle = &this->m_particleModels[slot].base;
        } else {
            particle = &this->m_particles[slot];
        }

        particle->m_age += dt;

        float lifespan = 0.001f;

        if (this->m_lifeVariation == 0.0f) {
            if (this->m_life > 0.001f) {
                lifespan = this->m_life;
            }
        } else {
            float varied = this->m_life + (static_cast<float>(particle->m_lifeSeed) * this->m_lifeVariation * 0.0000305185094f);

            if (varied > 0.001f) {
                lifespan = varied;
            }
        }

        if (particle->m_age > lifespan) {
            this->DestroyParticle(particle);

            this->m_freeList.Add(&slot);

            this->m_liveList[i] = this->m_liveList[this->m_liveList.Count() - 1];
            this->m_liveList.Pop();
        } else if (this->UpdateLiveParticle(dt, particle, i)) {
            i++;
        }
    }

    for (uint32_t c = 0; c < this->m_childEmitterCount; c++) {
        this->m_childEmitters[c]->InternalUpdate(dt, 1);
    }
}

// OFFSET: 0x632050
void CParticleEmitter2::DestroyParticle(CParticle2* particle) {
}

// OFFSET: 0x9792D0
float CParticleEmitter2::CalcVelocity() {
    uint32_t raw = CRandom::uint32(this->m_random);
    uint32_t bits = (raw & 0x7FFFFF) | 0x3F800000;
    float unit = *reinterpret_cast<float*>(&bits);
    float variation = static_cast<int32_t>(raw) >= 0 ? unit - 2.0f : 2.0f - unit;

    return ((variation * this->m_variation) + 1.0f) * this->m_speed;
}

// OFFSET: 0x979740
void CParticleEmitter2::ProjectParticle(CParticle2* particle) {
    float groundZ;

    if (!ParticleSystemManager::s_projectCallback(&particle->m_position, &groundZ, ParticleSystemManager::s_projectParam)) {
        return;
    }

    float lifespan = (static_cast<float>(particle->m_lifeSeed) * this->m_lifeVariation * 0.0000305185094f) + this->m_life;

    if (lifespan <= 0.001f) {
        lifespan = 0.001f;
    }

    float t = particle->m_age / lifespan;

    C2Vector scale;
    M2PartTrackSample(&scale, this->m_scaleTrack, t);

    particle->m_position.z = (scale.x <= scale.y ? scale.y : scale.x) + groundZ;
}

// OFFSET: 0x97E480
void CParticleEmitter2::SyncAllocation(uint32_t count) {
    uint32_t oldCount;

    if (!this->m_hasModel) {
        oldCount = this->m_particles.Count();

        if (oldCount >= count) {
            return;
        }

        this->SyncReserve(count, oldCount, this->m_particles.m_alloc - this->m_particles.Count());
        this->m_particles.SetCount(count);
    } else {
        oldCount = this->m_particleModels.Count();

        if (oldCount >= count) {
            return;
        }

        this->SyncReserve(count, oldCount, this->m_particleModels.m_alloc - this->m_particleModels.Count());
        this->m_particleModels.SetCount(count);
    }

    if (this->m_liveList.Count() + count > this->m_liveList.m_alloc) {
        this->m_liveList.ReallocData(this->m_liveList.Count() + count);
    }

    if (this->m_freeList.Count() + count > this->m_freeList.m_alloc) {
        this->m_freeList.ReallocData(this->m_freeList.Count() + count);
    }

    for (uint32_t i = oldCount; i != count; i++) {
        uint32_t slot = i;
        this->m_freeList.Add(&slot);
    }
}

// OFFSET: 0x97E3F0
void CParticleEmitter2::SyncReserve(uint32_t count, uint32_t used, uint32_t spare) {
    uint32_t target = count;

    if ((target & (target - 1)) != 0) {
        target = (target + target) - 1;

        while ((target & (target - 1)) != 0) {
            target = target & (target - 1);
        }
    }

    if (used + spare >= target) {
        return;
    }

    uint32_t shortfall = target - (used + spare);

    if (!this->m_hasModel) {
        if (this->m_particles.Count() + shortfall > this->m_particles.m_alloc) {
            this->m_particles.ReallocData(this->m_particles.Count() + shortfall);
        }
    } else {
        if (this->m_particleModels.Count() + shortfall > this->m_particleModels.m_alloc) {
            this->m_particleModels.ReallocData(this->m_particleModels.Count() + shortfall);
        }
    }

    if (this->m_liveList.Count() + shortfall > this->m_liveList.m_alloc) {
        this->m_liveList.ReallocData(this->m_liveList.Count() + shortfall);
    }

    if (this->m_freeList.Count() + shortfall > this->m_freeList.m_alloc) {
        this->m_freeList.ReallocData(this->m_freeList.Count() + shortfall);
    }
}

// OFFSET: 0x97EA60
void CParticleEmitter2::Render(C44Matrix* xform, void* vertexData, uint8_t enabled) {
    this->m_worldBounds.b.x = FLT_MAX;
    this->m_worldBounds.b.y = FLT_MAX;
    this->m_worldBounds.b.z = FLT_MAX;
    this->m_worldBounds.t.x = -FLT_MAX;
    this->m_worldBounds.t.y = -FLT_MAX;
    this->m_worldBounds.t.z = -FLT_MAX;

    if (!this->m_liveList.Count()) {
        this->m_batchFlags &= ~0x1u;
        this->m_renderedParticles = 0;
        return;
    }

    this->m_batchFlags ^= (enabled ^ this->m_batchFlags) & 0x1;

    if (!this->m_hasModel) {
        this->RenderParticles(xform, vertexData);
    }
}

// OFFSET: 0x97E730
void CParticleEmitter2::RenderParticles(C44Matrix* xform, void* vertexData) {
    CGxBuf* buf = nullptr;

    if (!g_particleRenderEnable) {
        this->m_batchFlags &= ~0x1u;
        this->m_renderedParticles = 0;
        return;
    }

    C44Matrix view;
    view = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];

    this->RenderParticlesPrep(xform, &view);

    if (!TextureGetGxTex(this->m_texture, 0, 0)) {
        this->m_batchFlags &= ~0x1u;
        this->m_renderedParticles = 0;
        return;
    }

    uint32_t maxParticles = 0x4000 / this->m_vertsPerParticle;

    if (maxParticles >= this->m_liveList.Count()) {
        maxParticles = this->m_liveList.Count();
    }

    void* dest = vertexData;
    EGxVertexBufferFormat format = (this->m_materialFlags & 0x1) != 0 ? GxVBF_PNCT : GxVBF_PCT;

    if (!vertexData) {
        uint32_t vertexCount = maxParticles * this->m_vertsPerParticle;
        buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, GxVertexSize(format), vertexCount);
        dest = g_theGxDevicePtr->BufLock(buf);
    }

    ParticleVertexWriter writer;
    writer.count = 0;

    this->FillOutParticleVertex(dest, format, &writer);

    C44Matrix inverseView = view.AffineInverse();

    this->BuildParticleVertices(&writer, maxParticles, &inverseView);

    if (!buf) {
        return;
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk1C = 1;

    if (this->m_renderedParticles) {
        this->RenderParticleVertices(buf, format, writer.count, this->m_renderedParticles * this->m_indicesPerParticle);
        g_theGxDevicePtr->XformSetView(view);
    }
}

// OFFSET: 0x97A390
void CParticleEmitter2::RenderParticlesPrep(C44Matrix* xform, C44Matrix* view) {
    C44Matrix translate;
    translate.d0 = -this->m_cameraPos.x;
    translate.d1 = -this->m_cameraPos.y;
    translate.d2 = -this->m_cameraPos.z;

    if ((this->m_flags & 0x200) != 0) {
        s_particleXform = (this->m_xform * translate) * *view;
    } else if (xform) {
        s_particleXform = (*xform * translate) * *view;
    } else {
        s_particleXform = translate * *view;
    }

    s_particleViewAxis.x = view->c0;
    s_particleViewAxis.y = view->c1;
    s_particleViewAxis.z = view->c2;

    if ((this->m_flags & 0x4000) == 0) {
        return;
    }

    s_particleBasis = C33Matrix(s_particleXform);

    if ((this->m_flags & 0x200) != 0 && fabs(this->m_xformScale) >= 2.384185791015625e-07f) {
        s_particleBasis.Scale(1.0f / this->m_xformScale);
    }

    this->m_billboardAxis.x = s_particleBasis.c0;
    this->m_billboardAxis.y = s_particleBasis.c1;
    this->m_billboardAxis.z = s_particleBasis.c2;

    float lengthSq = (this->m_billboardAxis.z * this->m_billboardAxis.z) + (this->m_billboardAxis.y * this->m_billboardAxis.y) + (this->m_billboardAxis.x * this->m_billboardAxis.x);

    if (lengthSq > 2.384185791015625e-07f) {
        float invLength = 1.0f / sqrt(lengthSq);

        this->m_billboardAxis.x = this->m_billboardAxis.x * invLength;
        this->m_billboardAxis.y = this->m_billboardAxis.y * invLength;
        this->m_billboardAxis.z = invLength * this->m_billboardAxis.z;
    }
}

// OFFSET: 0x97A2E0
void CParticleEmitter2::FillOutParticleVertex(void* base, EGxVertexBufferFormat format, ParticleVertexWriter* writer) {
    uint32_t stride = GxVertexSize(format);
    char* bytes = static_cast<char*>(base);

    writer->position = reinterpret_cast<float*>(bytes + GxVertexAttribOffset(format, GxVA_Position));
    writer->positionStride = stride;

    if ((this->m_materialFlags & 0x1) != 0) {
        writer->normal = reinterpret_cast<float*>(bytes + GxVertexAttribOffset(format, GxVA_Normal));
        writer->normalStride = stride;
    } else {
        writer->normal = &s_zeroNormal.x;
        writer->normalStride = 0;
    }

    writer->color = reinterpret_cast<uint32_t*>(bytes + GxVertexAttribOffset(format, GxVA_Color0));
    writer->colorStride = stride;

    writer->texCoord = reinterpret_cast<float*>(bytes + GxVertexAttribOffset(format, GxVA_TexCoord0));
    writer->texCoordStride = stride;
}

// OFFSET: 0x97E580
uint32_t CParticleEmitter2::BuildParticleVertices(ParticleVertexWriter* writer, uint32_t count, C44Matrix* inverseView) {
    if ((this->m_flags & 0x20) == 0) {
        for (uint32_t i = 0; i < count; i++) {
            uint32_t slot = this->m_liveList[i];
            CParticle2* particle = this->m_hasModel ? &this->m_particleModels[slot].base : &this->m_particles[slot];

            this->BuildVertex(particle, writer);
        }
    } else {
        for (uint32_t i = 0; i < this->m_liveList.Count(); i++) {
            uint32_t slot = this->m_liveList[i];
            CParticle2* particle = this->m_hasModel ? &this->m_particleModels[slot].base : &this->m_particles[slot];

            float depth = (s_particleXform.c2 * particle->m_position.z) + (s_particleXform.b2 * particle->m_position.y) + (s_particleXform.a2 * particle->m_position.x) + s_particleXform.d2;

            CParticleSortEntry entry;
            entry.m_key = depth;
            entry.m_particle = particle;
            g_particleSortBuffer.Insert(entry);
        }

        for (uint32_t i = count; i > 0; i--) {
            CParticleSortEntry particle;
            g_particleSortBuffer.Pop(&particle);

            this->BuildVertex(particle.m_particle, writer);
        }
    }

    if (count) {
        this->m_worldBounds = inverseView->Transform(this->m_worldBounds);

        this->m_worldBounds.b.x = this->m_worldBounds.b.x + this->m_cameraPos.x;
        this->m_worldBounds.b.y = this->m_worldBounds.b.y + this->m_cameraPos.y;
        this->m_worldBounds.b.z = this->m_cameraPos.z + this->m_worldBounds.b.z;
        this->m_worldBounds.t.x = this->m_worldBounds.t.x + this->m_cameraPos.x;
        this->m_worldBounds.t.y = this->m_worldBounds.t.y + this->m_cameraPos.y;
        this->m_worldBounds.t.z = this->m_cameraPos.z + this->m_worldBounds.t.z;
    }

    this->m_renderedParticles = writer->count / this->m_vertsPerParticle;

    return this->m_renderedParticles;
}

// OFFSET: 0x97A260
void CParticleEmitter2::RenderIndices(CGxBuf* buf) {
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));
    uint16_t* end = indices + 131064;
    uint32_t vertex = 0;

    while (indices < end) {
        indices[0] = vertex;
        indices[1] = vertex + 1;
        indices[2] = vertex + 2;
        indices[3] = vertex + 3;
        indices[4] = vertex + 2;
        indices[5] = vertex + 1;

        indices += 6;
        vertex += 4;
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk1C = 1;
}

// OFFSET: 0x97A580
void CParticleEmitter2::RenderParticleVertices(CGxBuf* vertexBuf, EGxVertexBufferFormat format, uint32_t vertexCount, uint32_t indexCount) {
    if (!g_particleRenderEnable) {
        return;
    }

    if (!CParticleEmitter2::s_indexBuf->unk1C || !CParticleEmitter2::s_indexBuf->unk1D) {
        CParticleEmitter2::RenderIndices(CParticleEmitter2::s_indexBuf);
    }

    C44Matrix identity;
    g_theGxDevicePtr->XformSetView(identity);

    GxPrimVertexPtr(vertexBuf, format);
    g_theGxDevicePtr->PrimIndexPtr(CParticleEmitter2::s_indexBuf);

    CShaderEffect::SetTexMtx_Identity(0);
    CShaderEffect::SetDefaultShaders(0);
    CShaderEffect::UpdateWorldViewMatrix();

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = vertexCount - 1;

    g_theGxDevicePtr->Draw(&batch, 1);
}

// OFFSET: 0x6F7A60
void SinCos(float angle, float* outSin, float* outCos) {
    *outSin = sin(angle);
    *outCos = cos(angle);
}

// OFFSET: 0x97A130
void CParticleEmitter2::RandomizeSpin(CParticle2* particle, float* outSpin, float* outSpinRate) {
    if (this->m_initialSpinVariation == 0.0f && this->m_spinVariation == 0.0f) {
        *outSpin = this->m_initialSpin;
        *outSpinRate = this->m_spin;
        return;
    }

    CRndSeed seed(particle->m_randomSeed);

    if (this->m_initialSpinVariation == 0.0f) {
        *outSpin = this->m_initialSpin;
    } else {
        uint32_t raw = CRandom::uint32(seed);
        uint32_t bits = (raw & 0x7FFFFF) | 0x3F800000;
        float unit = *reinterpret_cast<float*>(&bits);
        float variation = static_cast<int32_t>(raw) >= 0 ? unit - 2.0f : 2.0f - unit;

        *outSpin = (variation * this->m_initialSpinVariation) + this->m_initialSpin;
    }

    if (this->m_spinVariation == 0.0f) {
        *outSpinRate = this->m_spin;
    } else {
        uint32_t raw = CRandom::uint32(seed);
        uint32_t bits = (raw & 0x7FFFFF) | 0x3F800000;
        float unit = *reinterpret_cast<float*>(&bits);
        float variation = static_cast<int32_t>(raw) >= 0 ? unit - 2.0f : 2.0f - unit;

        *outSpinRate = (variation * this->m_spinVariation) + this->m_spin;
    }
}

// OFFSET: 0x979D60
void CParticleEmitter2::InterpolateAllTracksSimple(const float* age, CImVector* color, C2Vector* scale, uint32_t* headCell, uint32_t* tailCell) {
    float lifespan = 0.001f;

    if (this->m_life > 0.001f) {
        lifespan = this->m_life;
    }

    float t = *age / lifespan;
    CParticleSimpleKeys* keys = this->m_simpleKeys;
    float ratio;

    if (t >= this->m_simpleMidTime) {
        keys++;
        ratio = (t - this->m_simpleMidTime) / (1.0f - this->m_simpleMidTime);
    } else {
        ratio = t / this->m_simpleMidTime;
    }

    color->a = static_cast<uint8_t>(((ratio * keys->m_alphaDelta) + keys->m_color.a) * this->m_alphaScale);
    color->r = static_cast<uint8_t>((static_cast<float>(keys->m_redDelta) * ratio) + keys->m_color.r);
    color->g = static_cast<uint8_t>((static_cast<float>(keys->m_greenDelta) * ratio) + keys->m_color.g);
    color->b = static_cast<uint8_t>((static_cast<float>(keys->m_blueDelta) * ratio) + keys->m_color.b);

    float value = (keys->m_scaleDelta * ratio) + keys->m_scaleBase;
    scale->x = value;
    scale->y = value;

    *headCell = static_cast<uint32_t>((ratio * keys->m_headCellDelta) + keys->m_headCellBase);
    *tailCell = static_cast<uint32_t>((static_cast<float>(keys->m_tailCellDelta) * ratio) + keys->m_tailCellBase);
}

// OFFSET: 0x979E90
void CParticleEmitter2::InterpolateAllTracks(CParticle2* particle, CImVector* color, C2Vector* scale, uint32_t* headCell, uint32_t* tailCell) {
    float lifespan = (static_cast<float>(particle->m_lifeSeed) * this->m_lifeVariation * 0.0000305185094f) + this->m_life;

    if (lifespan <= 0.001f) {
        lifespan = 0.001f;
    }

    float t = particle->m_age / lifespan;

    CRndSeed seed(particle->m_randomSeed);

    *color = this->InterpolateColorTrack(t);
    color->a = static_cast<uint8_t>(M2PartTrackSampleFixed16(this->m_alphaTrack, t) * this->m_alphaScale * 255.0f);

    M2PartTrackSample(scale, this->m_scaleTrack, t);

    *headCell = 0;
    *tailCell = 0;

    if (this->m_headCellTrack->times.Count()) {
        *headCell = M2PartTrackSampleUint16(this->m_headCellTrack, t);
    } else if ((this->m_flags & 0x100000) != 0) {
        uint32_t r = CRandom::uint32(seed);
        *headCell = static_cast<uint32_t>((static_cast<uint64_t>(this->m_textureCols * this->m_textureRows) * r) >> 32);
    }

    if (this->m_tailCellTrack->times.Count()) {
        *tailCell = M2PartTrackSampleUint16(this->m_tailCellTrack, t);
    }

    if ((this->m_flags & 0x800000) != 0) {
        uint32_t rawY = CRandom::uint32(seed);
        uint32_t bitsY = (rawY & 0x7FFFFF) | 0x3F800000;
        float unitY = *reinterpret_cast<float*>(&bitsY);
        float variationY = static_cast<int32_t>(rawY) >= 0 ? unitY - 2.0f : 2.0f - unitY;

        uint32_t rawX = CRandom::uint32(seed);
        uint32_t bitsX = (rawX & 0x7FFFFF) | 0x3F800000;
        float unitX = *reinterpret_cast<float*>(&bitsX);
        float variationX = static_cast<int32_t>(rawX) >= 0 ? unitX - 2.0f : 2.0f - unitX;

        float factorX = (variationX * this->m_scaleVariationX) + 1.0f;
        float factorY = (this->m_scaleVariationY * variationY) + 1.0f;

        if (factorY <= 0.000099999997f) {
            factorY = 0.000099999997f;
        }

        if (factorX <= 0.000099999997f) {
            scale->x = 0.000099999997f * scale->x;
        } else {
            scale->x = factorX * scale->x;
        }

        scale->y = factorY * scale->y;
    } else {
        uint32_t raw = CRandom::uint32(seed);
        uint32_t bits = (raw & 0x7FFFFF) | 0x3F800000;
        float unit = *reinterpret_cast<float*>(&bits);
        float variation = static_cast<int32_t>(raw) >= 0 ? unit - 2.0f : 2.0f - unit;

        float factor = (variation * this->m_scaleVariationX) + 1.0f;

        if (factor < 0.000099999997f) {
            factor = 0.000099999997f;
        }

        scale->x = scale->x * factor;
        scale->y = factor * scale->y;
    }
}

// OFFSET: 0x9795D0
CImVector CParticleEmitter2::InterpolateColorTrack(float t) {
    CImVector result;
    float red;
    float green;
    float blue;

    if (this->m_colorTrack->values.Count() == 1) {
        red = this->m_colorTrack->values[0].x;
        green = this->m_colorTrack->values[0].y;
        blue = this->m_colorTrack->values[0].z;
    } else {
        uint32_t low;
        uint32_t high;
        float ratio = M2PartTrackFindKeyPair(this->m_colorTrack, t, &low, &high);

        if ((this->m_flags & 0x10) != 0) {
            red = ((this->m_replacementColors[high].x - this->m_replacementColors[low].x) * ratio) + this->m_replacementColors[low].x;
            green = ((this->m_replacementColors[high].y - this->m_replacementColors[low].y) * ratio) + this->m_replacementColors[low].y;
            blue = (ratio * (this->m_replacementColors[high].z - this->m_replacementColors[low].z)) + this->m_replacementColors[low].z;
        } else {
            red = ((this->m_colorTrack->values[high].x - this->m_colorTrack->values[low].x) * ratio) + this->m_colorTrack->values[low].x;
            green = ((this->m_colorTrack->values[high].y - this->m_colorTrack->values[low].y) * ratio) + this->m_colorTrack->values[low].y;
            blue = (ratio * (this->m_colorTrack->values[high].z - this->m_colorTrack->values[low].z)) + this->m_colorTrack->values[low].z;
        }
    }

    result.b = static_cast<uint8_t>(blue);
    result.a = 255;
    result.r = static_cast<uint8_t>(red);
    result.g = static_cast<uint8_t>(green);

    return result;
}

// OFFSET: 0x97BE80
int32_t CParticleEmitter2::BuildVertex(CParticle2* particle, ParticleVertexWriter* writer) {
    uint32_t twinkleIndex = 0;

    if (this->m_twinkleOnOff < 1.0f || this->m_twinkleScaleSpan != 0.0f) {
        float phase = this->m_twinkleFPS * particle->m_age;
        twinkleIndex = ((reinterpret_cast<uint32_t>(particle) >> 5) + static_cast<int32_t>(phase)) & 0x7F;
    }

    if (this->m_twinkleOnOff < 1.0f && this->m_twinkleOnOff < s_twinkleTable[twinkleIndex]) {
        return 0;
    }

    CImVector color;
    C2Vector size;
    uint32_t headCell;
    uint32_t tailCell;

    color.value = 0;
    size.x = 0.0f;
    size.y = 0.0f;

    if ((this->m_flags & 0x1000000) != 0) {
        this->InterpolateAllTracksSimple(&particle->m_age, &color, &size, &headCell, &tailCell);
    } else {
        this->InterpolateAllTracks(particle, &color, &size, &headCell, &tailCell);
    }

    float spin;
    float spinRate;
    this->RandomizeSpin(particle, &spin, &spinRate);

    if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
        CImVector swapped;
        swapped.value = 0;
        swapped.a = color.a;
        swapped.r = color.b;
        swapped.g = color.g;
        swapped.b = color.r;
        color = swapped;
    }

    float twinkle = (s_twinkleTable[twinkleIndex] * this->m_twinkleScaleSpan) + this->m_twinkleScaleBase;
    size.x = size.x * twinkle;
    size.y = twinkle * size.y;

    if ((this->m_flags & 0x400) != 0) {
        size.x = size.x * this->m_xformScale;
        size.y = this->m_xformScale * size.y;
    }

    C3Vector center = s_particleXform.TransformPoint(particle->m_position);

    if ((this->m_flags & 0x4) != 0) {
        uint32_t headColumn = headCell & (this->m_textureCols - 1);
        int32_t headRow = static_cast<int32_t>(headCell) >> this->m_textureCellShift;

        float uBase = headColumn * this->m_textureCellWidth;
        float vBase = headRow * this->m_textureCellHeight;

        float velocityLengthSq = (particle->m_velocity.z * particle->m_velocity.z) + (particle->m_velocity.y * particle->m_velocity.y) + (particle->m_velocity.x * particle->m_velocity.x);

        if ((this->m_flags & 0x200000) != 0 && velocityLengthSq > 0.00000023841858f) {
            C3Vector reverse;
            reverse.x = -particle->m_velocity.x;
            reverse.y = -particle->m_velocity.y;
            reverse.z = -particle->m_velocity.z;

            C3Vector viewDir = s_particleXform.TransformDirection(reverse);
            C2Vector flat(viewDir.x, viewDir.y);

            float flatLengthSq = (flat.y * flat.y) + (flat.x * flat.x);
            float viewLengthSq = (viewDir.y * viewDir.y) + (viewDir.z * viewDir.z) + (viewDir.x * viewDir.x);
            float invLength = 0.0f;

            if (flatLengthSq > 0.00000023841858f) {
                invLength = 1.0f / sqrt(flatLengthSq);
            }

            float cosine = flat.x * invLength;
            float sine = flat.y * invLength;

            C2Vector stretch = size;

            if (0.00000023841858f < invLength) {
                stretch.x = ((1.0f / sqrt(viewLengthSq)) / invLength) * size.x;
            }

            for (uint32_t i = 0; i < 4; i++) {
                float x = s_cornerOffset[i].x * stretch.x;
                float y = s_cornerOffset[i].y * stretch.y;

                float px = (y * -sine) + (x * cosine) + center.x;
                float py = (x * sine) + (y * cosine) + center.y;
                float pz = center.z;

                writer->position[0] = px;
                writer->position[1] = py;
                writer->position[2] = pz;

                if (this->m_worldBounds.b.x > px) {
                    this->m_worldBounds.b.x = px;
                }
                if (this->m_worldBounds.b.y > py) {
                    this->m_worldBounds.b.y = py;
                }
                if (pz < this->m_worldBounds.b.z) {
                    this->m_worldBounds.b.z = pz;
                }
                if (this->m_worldBounds.t.x < px) {
                    this->m_worldBounds.t.x = px;
                }
                if (py > this->m_worldBounds.t.y) {
                    this->m_worldBounds.t.y = py;
                }
                if (pz > this->m_worldBounds.t.z) {
                    this->m_worldBounds.t.z = pz;
                }

                writer->normal[0] = s_particleViewAxis.x;
                writer->normal[1] = s_particleViewAxis.y;
                writer->normal[2] = s_particleViewAxis.z;

                *writer->color = color.value;

                writer->texCoord[0] = (s_cornerUV[i].x * this->m_textureCellWidth) + uBase;
                writer->texCoord[1] = (s_cornerUV[i].y * this->m_textureCellHeight) + vBase;

                writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
                writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
                writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
                writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);
                writer->count++;
            }
        } else if (this->m_spin == 0.0f && this->m_spinVariation == 0.0f) {
            for (uint32_t i = 0; i < 4; i++) {
                float x = s_cornerOffset[i].x * size.x;
                float y = s_cornerOffset[i].y * size.y;

                float px;
                float py;
                float pz;

                if ((this->m_flags & 0x4000) != 0) {
                    px = (s_particleBasis.b0 * y) + (x * s_particleBasis.a0) + center.x;
                    py = (y * s_particleBasis.b1) + (x * s_particleBasis.a1) + center.y;
                    pz = (y * s_particleBasis.b2) + (x * s_particleBasis.a2) + center.z;
                } else {
                    px = x + center.x;
                    py = y + center.y;
                    pz = center.z;
                }

                writer->position[0] = px;
                writer->position[1] = py;
                writer->position[2] = pz;

                if (this->m_worldBounds.b.x > px) {
                    this->m_worldBounds.b.x = px;
                }
                if (this->m_worldBounds.b.y > py) {
                    this->m_worldBounds.b.y = py;
                }
                if (pz < this->m_worldBounds.b.z) {
                    this->m_worldBounds.b.z = pz;
                }
                if (this->m_worldBounds.t.x < px) {
                    this->m_worldBounds.t.x = px;
                }
                if (py > this->m_worldBounds.t.y) {
                    this->m_worldBounds.t.y = py;
                }
                if (pz > this->m_worldBounds.t.z) {
                    this->m_worldBounds.t.z = pz;
                }

                writer->normal[0] = s_particleViewAxis.x;
                writer->normal[1] = s_particleViewAxis.y;
                writer->normal[2] = s_particleViewAxis.z;

                *writer->color = color.value;

                writer->texCoord[0] = (s_cornerUV[i].x * this->m_textureCellWidth) + uBase;
                writer->texCoord[1] = (s_cornerUV[i].y * this->m_textureCellHeight) + vBase;

                writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
                writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
                writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
                writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);
                writer->count++;
            }
        } else {
            float angle = (particle->m_age * spinRate) + spin;

            if ((this->m_flags & 0x10000) != 0 && (reinterpret_cast<uint32_t>(particle) & 0x20) != 0) {
                angle = -angle;
            }

            if ((this->m_flags & 0x4000) != 0) {
                for (uint32_t i = 0; i < 4; i++) {
                    C33Matrix rotation = C33Matrix::Rotation(angle, this->m_billboardAxis, true);

                    float x = s_cornerOffset[i].x * size.x;
                    float y = s_cornerOffset[i].y * size.y;

                    float bx = (s_particleBasis.b0 * y) + (x * s_particleBasis.a0);
                    float by = (y * s_particleBasis.b1) + (x * s_particleBasis.a1);
                    float bz = (y * s_particleBasis.b2) + (x * s_particleBasis.a2);

                    float px = (rotation.a2 * bz) + (rotation.a1 * by) + (bx * rotation.a0) + center.x;
                    float py = (rotation.b2 * bz) + (rotation.b1 * by) + (bx * rotation.b0) + center.y;
                    float pz = (bx * rotation.c0) + (by * rotation.c1) + (bz * rotation.c2) + center.z;

                    writer->position[0] = px;
                    writer->position[1] = py;
                    writer->position[2] = pz;

                    if (this->m_worldBounds.b.x > px) {
                        this->m_worldBounds.b.x = px;
                    }
                    if (this->m_worldBounds.b.y > py) {
                        this->m_worldBounds.b.y = py;
                    }
                    if (pz < this->m_worldBounds.b.z) {
                        this->m_worldBounds.b.z = pz;
                    }
                    if (this->m_worldBounds.t.x < px) {
                        this->m_worldBounds.t.x = px;
                    }
                    if (py > this->m_worldBounds.t.y) {
                        this->m_worldBounds.t.y = py;
                    }
                    if (pz > this->m_worldBounds.t.z) {
                        this->m_worldBounds.t.z = pz;
                    }

                    writer->normal[0] = s_particleViewAxis.x;
                    writer->normal[1] = s_particleViewAxis.y;
                    writer->normal[2] = s_particleViewAxis.z;

                    *writer->color = color.value;

                    writer->texCoord[0] = (s_cornerUV[i].x * this->m_textureCellWidth) + uBase;
                    writer->texCoord[1] = (s_cornerUV[i].y * this->m_textureCellHeight) + vBase;

                    writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
                    writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
                    writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
                    writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);
                    writer->count++;
                }
            } else {
                float sine;
                float cosine;
                SinCos(angle, &sine, &cosine);

                float cx = cosine * size.x;
                float cy = cosine * size.y;
                float sx = size.x * sine;
                float sy = size.y * sine;

                const float px[4] = { center.x - cx - sy, center.x - cx + sy, center.x + cx - sy, center.x + cx + sy };
                const float py[4] = { center.y - sx + cy, center.y - sx - cy, center.y + sx + cy, center.y + sx - cy };

                for (uint32_t i = 0; i < 4; i++) {
                    writer->position[0] = px[i];
                    writer->position[1] = py[i];
                    writer->position[2] = center.z;

                    if (this->m_worldBounds.b.x > px[i]) {
                        this->m_worldBounds.b.x = px[i];
                    }
                    if (this->m_worldBounds.b.y > py[i]) {
                        this->m_worldBounds.b.y = py[i];
                    }
                    if (center.z < this->m_worldBounds.b.z) {
                        this->m_worldBounds.b.z = center.z;
                    }
                    if (this->m_worldBounds.t.x < px[i]) {
                        this->m_worldBounds.t.x = px[i];
                    }
                    if (py[i] > this->m_worldBounds.t.y) {
                        this->m_worldBounds.t.y = py[i];
                    }
                    if (center.z > this->m_worldBounds.t.z) {
                        this->m_worldBounds.t.z = center.z;
                    }

                    writer->normal[0] = s_particleViewAxis.x;
                    writer->normal[1] = s_particleViewAxis.y;
                    writer->normal[2] = s_particleViewAxis.z;

                    *writer->color = color.value;

                    writer->texCoord[0] = (s_cornerUV[i].x * this->m_textureCellWidth) + uBase;
                    writer->texCoord[1] = (s_cornerUV[i].y * this->m_textureCellHeight) + vBase;

                    writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
                    writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
                    writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
                    writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);
                    writer->count++;
                }
            }
        }
    }

    if ((this->m_flags & 0x8) == 0) {
        return 1;
    }

    uint32_t tailColumn = tailCell & (this->m_textureCols - 1);
    int32_t tailRow = static_cast<int32_t>(tailCell) >> this->m_textureCellShift;

    float tailU = tailColumn * this->m_textureCellWidth;
    float tailV = tailRow * this->m_textureCellHeight;

    float tailLength = this->m_tailLength;

    if ((this->m_flags & 0x20000) != 0 && tailLength > particle->m_age) {
        tailLength = particle->m_age;
    }

    C3Vector reverse;
    reverse.x = -particle->m_velocity.x;
    reverse.y = -particle->m_velocity.y;
    reverse.z = -particle->m_velocity.z;

    C3Vector tail;
    tail.x = ((reverse.y * s_particleXform.b0) + (reverse.z * s_particleXform.c0) + (reverse.x * s_particleXform.a0)) * tailLength;
    tail.y = ((reverse.y * s_particleXform.b1) + (reverse.z * s_particleXform.c1) + (reverse.x * s_particleXform.a1)) * tailLength;
    tail.z = tailLength * ((reverse.y * s_particleXform.b2) + (reverse.z * s_particleXform.c2) + (reverse.x * s_particleXform.a2));

    C2Vector flatTail(tail.x, tail.y);
    float flatTailLengthSq = (flatTail.y * flatTail.y) + (flatTail.x * flatTail.x);

    if (flatTailLengthSq < 0.00077160494f) {
        for (uint32_t i = 0; i < 4; i++) {
            float px = (s_cornerOffset[i].x * size.x) + center.x;
            float py = (s_cornerOffset[i].y * size.y) + center.y;
            float pz = center.z;

            writer->position[0] = px;
            writer->position[1] = py;
            writer->position[2] = pz;

            if (this->m_worldBounds.b.x > px) {
                this->m_worldBounds.b.x = px;
            }
            if (this->m_worldBounds.b.y > py) {
                this->m_worldBounds.b.y = py;
            }
            if (pz < this->m_worldBounds.b.z) {
                this->m_worldBounds.b.z = pz;
            }
            if (this->m_worldBounds.t.x < px) {
                this->m_worldBounds.t.x = px;
            }
            if (py > this->m_worldBounds.t.y) {
                this->m_worldBounds.t.y = py;
            }
            if (pz > this->m_worldBounds.t.z) {
                this->m_worldBounds.t.z = pz;
            }

            writer->normal[0] = s_particleViewAxis.x;
            writer->normal[1] = s_particleViewAxis.y;
            writer->normal[2] = s_particleViewAxis.z;

            *writer->color = color.value;

            writer->texCoord[0] = (s_cornerUV[i].x * this->m_textureCellWidth) + tailU;
            writer->texCoord[1] = (s_cornerUV[i].y * this->m_textureCellHeight) + tailV;

            writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
            writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
            writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
            writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);
            writer->count++;
        }

        return 1;
    }

    C3Vector end;
    end.x = center.x + tail.x;
    end.y = center.y + tail.y;
    end.z = center.z + tail.z;

    float tailInvLength = 1.0f / sqrt(flatTailLengthSq);
    float offsetX = tail.x * (size.x * tailInvLength);
    float offsetY = tail.y * (tailInvLength * size.y);

    float headLeftX = center.x - offsetY;
    float headLeftY = center.y + offsetX;

    writer->position[0] = headLeftX;
    writer->position[1] = headLeftY;
    writer->position[2] = center.z;

    if (this->m_worldBounds.b.x > headLeftX) {
        this->m_worldBounds.b.x = headLeftX;
    }
    if (this->m_worldBounds.b.y > headLeftY) {
        this->m_worldBounds.b.y = headLeftY;
    }
    if (center.z < this->m_worldBounds.b.z) {
        this->m_worldBounds.b.z = center.z;
    }
    if (this->m_worldBounds.t.x < headLeftX) {
        this->m_worldBounds.t.x = headLeftX;
    }
    if (headLeftY > this->m_worldBounds.t.y) {
        this->m_worldBounds.t.y = headLeftY;
    }
    if (center.z > this->m_worldBounds.t.z) {
        this->m_worldBounds.t.z = center.z;
    }

    writer->normal[0] = s_particleViewAxis.x;
    writer->normal[1] = s_particleViewAxis.y;
    writer->normal[2] = s_particleViewAxis.z;

    *writer->color = color.value;

    writer->texCoord[0] = (this->m_textureCellWidth * s_cornerUV[0].x) + tailU;
    writer->texCoord[1] = (this->m_textureCellHeight * s_cornerUV[0].y) + tailV;

    writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
    writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
    writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
    writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);
    writer->count++;

    float headRightX = center.x + offsetY;
    float headRightY = center.y - offsetX;

    writer->position[0] = headRightX;
    writer->position[1] = headRightY;
    writer->position[2] = center.z;

    if (this->m_worldBounds.b.x > headRightX) {
        this->m_worldBounds.b.x = headRightX;
    }
    if (this->m_worldBounds.b.y > headRightY) {
        this->m_worldBounds.b.y = headRightY;
    }
    if (center.z < this->m_worldBounds.b.z) {
        this->m_worldBounds.b.z = center.z;
    }
    if (this->m_worldBounds.t.x < headRightX) {
        this->m_worldBounds.t.x = headRightX;
    }
    if (headRightY > this->m_worldBounds.t.y) {
        this->m_worldBounds.t.y = headRightY;
    }
    if (center.z > this->m_worldBounds.t.z) {
        this->m_worldBounds.t.z = center.z;
    }

    writer->normal[0] = s_particleViewAxis.x;
    writer->normal[1] = s_particleViewAxis.y;
    writer->normal[2] = s_particleViewAxis.z;

    *writer->color = color.value;

    writer->texCoord[0] = (this->m_textureCellWidth * s_cornerUV[1].x) + tailU;
    writer->texCoord[1] = (this->m_textureCellHeight * s_cornerUV[1].y) + tailV;

    writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
    writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
    writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
    writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);
    writer->count++;

    float tailLeftX = end.x - offsetY;
    float tailLeftY = offsetX + end.y;

    writer->position[0] = tailLeftX;
    writer->position[1] = tailLeftY;
    writer->position[2] = end.z;

    if (this->m_worldBounds.b.x > tailLeftX) {
        this->m_worldBounds.b.x = tailLeftX;
    }
    if (this->m_worldBounds.b.y > tailLeftY) {
        this->m_worldBounds.b.y = tailLeftY;
    }
    if (end.z < this->m_worldBounds.b.z) {
        this->m_worldBounds.b.z = end.z;
    }
    if (this->m_worldBounds.t.x < tailLeftX) {
        this->m_worldBounds.t.x = tailLeftX;
    }
    if (this->m_worldBounds.t.y < tailLeftY) {
        this->m_worldBounds.t.y = tailLeftY;
    }
    if (end.z > this->m_worldBounds.t.z) {
        this->m_worldBounds.t.z = end.z;
    }

    writer->normal[0] = s_particleViewAxis.x;
    writer->normal[1] = s_particleViewAxis.y;
    writer->normal[2] = s_particleViewAxis.z;

    *writer->color = color.value;

    writer->texCoord[0] = (this->m_textureCellWidth * s_cornerUV[2].x) + tailU;
    writer->texCoord[1] = (this->m_textureCellHeight * s_cornerUV[2].y) + tailV;

    writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
    writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
    writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
    writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);
    writer->count++;

    float tailRightX = end.x + offsetY;
    float tailRightY = end.y - offsetX;

    writer->position[0] = tailRightX;
    writer->position[1] = tailRightY;
    writer->position[2] = end.z;

    if (this->m_worldBounds.b.x > tailRightX) {
        this->m_worldBounds.b.x = tailRightX;
    }
    if (this->m_worldBounds.b.y > tailRightY) {
        this->m_worldBounds.b.y = tailRightY;
    }
    if (end.z < this->m_worldBounds.b.z) {
        this->m_worldBounds.b.z = end.z;
    }
    if (this->m_worldBounds.t.x < tailRightX) {
        this->m_worldBounds.t.x = tailRightX;
    }
    if (tailRightY > this->m_worldBounds.t.y) {
        this->m_worldBounds.t.y = tailRightY;
    }
    if (this->m_worldBounds.t.z < end.z) {
        this->m_worldBounds.t.z = end.z;
    }

    writer->normal[0] = s_particleViewAxis.x;
    writer->normal[1] = s_particleViewAxis.y;
    writer->normal[2] = s_particleViewAxis.z;

    *writer->color = color.value;

    writer->texCoord[0] = (s_cornerUV[3].x * this->m_textureCellWidth) + tailU;
    writer->texCoord[1] = (this->m_textureCellHeight * s_cornerUV[3].y) + tailV;

    writer->count++;
    writer->position = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->position) + writer->positionStride);
    writer->normal = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->normal) + writer->normalStride);
    writer->color = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(writer->color) + writer->colorStride);
    writer->texCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(writer->texCoord) + writer->texCoordStride);

    return 1;
}

// OFFSET: 0x981310
CPlaneParticleEmitter::CPlaneParticleEmitter() {
    this->m_width = 0.0f;
    this->m_height = 0.0f;
    this->m_emitterType = 1;
    this->m_latitude = 0.0f;
    this->m_longitude = 0.0f;
}

// OFFSET: 0x9813B0
void CPlaneParticleEmitter::SetWidth(float width) {
    this->m_width = width;
}

// OFFSET: 0x9813C0
void CPlaneParticleEmitter::SetHeight(float height) {
    this->m_height = height;
}

// OFFSET: 0x9813D0
void CPlaneParticleEmitter::SetLatitude(float latitude) {
    this->m_latitude = latitude;
}

// OFFSET: 0x9813E0
void CPlaneParticleEmitter::SetLongitude(float longitude) {
    this->m_longitude = longitude;
}

// OFFSET: 0x9815C0
void CPlaneParticleEmitter::CreateParticle(CParticle2* particle, float dt, C44Matrix* xform) {
    uint32_t ageBits = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
    particle->m_age = (*reinterpret_cast<float*>(&ageBits) - 1.0f) * dt;
    particle->m_randomSeed = static_cast<uint16_t>(CRandom::uint32(this->m_random));

    uint32_t heightRaw = CRandom::uint32(this->m_random);
    uint32_t heightBits = (heightRaw & 0x7FFFFF) | 0x3F800000;
    float heightUnit = *reinterpret_cast<float*>(&heightBits);
    float heightVariation = static_cast<int32_t>(heightRaw) >= 0 ? heightUnit - 2.0f : 2.0f - heightUnit;

    uint32_t widthRaw = CRandom::uint32(this->m_random);
    uint32_t widthBits = (widthRaw & 0x7FFFFF) | 0x3F800000;
    float widthUnit = *reinterpret_cast<float*>(&widthBits);
    float widthVariation = static_cast<int32_t>(widthRaw) >= 0 ? widthUnit - 2.0f : 2.0f - widthUnit;

    particle->m_position.x = widthVariation * this->m_width * 0.5f;
    particle->m_position.y = 0.5f * (this->m_height * heightVariation);
    particle->m_position.z = 0.0f;

    float speed = this->CalcVelocity();
    C3Vector direction;

    if (this->m_zsource == 0.0f) {
        uint32_t latitudeRaw = CRandom::uint32(this->m_random);
        uint32_t latitudeBits = (latitudeRaw & 0x7FFFFF) | 0x3F800000;
        float latitudeUnit = *reinterpret_cast<float*>(&latitudeBits);
        float latitudeVariation = static_cast<int32_t>(latitudeRaw) >= 0 ? latitudeUnit - 2.0f : 2.0f - latitudeUnit;
        float latitude = latitudeVariation * this->m_latitude;

        uint32_t longitudeRaw = CRandom::uint32(this->m_random);
        uint32_t longitudeBits = (longitudeRaw & 0x7FFFFF) | 0x3F800000;
        float longitudeUnit = *reinterpret_cast<float*>(&longitudeBits);
        float longitudeVariation = static_cast<int32_t>(longitudeRaw) >= 0 ? longitudeUnit - 2.0f : 2.0f - longitudeUnit;
        float longitude = longitudeVariation * this->m_longitude;

        float cosLatitude = cos(latitude);
        float sinLatitude = sin(latitude);
        float cosLongitude = cos(longitude);
        float sinLongitude = sin(longitude);

        direction.x = cosLongitude * sinLatitude * speed;
        direction.y = sinLatitude * sinLongitude * speed;
        direction.z = speed * cosLatitude;
    } else {
        float dz = particle->m_position.z - this->m_zsource;
        float scale = speed / sqrt((particle->m_position.y * particle->m_position.y) + (particle->m_position.x * particle->m_position.x) + (dz * dz));

        direction.x = particle->m_position.x * scale;
        direction.y = particle->m_position.y * scale;
        direction.z = scale * dz;
    }

    if ((this->m_flags & 0x200) != 0) {
        particle->m_velocity = direction;
    } else {
        particle->m_velocity.x = (xform->b0 * direction.y) + (xform->c0 * direction.z) + (xform->a0 * direction.x);
        particle->m_velocity.y = (xform->b1 * direction.y) + (xform->a1 * direction.x) + (xform->c1 * direction.z);
        particle->m_velocity.z = (direction.y * xform->b2) + (direction.x * xform->a2) + (direction.z * xform->c2);

        particle->m_position = particle->m_position * *xform;

        if ((this->m_flags & 0x40000) != 0) {
            this->ProjectParticle(particle);
        }
    }

    if ((this->m_flags & 0x800) != 0) {
        uint32_t raw = CRandom::uint32(this->m_random);
        uint32_t bits = (raw & 0x7FFFFF) | 0x3F800000;
        float unit = *reinterpret_cast<float*>(&bits);
        float variation = static_cast<int32_t>(raw) >= 0 ? unit - 2.0f : 2.0f - unit;
        float factor = (variation * this->m_variation) + 1.0f;

        particle->m_velocity.x = (this->m_parentVelocity.x * factor) + particle->m_velocity.x;
        particle->m_velocity.y = (this->m_parentVelocity.y * factor) + particle->m_velocity.y;
        particle->m_velocity.z = (factor * this->m_parentVelocity.z) + particle->m_velocity.z;
    }
}

// OFFSET: 0x9813F0
CSphereParticleEmitter::CSphereParticleEmitter() {
    this->m_minRadius = 0.0f;
    this->m_maxRadius = 0.0f;
    this->m_emitterType = 2;
    this->m_latitude = 0.0f;
    this->m_longitude = 0.0f;
}

// OFFSET: 0x981490
void CSphereParticleEmitter::SetWidth(float width) {
    this->m_minRadius = width;
    this->m_radiusSpan = this->m_maxRadius - width;
}

// OFFSET: 0x9814B0
void CSphereParticleEmitter::SetHeight(float height) {
    this->m_maxRadius = height;
    this->m_radiusSpan = height - this->m_minRadius;
}

// OFFSET: 0x9813E0
void CSphereParticleEmitter::SetLatitude(float latitude) {
    this->m_latitude = latitude;
}

// OFFSET: 0x9814D0
void CSphereParticleEmitter::SetLongitude(float longitude) {
    this->m_longitude = longitude;
}

// OFFSET: 0x981950
void CSphereParticleEmitter::CreateParticle(CParticle2* particle, float dt, C44Matrix* xform) {
    uint32_t ageBits = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
    particle->m_age = (*reinterpret_cast<float*>(&ageBits) - 1.0f) * dt;
    particle->m_randomSeed = static_cast<uint16_t>(CRandom::uint32(this->m_random));

    uint32_t radiusBits = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
    float radius = ((*reinterpret_cast<float*>(&radiusBits) - 1.0f) * this->m_radiusSpan) + this->m_minRadius;

    uint32_t latitudeRaw = CRandom::uint32(this->m_random);
    uint32_t latitudeBits = (latitudeRaw & 0x7FFFFF) | 0x3F800000;
    float latitudeUnit = *reinterpret_cast<float*>(&latitudeBits);
    float latitudeVariation = static_cast<int32_t>(latitudeRaw) >= 0 ? latitudeUnit - 2.0f : 2.0f - latitudeUnit;
    float latitude = latitudeVariation * this->m_latitude;

    uint32_t longitudeRaw = CRandom::uint32(this->m_random);
    uint32_t longitudeBits = (longitudeRaw & 0x7FFFFF) | 0x3F800000;
    float longitudeUnit = *reinterpret_cast<float*>(&longitudeBits);
    float longitudeVariation = static_cast<int32_t>(longitudeRaw) >= 0 ? longitudeUnit - 2.0f : 2.0f - longitudeUnit;
    float longitude = longitudeVariation * this->m_longitude;

    float cosLatitude = cos(latitude);
    float sinLatitude = sin(latitude);
    float cosLongitude = cos(longitude);
    float sinLongitude = sin(longitude);

    C3Vector direction;
    direction.x = cosLongitude * cosLatitude;
    direction.y = cosLatitude * sinLongitude;
    direction.z = sinLatitude;

    particle->m_position.x = direction.x * radius;
    particle->m_position.y = direction.y * radius;
    particle->m_position.z = radius * direction.z;

    if (this->m_zsource != 0.0f) {
        float dz = particle->m_position.z - this->m_zsource;

        direction.x = particle->m_position.x;
        direction.y = particle->m_position.y;
        direction.z = dz;

        float lengthSq = (direction.y * direction.y) + (direction.x * direction.x) + (dz * dz);

        if (lengthSq > 0.00000023841858f) {
            float inverse = 1.0f / sqrt(lengthSq);

            direction.x = direction.x * inverse;
            direction.y = direction.y * inverse;
            direction.z = dz * inverse;
        }
    } else if ((this->m_flags & 0x8000) != 0) {
        direction.x = 0.0f;
        direction.y = 0.0f;
        direction.z = 1.0f;
    }

    float speed = this->CalcVelocity();

    direction.x = direction.x * speed;
    direction.y = direction.y * speed;
    direction.z = speed * direction.z;

    if ((this->m_flags & 0x200) != 0) {
        particle->m_velocity = direction;
    } else {
        particle->m_velocity.x = (xform->c0 * direction.z) + (xform->b0 * direction.y) + (xform->a0 * direction.x);
        particle->m_velocity.y = (xform->c1 * direction.z) + (xform->b1 * direction.y) + (xform->a1 * direction.x);
        particle->m_velocity.z = (direction.y * xform->b2) + (direction.z * xform->c2) + (direction.x * xform->a2);

        particle->m_position = particle->m_position * *xform;

        if ((this->m_flags & 0x40000) != 0) {
            this->ProjectParticle(particle);
        }
    }

    if ((this->m_flags & 0x800) != 0) {
        uint32_t raw = CRandom::uint32(this->m_random);
        uint32_t bits = (raw & 0x7FFFFF) | 0x3F800000;
        float unit = *reinterpret_cast<float*>(&bits);
        float variation = static_cast<int32_t>(raw) >= 0 ? unit - 2.0f : 2.0f - unit;
        float factor = (variation * this->m_variation) + 1.0f;

        particle->m_velocity.x = (this->m_parentVelocity.x * factor) + particle->m_velocity.x;
        particle->m_velocity.y = (this->m_parentVelocity.y * factor) + particle->m_velocity.y;
        particle->m_velocity.z = (factor * this->m_parentVelocity.z) + particle->m_velocity.z;
    }
}

// OFFSET: 0x9820F0
CSplineParticleEmitter::CSplineParticleEmitter() {
    this->m_width = 0.0f;
    this->m_splineLength = 0.0f;
    this->m_latitude = 0.0f;
    this->m_emitterType = 3;
    this->m_longitude = 0.0f;
    this->m_spawnAtEnd = 0;
    this->m_ratePerUnit = 0.0f;
}

// OFFSET: 0x981C90
void CSplineParticleEmitter::SetWidth(float width) {
    if (width < 0.0f) {
        this->m_width = 0.0f;
    } else if (width < 1.0f) {
        this->m_width = width;
    } else {
        this->m_width = 1.0f;
    }
}

// OFFSET: 0x981CD0
void CSplineParticleEmitter::SetHeight(float height) {
    float length = 0.0f;

    if (height >= 0.0f) {
        length = height >= 1.0f ? 1.0f : height;
    }

    if (fabs(length - this->m_splineLength) >= 0.00000023841858f) {
        this->m_splineLength = length;
        this->m_spawnAtEnd = 1;
        this->m_emissionRate = length * this->m_ratePerUnit;
    }
}

// OFFSET: 0x9813E0
void CSplineParticleEmitter::SetLatitude(float latitude) {
    this->m_latitude = latitude;
}

// OFFSET: 0x9814D0
void CSplineParticleEmitter::SetLongitude(float longitude) {
    this->m_longitude = longitude;
}

// OFFSET: 0x9814E0
void CSplineParticleEmitter::SetEmissionRate(float rate) {
    this->m_ratePerUnit = rate;
    this->m_emissionRate = rate * this->m_splineLength;
}

// OFFSET: 0x981D40
void CSplineParticleEmitter::CreateParticle(CParticle2* particle, float dt, C44Matrix* xform) {
    uint32_t ageBits = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
    particle->m_age = (*reinterpret_cast<float*>(&ageBits) - 1.0f) * dt;
    particle->m_randomSeed = static_cast<uint16_t>(CRandom::uint32(this->m_random));

    float t;

    if (this->m_spawnAtEnd) {
        t = this->m_splineLength;
        this->m_spawnAtEnd = 0;
    } else {
        uint32_t bits = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
        t = ((this->m_splineLength - this->m_width) * (*reinterpret_cast<float*>(&bits) - 1.0f)) + this->m_width;
    }

    this->m_spline.Pos(t, &particle->m_position, 1);

    float speed = this->CalcVelocity();

    C3Vector direction;
    direction.x = 0.0f;
    direction.y = 0.0f;
    direction.z = speed;

    if (this->m_zsource == 0.0f) {
        if (this->m_latitude != 0.0f) {
            C3Vector axis;
            axis.x = 0.0f;
            axis.y = 0.0f;
            axis.z = 1.0f;

            C3Vector tangent;
            tangent.x = 0.0f;
            tangent.y = 0.0f;
            tangent.z = 0.0f;

            this->m_spline.Vel(t, &tangent, 1);

            float inverse = 1.0f / sqrt((tangent.z * tangent.z) + (tangent.y * tangent.y) + (tangent.x * tangent.x));
            tangent.x = tangent.x * inverse;
            tangent.y = tangent.y * inverse;
            tangent.z = inverse * tangent.z;

            if (fabs(tangent.z + ((tangent.x + tangent.y) * 0.0f)) > 0.89999998f) {
                axis.z = 1.0f;
            }

            uint32_t raw = CRandom::uint32(this->m_random);
            uint32_t bits = (raw & 0x7FFFFF) | 0x3F800000;
            float unit = *reinterpret_cast<float*>(&bits);
            float variation = static_cast<int32_t>(raw) >= 0 ? unit - 2.0f : 2.0f - unit;

            C33Matrix rotation = C33Matrix::Rotation(variation * this->m_latitude, tangent, true);
            C3Vector spread = axis * rotation;

            if (this->m_longitude != 0.0f) {
                C3Vector offset;
                offset.x = spread.x * this->m_longitude;
                offset.y = spread.y * this->m_longitude;
                offset.z = this->m_longitude * spread.z;

                uint32_t distanceBits = (CRandom::uint32(this->m_random) & 0x7FFFFF) | 0x3F800000;
                float distance = *reinterpret_cast<float*>(&distanceBits) - 1.0f;

                particle->m_position.x = (offset.x * distance) + particle->m_position.x;
                particle->m_position.y = (offset.y * distance) + particle->m_position.y;
                particle->m_position.z = (distance * offset.z) + particle->m_position.z;
            }

            direction.x = spread.x * speed;
            direction.y = spread.y * speed;
            direction.z = speed * spread.z;
        }
    } else {
        float dz = particle->m_position.z - this->m_zsource;
        float scale = speed / sqrt((particle->m_position.y * particle->m_position.y) + (particle->m_position.x * particle->m_position.x) + (dz * dz));

        direction.x = particle->m_position.x * scale;
        direction.y = particle->m_position.y * scale;
        direction.z = scale * dz;
    }

    if ((this->m_flags & 0x200) != 0) {
        particle->m_velocity = direction;
    } else {
        particle->m_velocity.x = (xform->a0 * direction.x) + (xform->b0 * direction.y) + (xform->c0 * direction.z);
        particle->m_velocity.y = (xform->b1 * direction.y) + (xform->a1 * direction.x) + (xform->c1 * direction.z);
        particle->m_velocity.z = (direction.x * xform->a2) + (direction.y * xform->b2) + (direction.z * xform->c2);

        particle->m_position = particle->m_position * *xform;

        if ((this->m_flags & 0x40000) != 0) {
            this->ProjectParticle(particle);
        }
    }

    if ((this->m_flags & 0x800) != 0) {
        uint32_t raw = CRandom::uint32(this->m_random);
        uint32_t bits = (raw & 0x7FFFFF) | 0x3F800000;
        float unit = *reinterpret_cast<float*>(&bits);
        float variation = static_cast<int32_t>(raw) >= 0 ? unit - 2.0f : 2.0f - unit;
        float factor = (variation * this->m_variation) + 1.0f;

        particle->m_velocity.x = (this->m_parentVelocity.x * factor) + particle->m_velocity.x;
        particle->m_velocity.y = (this->m_parentVelocity.y * factor) + particle->m_velocity.y;
        particle->m_velocity.z = (factor * this->m_parentVelocity.z) + particle->m_velocity.z;
    }
}

// OFFSET: 0x981500
void CSplineParticleEmitter::SetSpline(C3Vector* points, uint32_t count) {
    this->m_spline.SetPoints(points, count);
}

float ParticleSystemManager::g_particleDensity = 1.0f;
bool (*ParticleSystemManager::s_projectCallback)(C3Vector*, float*, void*);
void* ParticleSystemManager::s_projectParam;
ParticleSystemManager* ParticleSystemManager::s_instance;

// OFFSET: 0x981130
ParticleSystemManager* ParticleSystemManager::GetInstance() {
    if (ParticleSystemManager::s_instance)
        return ParticleSystemManager::s_instance;

    ParticleSystemManager::s_instance = new (STORM_ALLOC(sizeof(ParticleSystemManager))) ParticleSystemManager();

    CRndSeed seed { 0 };
    seed.SetSeed((rand() << 16) | rand());
    for (int32_t i = 0; i < 128; i++) {
        uint32_t bits = (CRandom::uint32(seed) & 0x7FFFFF) | 0x3F800000;
        s_twinkleTable[i] = *reinterpret_cast<float*>(&bits) - 1.0f;
    }

    CParticleEmitter2::Init();
    return ParticleSystemManager::s_instance;
}

// OFFSET: 0x980ED0
float ParticleSystemManager::GetScaler() {
    return g_particleDensity;
}

// OFFSET: 0x980F70
void ParticleSystemManager::SetScaler(float scaler) {
    g_particleDensity = scaler;
    if (g_particleDensity < 0.0f)
        g_particleDensity = 0.0f;
    else if (g_particleDensity > 1.0f)
        g_particleDensity = 1.0f;
}
