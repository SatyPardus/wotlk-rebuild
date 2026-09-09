#include "model/CM2Model.hpp"
#include "async/AsyncFileRead.hpp"
#include "math/Types.hpp"
#include "model/CM2Scene.hpp"
#include "model/CM2Shared.hpp"
#include "model/M2Animate.hpp"
#include "model/M2Data.hpp"
#include "model/M2Model.hpp"
#include <cmath>
#include <new>
#include <common/DataMgr.hpp>
#include <common/ObjectAlloc.hpp>
#include <tempest/Math.hpp>
#include "model/M2Internal.hpp"
#include <db/StaticDb.hpp>
#include <tempest/facet/CFacet.hpp>
#include "model/CM2SequenceLoad.hpp"
#include "model/CParticleEmitter2.hpp"
#include "model/CRibbonEmitter.hpp"

uint32_t CM2Model::s_loadingSequence = 0xFFFFFFFF;
uint8_t* CM2Model::s_sequenceBase;
uint32_t CM2Model::s_sequenceBaseSize;
uint32_t CM2Model::s_skinProfileBoneCountMax[] = { 256, 64, 53, 21 };
TSGrowableArray<C3Vector> CM2Model::s_collisionPositions;
TSGrowableArray<uint32_t> CM2Model::s_collisionCodes;

static const C44Matrix s_particleBasis(
    0.0f, 1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);

CM2Model* CM2Model::AllocModel(uint32_t* heapId) {
    uint32_t memHandle;
    void* object = nullptr;

    if (ObjectAlloc(*heapId, &memHandle, &object, 0)) {
        CM2Model* model = new (object) CM2Model();

        model->m_handle = memHandle;

        return model;
    }

    return nullptr;
}

// OFFSET: 0x825E00
bool CM2Model::HasSequence(M2Data* data, uint32_t sequenceId) {
    uint16_t index = 0xFFFF;

    uint32_t hashCount = data->sequenceIdxHashById.Count();

    if (hashCount == 0) {
        for (uint32_t i = 0; i < data->sequences.Count(); i++) {
            if (data->sequences[i].id == sequenceId) {
                index = i;
                break;
            }
        }
    } else {
        uint32_t slot = sequenceId % hashCount;
        uint16_t probe = data->sequenceIdxHashById[slot];

        if (probe != 0xFFFF) {
            if (data->sequences[probe].id == sequenceId) {
                index = probe;
            } else {
                int32_t step = 1;

                while (true) {
                    slot = (slot + step * step) % hashCount;
                    probe = data->sequenceIdxHashById[slot];

                    if (probe == 0xFFFF) {
                        break;
                    }

                    step++;

                    if (data->sequences[probe].id == sequenceId) {
                        index = probe;
                        break;
                    }
                }
            }
        }
    }

    return index < data->sequences.Count();
}

// OFFSET: 0x8260C0
uint16_t CM2Model::Sub8260C0(M2Data* data, uint32_t sequenceId, int32_t a3) {
    uint32_t index = 0xFFFF;

    uint32_t hashCount = data->sequenceIdxHashById.Count();

    if (hashCount) {
        uint32_t slot = sequenceId % hashCount;
        uint16_t probe = data->sequenceIdxHashById[slot];

        if (probe != 0xFFFF) {
            if (data->sequences[probe].id == sequenceId) {
                index = probe;
            } else {
                int32_t step = 1;

                do {
                    slot = (slot + step * step) % hashCount;
                    probe = data->sequenceIdxHashById[slot];

                    if (probe == 0xFFFF) {
                        break;
                    }

                    step++;

                    if (data->sequences[probe].id == sequenceId) {
                        index = probe;
                        break;
                    }
                } while (true);
            }
        }
    } else {
        for (uint32_t i = 0; i < data->sequences.Count(); i++) {
            if (data->sequences[i].id == sequenceId) {
                index = i;
                break;
            }
        }
    }

    uint32_t count = data->sequences.Count();

    if (index >= count) {
        return 0xFFFF;
    }

    uint32_t cur = index;

    while (a3) {
        cur = data->sequences[cur].variationNext;
        a3--;

        if (cur >= count) {
            break;
        }
    }

    if (cur >= count || a3) {
        return 0xFFFF;
    }

    return static_cast<uint16_t>(cur);
}

CM2Model::~CM2Model() {
    if (this->model30) {
        this->model30->Release();
    }
    //this->CancelAllDeferredSequences();
    this->UnlinkFromCallbackList();
    if (this->m_animatePrev)
        *this->m_animatePrev = this->m_animateNext;
    if (this->m_animateNext)
        this->m_animateNext->m_animatePrev = this->m_animatePrev;
    //unk_0068 = this->unk_0068;
    //if (unk_0068)
    //    *unk_0068 = this->unk_006C;
    //unk_006C = this->unk_006C;
    //if (unk_006C)
    //    *(unk_006C + 104) = this->unk_0068;
    if (this->m_particlePrev) {
        *this->m_particlePrev = this->m_particleNext;
    }
    if (this->m_particleNext) {
        this->m_particleNext->m_particlePrev = this->m_particlePrev;
    }
    //unk_02D8 = this->unk_02D8;
    //if (unk_02D8)
    //    *unk_02D8 = this->unk_02DC;
    //unk_02DC = this->unk_02DC;
    //if (unk_02DC)
    //    *(unk_02DC + 728) = this->unk_02D8;
    this->DetachFromScene();
    if (this->m_shared) {
        //this->FreeExternalResources();
        //this->FreeInternalResources();
        this->m_shared->Release();
        this->m_shared = nullptr;
    }
    while (this->m_attachList) {
        auto attachList = this->m_attachList;
        auto attachPrev = attachList->m_attachPrev;
        if (attachPrev)
            *attachPrev = attachList->m_attachNext;
        auto attachNext = attachList->m_attachNext;
        if (attachNext)
            attachNext->m_attachPrev = attachList->m_attachPrev;
        attachList->f_flags &= ~0x40000u;
        attachList->m_refCount--;
        attachList->m_attachPrev = 0;
        attachList->m_attachNext = 0;
        attachList->m_attachParent = 0;
        attachList->m_attachmentId = -1;
        //attachList->unk_0170.a1 = 0.0;
        if (attachList->m_refCount == 0) {
            attachList->~CM2Model();
            ObjectFree(*g_modelPool, attachList->m_handle);
        }
    }
    if (this->m_attachPrev) {
        *this->m_attachPrev = this->m_attachNext;
    }

    if (this->m_attachNext) {
        this->m_attachNext->m_attachPrev = this->m_attachPrev;
    }
    while (this->m_modelCallList) {
        CM2ModelCall* call = this->m_modelCallList;
        this->m_modelCallList = call->modelCallNext;
        if (call->type == 0 && call->replaceTexture.texture) {
            HandleClose(call->replaceTexture.texture);
        }
        delete call;
    }
    this->UnoptimizeVisibleGeometry();
    SMemAlignedFree(this->m_boneMatrices);
    SMemAlignedFree(this->m_textureMatrices);
    this->m_attachParent = nullptr;
    //this->ukn_02A8 = 0;
}

// OFFSET: 0x830DC0
void CM2Model::Animate() {
    if (this->m_frameStamp == this->m_scene->m_frameStamp) {
        return;
    }

    if (this->m_attachParent) {
        this->m_attachParent->Animate();
    } else {
        C3Vector diffuse = { 1.0f, 1.0f, 1.0f };
        C3Vector emissive = { 0.0f, 0.0f, 0.0f };

        if ((this->f_flags & 0x1000) != 0) {
            this->AnimateMTSimple(&this->m_scene->m_view, diffuse, emissive, 1.0f, 1.0f);
        } else {
            this->AnimateMT(&this->m_scene->m_view, diffuse, emissive, 1.0f, 1.0f);
        }
    }

    if (this->m_frameStamp == this->m_scene->m_frameStamp) {
        return;
    }

    if (this->m_attachParent && (this->f_flags & 0x1) != 0) {
        C44Matrix matrix;
        C44Matrix* parentMatrix = &this->m_attachParent->matrixF4;

        if ((this->m_attachParent->f_flags & 0x1) != 0 && this->m_attachParent->m_frameStamp == this->m_scene->m_frameStamp && this->m_attachmentIndex != 0xFFFF) {
            auto& attachment = this->m_attachParent->m_shared->m_data->attachments[this->m_attachmentIndex];

            matrix = this->m_attachParent->m_boneMatrices[attachment.boneIndex];
            matrix.Translate(attachment.position);

            parentMatrix = &matrix;
        }

        CM2Model* parent = this->m_attachParent;

        if ((this->f_flags & 0x1000) != 0) {
            this->AnimateMTSimple(parentMatrix, parent->m_currentDiffuse, parent->m_currentEmissive, parent->float198, parent->alpha19C);
        } else {
            this->AnimateMT(parentMatrix, parent->m_currentDiffuse, parent->m_currentEmissive, parent->float198, parent->alpha19C);
        }

        if (this->m_frameStamp == this->m_scene->m_frameStamp) {
            return;
        }
    }

    if (this->m_attachParent) {
        this->matrixF4 = this->m_attachParent->matrixF4;
    } else {
        this->matrixF4 = this->m_worldTransform * this->m_scene->m_view;
    }
}

void CM2Model::AnimateCamerasST() {
    for (int32_t i = 0; i < this->m_shared->m_data->cameras.Count(); i++) {
        auto& camera = this->m_shared->m_data->cameras[i];
        auto& modelCamera = this->m_cameras[i];

        C3Vector v56 = modelCamera.positionTrack.currentValue + camera.positionPivot;
        C3Vector cameraPos = (v56 * this->matrixF4) * this->m_scene->m_viewInv;
        DataMgrSetCoord(modelCamera.m_camera, 7, cameraPos, 0x0);

        C3Vector v57 = modelCamera.targetTrack.currentValue + camera.targetPivot;
        C3Vector targetPos = (v57 * this->matrixF4) * this->m_scene->m_viewInv;
        DataMgrSetCoord(modelCamera.m_camera, 8, targetPos, 0x0);

        DataMgrSetFloat(modelCamera.m_camera, 5, modelCamera.rollTrack.currentValue);
    }
}

// OFFSET: 0x82F0F0
void CM2Model::AnimateMT(const C44Matrix* view, const C3Vector& diffuse, const C3Vector& emissive, float alphaScale, float emissiveScale) {
    if ((this->f_flags & 0x1) == 0 || this->m_frameStamp == this->m_scene->m_frameStamp) {
        return;
    }

    M2Data* data = this->m_shared->m_data;

    if (this->m_attachParent) {
        bool inheritedFlag8 = (this->m_attachParent->f_flags & 0x8) != 0 && (this->f_flags & 0x80000000) != 0;
        this->f_flags ^= (this->f_flags ^ (inheritedFlag8 ? 0x8 : 0x0)) & 0x8;

        bool inheritedFlag10000 = (this->m_attachParent->f_flags & 0x10000) != 0 && (this->f_flags & 0x20000) != 0;
        this->f_flags ^= (this->f_flags ^ (inheritedFlag10000 ? 0x10000 : 0x0)) & 0x10000;

        //this->float174 = this->m_attachParent->float174;
    }

    //if (data->flags & 0x4) {
    //    this->float198 = this->float178;
    //    this->alpha19C = this->float17C * this->float178;
    //    this->m_currentDiffuse = this->vector180;
    //    this->m_currentEmissive = this->vector18C;
    //} else {
    //    this->m_currentDiffuse.x = diffuse.x * this->vector180.x;
    //    this->m_currentDiffuse.y = diffuse.y * this->vector180.y;
    //    this->m_currentDiffuse.z = diffuse.z * this->vector180.z;
    //    this->m_currentEmissive = this->vector18C;
    //
    //    if ((this->f_flags & 0x100000) != 0) {
    //        this->float198 = this->float178;
    //    } else {
    //        this->float198 = alphaScale * this->float178;
    //    }
    //
    //    this->alpha19C = this->float17C * emissiveScale * this->float178;
    //
    //    if ((this->f_flags & 0x80000) == 0) {
    //        this->m_currentEmissive.x += emissive.x;
    //        this->m_currentEmissive.y += emissive.y;
    //        this->m_currentEmissive.z += emissive.z;
    //    }
    //}

    for (uint32_t i = 0; i < data->loops.Count(); i++) {
        uint32_t loopLength = data->loops[i].length;
        this->m_loops[i] = loopLength ? (this->m_scene->m_time - this->m_loopOrigin) % loopLength : 0;
    }

    this->matrixF4 = this->m_worldTransform * *view;

    if (!this->m_attachParent || (this->m_attachParent->m_flags & 0x1) != 0) {
        this->float88 = this->matrixF4.d2 * this->matrixF4.d2 + this->matrixF4.d1 * this->matrixF4.d1 + this->matrixF4.d0 * this->matrixF4.d0;
    } else {
        this->float88 = this->m_attachParent->float88;
    }

    uint32_t elapsedTime = 0;

    if (this->m_lastAnimTime && this->m_scene->m_time) {
        elapsedTime = this->m_scene->m_time - this->m_lastAnimTime;
        this->m_lastAnimTime = this->m_scene->m_time;
    }

    for (uint32_t i = 0; i < data->bones.Count(); i++) {
        auto& bone = data->bones[i];
        auto& modelBone = this->m_bones[i];

        if (modelBone.sequence.m_sequenceIndex == 0xFFFF) {
            if (bone.parentIndex < data->bones.Count()) {
                auto& parentBone = this->m_bones[bone.parentIndex];
                modelBone.sequence.m_currentTime = parentBone.sequence.m_currentTime;
                modelBone.sequence.m_animIndex = parentBone.sequence.m_animIndex;
                modelBone.sequence.m_sourceBoneIndex = parentBone.sequence.m_sourceBoneIndex;
            } else if (i != 0) {
                modelBone.sequence.m_currentTime = this->m_bones[0].sequence.m_currentTime;
                modelBone.sequence.m_animIndex = this->m_bones[0].sequence.m_animIndex;
                modelBone.sequence.m_sourceBoneIndex = this->m_bones[0].sequence.m_sourceBoneIndex;
            }
        } else {
            if (this->m_lastAnimTime) {
                modelBone.sequence.m_startTime += elapsedTime;
                modelBone.sequence.m_endTime += elapsedTime;
            }

            auto& sequence = data->sequences[modelBone.sequence.m_sequenceIndex];
            int32_t sampleTime = this->m_scene->m_time;
            int32_t currentTime = 0;
            bool clamped = false;

            if (sequence.flags & 0x1) {
                if (static_cast<int32_t>(modelBone.sequence.m_endTime - sampleTime) <= 0) {
                    int32_t span = modelBone.sequence.m_endTime - modelBone.sequence.m_startTime;
                    currentTime = modelBone.sequence.m_startOffset + CMath::fuint(span * modelBone.sequence.m_speed);
                    currentTime = currentTime >= 0 ? std::min(currentTime, static_cast<int32_t>(sequence.duration)) : 0;
                    clamped = true;
                } else if (static_cast<int32_t>(modelBone.sequence.m_startTime - sampleTime) > 0) {
                    sampleTime = modelBone.sequence.m_startTime;
                }
            }

            if (!clamped && sequence.duration) {
                int32_t span = sampleTime - modelBone.sequence.m_startTime;
                currentTime = (modelBone.sequence.m_startOffset + CMath::fuint(span * modelBone.sequence.m_speed)) % sequence.duration;
            }

            modelBone.sequence.m_currentTime = currentTime;
            modelBone.sequence.m_animIndex = modelBone.sequence.m_sequenceIndex;
            modelBone.sequence.m_sourceBoneIndex = i;
        }

        if (modelBone.secondarySequence.m_sequenceIndex == 0xFFFF) {
            if (bone.parentIndex < data->bones.Count()) {
                auto& parentBone = this->m_bones[bone.parentIndex];
                modelBone.secondarySequence.m_currentTime = parentBone.secondarySequence.m_currentTime;
                modelBone.secondarySequence.m_animIndex = parentBone.secondarySequence.m_animIndex;
            } else if (i != 0) {
                modelBone.secondarySequence.m_currentTime = this->m_bones[0].secondarySequence.m_currentTime;
                modelBone.secondarySequence.m_animIndex = this->m_bones[0].secondarySequence.m_animIndex;
            } else {
                modelBone.secondarySequence.m_currentTime = modelBone.sequence.m_currentTime;
                modelBone.secondarySequence.m_animIndex = modelBone.sequence.m_animIndex;
            }
        } else {
            if (this->m_lastAnimTime) {
                modelBone.secondarySequence.m_startTime += elapsedTime;
                modelBone.secondarySequence.m_endTime += elapsedTime;
            }

            auto& sequence = data->sequences[modelBone.secondarySequence.m_sequenceIndex];
            int32_t sampleTime = this->m_scene->m_time;
            int32_t currentTime = 0;
            bool clamped = false;

            if (sequence.flags & 0x1) {
                if (static_cast<int32_t>(modelBone.secondarySequence.m_endTime - sampleTime) <= 0) {
                    int32_t span = modelBone.secondarySequence.m_endTime - modelBone.secondarySequence.m_startTime;
                    currentTime = modelBone.secondarySequence.m_startOffset + CMath::fuint(span * modelBone.secondarySequence.m_speed);
                    currentTime = currentTime >= 0 ? std::min(currentTime, static_cast<int32_t>(sequence.duration)) : 0;
                    clamped = true;
                } else if (static_cast<int32_t>(modelBone.secondarySequence.m_startTime - sampleTime) > 0) {
                    sampleTime = modelBone.secondarySequence.m_startTime;
                }
            }

            if (!clamped && sequence.duration) {
                int32_t span = sampleTime - modelBone.secondarySequence.m_startTime;
                currentTime = (modelBone.secondarySequence.m_startOffset + CMath::fuint(span * modelBone.secondarySequence.m_speed)) % sequence.duration;
            }

            modelBone.secondarySequence.m_currentTime = currentTime;
            modelBone.secondarySequence.m_animIndex = modelBone.secondarySequence.m_sequenceIndex;

            if (static_cast<int32_t>(this->m_scene->m_time - modelBone.m_blendEndTime) >= 0) {
                modelBone.secondarySequence.m_sequenceIndex = 0xFFFF;
            }
        }

        if (modelBone.sequence.m_sequenceIndex == 0xFFFF && modelBone.secondarySequence.m_sequenceIndex == 0xFFFF) {
            if (bone.parentIndex < data->bones.Count()) {
                modelBone.m_blendFactor = this->m_bones[bone.parentIndex].m_blendFactor;
            } else if (i != 0) {
                modelBone.m_blendFactor = this->m_bones[0].m_blendFactor;
            } else {
                modelBone.m_blendFactor = 0.0f;
            }
        } else {
            int32_t remaining = modelBone.m_blendEndTime - this->m_scene->m_time;
            bool sameSample = modelBone.sequence.m_currentTime == modelBone.secondarySequence.m_currentTime && modelBone.sequence.m_animIndex == modelBone.secondarySequence.m_animIndex;

            if (remaining <= 0 || sameSample) {
                modelBone.m_blendFactor = 0.0f;
            } else {
                float t = remaining * modelBone.m_invBlendDuration;

                if (t < 0.0f) {
                    modelBone.m_blendFactor = 0.0f * modelBone.m_blendWeightMax;
                } else if (t > 1.0f) {
                    modelBone.m_blendFactor = 1.0f * modelBone.m_blendWeightMax;
                } else {
                    modelBone.m_blendFactor = t * ((3.0f - (t + t)) * t) * modelBone.m_blendWeightMax;
                }
            }
        }

        uint32_t boneFlags = bone.flags | modelBone.m_flags;
        C44Matrix billboardParent;
        C44Matrix* boneParentMatrix;

        if (bone.parentIndex == 0xFFFF) {
            boneParentMatrix = &this->matrixF4;
        } else {
            boneParentMatrix = &this->m_boneMatrices[bone.parentIndex];

            if (boneFlags & 0x7) {
                billboardParent = this->m_boneMatrices[bone.parentIndex];
                boneParentMatrix = &billboardParent;

                C3Vector pivot = billboardParent.TransformPoint(bone.pivot);

                if ((boneFlags & 0x6) == 0x2) {
                    C3Vector rowA = { billboardParent.a0, billboardParent.a1, billboardParent.a2 };
                    C3Vector rowB = { billboardParent.b0, billboardParent.b1, billboardParent.b2 };
                    C3Vector rowC = { billboardParent.c0, billboardParent.c1, billboardParent.c2 };
                    rowA.Normalize();
                    rowB.Normalize();
                    rowC.Normalize();

                    float lenA = sqrt(this->matrixF4.a2 * this->matrixF4.a2 + this->matrixF4.a1 * this->matrixF4.a1 + this->matrixF4.a0 * this->matrixF4.a0);
                    billboardParent.a0 = rowA.x * lenA;
                    billboardParent.a1 = rowA.y * lenA;
                    billboardParent.a2 = rowA.z * lenA;

                    float lenB = sqrt(this->matrixF4.b2 * this->matrixF4.b2 + this->matrixF4.b1 * this->matrixF4.b1 + this->matrixF4.b0 * this->matrixF4.b0);
                    billboardParent.b0 = rowB.x * lenB;
                    billboardParent.b1 = rowB.y * lenB;
                    billboardParent.b2 = rowB.z * lenB;

                    float lenC = sqrt(this->matrixF4.c2 * this->matrixF4.c2 + this->matrixF4.c1 * this->matrixF4.c1 + this->matrixF4.c0 * this->matrixF4.c0);
                    billboardParent.c0 = rowC.x * lenC;
                    billboardParent.c1 = rowC.y * lenC;
                    billboardParent.c2 = rowC.z * lenC;
                } else if ((boneFlags & 0x6) == 0x4) {
                    float refA = this->matrixF4.a2 * this->matrixF4.a2 + this->matrixF4.a1 * this->matrixF4.a1 + this->matrixF4.a0 * this->matrixF4.a0;
                    float scaleA = refA <= 0.0000099999997f ? 1.0f : sqrt((billboardParent.a0 * billboardParent.a0 + billboardParent.a2 * billboardParent.a2 + billboardParent.a1 * billboardParent.a1) / refA);
                    billboardParent.a0 = this->matrixF4.a0 * scaleA;
                    billboardParent.a1 = this->matrixF4.a1 * scaleA;
                    billboardParent.a2 = this->matrixF4.a2 * scaleA;

                    float refB = this->matrixF4.b2 * this->matrixF4.b2 + this->matrixF4.b1 * this->matrixF4.b1 + this->matrixF4.b0 * this->matrixF4.b0;
                    float scaleB = refB <= 0.0000099999997f ? 1.0f : sqrt((billboardParent.b2 * billboardParent.b2 + billboardParent.b1 * billboardParent.b1 + billboardParent.b0 * billboardParent.b0) / refB);
                    billboardParent.b0 = this->matrixF4.b0 * scaleB;
                    billboardParent.b1 = this->matrixF4.b1 * scaleB;
                    billboardParent.b2 = this->matrixF4.b2 * scaleB;

                    float refC = this->matrixF4.c2 * this->matrixF4.c2 + this->matrixF4.c1 * this->matrixF4.c1 + this->matrixF4.c0 * this->matrixF4.c0;
                    float scaleC = refC <= 0.0000099999997f ? 1.0f : sqrt((billboardParent.c2 * billboardParent.c2 + billboardParent.c1 * billboardParent.c1 + billboardParent.c0 * billboardParent.c0) / refC);
                    billboardParent.c0 = this->matrixF4.c0 * scaleC;
                    billboardParent.c1 = this->matrixF4.c1 * scaleC;
                    billboardParent.c2 = this->matrixF4.c2 * scaleC;
                } else if ((boneFlags & 0x6) == 0x6) {
                    billboardParent.a0 = this->matrixF4.a0;
                    billboardParent.a1 = this->matrixF4.a1;
                    billboardParent.a2 = this->matrixF4.a2;
                    billboardParent.b0 = this->matrixF4.b0;
                    billboardParent.b1 = this->matrixF4.b1;
                    billboardParent.b2 = this->matrixF4.b2;
                    billboardParent.c0 = this->matrixF4.c0;
                    billboardParent.c1 = this->matrixF4.c1;
                    billboardParent.c2 = this->matrixF4.c2;
                }

                if (boneFlags & 0x1) {
                    billboardParent.d0 = this->matrixF4.d0;
                    billboardParent.d1 = this->matrixF4.d1;
                    billboardParent.d2 = this->matrixF4.d2;
                } else {
                    billboardParent.d0 = pivot.x - (bone.pivot.x * billboardParent.a0 + bone.pivot.y * billboardParent.b0 + bone.pivot.z * billboardParent.c0);
                    billboardParent.d1 = pivot.y - (bone.pivot.x * billboardParent.a1 + bone.pivot.y * billboardParent.b1 + bone.pivot.z * billboardParent.c1);
                    billboardParent.d2 = pivot.z - (bone.pivot.x * billboardParent.a2 + bone.pivot.y * billboardParent.b2 + bone.pivot.z * billboardParent.c2);
                }
            }
        }

        C44Matrix boneLocalMatrix;

        if (boneFlags & 0x280) {
            if (bone.rotationTrack.sequenceTimes.Count()) {
                if (bone.rotationTrack.sequenceTimes.Count() > 1 || (bone.rotationTrack.sequenceTimes.Count() == 1 && bone.rotationTrack.sequenceTimes[0].times.Count() > this->uint90)) {
                    C4Quaternion defaultValue = { 0.0f, 0.0f, 0.0f, 1.0f };
                    M2AnimateTrack<M2CompQuat, C4Quaternion>(this, &modelBone, bone.rotationTrack, modelBone.rotationTrack, defaultValue);
                }

                boneLocalMatrix = C44Matrix(modelBone.rotationTrack.currentValue);
            }

            if (bone.scaleTrack.sequenceTimes.Count()) {
                if (bone.scaleTrack.sequenceTimes.Count() > 1 || (bone.scaleTrack.sequenceTimes.Count() == 1 && bone.scaleTrack.sequenceTimes[0].times.Count() > this->uint90)) {
                    C3Vector defaultValue = { 1.0f, 1.0f, 1.0f };
                    M2AnimateTrack<C3Vector, C3Vector>(this, &modelBone, bone.scaleTrack, modelBone.scaleTrack, defaultValue);
                }

                boneLocalMatrix.Scale(modelBone.scaleTrack.currentValue);
            }

            if ((boneFlags & 0x80000000) != 0 && modelBone.m_proceduralTransform) {
                boneLocalMatrix *= *modelBone.m_proceduralTransform;
            }

            C3Vector translation = bone.pivot;

            if (bone.translationTrack.sequenceTimes.Count()) {
                if (bone.translationTrack.sequenceTimes.Count() > 1 || (bone.translationTrack.sequenceTimes.Count() == 1 && bone.translationTrack.sequenceTimes[0].times.Count() > this->uint90)) {
                    C3Vector defaultValue = { 0.0f, 0.0f, 0.0f };
                    M2AnimateTrack<C3Vector, C3Vector>(this, &modelBone, bone.translationTrack, modelBone.translationTrack, defaultValue);
                }

                translation = modelBone.translationTrack.currentValue + bone.pivot;
            }

            boneLocalMatrix.d0 += translation.x;
            boneLocalMatrix.d1 += translation.y;
            boneLocalMatrix.d2 += translation.z;

            C3Vector negPivot = { -bone.pivot.x, -bone.pivot.y, -bone.pivot.z };
            boneLocalMatrix.Translate(negPivot);

            this->m_boneMatrices[i] = boneLocalMatrix * *boneParentMatrix;
        } else {
            this->m_boneMatrices[i] = *boneParentMatrix;
        }

        if (boneFlags & 0x78) {
            C44Matrix& boneMatrix = this->m_boneMatrices[i];

            C3Vector basisLengths;
            basisLengths.x = sqrt(boneMatrix.a0 * boneMatrix.a0 + boneMatrix.a1 * boneMatrix.a1 + boneMatrix.a2 * boneMatrix.a2);
            basisLengths.y = sqrt(boneMatrix.b2 * boneMatrix.b2 + boneMatrix.b1 * boneMatrix.b1 + boneMatrix.b0 * boneMatrix.b0);
            basisLengths.z = sqrt(boneMatrix.c2 * boneMatrix.c2 + boneMatrix.c1 * boneMatrix.c1 + boneMatrix.c0 * boneMatrix.c0);

            C3Vector pivot = boneMatrix.TransformPoint(bone.pivot);

            if ((boneFlags & 0x78) == 0x8) {
                if (boneFlags & 0x280) {
                    C3Vector rowA = { boneLocalMatrix.a1, boneLocalMatrix.a2, -boneLocalMatrix.a0 };
                    C3Vector rowB = { boneLocalMatrix.b1, boneLocalMatrix.b2, -boneLocalMatrix.b0 };
                    C3Vector rowC = { boneLocalMatrix.c1, boneLocalMatrix.c2, -boneLocalMatrix.c0 };
                    rowA.Normalize();
                    rowB.Normalize();
                    rowC.Normalize();

                    boneMatrix.a0 = rowA.x;
                    boneMatrix.a1 = rowA.y;
                    boneMatrix.a2 = rowA.z;
                    boneMatrix.b0 = rowB.x;
                    boneMatrix.b1 = rowB.y;
                    boneMatrix.b2 = rowB.z;
                    boneMatrix.c0 = rowC.x;
                    boneMatrix.c1 = rowC.y;
                    boneMatrix.c2 = rowC.z;
                } else {
                    boneMatrix.a0 = 0.0f;
                    boneMatrix.a1 = 0.0f;
                    boneMatrix.a2 = -1.0f;
                    boneMatrix.b0 = 1.0f;
                    boneMatrix.b1 = 0.0f;
                    boneMatrix.b2 = 0.0f;
                    boneMatrix.c0 = 0.0f;
                    boneMatrix.c1 = 1.0f;
                    boneMatrix.c2 = 0.0f;
                }
            } else if ((boneFlags & 0x78) == 0x10) {
                C3Vector rowA = { boneMatrix.a0, boneMatrix.a1, boneMatrix.a2 };
                rowA.Normalize();
                boneMatrix.a0 = rowA.x;
                boneMatrix.a1 = rowA.y;
                boneMatrix.a2 = rowA.z;

                C3Vector rowB = { boneMatrix.a1, -boneMatrix.a0, 0.0f };
                rowB.Normalize();
                boneMatrix.b0 = rowB.x;
                boneMatrix.b1 = rowB.y;
                boneMatrix.b2 = rowB.z;

                boneMatrix.c0 = boneMatrix.a2 * boneMatrix.b1 - boneMatrix.b2 * boneMatrix.a1;
                boneMatrix.c1 = boneMatrix.a0 * boneMatrix.b2 - boneMatrix.b0 * boneMatrix.a2;
                boneMatrix.c2 = boneMatrix.b0 * boneMatrix.a1 - boneMatrix.a0 * boneMatrix.b1;
            } else if ((boneFlags & 0x78) == 0x20) {
                C3Vector rowB = { boneMatrix.b0, boneMatrix.b1, boneMatrix.b2 };
                rowB.Normalize();
                boneMatrix.b0 = rowB.x;
                boneMatrix.b1 = rowB.y;
                boneMatrix.b2 = rowB.z;

                C3Vector rowA = { -boneMatrix.b1, boneMatrix.b0, 0.0f };
                rowA.Normalize();
                boneMatrix.a0 = rowA.x;
                boneMatrix.a1 = rowA.y;
                boneMatrix.a2 = rowA.z;

                boneMatrix.c0 = boneMatrix.a2 * boneMatrix.b1 - boneMatrix.b2 * boneMatrix.a1;
                boneMatrix.c1 = boneMatrix.a0 * boneMatrix.b2 - boneMatrix.b0 * boneMatrix.a2;
                boneMatrix.c2 = boneMatrix.b0 * boneMatrix.a1 - boneMatrix.a0 * boneMatrix.b1;
            } else if ((boneFlags & 0x78) == 0x40) {
                C3Vector rowC = { boneMatrix.c0, boneMatrix.c1, boneMatrix.c2 };
                rowC.Normalize();
                boneMatrix.c0 = rowC.x;
                boneMatrix.c1 = rowC.y;
                boneMatrix.c2 = rowC.z;

                C3Vector rowB = { boneMatrix.c1, -boneMatrix.c0, 0.0f };
                rowB.Normalize();
                boneMatrix.b0 = rowB.x;
                boneMatrix.b1 = rowB.y;
                boneMatrix.b2 = rowB.z;

                boneMatrix.a0 = boneMatrix.c1 * boneMatrix.b2 - boneMatrix.c2 * boneMatrix.b1;
                boneMatrix.a1 = boneMatrix.b0 * boneMatrix.c2 - boneMatrix.b2 * boneMatrix.c0;
                boneMatrix.a2 = boneMatrix.b1 * boneMatrix.c0 - boneMatrix.b0 * boneMatrix.c1;
            }

            boneMatrix.Scale(basisLengths);

            boneMatrix.d0 = pivot.x - (boneMatrix.a0 * bone.pivot.x + boneMatrix.b0 * bone.pivot.y + boneMatrix.c0 * bone.pivot.z);
            boneMatrix.d1 = pivot.y - (boneMatrix.a1 * bone.pivot.x + boneMatrix.b1 * bone.pivot.y + boneMatrix.c1 * bone.pivot.z);
            boneMatrix.d2 = pivot.z - (boneMatrix.a2 * bone.pivot.x + boneMatrix.b2 * bone.pivot.y + boneMatrix.c2 * bone.pivot.z);
            boneMatrix.a3 = 0.0f;
            boneMatrix.b3 = 0.0f;
            boneMatrix.c3 = 0.0f;
            boneMatrix.d3 = 1.0f;
        }
    }

    for (uint32_t i = 0; i < data->colors.Count(); i++) {
        auto& color = data->colors[i];
        auto& modelColor = this->m_colors[i];

        if (color.colorTrack.sequenceTimes.Count() > 1 || (color.colorTrack.sequenceTimes.Count() == 1 && color.colorTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            C3Vector defaultValue = { 0.0f, 0.0f, 0.0f };
            M2AnimateTrack<C3Vector, C3Vector>(this, this->m_bones, color.colorTrack, modelColor.colorTrack, defaultValue);
        }

        if (color.alphaTrack.sequenceTimes.Count() > 1 || (color.alphaTrack.sequenceTimes.Count() == 1 && color.alphaTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 1.0f;
            M2AnimateTrack<fixed16, float>(this, this->m_bones, color.alphaTrack, modelColor.alphaTrack, defaultValue);
        }
    }

    for (uint32_t i = 0; i < data->textureWeights.Count(); i++) {
        auto& textureWeight = data->textureWeights[i];
        auto& modelTextureWeight = this->m_textureWeights[i];

        if (textureWeight.weightTrack.sequenceTimes.Count() > 1 || (textureWeight.weightTrack.sequenceTimes.Count() == 1 && textureWeight.weightTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 1.0f;
            M2AnimateTrack<fixed16, float>(this, this->m_bones, textureWeight.weightTrack, modelTextureWeight.weightTrack, defaultValue);
        }
    }

    if (data->textureTransforms.Count()) {
        this->AnimateTextureTransformsMT();
    }

    for (uint32_t i = 0; i < data->lights.Count(); i++) {
        auto& light = data->lights[i];
        auto& modelLight = this->m_lights[i];

        if (modelLight.uint64 && light.visibilityTrack.sequenceTimes.Count()) {
            uint8_t defaultValue = 1;
            M2AnimateTrack<uint8_t, uint8_t>(this, &this->m_bones[light.boneIndex], light.visibilityTrack, modelLight.visibilityTrack, defaultValue);
        }

        if ((modelLight.uint64 == 0 || modelLight.visibilityTrack.currentValue == 0) && this->uint90) {
            continue;
        }

        if (light.ambientIntensityTrack.sequenceTimes.Count() > 1 || (light.ambientIntensityTrack.sequenceTimes.Count() == 1 && light.ambientIntensityTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[light.boneIndex], light.ambientIntensityTrack, modelLight.ambientIntensityTrack, defaultValue);
        }

        if (light.ambientColorTrack.sequenceTimes.Count() > 1 || (light.ambientColorTrack.sequenceTimes.Count() == 1 && light.ambientColorTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            C3Vector defaultValue = { 0.0f, 0.0f, 0.0f };
            M2AnimateTrack<C3Vector, C3Vector>(this, &this->m_bones[light.boneIndex], light.ambientColorTrack, modelLight.ambientColorTrack, defaultValue);

            float mul = modelLight.ambientIntensityTrack.currentValue * this->float198;
            modelLight.light.m_ambColor.x = modelLight.ambientColorTrack.currentValue.x * mul;
            modelLight.light.m_ambColor.y = modelLight.ambientColorTrack.currentValue.y * mul;
            modelLight.light.m_ambColor.z = modelLight.ambientColorTrack.currentValue.z * mul;
        }

        if (light.diffuseIntensityTrack.sequenceTimes.Count() > 1 || (light.diffuseIntensityTrack.sequenceTimes.Count() == 1 && light.diffuseIntensityTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[light.boneIndex], light.diffuseIntensityTrack, modelLight.diffuseIntensityTrack, defaultValue);
        }

        if (light.diffuseColorTrack.sequenceTimes.Count() > 1 || (light.diffuseColorTrack.sequenceTimes.Count() == 1 && light.diffuseColorTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            C3Vector defaultValue = { 0.0f, 0.0f, 0.0f };
            M2AnimateTrack<C3Vector, C3Vector>(this, &this->m_bones[light.boneIndex], light.diffuseColorTrack, modelLight.diffuseColorTrack, defaultValue);

            float mul = modelLight.diffuseIntensityTrack.currentValue * this->float198;
            modelLight.light.m_dirColor.x = modelLight.diffuseColorTrack.currentValue.x * mul;
            modelLight.light.m_dirColor.y = modelLight.diffuseColorTrack.currentValue.y * mul;
            modelLight.light.m_dirColor.z = modelLight.diffuseColorTrack.currentValue.z * mul;
        }
    }

    for (uint32_t i = 0; i < data->cameras.Count(); i++) {
        auto& camera = data->cameras[i];
        auto& modelCamera = this->m_cameras[i];

        if (camera.positionTrack.sequenceTimes.Count() > 1 || (camera.positionTrack.sequenceTimes.Count() == 1 && camera.positionTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            C3Vector defaultValue = { 0.0f, 0.0f, 0.0f };
            M2AnimateSplineTrack<M2SplineKey<C3Vector>, C3Vector>(this, this->m_bones, camera.positionTrack, modelCamera.positionTrack, defaultValue);
        }

        if (camera.targetTrack.sequenceTimes.Count() > 1 || (camera.targetTrack.sequenceTimes.Count() == 1 && camera.targetTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            C3Vector defaultValue = { 0.0f, 0.0f, 0.0f };
            M2AnimateSplineTrack<M2SplineKey<C3Vector>, C3Vector>(this, this->m_bones, camera.targetTrack, modelCamera.targetTrack, defaultValue);
        }

        if (camera.rollTrack.sequenceTimes.Count() > 1 || (camera.rollTrack.sequenceTimes.Count() == 1 && camera.rollTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateSplineTrack<M2SplineKey<float>, float>(this, this->m_bones, camera.rollTrack, modelCamera.rollTrack, defaultValue);
        }
    }

    //for (uint32_t i = 0; i < data->ribbons.Count(); i++) {
    //    auto& ribbon = data->ribbons[i];
    //    auto& modelRibbon = this->m_ribbons[i];
    //
    //    if (ribbon.visibilityTrack.sequenceTimes.Count() > 1 || (ribbon.visibilityTrack.sequenceTimes.Count() == 1 && ribbon.visibilityTrack.sequenceTimes[0].times.Count() > this->uint90)) {
    //        uint8_t defaultValue = 1;
    //        M2AnimateTrack<uint8_t, uint8_t>(this, &this->m_bones[ribbon.boneIndex], ribbon.visibilityTrack, modelRibbon.visibilityTrack, defaultValue);
    //    }
    //
    //    if (ribbon.colorTrack.sequenceTimes.Count() > 1 || (ribbon.colorTrack.sequenceTimes.Count() == 1 && ribbon.colorTrack.sequenceTimes[0].times.Count() > this->uint90)) {
    //        C3Vector defaultValue = { 0.0f, 0.0f, 0.0f };
    //        M2AnimateTrack<C3Vector, C3Vector>(this, &this->m_bones[ribbon.boneIndex], ribbon.colorTrack, modelRibbon.colorTrack, defaultValue);
    //    }
    //
    //    if (ribbon.alphaTrack.sequenceTimes.Count() > 1 || (ribbon.alphaTrack.sequenceTimes.Count() == 1 && ribbon.alphaTrack.sequenceTimes[0].times.Count() > this->uint90)) {
    //        float defaultValue = 1.0f;
    //        M2AnimateTrack<fixed16, float>(this, &this->m_bones[ribbon.boneIndex], ribbon.alphaTrack, modelRibbon.alphaTrack, defaultValue);
    //    }
    //
    //    if (ribbon.heightAboveTrack.sequenceTimes.Count() > 1 || (ribbon.heightAboveTrack.sequenceTimes.Count() == 1 && ribbon.heightAboveTrack.sequenceTimes[0].times.Count() > this->uint90)) {
    //        float defaultValue = 0.0f;
    //        M2AnimateTrack<float, float>(this, &this->m_bones[ribbon.boneIndex], ribbon.heightAboveTrack, modelRibbon.heightAboveTrack, defaultValue);
    //    }
    //
    //    if (ribbon.heightBelowTrack.sequenceTimes.Count() > 1 || (ribbon.heightBelowTrack.sequenceTimes.Count() == 1 && ribbon.heightBelowTrack.sequenceTimes[0].times.Count() > this->uint90)) {
    //        float defaultValue = 0.0f;
    //        M2AnimateTrack<float, float>(this, &this->m_bones[ribbon.boneIndex], ribbon.heightBelowTrack, modelRibbon.heightBelowTrack, defaultValue);
    //    }
    //
    //    if (ribbon.textureSlotTrack.sequenceTimes.Count() > 1 || (ribbon.textureSlotTrack.sequenceTimes.Count() == 1 && ribbon.textureSlotTrack.sequenceTimes[0].times.Count() > this->uint90)) {
    //        uint16_t defaultValue = 0;
    //        M2AnimateTrack<uint16_t, uint16_t>(this, &this->m_bones[ribbon.boneIndex], ribbon.textureSlotTrack, modelRibbon.textureSlotTrack, defaultValue);
    //    }
    //}

    this->f_flags &= ~0x400u;

    if (data->particles.Count()) {
        this->AnimateParticlesMT();
    }

    if (this->m_attachments || this->m_attachList) {
        this->AnimateAttachmentsMT();
    }

    this->m_frameStamp = this->m_scene->m_frameStamp;
}

void CM2Model::AnimateMTSimple(const C44Matrix* view, const C3Vector& a3, const C3Vector& a4, float a5, float a6) {
    // TODO
}

// OFFSET: 0x82E550
void CM2Model::AnimateAttachmentsMT() {
    M2Data* data = this->m_shared->m_data;

    for (uint32_t i = 0; i < data->attachments.Count(); i++) {
        auto& attachment = data->attachments[i];

        if (attachment.visibilityTrack.sequenceTimes.Count() > 1 || (attachment.visibilityTrack.sequenceTimes.Count() == 1 && attachment.visibilityTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            uint8_t defaultValue = 1;
            M2AnimateTrack<uint8_t, uint8_t>(this, &this->m_bones[attachment.boneIndex], attachment.visibilityTrack, this->m_attachments[i].visibilityTrack, defaultValue);
        }
    }

    for (auto child = this->m_attachList; child; child = child->m_attachNext) {
        if (child->m_attachmentIndex == 0xFFFF) {
            if (!child->m_flag40000) {
                continue;
            }

            C44Matrix matrix = this->m_boneMatrices[0];

            if (child->m_flag1000) {
                child->AnimateMTSimple(&matrix, this->m_currentDiffuse, this->m_currentEmissive, this->float198, this->alpha19C);
            } else {
                child->AnimateMT(&matrix, this->m_currentDiffuse, this->m_currentEmissive, this->float198, this->alpha19C);
            }
        } else {
            if (!this->m_attachments[child->m_attachmentIndex].visibilityTrack.currentValue) {
                continue;
            }

            auto& attachment = this->m_shared->m_data->attachments[child->m_attachmentIndex];

            C44Matrix matrix = this->m_boneMatrices[attachment.boneIndex];
            matrix.Translate(attachment.position);

            if (child->m_flag1000) {
                child->AnimateMTSimple(&matrix, this->m_currentDiffuse, this->m_currentEmissive, this->float198, this->alpha19C);
            } else {
                child->AnimateMT(&matrix, this->m_currentDiffuse, this->m_currentEmissive, this->float198, this->alpha19C);
            }
        }
    }
}

// OFFSET: 0x828A00
void CM2Model::AnimateST() {
    if (!this->m_loaded) {
        return;
    }

    auto attachParent = this->m_attachParent;

    if (!attachParent) {
        this->m_currentLighting = &this->m_lighting;
    } else {
        this->m_flag8000 = attachParent->m_flag8000;

        if (this->m_flag8000 && attachParent->m_flags & 0x1) {
            this->m_currentLighting = &this->m_lighting;
        } else {
            this->m_currentLighting = attachParent->m_currentLighting;
        }
    }

    if (!this->m_currentLighting) {
        this->m_currentLighting = &this->m_lighting;
    }

    for (int32_t i = 0; i < this->m_shared->m_data->lights.Count(); i++) {
        auto& light = this->m_shared->m_data->lights[i];
        auto& modelLight = this->m_lights[i];

        int32_t visible = 0;
        if (modelLight.uint64 && modelLight.visibilityTrack.currentValue) {
            visible = 1;

            if (light.lightType == M2LIGHT_1) {
                C3Vector pos = this->m_boneMatrices[light.boneIndex].TransformPoint(light.position);
                pos = this->m_scene->m_viewInv.TransformPoint(pos);
                modelLight.light.SetPosition(pos);
            } else {
                float v10 = -this->m_boneMatrices[light.boneIndex].c0;
                float v11 = -this->m_boneMatrices[light.boneIndex].c1;
                float v12 = -this->m_boneMatrices[light.boneIndex].c2;

                float x = this->m_scene->m_viewInv.a0 * v10
                        + this->m_scene->m_viewInv.b0 * v11
                        + this->m_scene->m_viewInv.c0 * v12;
                float y = this->m_scene->m_viewInv.a1 * v10
                        + this->m_scene->m_viewInv.b1 * v11
                        + this->m_scene->m_viewInv.c1 * v12;
                float z = this->m_scene->m_viewInv.a2 * v10
                        + this->m_scene->m_viewInv.b2 * v11
                        + this->m_scene->m_viewInv.c2 * v12;

                C3Vector dir = { x, y, z };

                modelLight.light.SetDirection(dir);
            }
        }

        modelLight.light.SetVisible(visible);
        modelLight.light.m_stamp = this->m_scene->m_frameStamp;
    }

    if (this->m_shared->m_data->cameras.Count()) {
        this->AnimateCamerasST();
    }

    uint32_t time = this->m_scene->m_time;
    float dt = (time - this->m_lastEmitterTime) * 0.001f;
    this->m_lastEmitterTime = time;
    //v44 = m_data->ribbons.count == 0;
    //v58 = v26;
    //v63 = 0;
    //if (!v44) {
    //    v62 = 0;
    //    v61 = 0;
    //    do {
    //        v27 = v61 + m_data->ribbons.offset;
    //        v28 = *(this->unk_02BC + 4 * v63);
    //        v29 = v62 + this->m_ribbons;
    //        v30 = *(v27 + 40);
    //        v55 = v27;
    //        v59 = v29;
    //        v64 = v28;
    //        if (v30 > 1 || v30 == 1 && **(v27 + 44) > this->unk_0090) {
    //            CRibbonEmitter::SetColor(v28, *(v29 + 8), *(v29 + 12), *(v29 + 16));
    //            v28 = v64;
    //        }
    //        v48 = *(v29 + 28) * this->unk_0170.c2;
    //        CRibbonEmitter::SetAlpha(v28, v48);
    //        v31 = *(v27 + 80);
    //        if (v31 > 1 || v31 == 1 && **(v27 + 84) > this->unk_0090)
    //            CRibbonEmitter::SetAbove(v64, *(v29 + 40));
    //        v32 = *(v27 + 100);
    //        if (v32 > 1 || v32 == 1 && **(v27 + 104) > this->unk_0090)
    //            CRibbonEmitter::SetBelow(v64, *(v29 + 52));
    //        v33 = *(v27 + 136);
    //        if (v33 > 1 || v33 == 1 && **(v27 + 140) > this->unk_0090)
    //            CRibbonEmitter::SetTexSlot(v64, *(v29 + 64));
    //        qmemcpy(&v50, &this->m_boneMatrices[*(v27 + 4)], sizeof(v50));
    //        C44Matrix::Translate(&v50, (v55 + 8));
    //        C44Matrix::operator*=(&v50, &this->m_scene->m_viewInv);
    //        v34 = v59;
    //        v35 = v64;
    //        CRibbonEmitter::SetDataEnabled(v64, *(v59 + 76) != 0);
    //        if ((this->f_flags & 0x8000) != 0) {
    //            a1 = this->unk_0170.a1;
    //            v57.x = 0.0;
    //            v57.y = 0.0;
    //            v57.z = 0.0;
    //            CRibbonEmitter::SetPos(v35, &v50, &v57.x, LODWORD(a1));
    //            CRibbonEmitter::Update(v35, v58, *(v34 + 76) == 0);
    //        }
    //        v61 += 176;
    //        v62 += 80;
    //        v17 = ++v63 < v60->ribbons.count;
    //        m_data = v60;
    //    } while (v17);
    //}
    for (uint32_t i = 0; i < this->m_shared->m_data->particles.Count(); i++) {
        this->AnimateParticleST(dt, i);
    }

    if (this->m_flag8) {
        this->m_drawPrev = &this->m_scene->m_drawList;
        this->m_drawNext = this->m_scene->m_drawList;
        this->m_scene->m_drawList = this;

        if (this->m_drawNext) {
            this->m_drawNext->m_drawPrev = &this->m_drawNext;
        }
    }

    if ((this->f_flags & 0x400) != 0 && (this->f_flags & 0x10000) != 0) {
        this->m_particlePrev = &this->m_scene->m_particleList;
        this->m_particleNext = this->m_scene->m_particleList;
        this->m_scene->m_particleList = this;

        if (this->m_particleNext) {
            this->m_particleNext->m_particlePrev = &this->m_particleNext;
        }
    }

    for (auto child = this->m_attachList; child; child = child->m_attachNext) {
        // TODO: v43
        child->AnimateST();
    }
}

// OFFSET: 0x82D6F0
void CM2Model::AnimateTextureTransformsMT() {
    M2Data* data = this->m_shared->m_data;
    if (!data->textureTransforms.Count())
        return;

    static bool initializedLazy;
    static C3Vector center;
    if (!initializedLazy) {
        initializedLazy = true;
        center = C3Vector(0.5f, 0.5f, 0.0f);
    }

    for (int32_t i = 0; i < data->textureTransforms.Count(); i++) {
        M2TextureTransform* src = &data->textureTransforms[i];
        M2ModelTextureTransform* dst = &this->m_textureTransforms[i];
        C44Matrix* mtx = &this->m_textureMatrices[i];

        *mtx = C44Matrix();

        if (src->rotationTrack.sequenceTimes.Count()) {
            C4Quaternion defaultValue;
            M2AnimateTrack<M2CompQuat, C4Quaternion>(this, this->m_bones, src->rotationTrack, dst->rotation, defaultValue);
            mtx->Translate(center);
            mtx->Rotate(dst->rotation.currentValue);
            mtx->Translate(C3Vector(-center.x, -center.y, -center.z));
        }
        if (src->scaleTrack.sequenceTimes.Count()) {
            C3Vector defaultValue = { 1.0f, 1.0f, 1.0f };
            M2AnimateTrack<C3Vector, C3Vector>(this, this->m_bones, src->scaleTrack, dst->scaling, defaultValue);
            mtx->Translate(center);
            mtx->Scale(dst->scaling.currentValue);
            mtx->Translate(C3Vector(-center.x, -center.y, -center.z));
        }
        if (src->translationTrack.sequenceTimes.Count()) {
            C3Vector defaultValue;
            M2AnimateTrack<C3Vector, C3Vector>(this, this->m_bones, src->translationTrack, dst->translation, defaultValue);
            mtx->Translate(dst->translation.currentValue);
        }
    }
}

// OFFSET: 0x82D2F0
void CM2Model::AnimateParticlesMT() {
    auto data = this->m_shared->m_data;

    for (uint32_t i = 0; i < data->particles.Count(); i++) {
        auto& particle = data->particles[i];
        auto& modelParticle = this->m_particles[i];
        auto emitter = this->m_particleEmitters[i];

        if (particle.visibilityTrack.sequenceTimes.Count() > 1 || (particle.visibilityTrack.sequenceTimes.Count() == 1 && particle.visibilityTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            uint8_t defaultValue = 1;
            M2AnimateTrack<uint8_t, uint8_t>(this, &this->m_bones[particle.boneIndex], particle.visibilityTrack, modelParticle.visibilityTrack, defaultValue);
        }

        modelParticle.m_emitting = modelParticle.visibilityTrack.currentValue && (emitter->m_flags & 0x2);
        modelParticle.m_active = modelParticle.m_emitting || emitter->HasLiveParticles();

        if (modelParticle.m_active) {
            this->f_flags |= 0x400;
        }

        if (!modelParticle.visibilityTrack.currentValue && this->uint90) {
            continue;
        }

        if (particle.speedTrack.sequenceTimes.Count() > 1 || (particle.speedTrack.sequenceTimes.Count() == 1 && particle.speedTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.speedTrack, modelParticle.speedTrack, defaultValue);
        }

        if (particle.variationTrack.sequenceTimes.Count() > 1 || (particle.variationTrack.sequenceTimes.Count() == 1 && particle.variationTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.variationTrack, modelParticle.variationTrack, defaultValue);
        }

        if (particle.latitudeTrack.sequenceTimes.Count() > 1 || (particle.latitudeTrack.sequenceTimes.Count() == 1 && particle.latitudeTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.latitudeTrack, modelParticle.latitudeTrack, defaultValue);
        }

        if (particle.longitudeTrack.sequenceTimes.Count() > 1 || (particle.longitudeTrack.sequenceTimes.Count() == 1 && particle.longitudeTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.longitudeTrack, modelParticle.longitudeTrack, defaultValue);
        }

        if (particle.gravityTrack.sequenceTimes.Count() > 1 || (particle.gravityTrack.sequenceTimes.Count() == 1 && particle.gravityTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.gravityTrack, modelParticle.gravityTrack, defaultValue);
        }

        if (particle.lifeTrack.sequenceTimes.Count() > 1 || (particle.lifeTrack.sequenceTimes.Count() == 1 && particle.lifeTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.lifeTrack, modelParticle.lifeTrack, defaultValue);
        }

        if (particle.emissionRateTrack.sequenceTimes.Count() > 1 || (particle.emissionRateTrack.sequenceTimes.Count() == 1 && particle.emissionRateTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.emissionRateTrack, modelParticle.emissionRateTrack, defaultValue);
        }

        if (particle.widthTrack.sequenceTimes.Count() > 1 || (particle.widthTrack.sequenceTimes.Count() == 1 && particle.widthTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.widthTrack, modelParticle.widthTrack, defaultValue);
        }

        if (particle.lengthTrack.sequenceTimes.Count() > 1 || (particle.lengthTrack.sequenceTimes.Count() == 1 && particle.lengthTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.lengthTrack, modelParticle.lengthTrack, defaultValue);
        }

        if (particle.zsourceTrack.sequenceTimes.Count() > 1 || (particle.zsourceTrack.sequenceTimes.Count() == 1 && particle.zsourceTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            float defaultValue = 0.0f;
            M2AnimateTrack<float, float>(this, &this->m_bones[particle.boneIndex], particle.zsourceTrack, modelParticle.zsourceTrack, defaultValue);
        }
    }
}

// OFFSET: 0x8309C0
void CM2Model::AnimateParticleST(float dt, uint32_t index) {
    if ((this->f_flags & 0x1) == 0) {
        return;
    }

    auto& particle = this->m_shared->m_data->particles[index];
    auto& modelParticle = this->m_particles[index];
    auto emitter = this->m_particleEmitters[index];

    if ((particle.flags & 0x8000) == 0) {
        if (modelParticle.m_emitting) {
            emitter->m_flags |= 0x1;
        } else {
            emitter->m_flags &= ~0x1u;
        }
    } else if (modelParticle.visibilityTrack.currentValue && modelParticle.emissionRateTrack.currentValue > 0.0f) {
        if (!modelParticle.m_burstFired) {
            emitter->m_flags |= 0x40;
        }

        modelParticle.m_burstFired = 1;
    } else {
        modelParticle.m_burstFired = 0;
    }

    float rate = 0.0f;

    if (modelParticle.m_emitting) {
        rate = modelParticle.emissionRateTrack.currentValue;
    }

    emitter->SetEmissionRate(rate);

    if (modelParticle.visibilityTrack.currentValue || !this->uint90) {
        if (particle.speedTrack.sequenceTimes.Count() > 1 || (particle.speedTrack.sequenceTimes.Count() == 1 && particle.speedTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->m_speed = modelParticle.speedTrack.currentValue;
        }

        if (particle.variationTrack.sequenceTimes.Count() > 1 || (particle.variationTrack.sequenceTimes.Count() == 1 && particle.variationTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->m_variation = modelParticle.variationTrack.currentValue;
        }

        if (particle.latitudeTrack.sequenceTimes.Count() > 1 || (particle.latitudeTrack.sequenceTimes.Count() == 1 && particle.latitudeTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->SetLatitude(modelParticle.latitudeTrack.currentValue);
        }

        if (particle.longitudeTrack.sequenceTimes.Count() > 1 || (particle.longitudeTrack.sequenceTimes.Count() == 1 && particle.longitudeTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->SetLongitude(modelParticle.longitudeTrack.currentValue);
        }

        if (particle.gravityTrack.sequenceTimes.Count() > 1 || (particle.gravityTrack.sequenceTimes.Count() == 1 && particle.gravityTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->m_gravity = modelParticle.gravityTrack.currentValue;
        }

        if (particle.lifeTrack.sequenceTimes.Count() > 1 || (particle.lifeTrack.sequenceTimes.Count() == 1 && particle.lifeTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->m_life = modelParticle.lifeTrack.currentValue;
        }

        if (particle.widthTrack.sequenceTimes.Count() > 1 || (particle.widthTrack.sequenceTimes.Count() == 1 && particle.widthTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->SetWidth(modelParticle.widthTrack.currentValue);
        }

        if (particle.lengthTrack.sequenceTimes.Count() > 1 || (particle.lengthTrack.sequenceTimes.Count() == 1 && particle.lengthTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->SetHeight(modelParticle.lengthTrack.currentValue);
        }

        if (particle.zsourceTrack.sequenceTimes.Count() > 1 || (particle.zsourceTrack.sequenceTimes.Count() == 1 && particle.zsourceTrack.sequenceTimes[0].times.Count() > this->uint90)) {
            emitter->SetZsource(modelParticle.zsourceTrack.currentValue);
        }

        float alpha = this->float198;

        if (alpha < 0.0f) {
            alpha = 0.0f;
        } else if (alpha >= 1.0f) {
            alpha = 1.0f;
        }

        emitter->m_alphaScale = alpha;
    }

    if (modelParticle.m_active) {
        C44Matrix xform = this->m_boneMatrices[particle.boneIndex];
        xform.Translate(particle.position);
        xform *= this->m_scene->m_viewInv;
        xform = s_particleBasis * xform;

        C3Vector vec = { this->m_scene->m_viewInv.d0, this->m_scene->m_viewInv.d1, this->m_scene->m_viewInv.d2 };

        emitter->Update(dt, &xform, &vec, &this->matrix174);

        uint32_t modelCount = emitter->GetNumParticleModels();

        for (uint32_t i = 0; i < modelCount; i++) {
            uint32_t modelIndex = i;
            CM2Model* particleModel = emitter->GetParticleModelInternal(&modelIndex);

            particleModel->AnimateMT(&this->m_scene->m_view, this->m_currentDiffuse, this->m_currentEmissive, this->float198, this->alpha19C);
            particleModel->AnimateST();

            particleModel->uint2A8 = this->uint2A8;
        }
    }
}

void CM2Model::AttachToScene(CM2Scene* scene) {
    this->DetachFromScene();

    this->m_scene = scene;

    this->m_scenePrev = &this->m_scene->m_modelList;
    this->m_sceneNext = this->m_scene->m_modelList;
    this->m_scene->m_modelList = this;
    if (this->m_sceneNext) {
        this->m_sceneNext->m_scenePrev = &this->m_sceneNext;
    }

    if (this->m_loaded) {
        for (int32_t i = 0; i < this->m_shared->m_data->lights.Count(); i++) {
            this->m_lights[i].light.Initialize(this->m_scene);
        }

        // TODO
        // - sequence / sequence fallback logic
    } else {
        for (auto modelCall = this->m_modelCallList; modelCall; modelCall = modelCall->modelCallNext) {
            modelCall->time += this->m_scene->m_time;
        }
    }
}

uint16_t CM2Model::AttachToParent(CM2Model* parent, uint32_t attachmentId, const C3Vector* a4, int32_t a5) {
    if (this->m_attachParent) {
        this->DetachFromParent();
    }

    this->SetAnimating(0);

    uint16_t attachmentIndex = 0xFFFF;

    if (parent->m_loaded) {
        if (attachmentId < parent->m_shared->m_data->attachmentIndicesById.Count()) {
            attachmentIndex = parent->m_shared->m_data->attachmentIndicesById[attachmentId];
        }

        if (attachmentIndex == 0xFFFF && !a5) {
            return attachmentIndex;
        }
    }

    this->m_attachmentIndex = attachmentIndex;
    this->m_attachmentId = attachmentId;
    this->m_attachParent = parent;

    if (a5) {
        this->f_flags = this->f_flags ^ (this->f_flags ^ (1 << 18)) & 0x40000 | 0x20080;
    } else {
        this->f_flags = this->f_flags ^ (this->f_flags ^ 0) & 0x40000 | 0x20080;
    }

    this->m_attachPrev = &parent->m_attachList;
    this->m_attachNext = parent->m_attachList;
    if (parent->m_attachList) {
        parent->m_attachList->m_attachPrev = &this->m_attachNext;
    }
    parent->m_attachList = this;

    if (!this->m_loaded || !this->m_flag100) {
        auto model = parent;
        while (model) {
            model->f_flags &= ~0x100u;
            model = model->m_attachParent;
        }
    }

    if (!this->m_flag2) {
        auto model = parent;
        while (model) {
            model->f_flags &= ~0x200u;
            model = model->m_attachParent;
        }
    }

    if (a4 && this->m_attachParent && this->m_attachParent->m_loaded && this->m_attachmentIndex != 0xFFFF) {
        auto transform = parent->GetAttachmentWorldTransform(attachmentId);

        float v12 = sqrt(transform.a0 * transform.a0 + transform.a1 * transform.a1 + transform.a2 * transform.a2);
        transform = transform.AffineInverse(v12);

        if (!this->m_flag8000) {
            this->m_worldTransform.Identity();
        }

        transform.Translate(*a4);

        this->m_worldTransform.d0 = transform.a0;
        this->m_worldTransform.d1 = transform.a1;
        this->m_worldTransform.d2 = transform.a2;
        this->m_flag8000 = 1;
    }

    ++this->m_refCount;
}

void CM2Model::CancelDeferredSequences(uint32_t boneIndex, bool a3) {
    // TODO
}

void CM2Model::DetachFromScene() {
    if (this->m_scenePrev) {
        *this->m_scenePrev = this->m_sceneNext;
    }

    if (this->m_sceneNext) {
        this->m_sceneNext->m_scenePrev = this->m_scenePrev;
    }

    this->m_scenePrev = nullptr;
    this->m_sceneNext = nullptr;
    if ((this->f_flags & 1) == 0) {
        for (CM2ModelCall* i = this->m_modelCallList; i; i = i->modelCallNext) {
            i->time -= this->m_scene->m_time;
        }
        this->m_scene = nullptr;
        return;
    }

    if (!this->m_shared->m_data->lights.Count()) {
        this->m_scene = nullptr;
        return;
    }

    for (int32_t i = 0; i < this->m_shared->m_data->lights.Count(); i++) {
        CM2Light* light = &this->m_lights[i].light;
        light->Unlink();
        light->m_scene = nullptr;
    }

    this->m_scene = nullptr;
}

void CM2Model::DetachFromParent() {
    if (this->m_attachPrev) {
        *this->m_attachPrev = this->m_attachNext;
    }

    if (this->m_attachNext) {
        this->m_attachNext->m_attachPrev = this->m_attachPrev;
    }

    this->f_flags &= ~0x40000u;
    this->m_attachPrev = nullptr;
    this->m_attachNext = nullptr;
    this->m_attachParent = nullptr;
    this->m_attachmentId = static_cast<uint32_t>(-1);
    // this->dword174 = 0;
    this->m_refCount--;
    if (this->m_refCount == 0) {
        delete this;
        ObjectFree(*g_modelPool, this->m_handle);
    }
}

void CM2Model::DetachAllChildrenById(uint32_t id) {
    CM2Model* attachmentNext = nullptr;

    auto attachmentBase = this->m_attachList;
    if (attachmentBase) {
        do {
            attachmentNext = attachmentBase->m_attachNext;
            auto v8 = attachmentNext;
            if (attachmentBase->m_attachmentId == id) {
                auto attachmentPrev = attachmentBase->m_attachPrev;
                if (attachmentPrev) {
                    *attachmentPrev = attachmentBase->m_attachNext;
                }
                auto v5 = attachmentBase->m_attachNext;
                if (v5) {
                    v5->m_attachPrev = attachmentBase->m_attachPrev;
                }
                attachmentBase->f_flags &= ~0x40000u;
                attachmentBase->m_attachPrev = nullptr;
                attachmentBase->m_attachNext = nullptr;
                attachmentBase->m_attachParent = nullptr;
                attachmentBase->m_attachmentId = -1;
                // attachmentBase->dword174 = 0;
                if (--attachmentBase->m_refCount == 0) {
                    attachmentBase->~CM2Model();
                    ObjectFree(*g_modelPool, attachmentBase->m_handle);
                    attachmentNext = v8;
                }
            }
            attachmentBase = attachmentNext;
        } while (attachmentNext);
    }
}

C44Matrix CM2Model::GetAttachmentWorldTransform(uint32_t attachmentId) {
    if (!this->m_loaded) {
        this->WaitForLoad(nullptr);
    }

    uint16_t attachmentIndex = 0xFFFF;

    auto data = this->m_shared->m_data;
    if (attachmentId < data->attachmentIndicesById.Count()) {
        attachmentIndex = data->attachmentIndicesById[attachmentId];
    }

    uint16_t boneIndex = 0xFFFF;
    if (attachmentIndex < data->attachments.Count()) {
        boneIndex = data->attachments[attachmentIndex].boneIndex;
    }

    this->Animate();

    C44Matrix matrix;

    if (attachmentIndex == 0xFFFF) {
        matrix = this->m_boneMatrices[0];
    } else {
        matrix = this->m_boneMatrices[boneIndex];
        matrix.Translate(data->attachments[attachmentIndex].position);
    }

    return matrix * this->m_scene->m_viewInv;
}

void CM2Model::FindKey(M2ModelBoneSeq* sequence, const M2TrackBase& track, uint32_t& currentKey, uint32_t& nextKey, float& ratio) {
    if (!track.sequenceTimes.Count()) {
        nextKey = 0;
        currentKey = 0;
        ratio = 0.0f;

        return;
    }

    uint32_t v6 = sequence->m_currentTime;
    uint32_t v7 = sequence->m_animIndex;

    if (track.loopIndex == 0xFFFF) {
        if (v7 >= track.sequenceTimes.Count()) {
            v7 = 0;
        }
    } else {
        v6 = this->m_loops[track.loopIndex];
        v7 = 0;
    }

    uint32_t v12 = track.sequenceTimes[v7].times.Count();

    if (v12 <= 1) {
        nextKey = 0;
        currentKey = 0;
        ratio = 0.0f;

        return;
    }

    if (currentKey >= v12) {
        currentKey = 0;
    }

    uint32_t v15 = currentKey;
    auto& v24 = track.sequenceTimes[v7];
    auto v14 = v24.times.Data();
    auto v16 = v6 - v14[currentKey];
    uint32_t* v17;
    uint32_t* v18;
    uint32_t* v19;
    uint32_t v20;
    uint32_t v21;

    if (v16 >= 500) {
        if (v16 < 0xFFFFFE0C) {
            v15 = 0;

            if (v6 >= 500) {
                v20 = v12;

                while (1) {
                    v21 = (v20 + v15) >> 1;

                    if (v6 >= v14[v21]) {
                        v15 = v21 + 1;

                        if (v21 + 1 >= v12 || v6 < v14[v21 + 1]) {
                            v15 = v21;
                            goto LABEL_36;
                        }
                    } else {
                        v20 = v21 - 1;
                    }

                    if (v15 >= v20) {
                        goto LABEL_36;
                    }
                }
            }

            v19 = v14 + 1;

            do {
                if (*v19 > v6) {
                    break;
                }

                ++v15;
                ++v19;
            } while (v15 < v12 - 1);
        } else if (v15) {
            v18 = &v14[v15];

            do {
                if (*v18 <= v6) {
                    break;
                }

                --v15;
                --v18;
            } while (v15);
        }
    } else if (v15 < v12 - 1) {
        v17 = &v14[v15 + 1];

        do {
            if (*v17 > v6) {
                break;
            }

            ++v15;
            ++v17;
        } while (v15 < v12 - 1);
    }

LABEL_36:

    if (v15 + 1 >= v24.times.Count()) {
        nextKey = v15;
        currentKey = v15;
        ratio = 0.0f;
    } else {
        currentKey = v15;
        nextKey = v15 + 1;

        uint32_t* v22 = &v24.times[v15];
        float v23 = static_cast<float>(v6 - v22[0]);
        float v25 = static_cast<float>(v22[1] - v22[0]);
        ratio = v23 / v25;
    }
}

CAaBox& CM2Model::GetBoundingBox() {
    if (!this->m_shared->m_m2DataLoaded)
        this->WaitForLoad(nullptr);

    return this->m_shared->m_data->bounds.extent;
}

CAaSphere& CM2Model::GetBoundingSphere() {
    if (!this->m_shared->m_m2DataLoaded)
        this->WaitForLoad(nullptr);

    const M2Bounds& b = this->m_shared->m_data->bounds;

    CAaSphere out;
    out.c.x = (b.extent.b.x + b.extent.t.x) * 0.5f;
    out.c.y = (b.extent.b.y + b.extent.t.y) * 0.5f;
    out.c.z = (b.extent.b.z + b.extent.t.z) * 0.5f;
    out.r = b.radius;
    return out;
}

HCAMERA CM2Model::GetCameraByIndex(uint32_t index) {
    if (!this->m_loaded) {
        this->WaitForLoad("GetCameraByIndex");
    }

    return this->m_cameras[index].m_camera;
}

C3Vector CM2Model::GetPosition() {
    return reinterpret_cast<C3Vector&>(this->matrixF4.d0) * this->m_scene->m_viewInv;
}

// OFFSET: 0x834810
int32_t CM2Model::Initialize(CM2Scene* scene, CM2Shared* shared, CM2Model* a4, uint32_t flags) {
    this->AttachToScene(scene);
    this->m_lastEmitterTime = this->m_scene->m_time;
    this->m_shared = shared;
    this->m_shared->AddRef();
    this->model30 = a4;

    if (a4) {
        a4->m_refCount++;
    }

    this->m_flags = flags;
    this->m_modelCallTail = &this->m_modelCallList;
    this->m_loopOrigin = this->m_scene->m_time;

    if (this->model30) {
        for (CM2Model* i = this->model30->m_attachList; i; i = i->m_attachNext) {
            CM2Model* v9 = this->m_scene->DuplicateModel(i, 0);
            if (v9) {
                v9->AttachToParent(this, i->m_attachmentId, nullptr, 0);
                v9->m_refCount--;
                if (v9->m_refCount == 0) {
                    v9->~CM2Model();
                    ObjectFree(*g_modelPool, v9->m_handle);
                }
            }
        }
    }

    return this->m_shared->CallbackWhenLoaded(this);
}

// OFFSET: 0x832EA0
int32_t CM2Model::InitializeLoaded() {
    if (!this->m_shared->m_m2DataLoaded || !this->m_shared->m_skinProfileLoaded) {
        return 1;
    }

    uint32_t emitterBytes = 0;

    for (uint32_t i = 0; i < this->m_shared->m_data->particles.Count(); i++) {
        uint8_t emitterType = this->m_shared->m_data->particles[i].emitterType;

        if (emitterType == 1) {
            emitterBytes += sizeof(CPlaneParticleEmitter);
        } else if (emitterType == 2) {
            emitterBytes += sizeof(CSphereParticleEmitter);
        } else if (emitterType == 3) {
            emitterBytes += sizeof(CSplineParticleEmitter);
        }
    }

    uint32_t dataSize
        = (sizeof(uint32_t) * this->m_shared->m_skinData->skinSections.Count())
        + (sizeof(HTEXTURE) * this->m_shared->m_data->textures.Count())
        + (sizeof(uint32_t) * this->m_shared->m_data->loops.Count())
        + (sizeof(M2ModelBone) * this->m_shared->m_data->bones.Count())
        + (sizeof(M2ModelColor) * this->m_shared->m_data->colors.Count())
        + (sizeof(M2ModelTextureWeight) * this->m_shared->m_data->textureWeights.Count())
        + (sizeof(M2ModelTextureTransform) * this->m_shared->m_data->textureTransforms.Count())
        + (sizeof(C44Matrix) * this->m_shared->m_data->textureTransforms.Count())
        + (sizeof(M2ModelAttachment) * this->m_shared->m_data->attachments.Count())
        + (sizeof(M2ModelLight) * this->m_shared->m_data->lights.Count())
        + (sizeof(M2ModelCamera) * this->m_shared->m_data->cameras.Count())
        + (sizeof(M2ModelRibbon) * this->m_shared->m_data->ribbons.Count())
        + (sizeof(CRibbonEmitter*) * this->m_shared->m_data->ribbons.Count())
        + (sizeof(M2ModelParticle) * this->m_shared->m_data->particles.Count())
        + (sizeof(CParticleEmitter2*) * this->m_shared->m_data->particles.Count());

    // TODO
    // allocate space for particles and ribbons

    uint32_t dataRibbonSize = dataSize + (sizeof(CRibbonEmitter) * this->m_shared->m_data->ribbons.Count());
    char* data = static_cast<char*>(SMemAlloc(dataRibbonSize + emitterBytes, __FILE__, __LINE__, 0));
    char* emitterCursor = data + dataRibbonSize;

    if (this->m_shared->m_data->bones.Count()) {
        this->m_bones = reinterpret_cast<M2ModelBone*>(&data[0]);
        data += (sizeof(M2ModelBone) * this->m_shared->m_data->bones.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->bones.Count(); i++) {
            new (&this->m_bones[i]) M2ModelBone();
        }

        for (int32_t i = 0; i < this->m_shared->m_data->bones.Count(); i++) {
            this->m_bones[i].m_flags = this->m_shared->m_data->bones[i].flags;
        }

        this->m_boneMatrices = static_cast<C44Matrix*>(SMemAlignedAlloc(sizeof(C44Matrix) * this->m_shared->m_data->bones.Count(), __FILE__, __LINE__));

        for (int32_t i = 0; i < this->m_shared->m_data->bones.Count(); i++) {
            new (&this->m_boneMatrices[i]) C44Matrix();
        }
    }

    if (this->m_shared->m_data->loops.Count()) {
        this->m_loops = reinterpret_cast<uint32_t*>(&data[0]);
        data += (sizeof(uint32_t) * this->m_shared->m_data->loops.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->loops.Count(); i++) {
            if (this->m_loops[i]) {
                this->m_loops[i] = 0;
            }
        }
    }

    if (this->m_shared->m_skinData->skinSections.Count()) {
        this->m_skinSections = reinterpret_cast<uint32_t*>(&data[0]);
        data += (sizeof(uint32_t) * this->m_shared->m_skinData->skinSections.Count());

        if (this->model30) {
            memcpy(
                this->m_skinSections,
                this->model30->m_skinSections,
                sizeof(uint32_t) * this->m_shared->m_skinData->skinSections.Count());
        } else {
            for (uint32_t i = 0; i < this->m_shared->m_skinData->skinSections.Count(); ++i) {
                this->m_skinSections[i] = 1;
            }
        }
    }

    if (this->m_shared->m_data->colors.Count()) {
        this->m_colors = reinterpret_cast<M2ModelColor*>(&data[0]);
        data += (sizeof(M2ModelColor) * this->m_shared->m_data->colors.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->colors.Count(); i++) {
            new (&this->m_colors[i]) M2ModelColor();
        }
    }

    if (this->m_shared->m_data->textures.Count()) {
        this->m_textures = reinterpret_cast<HTEXTURE*>(&data[0]);
        data += (sizeof(HTEXTURE) * this->m_shared->m_data->textures.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->textures.Count(); i++) {
            HTEXTURE textureHandle = this->model30
                ? this->model30->m_textures[i]
                : this->m_shared->textures[i];

            this->m_textures[i] = textureHandle
                ? HandleDuplicate(textureHandle)
                : nullptr;
        }
    }

    if (this->m_shared->m_data->textureWeights.Count()) {
        this->m_textureWeights = reinterpret_cast<M2ModelTextureWeight*>(&data[0]);
        data += (sizeof(M2ModelTextureWeight) * this->m_shared->m_data->textureWeights.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->textureWeights.Count(); i++) {
            new (&this->m_textureWeights[i]) M2ModelTextureWeight();
        }
    }

    if (this->m_shared->m_data->textureTransforms.Count()) {
        this->m_textureTransforms = reinterpret_cast<M2ModelTextureTransform*>(&data[0]);
        data += (sizeof(M2ModelTextureTransform) * this->m_shared->m_data->textureTransforms.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->textureTransforms.Count(); i++) {
            new (&this->m_textureTransforms[i]) M2ModelTextureTransform();
        }

        this->m_textureMatrices = reinterpret_cast<C44Matrix*>(SMemAlignedAlloc(sizeof(C44Matrix) * this->m_shared->m_data->textureTransforms.Count(), __FILE__, __LINE__));
        if (!this->m_textureMatrices)
            return 0;
    }

    if (this->m_shared->m_data->attachments.Count()) {
        this->m_attachments = reinterpret_cast<M2ModelAttachment*>(&data[0]);
        data += (sizeof(M2ModelAttachment) * this->m_shared->m_data->attachments.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->attachments.Count(); i++) {
            new (&this->m_attachments[i]) M2ModelAttachment();
        }

        for (int32_t i = 0; i < this->m_shared->m_data->attachments.Count(); i++) {
            this->m_attachments[i].visibilityTrack.currentValue = 1;
        }
    }

    if (this->m_shared->m_data->lights.Count()) {
        this->m_lights = reinterpret_cast<M2ModelLight*>(&data[0]);
        data += (sizeof(M2ModelLight) * this->m_shared->m_data->lights.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->lights.Count(); i++) {
            new (&this->m_lights[i]) M2ModelLight();

            auto& light = this->m_shared->m_data->lights[i];
            auto& modelLight = this->m_lights[i];

            modelLight.light.Initialize(this->m_scene);
            modelLight.light.SetLightType(static_cast<M2LIGHTTYPE>(light.lightType));
            modelLight.ambientIntensityTrack.currentValue = 1.0f;
            modelLight.diffuseIntensityTrack.currentValue = 1.0f;
            modelLight.visibilityTrack.currentValue = 1;
        }
    }

    if (this->m_shared->m_data->cameras.Count()) {
        this->m_cameras = reinterpret_cast<M2ModelCamera*>(&data[0]);
        data += (sizeof(M2ModelCamera) * this->m_shared->m_data->cameras.Count());

        for (int32_t i = 0; i < this->m_shared->m_data->cameras.Count(); i++) {
            new (&this->m_cameras[i]) M2ModelCamera();
        }

        for (int32_t i = 0; i < this->m_shared->m_data->cameras.Count(); i++) {
            auto& camera = this->m_shared->m_data->cameras[i];
            auto cameraHandle = CameraCreate();

            if (camera.fieldOfView <= 0.0f || camera.fieldOfView >= 3.1415927f || camera.farClip <= camera.nearClip) {
                break;
            }

            DataMgrSetFloat(cameraHandle, 4, camera.fieldOfView);
            DataMgrSetFloat(cameraHandle, 3, camera.nearClip);
            DataMgrSetFloat(cameraHandle, 2, camera.farClip);

            this->m_cameras[i].m_camera = cameraHandle;
        }
    }

    if (this->m_shared->m_data->particles.Count()) {
        this->m_particles = reinterpret_cast<M2ModelParticle*>(data);
        data += sizeof(M2ModelParticle) * this->m_shared->m_data->particles.Count();

        for (uint32_t i = 0; i < this->m_shared->m_data->particles.Count(); i++) {
            new (&this->m_particles[i]) M2ModelParticle();
        }

        this->m_particleEmitters = reinterpret_cast<CParticleEmitter2**>(data);
        data += sizeof(CParticleEmitter2*) * this->m_shared->m_data->particles.Count();

        memset(this->m_particleEmitters, 0, sizeof(CParticleEmitter2*) * this->m_shared->m_data->particles.Count());

        CParticleMaterial material;
        material.blend = 0;
        material.flags = 0;

        for (uint32_t i = 0; i < this->m_shared->m_data->particles.Count(); i++) {
            auto& particle = this->m_shared->m_data->particles[i];
            auto& modelParticle = this->m_particles[i];

            CParticleEmitter2* emitter = nullptr;

            if (particle.emitterType == 1) {
                emitter = new (emitterCursor) CPlaneParticleEmitter();
                emitterCursor += sizeof(CPlaneParticleEmitter);
            } else if (particle.emitterType == 2) {
                emitter = new (emitterCursor) CSphereParticleEmitter();
                emitterCursor += sizeof(CSphereParticleEmitter);
            } else if (particle.emitterType == 3) {
                emitter = new (emitterCursor) CSplineParticleEmitter();
                emitterCursor += sizeof(CSplineParticleEmitter);
            }

            this->m_particleEmitters[i] = emitter;

            emitter->m_flags &= ~0x1u;

            if (particle.emissionRateTrack.sequenceKeys[0].keys.Count()) {
                emitter->SetEmissionRate(particle.emissionRateTrack.sequenceKeys[0].keys[0]);
            }

            emitter->m_emissionRateVariation = particle.emissionRateVariation;

            if (particle.speedTrack.sequenceKeys[0].keys.Count()) {
                emitter->m_speed = particle.speedTrack.sequenceKeys[0].keys[0];
            }

            if (particle.variationTrack.sequenceKeys[0].keys.Count()) {
                emitter->m_variation = particle.variationTrack.sequenceKeys[0].keys[0];
            }

            if (particle.latitudeTrack.sequenceKeys[0].keys.Count()) {
                emitter->SetLatitude(particle.latitudeTrack.sequenceKeys[0].keys[0]);
            }

            if (particle.longitudeTrack.sequenceKeys[0].keys.Count()) {
                emitter->SetLongitude(particle.longitudeTrack.sequenceKeys[0].keys[0]);
            }

            if (particle.gravityTrack.sequenceKeys[0].keys.Count()) {
                emitter->m_gravity = particle.gravityTrack.sequenceKeys[0].keys[0];
            }

            if (particle.lifeTrack.sequenceKeys[0].keys.Count()) {
                emitter->m_life = particle.lifeTrack.sequenceKeys[0].keys[0];
            }

            emitter->m_lifeVariation = particle.lifeVariation;

            if (particle.widthTrack.sequenceKeys[0].keys.Count()) {
                emitter->SetWidth(particle.widthTrack.sequenceKeys[0].keys[0]);
            }

            if (particle.lengthTrack.sequenceKeys[0].keys.Count()) {
                emitter->SetHeight(particle.lengthTrack.sequenceKeys[0].keys[0]);
            }

            if (particle.zsourceTrack.sequenceKeys[0].keys.Count()) {
                emitter->SetZsource(particle.zsourceTrack.sequenceKeys[0].keys[0]);
            }

            if (particle.flags & 0x10) {
                emitter->m_flags |= 0x200;
            } else {
                emitter->m_flags &= ~0x200u;
            }

            if (particle.flags & 0x40) {
                emitter->m_flags |= 0x800;
            }

            if (particle.flags & 0x20) {
                emitter->m_flags |= 0x400;
            }

            if (particle.flags & 0x800) {
                emitter->m_flags |= 0x2000;
            }

            if (particle.flags & 0x1000) {
                emitter->m_flags |= 0x4000;
            }

            if (particle.emitterType == 2) {
                if (particle.flags & 0x80000000) {
                    emitter->m_flags |= 0x1000;
                }

                if (particle.flags & 0x100) {
                    emitter->m_flags |= 0x8000;
                }
            }

            if (particle.flags & 0x200) {
                emitter->m_flags |= 0x10000;
            }

            if (particle.flags & 0x2000) {
                emitter->m_flags |= 0x40000;
            }

            if (particle.flags & 0x4000) {
                emitter->m_flags |= 0x80000;
            }

            if (particle.flags & 0x8000) {
                emitter->m_flags &= ~0x1u;
            }

            if (particle.flags & 0x80000) {
                emitter->m_flags |= 0x800000;
            }

            emitter->SetParticleStyle(particle.flags & 0x20000, particle.flags & 0x40000, particle.tailLength, particle.flags & 0x400);

            material.flags |= 0x7;
            material.blend = 0;

            if (particle.blendMode == 0) {
                material.blend = 0;
                material.flags |= 0x4;
            } else if (particle.blendMode == 1) {
                material.blend = 1;
                material.flags |= 0x4;
            } else if (particle.blendMode == 2) {
                material.blend = 2;
                material.flags &= ~0x4u;
            } else if (particle.blendMode == 3) {
                material.blend = 10;
                material.flags &= ~0x4u;
            } else if (particle.blendMode == 4) {
                material.blend = 3;
                material.flags &= ~0x4u;
            } else if (particle.blendMode == 5) {
                material.blend = 4;
                material.flags &= ~0x4u;
            } else if (particle.blendMode == 6) {
                material.blend = 5;
                material.flags &= ~0x4u;
            }

            material.flags ^= (material.flags ^ ~particle.flags) & 0x1;
            material.flags ^= (material.flags ^ ~(particle.flags >> 2)) & 0x2;

            if (particle.flags & 0x2) {
                emitter->m_flags |= 0x20;
            }

            if (particle.flags & 0x4) {
                emitter->m_flags |= 0x200000;
            }

            emitter->SetTextureDimensions(particle.rows, particle.cols);

            if (particle.flags & 0x10000) {
                emitter->SetChooseRandomTexture(1);
            }

            emitter->SetMaterial(&material, this->m_shared->textures[particle.textureIndex]);

            emitter->m_colorTrack = &particle.colorTrack;
            emitter->m_alphaTrack = &particle.alphaTrack;
            emitter->m_scaleTrack = &particle.scaleTrack;
            emitter->m_scaleVariationX = particle.scaleVariation.x;
            emitter->m_scaleVariationY = particle.scaleVariation.y;
            emitter->m_headCellTrack = &particle.headCellTrack;
            emitter->m_tailCellTrack = &particle.tailCellTrack;

            if (this->model30 && (this->model30->m_particleEmitters[i]->m_flags & 0x10)) {
                CImVector replacement0 = {};
                CImVector replacement1 = {};
                CImVector replacement2 = {};

                this->model30->m_particleEmitters[i]->GetReplacementColors(&replacement0, &replacement1, &replacement2);
                emitter->SetParticleColors(&replacement0, &replacement1, &replacement2);
            }

            emitter->m_twinkleFPS = particle.twinkleFPS;
            emitter->m_twinkleOnOff = particle.twinkleOnOff;
            emitter->SetTwinkleScale(particle.twinkleScale);
            emitter->m_ivelScale = particle.ivelScale;

            emitter->m_tumbleBaseX = particle.tumble.b.x;
            emitter->m_tumbleSpanX = particle.tumble.t.x - particle.tumble.b.x;
            emitter->m_tumbleBaseY = particle.tumble.b.y;
            emitter->m_tumbleSpanY = particle.tumble.t.y - particle.tumble.b.y;
            emitter->m_tumbleBaseZ = particle.tumble.b.z;
            emitter->m_tumbleSpanZ = particle.tumble.t.z - particle.tumble.b.z;

            emitter->m_drag = particle.drag;
            emitter->m_initialSpin = particle.initialSpin;
            emitter->m_initialSpinVariation = particle.initialSpinVariation;
            emitter->m_spinVariation = particle.spinVariation;
            emitter->m_spin = particle.spin;

            emitter->m_windVectorX = particle.windVector.x;
            emitter->m_windVectorY = particle.windVector.y;
            emitter->m_windVectorZ = particle.windVector.z;
            emitter->m_windTime = particle.windTime;

            emitter->SetFollowParams(particle.followSpeed1, particle.followScale1, particle.followSpeed2, particle.followScale2);

            emitter->m_priorityPlane = particle.priorityPlane;

            if (particle.emitterType == 3) {
                static_cast<CSplineParticleEmitter*>(emitter)->SetSpline(particle.spline.Data(), particle.spline.Count());
            }

            char* geometryMdl = particle.geometryMdl.Data();

            if (geometryMdl && geometryMdl[0]) {
                emitter->SetModel(this->m_scene, geometryMdl);
            }

            char* recursionMdl = particle.recursionMdl.Data();

            if (recursionMdl && recursionMdl[0]) {
                emitter->CreateChildEmittersFromModel(this->m_scene, recursionMdl);
            }

            emitter->DetermineIfSimple();

            modelParticle.visibilityTrack.currentValue = 1;
        }
    }

    this->m_loaded = 1;

    uint32_t savedTime = this->m_scene->m_time;

    while (this->m_modelCallList) {
        auto modelCall = this->m_modelCallList;

        this->m_scene->m_time = modelCall->time;

        switch (modelCall->type) {
            case 0: {
                this->ReplaceTexture(
                    modelCall->replaceTexture.textureId,
                    modelCall->replaceTexture.texture);
                break;
            }

            case 1: {
                this->SetGeometryVisible(
                    modelCall->setGeometryVisible.start,
                    modelCall->setGeometryVisible.end,
                    modelCall->setGeometryVisible.visible);
                break;
            }

            case 2: {
                // TODO
                break;
            }

            case 3: {
                // TODO
                break;
            }

            case 4: {
                // TODO
                break;
            }

            case 5: {
                this->SetBoneSequence(
                    modelCall->setBoneSequence.boneId,
                    modelCall->setBoneSequence.sequenceId,
                    modelCall->setBoneSequence.variationIndex,
                    modelCall->setBoneSequence.time,
                    modelCall->setBoneSequence.blendTime,
                    modelCall->setBoneSequence.a7,
                    modelCall->setBoneSequence.a8);
                break;
            }

            case 6: {
                // TODO
                break;
            }

            case 7: {
                // TODO
                break;
            }

            case 8: {
                // TODO
                break;
            }

            case 9: {
                // TODO
                break;
            }

            case 10: {
                // TODO
                break;
            }

            case 11: {
                // TODO
                break;
            }

            case 12: {
                // TODO
                break;
            }

            case 13: {
                // TODO
                break;
            }

            case 14: {
                // TODO
                break;
            }
        }

        this->m_modelCallList = modelCall->modelCallNext;

        if (modelCall->type == 0 && modelCall->replaceTexture.texture) {
            HandleClose(modelCall->replaceTexture.texture);
        }

        SMemFree(modelCall);
    }

    this->m_scene->m_time = savedTime;

    this->UpdateLoaded();
    this->m_flag800 = 0;

    return 1;
}

int32_t CM2Model::IsBatchDoodadCompatible(M2Batch* batch) {
    // TODO

    return 0;
}

int32_t CM2Model::IsDrawable(int32_t a2, int32_t a3) {
    if (!this->m_loaded && a2) {
        this->WaitForLoad(nullptr);
    }

    if (!this->m_flag2) {
        if (!this->m_loaded) {
            return 0;
        }

        for (uint32_t i = 0; i < this->m_shared->m_data->textures.Count(); i++) {
            auto texture = this->m_textures[i];

            if (!texture) {
                continue;
            }

            if (!TextureGetGxTex(texture, a2, nullptr)) {
                return 0;
            }
        }

        this->m_flag2 = 1;
    }

    if (!this->m_flag200 && a3) {
        // TODO
    }

    return 1;
}

int32_t CM2Model::IsLoaded(int32_t a2, int32_t attachments) {
    if (this->m_flags & 0x20) {
        if (this->m_loaded) {
            return 1;
        }

        if (a2) {
            this->WaitForLoad(nullptr);
        }

        return this->m_loaded || (this->m_shared->m_m2DataLoaded && this->m_shared->m_skinProfileLoaded);
    }

    if (!this->m_loaded && a2) {
        this->WaitForLoad(nullptr);
    }

    if (!this->m_loaded) {
        return 0;
    }

    if (!attachments || this->m_flag100) {
        return 1;
    }

    // TODO

    return 0;
}

void CM2Model::LinkToCallbackListTail() {
    this->m_callbackPrev = this->m_shared->m_callbackListTail;
    this->m_callbackNext = nullptr;
    *this->m_shared->m_callbackListTail = this;
    this->m_shared->m_callbackListTail = &this->m_callbackNext;
}

int32_t CM2Model::ProcessCallbacks() {
    // TODO
    return 1;
}

void CM2Model::ProcessCallbacksRecursive() {
    if (!this->m_loaded) {
        return;
    }

    this->m_refCount++;

    if (this->ProcessCallbacks()) {
        // TODO process attachments
    }

    this->Release();
}

// OFFSET: 0x824ED0
void CM2Model::Release() {
    this->m_refCount--;
    if (this->m_refCount == 0) {
        uint32_t handle = this->m_handle;
        this->~CM2Model();
        ObjectFree(*g_modelPool, handle);
    }
}

// OFFSET: 0x823F10
void CM2Model::SetAnimating(int32_t animating) {
    if ((this->m_flags & 0x20) != 0) {
        if (!animating) {
            if (!this->m_animatePrev) {
                this->m_animatePrev = &this->m_scene->m_animateList;
                this->m_animateNext = this->m_scene->m_animateList;
                this->m_scene->m_animateList = this;

                if (this->m_animateNext) {
                    this->m_animateNext->m_animatePrev = &this->m_animateNext;
                }
            }
        }
        if ((this->f_flags & 1) == 0)
            this->WaitForLoad(nullptr);
    }

    if (animating) {
        if (!this->m_animatePrev) {
            this->m_animatePrev = &this->m_scene->m_animateList;
            this->m_animateNext = this->m_scene->m_animateList;
            this->m_scene->m_animateList = this;

            if (this->m_animateNext) {
                this->m_animateNext->m_animatePrev = &this->m_animateNext;
            }
        }

        return;
    }

    if (this->m_animatePrev) {
        *this->m_animatePrev = this->m_animateNext;

        if (this->m_animateNext) {
            this->m_animateNext->m_animatePrev = this->m_animatePrev;
        }

        this->m_animatePrev = nullptr;
        this->m_animateNext = nullptr;
    }
}

void CM2Model::SetBoneSequence(uint32_t boneId, uint32_t sequenceId, uint32_t a4, uint32_t time, float a6, int32_t a7, int32_t a8) {
    if (sequenceId == -1) {
        this->UnsetBoneSequence(boneId, a7, a8);
        return;
    }

    if (!this->m_loaded) {
        auto m = SMemAlloc(sizeof(CM2ModelCall), __FILE__, __LINE__, 0x0);
        auto modelCall = new (m) CM2ModelCall();

        modelCall->type = 5;
        modelCall->modelCallNext = nullptr;
        modelCall->time = this->m_scene->m_time;

        modelCall->setBoneSequence.boneId = boneId;
        modelCall->setBoneSequence.sequenceId = sequenceId;
        modelCall->setBoneSequence.variationIndex = a4;
        modelCall->setBoneSequence.time = time;
        modelCall->setBoneSequence.blendTime = a6;
        modelCall->setBoneSequence.a7 = a7;
        modelCall->setBoneSequence.a8 = a8;

        *this->m_modelCallTail = modelCall;
        this->m_modelCallTail = &modelCall->modelCallNext;

        return;
    }

    if (this->m_flag800) {
        a7 = 0;
    }

    uint16_t boneIndex;
    if (boneId == -1) {
        boneIndex = 0;
    } else if (boneId < this->m_shared->m_data->boneIndicesById.Count()) {
        boneIndex = this->m_shared->m_data->boneIndicesById[boneId];
    } else {
        boneIndex = -1;
    }

    if (boneIndex >= this->m_shared->m_data->bones.Count()) {
        return;
    }

    M2SequenceFallback fallback;
    this->SequenceFallbackById(fallback, sequenceId);
    int32_t v33 = a4 == -1;

    uint16_t v15 = CM2Model::Sub8260C0(this->m_shared->m_data, fallback.uint0, a4 != -1 ? a4 : 0);
    uint32_t v16 = v15;
    uint32_t v32 = v15;
    uint32_t v17;

    if (v15 != 0xFFFF) {
        if (!v33) {
            goto LABEL_30;
        }

        goto LABEL_29;
    }

    v17 = this->m_shared->m_data->sequenceIdxHashById.Count();
    v33 = 1;
    v32 = v17;
    uint16_t v18;

    if (v17) {
        uint32_t v20 = fallback.uint0 % v17;
        v18 = this->m_shared->m_data->sequenceIdxHashById[v20];

        if (v18 != 0xFFFF) {
            uint32_t v21 = 1;

            if (this->m_shared->m_data->sequences[v18].id != fallback.uint0) {
                while (1) {
                    v20 = (v20 + v21 * v21) % v32;
                    v18 = this->m_shared->m_data->sequenceIdxHashById[v20];

                    if (v18 == 0xFFFF) {
                        break;
                    }

                    ++v21;

                    if (this->m_shared->m_data->sequences[v18].id == fallback.uint0) {
                        goto LABEL_26;
                    }
                }

                v32 = 0xFFFF;

                goto LABEL_29;
            }

            goto LABEL_26;
        }

        v32 = 0xFFFF;
    } else {
        v18 = 0;

        if (this->m_shared->m_data->sequences.Count())  {
            while (this->m_shared->m_data->sequences[v18].id != fallback.uint0) {
                ++v18;

                if (v18 >= this->m_shared->m_data->sequences.Count()) {
                    goto LABEL_20;
                }
            }

LABEL_26:
            v32 = v18;
            goto LABEL_29;
        }

LABEL_20:
        v32 = 0xFFFF;
    }

LABEL_29:
    this->Sub826E60(&a4, &v32);
    v16 = v32;

LABEL_30:
    if (this->m_shared->m_data->sequences[v16].flags & 0x20) {
        if (this->Sub8269C0(boneId, boneIndex)) {
            this->CancelDeferredSequences(boneIndex, a8 != 0);

            auto& modelBone = this->m_bones[boneIndex];

            if (a8) {
                modelBone.m_sequenceId = sequenceId;
                modelBone.m_variationIndex = a4;

                this->SetPrimaryBoneSequence(v16, boneIndex, fallback, time, a6, a7);
                modelBone.sequence.m_pickRandomVariation = v33;
            } else {
                this->SetSecondaryBoneSequence(v16, boneIndex, fallback, time, a6);
                modelBone.secondarySequence.m_pickRandomVariation = v33;
            }
        }
    } else {
        this->SetBoneSequenceDeferred(v16, this->m_shared->m_data, boneIndex, time, a6, fallback, a7, a8, v33);
    }
}

// OFFSET: 0x831C30
void CM2Model::SetBoneSequenceDeferred(uint16_t sequenceIndex, M2Data* data, uint16_t boneIndex, uint32_t time, float speed, M2SequenceFallback fallback, int32_t blend, int32_t primary, int32_t pickRandomVariation) {
    CM2SequencePlayback* playback = nullptr;

    if (data->sequences[sequenceIndex].flags & 0x10) {
        uint16_t id = sequenceIndex;

        while (true) {
            CM2SequenceLoad* load = this->m_shared->m_sequenceLoads.Head();

            while (load && load->m_sequenceIndex != id) {
                load = this->m_shared->m_sequenceLoads.Next(load);
            }

            if (load) {
                for (CM2SequencePlayback* p = load->m_playbacks.Head(); p; p = load->m_playbacks.Next(p)) {
                    if (p->m_model == this) {
                        playback = p;
                        break;
                    }
                }

                if (!playback) {
                    playback = load->m_playbacks.NewNode(STORM_LIST_TAIL, 0, 0);
                    playback->m_model = this;
                }

                break;
            }

            id = data->sequences[id].aliasNext;

            if (id == sequenceIndex) {
                return;
            }
        }
    } else {
        //CM2SequenceLoad* load = this->m_shared->LoadLowPrioritySequence(sequenceIndex);
        //
        //if (!load) {
        //    return;
        //}
        //
        //playback = load->m_playbacks.NewNode(STORM_LIST_TAIL, 0, 0);
        //playback->m_model = this;
    }

    playback->m_speed = speed;
    playback->m_boneIndex = boneIndex;
    playback->m_time = time;
    playback->m_fallback = fallback;
    playback->m_flags = 0;

    if (blend) {
        playback->m_flags = 1;
    }

    if (primary) {
        playback->m_flags |= 2;
    }

    if (pickRandomVariation) {
        playback->m_flags |= 4;
    }
}

// OFFSET: 0x825EE0
bool CM2Model::HasSequence(uint32_t sequenceId) {
    if ((this->f_flags & 1) == 0) {
        if (this->m_shared->asyncObject)
            AsyncFileReadWait(this->m_shared->asyncObject);
        m_shared = this->m_shared;
        if (m_shared->asyncObject)
            AsyncFileReadWait(m_shared->asyncObject);
        if ((this->m_flags & 0x20) != 0)
            this->InitializeLoaded();
    }
    return CM2Model::HasSequence(this->m_shared->m_data, sequenceId);
}

// OFFSET: 0x8267E0
uint32_t CM2Model::GetBoneSequenceId(uint32_t boneId) {
    if ((this->f_flags & 1) == 0) {
        if (this->m_shared->asyncObject)
            AsyncFileReadWait(this->m_shared->asyncObject);
        if (this->m_shared->asyncObject)
            AsyncFileReadWait(this->m_shared->asyncObject);
        if ((this->m_flags & 0x20) != 0)
            this->InitializeLoaded();
    }
    uint32_t v5 = 0xFFFFFFFF;
    if (boneId == 0xFFFFFFFF) {
        v5 = 0;
    } else if (boneId < this->m_shared->m_data->boneIndicesById.count) {
        v5 = this->m_shared->m_data->boneIndicesById[boneId];
    }
    if (v5 < this->m_shared->m_data->bones.count)
        return this->m_bones[v5].m_sequenceId;

    return 0;
}

void CM2Model::SetIndices() {
    // TODO
}

void CM2Model::SetLightingCallback(void (*lightingCallback)(CM2Model*, CM2Lighting*, void*), void* lightingArg) {
    this->m_lightingCallback = lightingCallback;
    this->m_lightingArg = lightingArg;
}

void CM2Model::SetLoadedCallback(void (*loadedCallback)(CM2Model*, void*), void* loadedArg) {
    this->m_loadedCallback = loadedCallback;
    this->m_loadedArg = loadedArg;

    this->UpdateLoaded();
}

void CM2Model::SetPrimaryBoneSequence(uint16_t sequenceIndex, uint16_t boneIndex, M2SequenceFallback fallback, uint32_t time, float a6, int32_t a7) {
    auto& modelBone = this->m_bones[boneIndex];
    auto& sequence = this->m_shared->m_data->sequences[sequenceIndex];

    if (a7) {
        if (!modelBone.sequence.m_finished || sequenceIndex != modelBone.sequence.m_sequenceIndex) {
            double v10;
            double v11;
            double v12;

            if (modelBone.secondarySequence.m_sequenceIndex == 0xFFFF
                || ((v10 = (double)(modelBone.m_blendEndTime - this->m_scene->m_time) * modelBone.m_invBlendDuration, v10 >= 0.0) ? (v10 <= 1.0 ? (v11 = v10 * ((3.0 - (v10 + v10)) * v10)) : (v11 = 1.0)) : (v11 = 0.0), v11 * modelBone.m_blendWeightMax <= 0.5)
            ) {
                memcpy(&modelBone.secondarySequence, &modelBone.sequence, sizeof(modelBone.secondarySequence));

                modelBone.m_blendEndTime = this->m_scene->m_time + sequence.blendtime;
                if (sequence.blendtime) {
                    v12 = 1.0 / (double)sequence.blendtime;
                } else {
                    v12 = 1.0;
                }
                modelBone.m_invBlendDuration = v12;
                modelBone.m_blendWeightMax = 1.0f;
            }
        }
    } else {
        modelBone.secondarySequence.m_sequenceIndex = -1;
    }

    this->SetupBoneSequence(sequenceIndex, fallback, time, a6, &modelBone.sequence);

    int32_t v13 = modelBone.sequence.m_endTime;
    if (modelBone.sequence.m_startTime == v13 || ((sequence.flags & 0x1) != 0 && (v13 -= this->m_scene->m_time, v13 <= 0))) {
        modelBone.sequence.m_finished = 1;
    }

    // TODO
    // if (!modelBone.dword98) {
    //     modelBone.dword98 = (DWORD)&this->dword14;
    //     v14 = this->dword14;
    //     v15 = &smodelBone.word96;
    //     *v15 = v14;
    //     if (v14 != 0xFFFF) {
    //         this->m_bones[v14].dword98 = v15;
    //     }
    //     LOWORD(v13) = a3;
    //     LOWORD(this->dword14) = a3;
    // }
}

void CM2Model::SetSecondaryBoneSequence(uint16_t a2, uint16_t boneIndex, M2SequenceFallback fallback, uint32_t time, float a6) {
    // TODO
}

void CM2Model::SetupBoneSequence(uint16_t sequenceIndex, M2SequenceFallback fallback, uint32_t a4, float a5, M2ModelBoneSeq* boneSequence) {
    auto& sequence = this->m_shared->m_data->sequences[sequenceIndex];

    int32_t v9 = rand();
    uint32_t v10 = (sequence.replay.l + (sequence.replay.h - sequence.replay.l) * v9 / 0x8000 == 0)
        + sequence.replay.l + (sequence.replay.h - sequence.replay.l) * v9 / 0x8000;
    int32_t v11 = v10 * sequence.duration;

    double v12;
    double v13;
    long double v15;

    if (fallback.uint2 == 1 || fallback.uint2 == 3) {
        v12 = -a5;
    } else {
        v12 = a5;
    }

    v13 = 0.0;

    uint32_t v18 = 0;
    if (v12 < 0.0) {
        v18 = v11;
    }

    if (fallback.uint2 == 2 || fallback.uint2 == 3) {
        v12 = 0.0;
    }

    if (abs(v12) > 0.0000099999997) {
        v13 = 1.0 / v12;
    }

    v15 = abs(v13);
    uint32_t v16 = this->m_scene->m_time - floor((double)a4 * v15);

    if ((~(this->m_scene->m_flags >> 2) & 0x1) != 0) {
        v16++;
    }

    boneSequence->m_sequenceIndex = sequenceIndex;
    boneSequence->m_startTime = v16;
    boneSequence->m_repeatCount = v10;
    boneSequence->m_finished = 0;
    boneSequence->m_endTime = v16 + floor(v15 * (double)(unsigned int)v11);
    boneSequence->m_startOffset = v18;
    boneSequence->m_speed = v12;
    boneSequence->m_invSpeed = v13;
}

void CM2Model::SetupLighting() {
    if (!this->m_attachParent || this->m_attachParent->m_flags & 0x1) {
        this->Animate();

        CAaSphere sphere;
        sphere.c = this->GetPosition();
        sphere.r = 0.0f;

        this->m_lighting.Initialize(this->m_scene, sphere);
        this->m_scene->SelectLights(&this->m_lighting);

        if (this->m_lightingCallback) {
            this->m_lightingCallback(this, &this->m_lighting, this->m_lightingArg);
        }

        this->m_lighting.SetupSunlight();
        this->m_lighting.CameraSpace();
    }

    for (auto model = this->m_attachList; model; model = model->m_attachNext) {
        model->SetupLighting();
    }
}

void CM2Model::SetVisible(int32_t visible) {
    if (this->m_attachParent) {
        this->m_flag80 = visible ? 1 : 0;
    } else {
        this->m_flag8 = visible ? 1 : 0;
    }
}

// OFFSET: 0x8251D0
void CM2Model::SetWorldTransform(const C3Vector& position, float orientation, float scale) {
    this->m_worldTransform = C44Matrix();

    this->m_worldTransform.RotateAroundZ(orientation);
    this->m_worldTransform.Scale(scale);
    this->m_worldTransform.d0 = position.x;
    this->m_worldTransform.d1 = position.y;
    this->m_worldTransform.d2 = position.z;

    this->m_flag8000 = 1;
}

// OFFSET: 0x826350
void CM2Model::SequenceFallbackById(M2SequenceFallback& fallback, uint32_t sequenceId) {
    auto data = this->m_shared->m_data;

    uint32_t defaultId;
    if (CM2Model::HasSequence(data, 0)) {
        defaultId = 0;
    } else if (CM2Model::HasSequence(data, 147)) {
        defaultId = 147;
    } else {
        defaultId = data->sequences[0].id;
    }

    if (CM2Model::HasSequence(data, sequenceId)) {
        fallback.uint0 = sequenceId;
        fallback.uint2 = 0;
        return;
    }

    bool visited[506] = {};

    uint32_t id = sequenceId;
    int32_t direction = 1;
    int32_t swaps = 0;

    do {
        auto rec = g_animationDataDB.GetRecord(id);

        if (id >= 506 || visited[id] || !rec || id == static_cast<uint32_t>(rec->m_fallback)) {
            fallback.uint0 = defaultId;
            fallback.uint2 = 0;
            return;
        }

        visited[id] = true;

        int32_t flags = rec->m_flags;
        id = rec->m_fallback;

        if (flags & 0x10) {
            swaps += direction;
            direction = -direction;
        }

        if (flags & 0x20) { 
            swaps += direction;
            direction = 0;
        }
    } while (!CM2Model::HasSequence(data, id));

    if (direction > 0) {
        fallback.uint0 = id;
        fallback.uint2 = 0;
        return;
    }

    fallback.uint0 = id;
    fallback.uint2 = (direction == 0) ? ((swaps > 0) ? 3 : 2) : 1;
}

int32_t CM2Model::Sub8269C0(uint32_t boneId, uint16_t boneIndex) {
    // TODO
    return 1;
}

void CM2Model::Sub826E60(uint32_t* a2, uint32_t* a3) {
    // TODO
}

// OFFSET: 0x824510
void CM2Model::UnlinkFromCallbackList() {
    if (this->m_callbackPrev) {
        *this->m_callbackPrev = this->m_callbackNext;

        if (this->m_callbackNext) {
            this->m_callbackNext->m_callbackPrev = this->m_callbackPrev;
        } else {
            this->m_shared->m_callbackListTail = this->m_callbackPrev;
        }

        this->m_callbackPrev = nullptr;
        this->m_callbackNext = nullptr;
    }
}

void CM2Model::UnsetBoneSequence(uint32_t boneId, int32_t a3, int32_t a4) {
    // TODO
}

void CM2Model::UpdateLoaded() {
    auto model = this;

    while (model) {
        if (!model->IsLoaded(0, !(this->m_flags & 0x20))) {
            break;
        }

        if (model->m_loadedCallback) {
            model->m_loadedCallback(this, model->m_loadedArg);
            model->m_loadedCallback = nullptr;
        }

        model = model->m_attachParent;
    }
}

// OFFSET: 0x823ED0
void CM2Model::WaitForLoad(const char* a2) {
    if (this->m_shared->asyncObject) {
        AsyncFileReadWait(this->m_shared->asyncObject);
    }

    if (this->m_shared->asyncObject) {
        AsyncFileReadWait(this->m_shared->asyncObject);
    }

    if (this->m_flags & 0x20) {
        this->InitializeLoaded();
    }
}

void CM2Model::UnoptimizeVisibleGeometry() {
    // TODO
}

void CM2Model::SetGeometryVisible(uint32_t start, uint32_t end, int32_t visible) {
    if (this->m_loaded) {
        bool needUpdate = false;

        const auto& skinSections = this->m_shared->m_skinData->skinSections;

        for (uint32_t i = 0; i < skinSections.Count(); ++i) {
            uint32_t id = skinSections[i].skinSectionId;
            if (start <= id && id <= end) {
                if (this->m_skinSections[i] != static_cast<uint32_t>(visible)) {
                    this->m_skinSections[i] = static_cast<uint32_t>(visible);
                    needUpdate = true;
                }
            }
        }

        if (needUpdate) {
            this->UnoptimizeVisibleGeometry();
        }

    } else {
        auto modelCall = NEW(CM2ModelCall);

        modelCall->type = 1;
        modelCall->modelCallNext = nullptr;
        modelCall->time = this->m_scene->m_time;

        modelCall->setGeometryVisible.start = start;
        modelCall->setGeometryVisible.end = end;
        modelCall->setGeometryVisible.visible = visible;

        *this->m_modelCallTail = modelCall;
        this->m_modelCallTail = &modelCall->modelCallNext;
    }
}

void CM2Model::ReplaceTexture(uint32_t textureId, HTEXTURE texture) {
    if ((this->f_flags & 0x1) == 0) {
        auto call = new (STORM_ALLOC(sizeof(CM2ModelCall))) CM2ModelCall();
        if (!call)
            return;

        call->type = 0;
        call->modelCallNext = nullptr;
        call->time = this->m_scene->m_time;

        call->replaceTexture.textureId = textureId;
        call->replaceTexture.texture = texture ? HandleDuplicate(texture) : nullptr;

        *this->m_modelCallTail = call;
        this->m_modelCallTail = &call->modelCallNext;
        return;
    }

    auto data = this->m_shared->m_data;

    for (uint32_t i = 0; i < data->textures.count; i++) {
        if (data->textures[i].textureId != textureId)
            continue;

        if (this->m_textures[i])
            HandleClose(this->m_textures[i]);

        if (!texture) {
            this->m_textures[i] = 0;
            continue;
        }

        this->m_textures[i] = HandleDuplicate(texture);
        if (!TextureGetGxTex(this->m_textures[i], 0, nullptr))
            this->f_flags &= ~0x2u;
    }

    // TODO ribbon and particle stuff

    this->f_flags &= ~0x10u;
}

// OFFSET: 0x82EC30
void CM2Model::GetCollisionFacets(CAaBox* box, C44Matrix* mat, TSGrowableArray<CFacet>* facets) {
    if (!this->m_loaded) {
        this->WaitForLoad(nullptr);
    }

    M2Data* data = this->m_shared->m_data;

    if (CM2Model::s_collisionPositions.Count() < data->collisionPositions.count) {
        CM2Model::s_collisionPositions.SetCount(data->collisionPositions.count);
    }

    if (CM2Model::s_collisionCodes.Count() < data->collisionPositions.count) {
        CM2Model::s_collisionCodes.SetCount(data->collisionPositions.count);
    }

    for (uint32_t i = 0; i < data->collisionPositions.count; i++) {
        C3Vector& position = CM2Model::s_collisionPositions[i];
        position = mat->TransformPoint(data->collisionPositions[i]);

        uint32_t code = 0;

        if (position.x < box->b.x) {
            code = 1;
        } else if (position.x > box->t.x) {
            code = 2;
        }

        if (position.y < box->b.y) {
            code |= 4;
        } else if (position.y > box->t.y) {
            code |= 8;
        }

        if (position.z < box->b.z) {
            code |= 0x10;
        } else if (position.z > box->t.z) {
            code |= 0x20;
        }

        CM2Model::s_collisionCodes[i] = code;
    }

    C33Matrix rotation(*mat);

    float lengthSq = rotation.a1 * rotation.a1 + rotation.a2 * rotation.a2 + rotation.a0 * rotation.a0;
    if (lengthSq > 0.00000023841858f) {
        float inverse = 1.0f / sqrtf(lengthSq);
        rotation.a0 *= inverse;
        rotation.a1 *= inverse;
        rotation.a2 *= inverse;
    }

    lengthSq = rotation.b2 * rotation.b2 + rotation.b1 * rotation.b1 + rotation.b0 * rotation.b0;
    if (lengthSq > 0.00000023841858f) {
        float inverse = 1.0f / sqrtf(lengthSq);
        rotation.b0 *= inverse;
        rotation.b1 *= inverse;
        rotation.b2 *= inverse;
    }

    lengthSq = rotation.c2 * rotation.c2 + rotation.c1 * rotation.c1 + rotation.c0 * rotation.c0;
    if (lengthSq > 0.00000023841858f) {
        float inverse = 1.0f / sqrtf(lengthSq);
        rotation.c0 *= inverse;
        rotation.c1 *= inverse;
        rotation.c2 *= inverse;
    }

    uint32_t total = facets->Count();

    for (uint32_t i = 0; i < data->collisionFaceNormals.count; i++) {
        uint32_t code = CM2Model::s_collisionCodes[data->collisionIndices[i * 3 + 1]] & CM2Model::s_collisionCodes[data->collisionIndices[i * 3 + 2]] & CM2Model::s_collisionCodes[data->collisionIndices[i * 3]];

        if (code == 0) {
            total++;
        }
    }

    facets->Reserve(total, 0);

    for (uint32_t i = 0; i < data->collisionFaceNormals.count; i++) {
        uint16_t i0 = data->collisionIndices[i * 3];
        uint16_t i1 = data->collisionIndices[i * 3 + 1];
        uint16_t i2 = data->collisionIndices[i * 3 + 2];

        if ((CM2Model::s_collisionCodes[i1] & CM2Model::s_collisionCodes[i2] & CM2Model::s_collisionCodes[i0]) != 0) {
            continue;
        }

        CFacet* facet = facets->New();

        facet->v[0] = CM2Model::s_collisionPositions[i0];
        facet->v[1] = CM2Model::s_collisionPositions[i1];
        facet->v[2] = CM2Model::s_collisionPositions[i2];

        C3Vector& normal = data->collisionFaceNormals[i];

        facet->plane.n.x = normal.y * rotation.b0 + normal.z * rotation.c0 + normal.x * rotation.a0;
        facet->plane.n.y = normal.z * rotation.c1 + normal.x * rotation.a1 + normal.y * rotation.b1;
        facet->plane.n.z = normal.x * rotation.a2 + normal.y * rotation.b2 + normal.z * rotation.c2;
        facet->plane.d = -(facet->v[0].z * facet->plane.n.z + facet->v[0].y * facet->plane.n.y + facet->v[0].x * facet->plane.n.x);
    }
}
