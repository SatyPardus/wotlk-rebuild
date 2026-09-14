#include "model/CM2Shared.hpp"
#include "async/AsyncFile.hpp"
#include "gx/Buffer.hpp"
#include "gx/Shader.hpp"
#include "gx/Texture.hpp"
#include "model/CM2Cache.hpp"
#include "model/CM2Model.hpp"
#include "model/M2Data.hpp"
#include "model/M2Init.hpp"
#include "model/M2Types.hpp"
#include "util/CStatus.hpp"
#include "util/SFile.hpp"
#include <cstring>
#include <cstdio>

// OFFSET: 0x835A00
void CM2Shared::LoadFailedCallback(void* arg) {
    CM2Shared* shared = static_cast<CM2Shared*>(arg);

    AsyncFileReadDestroyObject(shared->asyncObject);
    shared->asyncObject = nullptr;
}

void CM2Shared::LoadSucceededCallback(void* arg) {
    CM2Shared* shared = static_cast<CM2Shared*>(arg);

    AsyncFileReadDestroyObject(shared->asyncObject);
    shared->asyncObject = nullptr;

    uint8_t* base = reinterpret_cast<uint8_t*>(shared->m_data);
    uint32_t size = shared->m_fileSize;
    M2Data& data = *shared->m_data;

    if (!M2Init(base, size, data)) {
        return;
    }

    if (!shared->Initialize()) {
        return;
    }

    // TODO
    // - allocate space for low priority sequence pointers

    shared->m_m2DataLoaded = 1;
}

// OFFSET: 0x83CB10
void CM2Shared::SkinProfileLoadedCallback(void* arg) {
    CM2Shared* shared = static_cast<CM2Shared*>(arg);

    shared->FinishLoadingSkinProfile(shared->asyncObject->size);

    AsyncFileReadDestroyObject(shared->asyncObject);
    shared->asyncObject = nullptr;
}

// OFFSET: 0x835970
void CM2Shared::AddRef() {
    if (this->m_refCount || !this->m_cache) {
        this->m_refCount++;
        return;
    }

    //if (this->unk_0030)
    //    *this->unk_0030 = this->unk_0034;
    //if (this->unk_0034) {
    //    *(this->unk_0034 + 48) = this->unk_0030;
    //    this->m_refCount++;
    //    this->unk_0030 = 0;
    //    this->unk_0034 = 0;
    //    this->unk_0038 = 0;
    //    return;
    //}
    //this->m_cache->unk_000C = v3;
    //this->unk_0030 = 0;
    //this->unk_0034 = 0;
    //this->unk_0038 = 0;
}

int32_t CM2Shared::CallbackWhenLoaded(CM2Model* model) {
    if (model->m_flags & 0x20) {
        return 1;
    }

    if (this->m_m2DataLoaded && this->m_skinProfileLoaded) {
        model->InitializeLoaded();

        return 1;
    }

    model->LinkToCallbackListTail();

    return 1;
}

// OFFSET: 0x836600
CShaderEffect* CM2Shared::CreateSimpleEffect(uint32_t textureCount, uint16_t shader, uint16_t textureCoordComboIndex) {
    uint32_t combiner[2];
    uint32_t envmap[2];

    combiner[0] = (shader >> M2COMBINER_STAGE_SHIFT)    & M2COMBINER_OP_MASK;  // T1 Combiner
    combiner[1] = (shader >> 0)                         & M2COMBINER_OP_MASK;  // T2 Combiner
    envmap[0]   = (shader >> M2COMBINER_STAGE_SHIFT)    & M2COMBINER_ENVMAP;   // T1 Env Mapped
    envmap[1]   = (shader >> 0)                         & M2COMBINER_ENVMAP;   // T2 Env Mapped

    const char* vsName;
    const char* psName;

    // 1 texture
    if (textureCount == 1) {
        if (envmap[0]) {
            vsName = "Diffuse_Env";
        } else if (this->m_data->textureCoordCombos[textureCoordComboIndex] == 0) {
            vsName = "Diffuse_T1";
        } else {
            vsName = "Diffuse_T2";
        }

        switch (combiner[0]) {
            case M2COMBINER_OPAQUE:
                psName = "Combiners_Opaque";
                break;

            case M2COMBINER_MOD:
                psName = "Combiners_Mod";
                break;

            case M2COMBINER_DECAL:
                psName = "Combiners_Decal";
                break;

            case M2COMBINER_ADD:
                psName = "Combiners_Add";
                break;

            case M2COMBINER_MOD2X:
                psName = "Combiners_Mod2x";
                break;

            case M2COMBINER_FADE:
                psName = "Combiners_Fade";
                break;

            default:
                psName = "Combiners_Mod";
                break;
        }

    // 2 textures
    } else {
        if (envmap[0] && envmap[1]) {
            vsName = "Diffuse_Env_Env";
        } else if (envmap[0]) {
            vsName = "Diffuse_Env_T2";
        } else if (envmap[1]) {
            vsName = "Diffuse_T1_Env";
        } else {
            vsName = "Diffuse_T1_T2";
        }

        switch (combiner[0]) {
            case M2COMBINER_OPAQUE:
                switch (combiner[1]) {
                    case M2COMBINER_OPAQUE:
                        psName = "Combiners_Opaque_Opaque";
                        break;

                    case M2COMBINER_ADD:
                        psName = "Combiners_Opaque_Add";
                        break;

                    case M2COMBINER_MOD2X:
                        psName = "Combiners_Opaque_Mod2x";
                        break;

                    case M2COMBINER_MOD2X_NA:
                        psName = "Combiners_Opaque_Mod2xNA";
                        break;

                    case M2COMBINER_ADD_NA:
                        psName = "Combiners_Opaque_AddNA";
                        break;

                    default:
                        psName = "Combiners_Opaque_Mod";
                        break;
                }

                break;

            case M2COMBINER_MOD:
                switch (combiner[1]) {
                    case M2COMBINER_OPAQUE:
                        psName = "Combiners_Mod_Opaque";
                        break;

                    case M2COMBINER_MOD:
                        psName = "Combiners_Mod_Mod";
                        break;

                    case M2COMBINER_ADD:
                        psName = "Combiners_Mod_Add";
                        break;

                    case M2COMBINER_MOD2X:
                        psName = "Combiners_Mod_Mod2x";
                        break;

                    case M2COMBINER_MOD2X_NA:
                        psName = "Combiners_Mod_Mod2xNA";
                        break;

                    case M2COMBINER_ADD_NA:
                        psName = "Combiners_Mod_AddNA";
                        break;

                    default:
                        psName = "Combiners_Mod_Mod";
                        break;
                }

                break;

            case M2COMBINER_ADD:
                switch (combiner[1]) {
                    case M2COMBINER_MOD:
                        psName = "Combiners_Add_Mod";
                        break;

                    default:
                        return nullptr;
                }

                break;

            case M2COMBINER_MOD2X:
                switch (combiner[1]) {
                    case M2COMBINER_MOD2X:
                        psName = "Combiners_Mod2x_Mod2x";
                        break;

                    default:
                        return nullptr;
                }

                break;
        }
    }

    char effectName[256];

    // Create effect name for hashing
    strcpy(effectName, vsName);
    strcat(effectName, psName);

    CShaderEffect* effect = CShaderEffectManager::GetEffect(effectName);

    if (!effect) {
        effect = CShaderEffectManager::CreateEffect(effectName);

        effect->InitEffect(vsName, psName);

        // M2GetCombinerOps (inlined)
        for (int32_t textureIndex = 0; textureIndex < textureCount; textureIndex++) {
            // TODO
            // colorOps[textureIndex] = colorOpTable[combiner[textureIndex]];
            // alphaOps[textureIndex] = alphaOpTable[combiner[textureIndex]];
        }

        // TODO
        // effect->InitFixedFuncPass(colorOps, alphaOps, textureCount);
    }

    return effect;
}

// OFFSET: 0x838490
int32_t CM2Shared::FinishLoadingSkinProfile(uint32_t size) {
    if (this->m_skinProfileLoaded) {
        return 1;
    }

    uint8_t* base = reinterpret_cast<uint8_t*>(this->m_skinData);
    M2Data& data = *this->m_data;
    M2SkinProfile& skinProfile = *this->m_skinData;

    if (!M2Init(base, size, data, skinProfile)) {
        return 0;
    }

    if (!this->InitializeSkinProfile()) {
        return 0;
    }

    this->m_skinProfileLoaded = 1;

    for (auto model = this->m_callbackList; model; model = this->m_callbackList) {
        STORM_ASSERT(model->m_callbackPrev);
        STORM_ASSERT(*model->m_callbackPrev == this->m_callbackList);

        model->UnlinkFromCallbackList();
        model->InitializeLoaded();
    }

    return 1;
}

// OFFSET: 0x836C90
CShaderEffect* CM2Shared::GetEffect(M2Batch* batch) {
    CShaderEffect* effect;

    // Simple effect
    if (!(batch->shader & 0x8000)) {
        effect = this->CreateSimpleEffect(batch->textureCount, batch->shader, batch->textureCoordComboIndex);

        // Fallback
        // 0000000000010001
        // 1 texture: Combiners_Mod
        // 2 texture: Combiners_Mod_Mod
        if (!effect) {
            effect = this->CreateSimpleEffect(batch->textureCount, 0x11, batch->textureCoordComboIndex);
        }

        return effect;
    }

    // Specialized effect
    const char* vsName = nullptr;
    const char* psName = nullptr;
    uint32_t colorOp = 0;
    uint32_t alphaOp = 0;

    switch (batch->shader & 0x7FFF) {
        case 0:
            return nullptr;

        case 1:
            vsName = "Diffuse_T1_Env";
            psName = "Combiners_Opaque_Mod2xNA_Alpha";
            colorOp = 0;
            alphaOp = 3;

            break;

        case 2:
            vsName = "Diffuse_T1_Env";
            psName = "Combiners_Opaque_AddAlpha";
            colorOp = 0;
            alphaOp = 3;

            break;

        case 3:
            vsName = "Diffuse_T1_Env";
            psName = "Combiners_Opaque_AddAlpha_Alpha";
            colorOp = 0;
            alphaOp = 3;

            break;

        default:
            break;
    }

    char effectName[256];

    // Create effect name for hashing
    strcpy(effectName, vsName);
    strcat(effectName, psName);

    effect = CShaderEffectManager::GetEffect(effectName);

    if (!effect) {
        effect = CShaderEffectManager::CreateEffect(effectName);

        effect->InitEffect(vsName, psName);

        // TODO
        // effect->InitFixedFuncPass(effect, &colorOp, &alphaOp, 1);

        if (!effect) {
            effect = this->CreateSimpleEffect(batch->textureCount, 0x11, batch->textureCoordComboIndex);
        }
    }

    return effect;
}

int32_t CM2Shared::Initialize() {
    this->m_skinData = nullptr;

    // TODO
    // implement logic to select skin profile

    uint32_t profile = this->m_data->numSkinProfiles - 1;

    if (!this->LoadSkinProfile(profile)) {
        return 0;
    }

    void* textures = SMemAlloc(sizeof(HTEXTURE) * this->m_data->textures.count, __FILE__, __LINE__, 0x8);
    this->textures = static_cast<HTEXTURE*>(textures);

    if (!textures) {
        // TODO
        // CM2Model::ErrorSetFileLine(__FILE__, __LINE__);
        // CM2Model::ErrorSet("Failed to allocate texture array: %s", this + 60);

        return 0;
    }

    for (int32_t i = 0; i < this->m_data->materials.count; i++) {
        M2Material& material = this->m_data->materials[i];
        if (material.blendMode >= M2BLEND_MOD) {
            material.flags |= 0x1;
        }
    }

    for (int32_t i = 0; i < this->m_data->textures.count; i++) {
        M2Texture& texture = this->m_data->textures[i];

        if (texture.filename.count > 1) {
            CGxTexFlags texFlags = CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1);

            int32_t createFlags = 0;

            if (this->m_flag4) {
                createFlags |= 0x2;
            }

            if (this->m_flag40) {
                createFlags |= 0x30;
            }

            CStatus* status = &GetGlobalStatusObj();

            this->textures[i] = TextureCreate(&texture.filename[0], texFlags, status, createFlags);
        } else {
            static CImVector FRIENDLY_WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
            this->textures[i] = TextureCreateSolid(FRIENDLY_WHITE);
        }
    }

    for (int32_t i = 0; i < this->m_data->bones.count; i++) {
        M2CompBone& bone = this->m_data->bones[i];

        if (bone.flags & (0x200 | 0x80 | 0x40 | 0x20 | 0x10 | 0x8)) {
            // TODO
        }
    }

    return 1;
}

// OFFSET: 0x837A40
int32_t CM2Shared::InitializeSkinProfile() {
    this->uint194 = this->m_skinData->indices.Count()
        ? 65536 / this->m_skinData->indices.Count()
        : 1;

    for (int32_t i = 0; i < this->m_skinData->skinSections.Count(); i++) {
        uint32_t v6 = this->m_skinData->boneCountMax / this->m_skinData->skinSections[i].boneCount;

        if (this->uint194 > v6) {
            this->uint194 = v6;
        }
    }

    if (!this->uint194) {
        this->uint194 = 1;
    }

    this->uint190 = 1;

    uint32_t dataSize = sizeof(M2SkinSection) * this->m_skinData->skinSections.Count();
    if (this->m_skinData->indices.Count()) {
        dataSize += sizeof(CShaderEffect*) * this->m_skinData->batches.Count();
    }

    char* data = static_cast<char*>(SMemAlloc(dataSize, __FILE__, __LINE__, 0x0));
    if (!data) {
        return 0;
    }

    this->m_skinSections = reinterpret_cast<M2SkinSection*>(data);
    if (this->m_skinData->skinSections.Count()) {
        data += sizeof(M2SkinSection) * this->m_skinData->skinSections.Count();
        memcpy(this->m_skinSections, this->m_skinData->skinSections.Data(), this->m_skinData->skinSections.Count() * sizeof(M2SkinSection));
    }

    if (this->m_skinData->indices.Count()) {
        this->m_batchShaders = reinterpret_cast<CShaderEffect**>(data);
        memset(this->m_batchShaders, 0, sizeof(CShaderEffect*) * this->m_skinData->batches.Count());
    }

    this->SubstituteSimpleShaders();
    this->SubstituteSpecializedShaders();

    if (this->m_skinData->indices.Count()) {
        for (int32_t i = 0; i < this->m_skinData->batches.Count(); i++) {
            this->m_batchShaders[i] = this->GetEffect(&this->m_skinData->batches[i]);
        }
    }

    if (!(this->m_cache->m_flags & 0x8)) {
        //v17 = this->m_skinData->vertices.count;
        //v18 = SMemAlloc((48 * v17) >> 32 != 0 ? -1 : 48 * v17, ".\\M2Shared.cpp", 1320, 0);
        //v41 = v18;
        //if (v18) {
        //    v19 = v17 - 1;
        //    if (v19 >= 0) {
        //        v20 = v18 + 7;
        //        do {
        //            *(v20 - 7) = 0.0;
        //            *(v20 - 6) = 0.0;
        //            *(v20 - 5) = 0.0;
        //            *(v20 - 2) = 0.0;
        //            *(v20 - 1) = 0.0;
        //            *v20 = 0.0;
        //            ForEachElement((v20 + 1), 8, 2, function);
        //            v20 += 12;
        //            --v19;
        //        } while (v19 >= 0);
        //        v18 = v41;
        //    }
        //    v21 = v18;
        //    v44 = v18;
        //} else {
        //    v44 = 0;
        //    v21 = 0;
        //}
        //v42 = 0;
        //if (this->m_skinData->skinSections.count) {
        //    v43 = 0;
        //    do {
        //        v22 = (v43 + this->m_skinData->skinSections.offset);
        //        v23 = v22[2];
        //        v40 = v23 + v22[3];
        //        v47 = v23;
        //        if (v23 < v40) {
        //            v24 = &v21[12 * v23];
        //            v46 = v24;
        //            while (1) {
        //                v25 = (this->m_data->vertices.offset + 48 * *(this->m_skinData->vertices.offset + 2 * v23));
        //                v26 = 0;
        //                qmemcpy(v24, v25, 0x30u);
        //                if (v22[8]) {
        //                    do {
        //                        v46[v26 + 16] = *(this->m_data->boneCombos.offset + 2 * (v22[7] + *(4 * v47 + this->m_skinData->bones.offset + v26)));
        //                        ++v26;
        //                    } while (v26 < v22[8]);
        //                }
        //                v46 += 48;
        //                if (++v47 >= v40)
        //                    break;
        //                v24 = v46;
        //                v23 = v47;
        //            }
        //            v21 = v44;
        //        }
        //        v43 += 48;
        //        ++v42;
        //    } while (v42 < this->m_skinData->skinSections.count);
        //}
        //v27 = 0;
        //if (this->m_skinData->skinSections.count) {
        //    v28 = 0;
        //    do {
        //        *(this->unk_018C + v28 + 14) = 0;
        //        *(this->m_skinData->skinSections.offset + v28 + 14) = 0;
        //        ++v27;
        //        v28 += 48;
        //    } while (v27 < this->m_skinData->skinSections.count);
        //}
        //for (i = 0; i < this->m_data->boneCombos.count; ++i)
        //    *(this->m_data->boneCombos.offset + 2 * i) = i;
        //for (j = 0; j < this->m_skinData->vertices.count; ++j)
        //    *(this->m_skinData->vertices.offset + 2 * j) = j;
        //m_data = this->m_data;
        //v32 = this->m_skinData->vertices.count;
        //if (v32 <= m_data->vertices.count) {
        //    memcpy(m_data->vertices.offset, v21, 48 * v32);
        //    if (v21)
        //        SMemFree(v21, "delete[]", -1, 0);
        //} else {
        //    this->m_flags |= 8u;
        //    m_data->vertices.offset = v21;
        //}
        //this->m_data->vertices.count = this->m_skinData->vertices.count;
    }

    if (!(this->m_cache->m_flags & 0x8)) {
        for (int32_t i = 0; i < this->m_skinData->batches.Count(); i++) {
            auto& batch = this->m_skinData->batches[i];

            if (batch.textureCount > 1) {
                this->m_skinData->batches[i - batch.materialLayer].flags |= 0x40;
            }
        }

        for (int32_t i = 0; i < this->m_skinData->batches.Count(); i++) {
            auto& batch = this->m_skinData->batches[i];

            if (batch.materialLayer) {
                if (this->m_skinData->batches[i - batch.materialLayer].flags & 0x40) {
                    batch.flags |= 0x40;
                }
            }
        }
    }

    return 1;
}

// OFFSET: 0x83D410
int32_t CM2Shared::Load(SFile* file, int32_t a3, CAaBox* a4) {
    this->m_flags ^= (this->m_flags ^ (4 * (a3 != 0))) & 4;
    this->m_fileSize = SFile::GetFileSize(file, 0);

    this->m_data = static_cast<M2Data*>(SMemAlignedAlloc(this->m_fileSize, __FILE__, __LINE__));

    if (!this->m_data) {
        return 0;
    }

    if (a4) {
        this->m_boundingBox = *a4;
    }

    this->asyncObject = AsyncFileReadAllocObject();

    if (!this->asyncObject) {
        return 0;
    }

    this->asyncObject->file = file;
    this->asyncObject->buffer = this->m_data;
    this->asyncObject->size = this->m_fileSize;
    this->asyncObject->userArg = this;
    this->asyncObject->userPostloadCallback = &CM2Shared::LoadSucceededCallback;
    this->asyncObject->userFailedCallback = &CM2Shared::LoadFailedCallback;
    this->asyncObject->isRead = 0;
    this->asyncObject->isProcessed = 0;

    AsyncFileReadObject(this->asyncObject, 0);

    return 1;
}

// OFFSET: 0x835A80
void CM2Shared::MakeSkinFileName(char* fileName, uint32_t profile, char* out) {
    char* v3 = strcpy(out, fileName);
    char* v4 = strrchr(v3, '.');
    if (v4) {
        *v4 = 0;
    }
    sprintf(&v3[strlen(v3)], "%02d.skin", profile);
}

// OFFSET: 0x83CB40
int32_t CM2Shared::LoadSkinProfile(uint32_t profile) {
    char skinFilePath[STORM_MAX_PATH];
    CM2Shared::MakeSkinFileName(this->m_filePath, profile, skinFilePath);

    SFile* fileptr;

    if (!SFile::OpenEx(nullptr, skinFilePath, this->m_flag4, &fileptr)) {
        // NOP("Model2: File not found: %s\n");
        return 0;
    }

    uint32_t size = SFile::GetFileSize(fileptr, nullptr);

    this->m_skinData = static_cast<M2SkinProfile*>(SMemAlignedAlloc(size, __FILE__, __LINE__));

    if (!this->m_skinData) {
        SFile::Close(fileptr);

        return 0;
    }

    this->asyncObject = AsyncFileReadAllocObject();

    if (!this->asyncObject) {
        SFile::Close(fileptr);
        delete this->m_skinData;

        return 0;
    }

    this->asyncObject->file = fileptr;
    this->asyncObject->buffer = this->m_skinData;
    this->asyncObject->size = size,
    this->asyncObject->userArg = this;
    this->asyncObject->userPostloadCallback = &CM2Shared::SkinProfileLoadedCallback;
    this->asyncObject->userFailedCallback = &CM2Shared::LoadFailedCallback;
    this->asyncObject->isRead = 0;
    this->asyncObject->isProcessed = 0;
    this->asyncObject->priority = 125;

    AsyncFileReadObject(this->asyncObject, 1);

    return 1;
}

void CM2Shared::Release() {
    // TODO
}

// OFFSET: 0x8360A0
int32_t CM2Shared::SetIndices() {
    if (!this->m_indexPool) {
        this->m_indexPool = GxPoolCreate(
            GxPoolTarget_Index,
            GxPoolUsage_Dynamic,
            2 * this->uint190 * this->m_skinData->indices.Count(),
            GxPoolHintBit_Unk1,
            this->ext
        );

        this->m_indexBuf = GxBufCreate(
            this->m_indexPool,
            2,
            this->uint190 * this->m_skinData->indices.Count(),
            0
        );

        if (!this->m_indexPool || !this->m_indexBuf) {
            return 0;
        }
    }

    if (!this->m_indexBuf->unk1C || !this->m_indexBuf->unk1D) {
        char* indexData = GxBufLock(this->m_indexBuf);
        uint16_t* indexBuf = reinterpret_cast<uint16_t*>(indexData);

        if (!indexData) {
            return 0;
        }

        int32_t v10 = (this->m_cache->m_flags & 0x8) != 0
            || (this->m_data->bones.Count() == 1 && (this->m_cache->m_flags & 0x40) != 0);
        uint32_t v21 = 0;

        for (int32_t i = 0; i < this->m_skinData->skinSections.Count(); i++) {
            auto& skinSection = this->m_skinSections[i];
            auto indexStart = this->m_skinData->skinSections[i].indexStart;
            auto v25 = v10 ? 0 : -skinSection.vertexStart;

            for (int32_t j = 0; j < this->uint190; j++) {
                for (int32_t k = 0; k < skinSection.indexCount; k++) {
                    indexBuf[k] = this->m_skinData->indices[indexStart + k + v25];
                }

                indexBuf += skinSection.indexCount;

                v25 += v10 ? this->m_skinData->vertices.Count() : skinSection.vertexCount;
            }

            skinSection.indexStart = v21;
            v21 += this->uint190 * skinSection.indexCount;
        }

        GxBufUnlock(this->m_indexBuf, 0);
    }

    GxPrimIndexPtr(this->m_indexBuf);

    return 1;
}

int32_t CM2Shared::SetVertices(uint32_t a2) {
    if (!this->m_vertexPool) {
        this->m_vertexPool = GxPoolCreate(
            GxPoolTarget_Vertex,
            GxPoolUsage_Static,
            sizeof(CGxVertexPBNT2) * this->uint190 * this->m_skinData->vertices.Count(),
            GxPoolHintBit_Unk1,
            this->ext
        );

        this->m_vertexBuf = GxBufCreate(
            this->m_vertexPool,
            sizeof(CGxVertexPBNT2),
            this->uint190 * this->m_skinData->vertices.Count(),
            0
        );

        if (!this->m_vertexPool || !this->m_vertexBuf) {
            return 0;
        }
    }

    if (CShaderEffect::s_enableShaders) {
        if (!this->m_vertexBuf->unk1C || !this->m_vertexBuf->unk1D) {
            char* vertexData = GxBufLock(this->m_vertexBuf);
            CGxVertexPBNT2* vertexBuf = reinterpret_cast<CGxVertexPBNT2*>(vertexData);

            if (!vertexData) {
                return 0;
            }

            auto v27 = 0;
            for (int32_t i = 0; i < this->uint190; i++) {
                for (int32_t j = 0; j < this->m_skinData->skinSections.Count(); j++) {
                    auto& skinSection = this->m_skinData->skinSections[j];
                    auto vertexStart = skinSection.vertexStart;
                    auto vertexEnd = vertexStart + skinSection.vertexCount;

                    uint32_t v25 = 0x1010101 * i * skinSection.boneCount;

                    if (vertexStart < vertexEnd) {
                        for (int32_t k = vertexStart; k < vertexEnd; k++) {
                            auto vertex = &this->m_data->vertices[this->m_skinData->vertices[k]];
                            memcpy(&vertexBuf[k], vertex, sizeof(CGxVertexPBNT2));
                            vertexBuf[k].bi.u = v25 + this->m_skinData->bones[k].u;
                        }
                    }
                }

                vertexBuf += this->m_skinData->vertices.Count();
            }

            GxBufUnlock(this->m_vertexBuf, 0);
        }

        GxPrimVertexPtr(this->m_vertexBuf, GxVBF_PBNT2);

        return 1;
    } else {
        // TODO
        // non-shader code path

        return 1;
    }
}

// OFFSET: 0x836980
void CM2Shared::SubstituteSimpleShaders() {
    for (int32_t batchIndex = 0; batchIndex < this->m_skinData->batches.Count(); batchIndex++) {
        auto& batch = this->m_skinData->batches[batchIndex];

        if (batch.shader & 0x8000) {
            continue;
        }

        auto& material = this->m_data->materials[batch.materialIndex];
        uint16_t textureCoordComboIndex = batch.textureCoordComboIndex;

        // M2Data flag 0x8: use combiner combos
        if (this->m_data->flags & 0x8) {
            uint16_t textureCombinerComboIndex = batch.shader;

            batch.shader = 0;

            uint16_t shader[2] = { 0, 0 };

            for (int32_t textureIndex = 0; textureIndex < batch.textureCount; textureIndex++) {
                bool isFirstTexture = textureIndex == 0;
                bool isLastTexture = textureIndex == batch.textureCount - 1;

                // If this is the first texture and the batch material's blending mode is opaque,
                // override the combiner mode to opaque; otherwise, use the combiner mode from the
                // combiner combos
                uint16_t textureCombiner = isFirstTexture && material.blendMode == M2BLEND_OPAQUE
                    ? M2COMBINER_OPAQUE
                    : this->m_data->textureCombinerCombos[textureCombinerComboIndex + textureIndex];

                shader[textureIndex] |= textureCombiner;

                uint16_t textureCoord = this->m_data->textureCoordCombos[textureCoordComboIndex + textureIndex];

                // If the texture coord is env, set env bit for texture
                if (textureCoord > 2) {
                    shader[textureIndex] |= M2COMBINER_ENVMAP;
                }

                // If this is the last texture and the texture coord is T2, enable bit 15
                if (isLastTexture && textureCoord == 1) {
                    batch.shader |= 0x4000;
                }
            }

            batch.shader |= (shader[0] << M2COMBINER_STAGE_SHIFT) | shader[1];
        } else {
            uint16_t shader = 0;

            // If the material blend mode is opaque, force the combiner to opaque; otherwise,
            // default combiner to mod
            uint16_t textureCombiner = material.blendMode == M2BLEND_OPAQUE
                ? M2COMBINER_OPAQUE
                : M2COMBINER_MOD;

            shader |= textureCombiner;

            uint16_t textureCoord = this->m_data->textureCoordCombos[textureCoordComboIndex];

            // If the texture coord is env, set env bit for texture
            if (textureCoord > 2) {
                shader |= M2COMBINER_ENVMAP;
            }

            shader <<= M2COMBINER_STAGE_SHIFT;

            // If the texture coord is T2, enable bit 15
            if (textureCoord == 1) {
                shader |= 0x4000;
            }

            batch.shader = shader;
        }
    }
}

// OFFSET: 0x835E90
void CM2Shared::PackBatchTextureCombos() {
    for (uint32_t batchIndex = 0; batchIndex < this->m_skinData->batches.Count(); batchIndex++) {
        auto& batch = this->m_skinData->batches[batchIndex];

        if (batch.textureCount == 2) {
            uint16_t* textureCombo = &this->m_data->textureCombos[batch.textureComboIndex];
            uint16_t* transformCombo = &this->m_data->textureTransformCombos[batch.textureTransformComboIndex];

            uint32_t transform0 = transformCombo[0] == 0xFFFF ? 0 : transformCombo[0] + 1;
            uint32_t transform1 = transformCombo[1] == 0xFFFF ? 0 : transformCombo[1] + 1;

            batch.textureComboIndex = static_cast<uint8_t>(textureCombo[0]) | (static_cast<uint8_t>(textureCombo[1]) << 8);
            batch.textureTransformComboIndex = static_cast<uint8_t>(transform0) | (static_cast<uint8_t>(transform1) << 8);
        } else {
            uint16_t textureCombo = this->m_data->textureCombos[batch.textureComboIndex];
            uint16_t transformCombo = this->m_data->textureTransformCombos[batch.textureTransformComboIndex];

            batch.textureComboIndex = textureCombo;
            batch.textureTransformComboIndex = transformCombo == 0xFFFF ? 0 : transformCombo + 1;
        }
    }
}

// OFFSET: 0x837680
void CM2Shared::SubstituteSpecializedShaders() {
    if (!this->m_skinData->indices.Count()) {
        return;
    }

    if (!this->m_skinData->batches.Count()) {
        return;
    }

    bool layered = false;

    for (uint32_t batchIndex = 0; batchIndex < this->m_skinData->batches.Count(); batchIndex++) {
        if (this->m_skinData->batches[batchIndex].materialLayer > 0) {
            layered = true;
            break;
        }
    }

    if (!layered) {
        return;
    }

    this->PackBatchTextureCombos();

    uint8_t stateA = 0;
    uint8_t stateB = 0;
    bool repeatedMaterial = false;
    bool rebuildCombos = false;
    int32_t prevMaterialIndex = -1;
    M2Batch* base = nullptr;

    for (uint32_t batchIndex = 0; batchIndex < this->m_skinData->batches.Count(); batchIndex++) {
        M2Batch* batch = &this->m_skinData->batches[batchIndex];

        if (batch->materialIndex == prevMaterialIndex) {
            repeatedMaterial = true;
            continue;
        }

        uint32_t combiner = batch->shader & M2COMBINER_OP_MASK;
        M2Material* material = &this->m_data->materials[batch->materialIndex];

        prevMaterialIndex = batch->materialIndex;

        if (batch->materialLayer == 0) {
            stateA = 0;

            if (batch->textureCount >= 1 && material->blendMode == M2BLEND_OPAQUE) {
                batch->shader &= 0xFF8F;
            }

            base = batch;
        }

        if (stateA == 1) {
            if (
                (material->blendMode == M2BLEND_ALPHA || material->blendMode == M2BLEND_ALPHA_KEY) && batch->textureCount == 1 && ((this->m_data->materials[base->materialIndex].flags ^ material->flags) & 0x1) == 0 && batch->textureComboIndex == static_cast<uint8_t>(base->textureComboIndex) && this->m_data->textureWeightCombos[base->textureWeightComboIndex] == this->m_data->textureWeightCombos[batch->textureWeightComboIndex]) {
                batch->shader = 0x8000;
                base->shader = 0x8001;
                stateA = 3;

                continue;
            }

            stateA = 0;
        }

        if (stateA < 2) {
            if (
                material->blendMode == M2BLEND_OPAQUE && batch->textureCount == 2 && (combiner == M2COMBINER_MOD2X || combiner == M2COMBINER_MOD2X_NA) && this->m_data->textureCoordCombos[batch->textureCoordComboIndex] == 0 && this->m_data->textureCoordCombos[batch->textureCoordComboIndex + 1] > 2) {
                stateA = 1;
            }
        }

        if (stateB == 3) {
            continue;
        }

        if (stateB == 2) {
            if (
                (material->blendMode == M2BLEND_ALPHA || material->blendMode == M2BLEND_ALPHA_KEY) && batch->textureCount == 1 && ((this->m_data->materials[base->materialIndex].flags ^ material->flags) & 0x1) == 0 && static_cast<uint8_t>(batch->textureComboIndex) == static_cast<uint8_t>(base->textureComboIndex) && this->m_data->textureWeightCombos[base->textureWeightComboIndex] == this->m_data->textureWeightCombos[batch->textureWeightComboIndex]) {
                batch->shader = 0x8000;
                base->shader = base->shader == 0x8002 ? 0x8003 : 0x8001;
                stateB = 3;

                continue;
            }

            stateB = 0;
        } else if (stateB == 1) {
            if (
                (material->blendMode == M2BLEND_ADD || material->blendMode == M2BLEND_MOD_2X) && batch->textureCount == 1 && this->m_data->textureCoordCombos[batch->textureCoordComboIndex] > 2 && this->m_data->textureWeightCombos[base->textureWeightComboIndex] == this->m_data->textureWeightCombos[batch->textureWeightComboIndex]) {
                batch->shader = 0x8000;
                base->shader = material->blendMode == M2BLEND_ADD ? 0x8002 : 0x000E;
                base->textureCount = 2;
                base->textureComboIndex = static_cast<uint8_t>(base->textureComboIndex) | (static_cast<uint8_t>(batch->textureComboIndex) << 8);
                base->textureTransformComboIndex = static_cast<uint8_t>(base->textureTransformComboIndex) | (static_cast<uint8_t>(batch->textureTransformComboIndex) << 8);

                rebuildCombos = true;
                stateB = 2;

                continue;
            }

            stateB = 0;
        }

        if (
            material->blendMode == M2BLEND_OPAQUE && batch->textureCount == 1 && this->m_data->textureCoordCombos[batch->textureCoordComboIndex] == 0) {
            stateB = 1;
        }
    }

    if (rebuildCombos) {
        this->ConvertTextureValuesToCombos();
    }

    this->AssignBatchTextureComboIndices();

    if (repeatedMaterial) {
        int32_t lastMaterialIndex = -1;

        for (uint32_t batchIndex = 0; batchIndex < this->m_skinData->batches.Count(); batchIndex++) {
            M2Batch* batch = &this->m_skinData->batches[batchIndex];

            if (batch->materialIndex != lastMaterialIndex) {
                lastMaterialIndex = batch->materialIndex;
                continue;
            }

            M2Batch* prev = batch - 1;

            batch->shader = prev->shader;
            batch->textureCount = prev->textureCount;
            batch->textureComboIndex = prev->textureComboIndex;
            batch->textureTransformComboIndex = prev->textureTransformComboIndex;
        }
    }
}

// OFFSET: 0x837250
void CM2Shared::ConvertTextureValuesToCombos() {
    uint32_t batchCount = this->m_skinData->batches.Count();

    M2ComboList comboList;
    comboList.data = static_cast<uint16_t*>(_alloca(4 * batchCount));
    comboList.count = 0;

    M2ComboList transformList;
    transformList.data = static_cast<uint16_t*>(_alloca(4 * batchCount));
    transformList.count = 0;

    int32_t prevMaterialIndex = -1;

    for (uint32_t batchIndex = 0; batchIndex < this->m_skinData->batches.Count(); batchIndex++) {
        M2Batch* batch = &this->m_skinData->batches[batchIndex];

        if (batch->materialIndex == prevMaterialIndex) {
            continue;
        }

        prevMaterialIndex = batch->materialIndex;

        if (batch->textureCount < 2) {
            continue;
        }

        ConvertTextureComboEntry(&comboList, batch->textureComboIndex, 0);
        ConvertTextureComboEntry(&transformList, batch->textureTransformComboIndex, 1);
    }

    prevMaterialIndex = -1;

    uint16_t* comboEnd = comboList.data + comboList.count;
    uint16_t* transformEnd = transformList.data + transformList.count;

    for (uint32_t batchIndex = 0; batchIndex < this->m_skinData->batches.Count(); batchIndex++) {
        M2Batch* batch = &this->m_skinData->batches[batchIndex];

        if (batch->materialIndex == prevMaterialIndex) {
            continue;
        }

        prevMaterialIndex = batch->materialIndex;

        if (batch->textureCount > 1) {
            continue;
        }

        uint16_t* combo = comboList.data;

        while (combo != comboEnd && *combo != batch->textureComboIndex) {
            combo++;
        }

        if (combo == comboEnd) {
            comboList.count++;
            *comboEnd = batch->textureComboIndex;
            comboEnd++;
        }

        uint16_t transform = batch->textureTransformComboIndex
                                 ? batch->textureTransformComboIndex - 1
                                 : 0xFFFF;

        uint16_t* entry = transformList.data;

        while (entry != transformEnd && *entry != transform) {
            entry++;
        }

        if (entry == transformEnd) {
            transformList.count++;
            *transformEnd = transform;
            transformEnd++;
        }
    }

    uint32_t comboBytes = comboList.count > this->m_data->textureCombos.Count()
                              ? 2 * comboList.count
                              : 0;
    uint32_t transformBytes = transformList.count > this->m_data->textureTransformCombos.Count()
                                  ? 2 * transformList.count
                                  : 0;

    if (comboBytes || transformBytes) {
        uint32_t grownSize = this->m_fileSize + comboBytes + transformBytes;
        M2Data* grown = static_cast<M2Data*>(SMemAlignedAlloc(grownSize, __FILE__, __LINE__));

        if (!grown) {
            return;
        }

        memcpy(grown, this->m_data, this->m_fileSize);
        SMemAlignedFree(this->m_data);
        this->m_data = grown;

        uint8_t* tail = reinterpret_cast<uint8_t*>(this->m_data) + this->m_fileSize;

        if (comboBytes) {
            this->m_data->textureCombos.offset = tail - reinterpret_cast<uint8_t*>(&this->m_data->textureCombos);
            this->m_data->flags |= 0x20;
            tail += comboBytes;
        }

        if (transformBytes) {
            this->m_data->textureTransformCombos.offset = tail - reinterpret_cast<uint8_t*>(&this->m_data->textureTransformCombos);
            this->m_data->flags |= 0x40;
        }

        this->m_fileSize = grownSize;
    }

    memcpy(this->m_data->textureCombos.Data(), comboList.data, 2 * comboList.count);
    this->m_data->textureCombos.count = comboList.count;

    memcpy(this->m_data->textureTransformCombos.Data(), transformList.data, 2 * transformList.count);
    this->m_data->textureTransformCombos.count = transformList.count;
}

// OFFSET: 0x8374A0
void CM2Shared::AssignBatchTextureComboIndices() {
    int32_t prevMaterialIndex = -1;

    for (uint32_t batchIndex = 0; batchIndex < this->m_skinData->batches.Count(); batchIndex++) {
        M2Batch* batch = &this->m_skinData->batches[batchIndex];

        if (batch->materialIndex == prevMaterialIndex) {
            continue;
        }

        prevMaterialIndex = batch->materialIndex;

        if (batch->textureCount == 2) {
            uint16_t lo = static_cast<uint8_t>(batch->textureComboIndex);
            uint16_t hi = static_cast<uint8_t>(batch->textureComboIndex >> 8);

            uint16_t comboIndex = 0;
            uint32_t limit = this->m_data->textureCombos.Count() - 1;

            if (limit != 0) {
                uint16_t* data = this->m_data->textureCombos.Data();
                uint32_t i = 0;

                for (;;) {
                    if (data[i] == lo && data[i + 1] == hi) {
                        comboIndex = i;
                        break;
                    }

                    i++;

                    if (i >= limit) {
                        break;
                    }
                }
            }

            uint16_t transformLo = static_cast<uint8_t>(batch->textureTransformComboIndex);
            uint16_t transformHi = static_cast<uint8_t>(batch->textureTransformComboIndex >> 8);

            batch->textureComboIndex = comboIndex;

            transformLo = transformLo ? transformLo - 1 : 0xFFFF;
            transformHi = transformHi ? transformHi - 1 : 0xFFFF;

            uint16_t transformIndex = 0;
            uint32_t transformLimit = this->m_data->textureTransformCombos.Count() - 1;

            if (transformLimit != 0) {
                uint16_t* data = this->m_data->textureTransformCombos.Data();
                uint32_t i = 0;

                for (;;) {
                    if (data[i] == transformLo && data[i + 1] == transformHi) {
                        transformIndex = i;
                        break;
                    }

                    i++;

                    if (i >= transformLimit) {
                        break;
                    }
                }
            }

            batch->textureTransformComboIndex = transformIndex;
        } else {
            uint16_t* data = this->m_data->textureCombos.Data();
            uint16_t* end = data + this->m_data->textureCombos.Count();
            uint16_t* entry = data;

            while (entry != end && *entry != batch->textureComboIndex) {
                entry++;
            }

            batch->textureComboIndex = entry - data;

            uint16_t transform = batch->textureTransformComboIndex
                                     ? batch->textureTransformComboIndex - 1
                                     : 0xFFFF;

            uint16_t* transformData = this->m_data->textureTransformCombos.Data();
            uint16_t* transformEnd = transformData + this->m_data->textureTransformCombos.Count();
            uint16_t* transformEntry = transformData;

            while (transformEntry != transformEnd && *transformEntry != transform) {
                transformEntry++;
            }

            batch->textureTransformComboIndex = transformEntry - transformData;
        }
    }
}

// OFFSET: 0x835F90
void CM2Shared::ConvertTextureComboEntry(M2ComboList* list, uint16_t packed, int32_t transform) {
    uint16_t lo = static_cast<uint8_t>(packed);
    uint16_t hi = static_cast<uint8_t>(packed >> 8);

    if (transform) {
        lo = lo ? lo - 1 : 0xFFFF;
        hi = hi ? hi - 1 : 0xFFFF;
    }

    uint32_t count = list->count;
    int32_t limit = static_cast<int32_t>(count) - 1;

    if (limit > 0) {
        for (int32_t i = 0; i < limit; i++) {
            if (list->data[i] == lo && list->data[i + 1] == hi) {
                return;
            }
        }
    }

    uint32_t insert = count;

    if (limit > 0) {
        for (int32_t i = 0; i < limit; i += 2) {
            if (list->data[i] > lo || (list->data[i] == lo && list->data[i + 1] > hi)) {
                insert = i;
                break;
            }
        }
    }

    list->count = count + 2;

    memmove(&list->data[insert + 2], &list->data[insert], 2 * (count - insert));

    list->data[insert] = lo;
    list->data[insert + 1] = hi;
}
