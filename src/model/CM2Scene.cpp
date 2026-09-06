#include "model/CM2Scene.hpp"
#include "gx/Shader.hpp"
#include "gx/Transform.hpp"
#include "model/CM2Cache.hpp"
#include "model/CM2Light.hpp"
#include "model/CM2Model.hpp"
#include "model/CM2SceneRender.hpp"
#include "model/CM2Shared.hpp"
#include "model/M2Internal.hpp"
#include "model/M2Sort.hpp"
#include <algorithm>
#include <cassert>
#include <tempest/Math.hpp>
#include <common/ObjectAlloc.hpp>
#include <common/processor/Processor.hpp>
#include <tempest/Intersect.hpp>

uint32_t CM2Scene::s_optFlags = 0xFFFFFFFF;

void CM2Scene::AnimateThread(void* arg) {
    // TODO
}

void CM2Scene::ComputeElementShaders(M2Element* element) {
    auto model = element->model;
    auto batch = element->batch;
    auto material = &model->m_shared->m_data->materials[batch->materialIndex];
    auto lighting = model->m_currentLighting;

    int32_t shaded;
    int32_t lightCount;

    if (material->flags & 0x1 || CM2SceneRender::s_shadedList[material->blendMode] == 0) {
        shaded = 0;
        lightCount = material->flags & 0x1 ? 0 : lighting->m_lightCount;
    } else {
        shaded = 1;
        lightCount = lighting->m_lightCount;
    }

    int32_t boneInfluences = element->skinSection->boneInfluences;

    int32_t v18;
    if (material->blendMode == M2BLEND_OPAQUE) {
        v18 = 0;
    } else if (material->blendMode == M2BLEND_ALPHA_KEY) {
        v18 = CMath::fuint(element->alpha * 224.0f);
    } else {
        v18 = 1;
    }

    int32_t v8 = 0;
    if (!(material->flags & 0x1) && !(material->flags & 0x100) && lighting->m_flags & 0x10) {
        // TODO
        // v8 = Sub873FF0();

        if (v8) {
            if (lighting->m_flags & 0x8) {
                v8 = 1;
            }

            if (element->type == 1) {
                v8 = 0;
            }
        }
    }

    int32_t v9 = v18 && (/* TODO !GxCaps().dword130 ||*/ v8);
    int32_t v10 = std::min(boneInfluences, 2);
    int32_t v11 = std::min(v8, 2);

    element->vertexPermute = shaded + 2 * (v11 + v10 + 2 * v11 + lightCount + 4 * (v11 + v10 + 2 * v11));
    element->pixelPermute = v8 + 4 * (CShaderEffect::s_usePcfFiltering + 2 * v9);

    // TODO
    // element->dword3C = v8;
}

int32_t CM2Scene::SortOpaque(uint32_t a, uint32_t b, const void* userArg) {
    auto elements = static_cast<const CM2Scene*>(userArg)->m_elements.Ptr();
    auto elementA = const_cast<M2Element*>(&elements[a]);
    auto elementB = const_cast<M2Element*>(&elements[b]);

    if (elementA->type < elementB->type) {
        return -1;
    }

    if (elementA->type > elementB->type) {
        return 1;
    }

    switch (elementA->type) {
        case 0:
        case 1:
            return CM2Scene::SortOpaqueGeoBatches(elementA, elementB);

        case 3:
            return CM2Scene::SortOpaqueRibbons(elementA, elementB);

        case 4:
            return CM2Scene::SortOpaqueParticles(elementA, elementB);

        default:
            return 0;
    }
}

int32_t CM2Scene::SortOpaqueGeoBatches(M2Element* elementA, M2Element* elementB) {
    auto modelA = elementA->model;
    auto dataA = modelA->m_shared->m_data;
    auto batchA = elementA->batch;
    auto modelB = elementB->model;
    auto dataB = modelB->m_shared->m_data;
    auto batchB = elementB->batch;

    if (elementA->type == 0) {
        if (batchA->materialLayer < batchB->materialLayer) {
            return -1;
        }

        if (batchA->materialLayer > batchB->materialLayer) {
            return 1;
        }

        if (elementA->effect && elementB->effect) {
            auto effectA = elementA->effect;
            auto effectB = elementB->effect;
            auto vertexShaderA = effectA->m_vertexShaders[elementA->vertexPermute];
            auto pixelShaderA = effectA->m_pixelShaders[elementA->pixelPermute];
            auto vertexShaderB = effectB->m_vertexShaders[elementB->vertexPermute];
            auto pixelShaderB = effectB->m_pixelShaders[elementB->pixelPermute];

            if (vertexShaderA < vertexShaderB) {
                return -1;
            }

            if (vertexShaderA > vertexShaderB) {
                return 1;
            }

            if (pixelShaderA < pixelShaderB) {
                return -1;
            }

            if (pixelShaderA > pixelShaderB) {
                return 1;
            }
        }

        if (modelA->m_shared < modelB->m_shared) {
            return -1;
        }

        if (modelA->m_shared > modelB->m_shared) {
            return 1;
        }

        if ((elementA->flags & 0x4) < (elementB->flags & 0x4)) {
            return -1;
        }

        if ((elementA->flags & 0x4) > (elementB->flags & 0x4)) {
            return 1;
        }

        if (modelA < modelB) {
            return -1;
        }

        if (modelA > modelB) {
            return 1;
        }

        if (elementA->skinSection->boneComboIndex < elementB->skinSection->boneComboIndex) {
            return -1;
        }

        if (elementA->skinSection->boneComboIndex > elementB->skinSection->boneComboIndex) {
            return 1;
        }
    }

    auto materialA = &dataA->materials[batchA->materialIndex];
    auto materialB = &dataB->materials[batchB->materialIndex];

    if (materialA->blendMode < materialB->blendMode) {
        return -1;
    }

    if (materialA->blendMode > materialB->blendMode) {
        return 1;
    }

    if ((materialA->flags & 0x1F) < (materialB->flags & 0x1F)) {
        return -1;
    }

    if ((materialA->flags & 0x1F) > (materialB->flags & 0x1F)) {
        return 1;
    }

    if (batchA->textureCount > 0 && batchB->textureCount > 0) {
        for (int32_t i = 0; i < std::min(batchA->textureCount, batchB->textureCount); i++) {
            auto textureIndexA = dataA->textureCombos[batchA->textureComboIndex];
            auto textureA = textureIndexA >= dataA->textures.Count() ? 0 : reinterpret_cast<intptr_t>(modelA->m_textures[textureIndexA]);
            auto textureIndexB = dataB->textureCombos[batchB->textureComboIndex];
            auto textureB = textureIndexB >= dataB->textures.Count() ? 0 : reinterpret_cast<intptr_t>(modelB->m_textures[textureIndexB]);

            if ((textureA - textureB) / sizeof(void*) < 0) {
                return -1;
            }

            if ((textureA - textureB) / sizeof(void*) > 0) {
                return 1;
            }
        }
    }

    if (batchA->textureCount < batchB->textureCount) {
        return -1;
    }

    if (batchA->textureCount > batchB->textureCount) {
        return 1;
    }

    if (batchA < batchB)  {
        return -1;
    }

    return batchA > batchB;
}

int32_t CM2Scene::SortOpaqueParticles(M2Element* elementA, M2Element* elementB) {
    // TODO
    return 0;
}

int32_t CM2Scene::SortOpaqueRibbons(M2Element* elementA, M2Element* elementB) {
    // TODO
    return 0;
}

int32_t CM2Scene::SortTransparent(uint32_t a, uint32_t b, const void* userArg) {
    auto elements = static_cast<const CM2Scene*>(userArg)->m_elements.Ptr();
    auto elementA = const_cast<M2Element*>(&elements[a]);
    auto elementB = const_cast<M2Element*>(&elements[b]);

    if (elementA->float10 > elementB->float10) {
        return -1;
    }

    if (elementA->float10 < elementB->float10) {
        return 1;
    }

    if ((elementA->flags & 0x1) > (elementB->flags & 0x1)) {
        return -1;
    }

    if ((elementA->flags & 0x1) < (elementB->flags & 0x1)) {
        return 1;
    }

    if (elementA->priorityPlane < elementB->priorityPlane) {
        return -1;
    }

    if (elementA->priorityPlane > elementB->priorityPlane) {
        return 1;
    }

    if (elementA->float14 > elementB->float14) {
        return -1;
    }

    if (elementA->float14 < elementB->float14) {
        return 1;
    }

    if ((CM2Scene::s_optFlags & 0x4000)
        && (elementA->type != elementB->type || elementA->model != elementB->model)
        && elementA->effect
        && elementB->effect
    ) {
        auto effectA = elementA->effect;
        auto effectB = elementB->effect;
        auto vertexShaderA = effectA->m_vertexShaders[elementA->vertexPermute];
        auto pixelShaderA = effectA->m_pixelShaders[elementA->pixelPermute];
        auto vertexShaderB = effectB->m_vertexShaders[elementB->vertexPermute];
        auto pixelShaderB = effectB->m_pixelShaders[elementB->pixelPermute];

        if (vertexShaderA < vertexShaderB) {
            return -1;
        }

        if (vertexShaderA > vertexShaderB) {
            return 1;
        }

        if (pixelShaderA < pixelShaderB) {
            return -1;
        }

        if (pixelShaderA > pixelShaderB) {
            return 1;
        }
    }

    if (elementA->model < elementB->model) {
        return -1;
    }

    if (elementA->model > elementB->model) {
        return 1;
    }

    if (elementA->type < elementB->type) {
        return -1;
    }

    if (elementA->type > elementB->type) {
        return 1;
    }

    if (elementA->type <= 2) {
        if (elementA->batch->materialLayer < elementB->batch->materialLayer) {
            return -1;
        }

        if (elementA->batch->materialLayer > elementB->batch->materialLayer) {
            return 1;
        }
    }

    if (!(CM2Scene::s_optFlags & 0x4000) || !elementA->effect || !elementB->effect) {
        return CM2Scene::SortOpaque(a, b, userArg);
    }

    auto effectA = elementA->effect;
    auto effectB = elementB->effect;
    auto vertexShaderA = effectA->m_vertexShaders[elementA->vertexPermute];
    auto pixelShaderA = effectA->m_pixelShaders[elementA->pixelPermute];
    auto vertexShaderB = effectB->m_vertexShaders[elementB->vertexPermute];
    auto pixelShaderB = effectB->m_pixelShaders[elementB->pixelPermute];

    if (vertexShaderA < vertexShaderB) {
        return -1;
    }

    if (vertexShaderA > vertexShaderB) {
        return 1;
    }

    if (pixelShaderA < pixelShaderB) {
        return -1;
    }

    if (pixelShaderA > pixelShaderB) {
        return 1;
    }

    return CM2Scene::SortOpaque(a, b, userArg);
}

// OFFSET: 0x81C9C0
void CM2Scene::AdvanceTime(uint32_t a2) {
    this->m_time += a2;

    this->m_cache->UpdateShared();
    this->m_cache->GarbageCollect(0);

    this->m_flags |= 0x4;
    this->m_timeDelta = a2;

    if (a2) {
        for (auto model = this->m_animateList; model; model = model->m_animateNext) {
            model->ProcessCallbacksRecursive();
        }
    }

    this->m_flags &= ~0x4;
}

// OFFSET: 0x821A20
void CM2Scene::Animate(const C3Vector& cameraPos) {
    this->m_frameStamp++;

    uint32_t optFlags = this->m_cache->m_flags & 0xE000;
    if (CM2Scene::s_optFlags != optFlags) {
        CM2Scene::s_optFlags = optFlags;
    }

    GxXformView(this->m_view);
    C3Vector invCameraPos = { -cameraPos.x, -cameraPos.y, -cameraPos.z };
    this->m_view.Translate(invCameraPos);
    this->m_viewInv = this->m_view.Inverse(this->m_view.Determinant());

    if (this->m_cache->m_flags & 0x4) {
        // In multithreaded mode, iteration over the animate list is interleaved:
        // - the current thread animates entries 0, 2, 4, ...
        // - the newly created thread animates entries 1, 3, 5, ...

        this->m_cache->BeginThread(CM2Scene::AnimateThread, this);

        CM2Model* nextModel;
        for (auto model = this->m_animateList; model; model = nextModel->m_animateNext) {
            if (!model->m_attachParent) {
                if (model->m_flag1000) {
                    C3Vector v222 = { 0.0f, 0.0f, 0.0f };
                    C3Vector v218 = { 1.0f, 1.0f, 1.0f };

                    model->AnimateMTSimple(&this->m_view, v218, v222, 1.0f, 1.0f);
                } else {
                    C3Vector v220 = { 0.0f, 0.0f, 0.0f };
                    C3Vector v221 = { 1.0f, 1.0f, 1.0f };

                    model->AnimateMT(&this->m_view, v221, v220, 1.0f, 1.0f);
                }
            }

            nextModel = model->m_animateNext;
            if (!nextModel) {
                break;
            }
        }

        this->m_cache->WaitThread();
    } else {
        for (auto model = this->m_animateList; model; model = model->m_animateNext) {
            if (!model->m_attachParent) {
                if (model->m_flag1000 != 0) {
                    C3Vector v222 = { 0.0f, 0.0f, 0.0f };
                    C3Vector v218 = { 1.0f, 1.0f, 1.0f };

                    model->AnimateMTSimple(&this->m_view, v218, v222, 1.0f, 1.0f);
                } else {
                    C3Vector v220 = { 0.0f, 0.0f, 0.0f };
                    C3Vector v221 = { 1.0f, 1.0f, 1.0f };

                    model->AnimateMT(&this->m_view, v221, v220, 1.0f, 1.0f);
                }
            }
        }
    }

    for (auto model = this->m_animateList; model; model = model->m_animateNext) {
        if (!model->m_attachParent) {
            model->AnimateST();
        }
    }

    while (this->m_animateList) {
        // TODO
        // - this is clearing out the animate list; why? something must reattach things to it...
        auto model = this->m_animateList;
        this->m_animateList = model->m_animateNext;
        model->m_animatePrev = nullptr;
        model->m_animateNext = nullptr;

        model->SetupLighting();
    }

    this->m_doodadElements.SetCount(0);
    for (int32_t i = 0; i < M2PASS_COUNT; i++) {
        this->m_passElements[i].SetCount(0);
    }

    this->m_elements.SetCount(0);
    int32_t elementIndex = 0;

    while (this->m_drawList) {
        auto model = this->m_drawList;
        this->m_drawList = model->m_drawNext;

        model->m_flag8 = 0;
        model->m_flag10000 = 0;
        model->m_drawPrev = nullptr;
        model->m_drawNext = nullptr;

        if (!model->IsDrawable(0, 0) || model->m_flag4000) {
            continue;
        }

        auto v19 = model->m_currentLighting;
        auto data = model->m_shared->m_data;
        auto v21 = v19->m_flags & 0x20;
        auto v22 = v19->m_flags & 0x40;

        if (v21 && v22) {
            // TODO
            // - liquid plane stuff
        }

        auto skinProfile = model->m_shared->m_skinData;
        auto v17 = (this->m_cache->m_flags & 0x1) == 0;

        int32_t v229;
        if (v17 || (model->m_flags & 0x1) != 0 || (v17 = (model->m_flag40) == 0, v229 = 1, v17)) {
            v229 = 0;
        }

        uint32_t batchCount;
        if (model->ptr2D0) {
            // TODO
            // batchCount = (model->ptr2D0 + 4);

            assert(false);
        } else {
            batchCount = skinProfile->batches.Count();
        }

        for (int32_t batchIndex = 0; batchIndex < batchCount; batchIndex++) {
            M2Batch* batch;
            M2SkinSection* skinSection;
            CShaderEffect* effect;
            int32_t v221;
            int32_t v222;

            if (model->ptr2D0) {
                // TODO
                // batch = &model->m_optGeo->batches[batchIndex];
                // skinSection = model->m_optGeo->skinSections[batch->skinSectionIndex];

                assert(false);
            } else {
                batch = &skinProfile->batches[batchIndex];
                skinSection = &model->m_shared->m_skinSections[batch->skinSectionIndex];

                if (!model->m_skinSections[batch->skinSectionIndex]) {
                    continue;
                }
            }

            if (batch->shader == 0x8000) {
                continue;
            }

            float alpha = model->alpha19C;

            if (batch->colorIndex < data->colors.Count()) {
                auto& color = model->m_colors[batch->colorIndex];
                alpha *= color.alphaTrack.currentValue;
            }

            if (batch->textureCount) {
                auto& textureWeight = model->m_textureWeights[data->textureWeightCombos[batch->textureWeightComboIndex]];
                alpha *= textureWeight.weightTrack.currentValue;
            }

            if (alpha < 0.000099999997f) {
                continue;
            }

            M2Material* material = &data->materials[batch->materialIndex];

            auto v17 = (batch->flags & 0x4) == 0;
            if (v17 || (v17 = this->m_projectTextureCallback == 0, v222 = 1, v17)) {
                v222 = 0;
            }

            M2Material* layerMaterial = batch->materialLayer
                ? &data->materials[batch->materialIndex - batch->materialLayer]
                : &data->materials[batch->materialIndex];

            if (layerMaterial->blendMode > 1 || (v221 = 0, alpha < 0.99998999f)) {
                v221 = 1;
            }

            if (model->ptr2D0) {
                // TODO
                // effect = model->m_optGeo->effects[batchIndex];

                assert(false);
            } else {
                effect = model->m_shared->m_batchShaders[batchIndex];
            }

            if (!effect) {
                continue;
            }

            auto element = this->m_elements.New();

            if (v222) {
                element->type = 1;
            } else if (!model->IsBatchDoodadCompatible(batch) || v221) {
                element->type = 0;
            } else {
                element->type = 2;
            }

            element->model = model;

            element->flags = 0x0;
            if (v221 == 1 && v21 && v22 && !v222) {
                element->flags |= 0x2;
            }
            if (model->ptr2D0) {
                element->flags |= 0x4;
            }

            element->alpha = alpha;
            element->index = batchIndex;
            element->priorityPlane = batch->priorityPlane;
            element->batch = batch;
            element->skinSection = skinSection;
            element->effect = effect;

            CM2Scene::ComputeElementShaders(element);

            float v58;

            if (v221 < 1) {
                element->float14 = model->float88;
                v58 = model->float88;
            } else if (data->flags & 0x10) {
                element->float14 = (skinSection->sortCenterPosition * model->m_boneMatrices[skinSection->centerBoneIndex]).SquaredMag();
                v58 = model->float88;
            } else {
                // TODO other sort position logic

                v58 = model->float88;
            }

            element->float10 = v58;

            if (element->type == 2) {
                // TODO
            } else if (v221 == 1) {
                if (v222) {
                    if (v22) {
                        *this->m_passElements[2].New() = elementIndex;
                    } else {
                        *this->m_passElements[1].New() = elementIndex;
                    }
                } else {
                    if (v21) {
                        *this->m_passElements[1].New() = elementIndex;
                    }

                    if (v22) {
                        *this->m_passElements[2].New() = elementIndex;
                    }
                }
            } else {
                *this->m_passElements[v221].New() = elementIndex;
            }

            elementIndex++;

            if (v229 && !v222 && v221 >= 1 && !(material->flags & 0x10)) {
                // TODO
            }
        }

        // TODO
        // - ribbons

        // TODO
        // - draw callbacks
    }

    M2HeapSort(CM2Scene::SortOpaque, this->m_passElements[0].Ptr(), this->m_passElements[0].Count(), this);
    M2HeapSort(CM2Scene::SortTransparent, this->m_passElements[1].Ptr(), this->m_passElements[1].Count(), this);
    M2HeapSort(CM2Scene::SortTransparent, this->m_passElements[2].Ptr(), this->m_passElements[2].Count(), this);

    // TODO sort additive particles
}

// OFFSET: 0x81F8F0
CM2Model* CM2Scene::CreateModel(const char* file, uint32_t a3) {
    if (!file) {
        return nullptr;
    }

    CM2Shared* shared = this->m_cache->CreateShared(file, a3);
    if (!shared) {
        shared = this->m_cache->CreateShared("Spells\\ErrorCube.mdx", 0);
    }

    CM2Model* model = nullptr;

    if (shared) {
        model = CM2Model::AllocModel(g_modelPool);

        if (model) {
            if (!model->Initialize(this, shared, nullptr, a3)) {
                //CM2Model::~CM2Model(g_modelPool, model);
                model = 0;
            }
        }

        shared->Release();
    }

    return model;
}

// OFFSET: 0x823CB0
void CM2Scene::Draw(M2PASS pass) {
    if (((1 << pass) & this->m_passMask) == 0)
        return;

    if (CM2Scene::s_optFlags != (this->m_cache->m_flags & 0xE000)) {
        CM2Scene::s_optFlags = this->m_cache->m_flags & 0xE000;
    }

    CM2SceneRender render(this);

    render.Draw(pass, this->m_elements.m_data, this->m_passElements[pass].m_data, this->m_passElements[pass].Count());

    if (pass == M2PASS_0) {
        render.Draw(pass, this->m_elements.m_data, this->m_doodadElements.m_data, this->m_doodadElements.Count());
    }
}

// OFFSET: 0x81E400
void CM2Scene::SelectLights(CM2Lighting* lighting) {
    for (auto light = this->m_lightList; light; light = light->m_lightNext) {
        lighting->AddLight(light);
    }

    if (!this->m_lightGrid) {
        return;
    }

    const CAaSphere& s = lighting->sphere4;

    int32_t x0 = static_cast<int32_t>(std::floor((s.c.x - s.r) * 0.05f - 0.5f)) & 0x3F;
    int32_t x1 = static_cast<int32_t>(std::floor((s.c.x + s.r) * 0.05f + 0.5f)) & 0x3F;
    int32_t y0 = static_cast<int32_t>(std::floor((s.c.y - s.r) * 0.05f - 0.5f)) & 0x3F;
    int32_t y1 = static_cast<int32_t>(std::floor((s.c.y + s.r) * 0.05f + 0.5f)) & 0x3F;

    int32_t y = y0;

    for (;;) {
        int32_t x = x0;

        for (;;) {
            CM2Light* light = this->m_lightGrid[x + 64 * y];

            while (light) {
                CM2Light* next = light->m_lightNext;

                if (!light->m_scene || light->m_stamp == this->m_frameStamp) {
                    lighting->AddLight(light);
                } else {
                    light->SetVisible(0);
                }

                light = next;
            }

            if (x == x1) {
                break;
            }

            x = (x + 1) & 0x3F;
        }

        if (y == y1) {
            break;
        }

        y = (y + 1) & 0x3F;
    }
}

// OFFSET: 0x823040
void CM2Scene::Release() {
    this->m_refCount--;
    if (this->m_refCount <= 0) {
        delete this;
    }
}

// OFFSET: 0x81F970
CM2Model* CM2Scene::DuplicateModel(CM2Model* a2, uint32_t a3) {
    if (!a2)
        return nullptr;

    CM2Model* model = CM2Model::AllocModel(g_modelPool);
    if (model) {
        if (!model->Initialize(this, a2->m_shared, a2, a3)) {
            model->~CM2Model();
            ObjectFree(*g_modelPool, model->m_handle);
            return nullptr;
        }
    }
    return model;
}

// OFFSET: 0x81D2C0
static void ComputeRegionBoundsTransform(const C44Matrix* boneMatrices, ubyte4 weights, ubyte4 indices, C44Matrix* out) {
    float weight = weights.b[0] * 0.0039215689f;
    const C44Matrix& first = boneMatrices[indices.b[0]];

    float r0[4] = { first.a0 * weight, first.a1 * weight, first.a2 * weight, first.a3 * weight };
    float r1[4] = { first.b0 * weight, first.b1 * weight, first.b2 * weight, first.b3 * weight };
    float r2[4] = { first.c0 * weight, first.c1 * weight, first.c2 * weight, first.c3 * weight };
    float r3[4] = { first.d0 * weight, first.d1 * weight, first.d2 * weight, first.d3 * weight };

    for (uint32_t i = 0; i + 1 < 4; i++) {
        uint8_t b = weights.b[i + 1];

        if (!b) {
            break;
        }

        float w = b * 0.0039215689f;
        const C44Matrix& m = boneMatrices[indices.b[i + 1]];

        r0[0] += m.a0 * w;
        r0[1] += m.a1 * w;
        r0[2] += m.a2 * w;
        r0[3] += m.a3 * w;
        r1[0] += m.b0 * w;
        r1[1] += m.b1 * w;
        r1[2] += m.b2 * w;
        r1[3] += m.b3 * w;
        r2[0] += m.c0 * w;
        r2[1] += m.c1 * w;
        r2[2] += m.c2 * w;
        r2[3] += m.c3 * w;
        r3[0] += m.d0 * w;
        r3[1] += m.d1 * w;
        r3[2] += m.d2 * w;
        r3[3] += m.d3 * w;
    }

    out->a0 = r0[0];
    out->a1 = r0[1];
    out->a2 = r0[2];
    out->a3 = 0.0f;
    out->b0 = r1[0];
    out->b1 = r1[1];
    out->b2 = r1[2];
    out->b3 = 0.0f;
    out->c0 = r2[0];
    out->c1 = r2[1];
    out->c2 = r2[2];
    out->c3 = 0.0f;
    out->d0 = r3[0];
    out->d1 = r3[1];
    out->d2 = r3[2];
    out->d3 = 1.0f;
}

// OFFSET: 0x81D3D0
static void Sub81D3D0(const C44Matrix* boneMatrices, ubyte4 weights, ubyte4 indices, C44Matrix* out) {
    float weight = weights.b[0] * 0.0039215689f;
    const C44Matrix& first = boneMatrices[indices.b[0]];

    out->a0 = first.a0 * weight;
    out->a1 = first.a1 * weight;
    out->a2 = first.a2 * weight;
    out->b0 = first.b0 * weight;
    out->b1 = first.b1 * weight;
    out->b2 = first.b2 * weight;
    out->c0 = first.c0 * weight;
    out->c1 = first.c1 * weight;
    out->c2 = first.c2 * weight;
    out->d0 = first.d0 * weight;
    out->d1 = first.d1 * weight;
    out->d2 = weight * first.d2;

    for (uint32_t i = 0; i + 1 < 4; i++) {
        uint8_t b = weights.b[i + 1];

        if (!b) {
            break;
        }

        float w = b * 0.0039215689f;
        const C44Matrix& m = boneMatrices[indices.b[i + 1]];

        out->a0 = m.a0 * w + out->a0;
        out->a1 = m.a1 * w + out->a1;
        out->a2 = m.a2 * w + out->a2;
        out->b0 = m.b0 * w + out->b0;
        out->b1 = m.b1 * w + out->b1;
        out->b2 = m.b2 * w + out->b2;
        out->c0 = m.c0 * w + out->c0;
        out->c1 = m.c1 * w + out->c1;
        out->c2 = m.c2 * w + out->c2;
        out->d0 = m.d0 * w + out->d0;
        out->d1 = m.d1 * w + out->d1;
        out->d2 = w * m.d2 + out->d2;
    }
}

// OFFSET: 0x81CF20
int32_t CM2Scene::ComputeRayDirAndLen(const C3Vector& start, const C3Vector& end, float dist, float* len, C3Vector* dir) {
    if (dist >= 0.0000099999997f) {
        float dx = end.x - start.x;
        float dy = end.y - start.y;
        float dz = end.z - start.z;

        float length = std::sqrt(dx * dx + dz * dz + dy * dy);
        *len = length;

        if (length >= 0.0000099999997f) {
            float inv = 1.0f / length;
            dir->x = dx * inv;
            dir->y = dy * inv;
            dir->z = inv * dz;
            return 1;
        }
    }

    for (CM2Model* model = this->m_hitTestList; model; model = model->m_hitTestNext) {
        *model->m_hitTestPrev = nullptr;
        model->m_hitTestPrev = nullptr;
    }

    this->m_flags &= ~0x2u;
    return 0;
}

// OFFSET: 0x81CAD0
void CM2Scene::AllocateSpaceForHitList() {
    uint32_t needed = 0;

    for (CM2Model* model = this->m_hitTestList; model; model = model->m_hitTestNext) {
        needed++;
    }

    if (needed <= this->m_hitCapacity) {
        return;
    }

    if (this->m_hitRecs) {
        SMemFree(this->m_hitRecs, "delete[]", -1, 0);
    }

    if (this->m_hitOrder) {
        SMemFree(this->m_hitOrder, "delete[]", -1, 0);
    }

    if (!this->m_hitCapacity) {
        this->m_hitCapacity = 1;
    }

    while (this->m_hitCapacity < needed) {
        this->m_hitCapacity *= 2;
    }

    this->m_hitRecs = static_cast<M2HitRec*>(SMemAlloc(sizeof(M2HitRec) * this->m_hitCapacity, __FILE__, __LINE__, 0));
    this->m_hitOrder = static_cast<uint32_t*>(SMemAlloc(sizeof(uint32_t) * this->m_hitCapacity, __FILE__, __LINE__, 0));
}

// OFFSET: 0x81CBC0
int32_t CM2Scene::SortHitNear(uint32_t a, uint32_t b, const void* userArg) {
    auto recs = static_cast<const M2HitRec*>(userArg);
    auto recA = &recs[a];
    auto recB = &recs[b];

    if (recB->tNear > recA->tNear) {
        return -1;
    }

    if (recB->tNear < recA->tNear) {
        return 1;
    }

    if (recB->tFar > recA->tFar) {
        return -1;
    }

    if (recB->tFar < recA->tFar) {
        return 1;
    }

    if (b > a) {
        return -1;
    }

    return b < a;
}

// OFFSET: 0x81CFF0
uint32_t CM2Scene::SphereTestModels(const C3Vector& start, const C3Vector& dir, float len, int32_t requireCurrentFrame) {
    uint32_t count = 0;

    for (CM2Model* model = this->m_hitTestList; model; model = model->m_hitTestNext) {
        *model->m_hitTestPrev = nullptr;
        model->m_hitTestPrev = nullptr;

        if (!model->m_loaded) {
            continue;
        }

        if (model->m_frameStamp == 0xFFFFFFFF) {
            continue;
        }

        if (requireCurrentFrame && model->m_frameStamp != this->m_frameStamp) {
            if (model->m_hitTestMode != 3 || model->m_attachParent) {
                continue;
            }

            model->matrixF4 = model->m_worldTransform * this->m_view;
        }

        M2Data* data = model->m_shared->m_data;
        M2Bounds* bounds;

        if (model->m_hitTestMode == 3) {
            bounds = &data->collisionBounds;
        } else {
            bounds = &data->sequences[model->m_bones[0].sequence.uint8].bounds;
        }

        if (std::fabs(bounds->radius) < 0.00000023841858f) {
            continue;
        }

        C3Vector center;
        center.x = (bounds->extent.t.x + bounds->extent.b.x) * 0.5f;
        center.y = (bounds->extent.t.y + bounds->extent.b.y) * 0.5f;
        center.z = 0.5f * (bounds->extent.t.z + bounds->extent.b.z);

        C3Vector world = model->matrixF4.TransformPoint(center);

        float px = world.x - start.x;
        float py = world.y - start.y;
        float pz = world.z - start.z;

        float along = dir.x * px + dir.z * pz + dir.y * py;
        float perpZ = dir.z * along - pz;
        float perpY = dir.y * along - py;
        float perpX = dir.x * along - px;

        float scale = model->matrixF4.a2 * model->matrixF4.a2 + model->matrixF4.a1 * model->matrixF4.a1 + model->matrixF4.a0 * model->matrixF4.a0;
        float radiusSq = bounds->radius * (scale * bounds->radius);
        float offsetSq = perpX * perpX + perpY * perpY + perpZ * perpZ;

        if (offsetSq > radiusSq) {
            continue;
        }

        float disc = radiusSq - offsetSq;

        if (along < 0.0f && along * along > disc) {
            continue;
        }

        float beyond = along - len;

        if (beyond > 0.0f && beyond * beyond > disc) {
            continue;
        }

        float half = std::sqrt(disc);
        float tNear = along - half;
        float tFar = along + half;
        float limit = len;

        if ((tNear >= 0.0f ? tNear : 0.0f) <= limit) {
            if (tNear < 0.0f) {
                tNear = 0.0f;
            }
        } else {
            tNear = len;
        }

        if ((tFar >= 0.0f ? tFar : 0.0f) <= limit) {
            if (tFar < 0.0f) {
                tFar = 0.0f;
            }

            limit = tFar;
        }

        M2HitRec* rec = &this->m_hitRecs[count];
        rec->tNear = tNear;
        rec->model = model;
        rec->tFar = limit;
        rec->priority = model->m_hitTestGroup;

        this->m_hitOrder[count] = count;
        count++;
    }

    return count;
}

// OFFSET: 0x81D9C0
void CM2Scene::TransformHitTestVertices(CM2Model* model, M2SkinProfile* skin, M2SkinSection* section, uint32_t pass, const C3Vector& dir, float dirDotStart) {
    C3Vector* out = this->m_hitVerts;
    M2Data* data = model->m_shared->m_data;

    uint32_t first = section->vertexStart;
    uint32_t last = first + section->vertexCount;

    for (uint32_t i = first; i < last; i++) {
        M2Vertex* vertex = &data->vertices[skin->vertices[i]];
        C44Matrix* bone = &model->m_boneMatrices[vertex->indices.b[0]];

        C3Vector point = bone->TransformPoint(vertex->position);
        float x;
        float y;

        if (pass) {
            float ny = bone->c1 * vertex->normal.z + bone->b1 * vertex->normal.y + bone->a1 * vertex->normal.x;
            float nz = bone->c2 * vertex->normal.z + bone->b2 * vertex->normal.y + bone->a2 * vertex->normal.x;
            float nx = bone->c0 * vertex->normal.z + bone->b0 * vertex->normal.y + vertex->normal.x * bone->a0 + point.x;

            point.x = nx;
            x = nx;
            point.y = ny + point.y;
            y = point.y;
            point.z = nz + point.z;
        } else {
            y = point.y;
            x = point.x;
        }

        float t = dir.z * point.z + dir.y * y + dir.x * x - dirDotStart;

        out->x = x - dir.x * t;
        out->y = y - dir.y * t;
        out->z = t;
        out++;
    }
}

// OFFSET: 0x81D680
void CM2Scene::TransformHitTestBone(CM2Model* model, M2SkinProfile* skin, M2SkinSection* section, uint32_t pass, const C3Vector& dir, float dirDotStart) {
    C3Vector* out = this->m_hitVerts;
    M2Data* data = model->m_shared->m_data;

    C44Matrix blended;
    uint32_t lastWeights = 0;
    uint32_t lastIndices = 0;

    uint32_t first = section->vertexStart;
    uint32_t last = first + section->vertexCount;

    for (uint32_t i = first; i < last; i++) {
        M2Vertex* vertex = &data->vertices[skin->vertices[i]];

        if (vertex->weights.u != lastWeights || vertex->indices.u != lastIndices) {
            lastWeights = vertex->weights.u;
            lastIndices = vertex->indices.u;
            ComputeRegionBoundsTransform(model->m_boneMatrices, vertex->weights, vertex->indices, &blended);
        }

        C3Vector point = blended.TransformPoint(vertex->position);
        float x;
        float y;

        if (pass) {
            float ny = vertex->normal.z * blended.c1 + vertex->normal.y * blended.b1 + vertex->normal.x * blended.a1;
            float nz = vertex->normal.z * blended.c2 + vertex->normal.y * blended.b2 + vertex->normal.x * blended.a2;
            float nx = vertex->normal.z * blended.c0 + vertex->normal.y * blended.b0 + vertex->normal.x * blended.a0 + point.x;

            point.x = nx;
            x = nx;
            point.y = ny + point.y;
            y = point.y;
            point.z = nz + point.z;
        } else {
            y = point.y;
            x = point.x;
        }

        float t = dir.z * point.z + dir.y * y + dir.x * x - dirDotStart;

        out->x = x - dir.x * t;
        out->y = y - dir.y * t;
        out->z = t;
        out++;
    }
}

// OFFSET: 0x81D830
void CM2Scene::TransformHitTestBoneVariant(CM2Model* model, M2SkinProfile* skin, M2SkinSection* section, uint32_t pass, const C3Vector& dir, float dirDotStart) {
    C3Vector* out = this->m_hitVerts;
    M2Data* data = model->m_shared->m_data;

    C44Matrix blended;
    uint32_t lastWeights = 0;
    uint32_t lastIndices = 0;

    uint32_t first = section->vertexStart;
    uint32_t last = first + section->vertexCount;

    for (uint32_t i = first; i < last; i++) {
        M2Vertex* vertex = &data->vertices[skin->vertices[i]];

        if (vertex->weights.u != lastWeights || vertex->indices.u != lastIndices) {
            lastWeights = vertex->weights.u;
            lastIndices = vertex->indices.u;
            Sub81D3D0(model->m_boneMatrices, vertex->weights, vertex->indices, &blended);
        }

        C3Vector point = blended.TransformPoint(vertex->position);
        float x;
        float y;

        if (pass) {
            float ny = vertex->normal.z * blended.c1 + vertex->normal.y * blended.b1 + vertex->normal.x * blended.a1;
            float nz = vertex->normal.z * blended.c2 + vertex->normal.y * blended.b2 + vertex->normal.x * blended.a2;
            float nx = vertex->normal.z * blended.c0 + vertex->normal.y * blended.b0 + vertex->normal.x * blended.a0 + point.x;

            point.x = nx;
            x = nx;
            point.y = ny + point.y;
            y = point.y;
            point.z = nz + point.z;
        } else {
            y = point.y;
            x = point.x;
        }

        float t = dir.z * point.z + dir.y * y + dir.x * x - dirDotStart;

        out->x = x - dir.x * t;
        out->y = y - dir.y * t;
        out->z = t;
        out++;
    }
}

// OFFSET: 0x81D510
M2HitRec* CM2Scene::IntersectHitTestTriangles(const uint16_t* begin, const uint16_t* end, uint32_t vertexStart, const C3Vector& start, uint32_t pass, M2HitRec* rec, float* t, M2HitRec* best) {
    for (const uint16_t* index = begin; index < end; index += 3) {
        C3Vector* a = &this->m_hitVerts[index[0] - vertexStart];
        C3Vector* b = &this->m_hitVerts[index[1] - vertexStart];
        C3Vector* c = &this->m_hitVerts[index[2] - vertexStart];

        float det = (c->y - a->y) * (b->x - a->x) - (c->x - a->x) * (b->y - a->y);

        if (std::fabs(det) < 0.0000099999997f) {
            continue;
        }

        float inv = 1.0f / det;
        float bx = b->x - start.x;
        float by = b->y - start.y;
        float cx = c->x - start.x;
        float cy = c->y - start.y;

        float u = (cy * bx - cx * by) * inv;

        if (u < 0.0f) {
            continue;
        }

        float ay = a->y - start.y;
        float ax = a->x - start.x;

        float v = (cx * ay - cy * ax) * inv;

        if (v < 0.0f) {
            continue;
        }

        float w = inv * (by * ax - bx * ay);

        if (w < 0.0f) {
            continue;
        }

        float z = v * b->z + w * c->z + a->z * u;

        if (z < 0.0f) {
            continue;
        }

        if ((pass && (!best || best->priority != rec->priority)) || z <= *t) {
            *t = z;
            best = rec;
        }
    }

    return best;
}

// OFFSET: 0x81DAF0
M2HitRec* CM2Scene::HitTestGeometry(CM2Model* model, uint32_t pass, const C3Vector& dir, float dirDotStart, const C3Vector& start, M2HitRec* rec, float* t, M2HitRec* best) {
    CM2Shared* shared = model->m_shared;
    M2SkinProfile* skin = shared->m_skinData;
    M2Data* data = shared->m_data;

    for (uint32_t i = 0; i < skin->batches.count; i++) {
        M2Batch* batch = &skin->batches[i];

        if (batch->materialLayer) {
            continue;
        }

        if (batch->flags & 0x8) {
            continue;
        }

        M2Material* material = &data->materials[batch->materialIndex];

        if (model->m_hitTestMode == 2 && !material->blendMode) {
            continue;
        }

        if (model->m_hitTestMode == 1 && material->blendMode && !(material->flags & 0x20)) {
            continue;
        }

        if (!model->m_skinSections[batch->skinSectionIndex]) {
            continue;
        }

        float alpha = model->alpha19C;

        if (batch->colorIndex < data->colors.count) {
            alpha = alpha * model->m_colors[batch->colorIndex].alphaTrack.currentValue;
        }

        if (batch->textureCount) {
            alpha = alpha * model->m_textureWeights[data->textureWeightCombos[batch->textureWeightComboIndex]].weightTrack.currentValue;
        }

        if (alpha <= 0.0f) {
            continue;
        }

        M2SkinSection* section = &skin->skinSections[batch->skinSectionIndex];

        if (section->vertexCount > this->m_hitVertCapacity) {
            if (this->m_hitVerts) {
                SMemFree(this->m_hitVerts, "delete[]", -1, 0);
            }

            if (!this->m_hitVertCapacity) {
                this->m_hitVertCapacity = 1;
            }

            while (this->m_hitVertCapacity < section->vertexCount) {
                this->m_hitVertCapacity *= 2;
            }

            auto verts = static_cast<C3Vector*>(SMemAlloc(sizeof(C3Vector) * this->m_hitVertCapacity, __FILE__, __LINE__, 0));

            if (verts) {
                for (uint32_t v = 0; v < this->m_hitVertCapacity; v++) {
                    verts[v].x = 0.0f;
                    verts[v].y = 0.0f;
                    verts[v].z = 0.0f;
                }
            }

            this->m_hitVerts = verts;
        }

        int32_t vendor;
        if (section->boneInfluences == 1) {
            this->TransformHitTestVertices(model, skin, section, pass, dir, dirDotStart);
        } else if (OsGetProcessorFeaturesEx(vendor) & 0x4) {
            this->TransformHitTestBone(model, skin, section, pass, dir, dirDotStart);
        } else {
            this->TransformHitTestBoneVariant(model, skin, section, pass, dir, dirDotStart);
        }

        const uint16_t* indices = skin->indices.Data() + section->indexStart;
        best = this->IntersectHitTestTriangles(indices, indices + section->indexCount, section->vertexStart, start, pass, rec, t, best);
    }

    return best;
}

// OFFSET: 0x81DD50
M2HitRec* CM2Scene::HitTestCollision(CM2Model* model, uint32_t pass, const C3Vector& dir, float dirDotStart, const C3Vector& start, M2HitRec* rec, float* t, M2HitRec* best) {
    M2Data* data = model->m_shared->m_data;

    if (data->collisionPositions.count > this->m_hitVertCapacity) {
        if (this->m_hitVerts) {
            SMemFree(this->m_hitVerts, "delete[]", -1, 0);
        }

        if (!this->m_hitVertCapacity) {
            this->m_hitVertCapacity = 1;
        }

        while (this->m_hitVertCapacity < data->collisionPositions.count) {
            this->m_hitVertCapacity *= 2;
        }

        auto verts = static_cast<C3Vector*>(SMemAlloc(sizeof(C3Vector) * this->m_hitVertCapacity, __FILE__, __LINE__, 0));

        if (verts) {
            for (uint32_t v = 0; v < this->m_hitVertCapacity; v++) {
                verts[v].x = 0.0f;
                verts[v].y = 0.0f;
                verts[v].z = 0.0f;
            }
        }

        this->m_hitVerts = verts;
    }

    for (uint32_t i = 0; i < data->collisionPositions.count; i++) {
        C3Vector point = model->matrixF4.TransformPoint(data->collisionPositions[i]);
        C3Vector* out = &this->m_hitVerts[i];

        float t2 = dir.y * point.y + dir.x * point.x + dir.z * point.z - dirDotStart;

        out->x = point.x - dir.x * t2;
        out->y = point.y - dir.y * t2;
        out->z = t2;
    }

    const uint16_t* indices = data->collisionIndices.Data();
    return this->IntersectHitTestTriangles(indices, indices + data->collisionIndices.count, 0, start, pass, rec, t, best);
}

// OFFSET: 0x81CAC0
void CM2Scene::BeginHitTest() {
    this->m_flags |= 2;
}

// OFFSET: 0x81DF10
CMapBaseObj* CM2Scene::EndHitTest(const C3Vector& start, const C3Vector& end, float* dist, int32_t allowSecondPass) {
    C3Vector dir = { 0.0f, 0.0f, 0.0f };
    float len = 0.0f;

    if (!this->ComputeRayDirAndLen(start, end, *dist, &len, &dir)) {
        return nullptr;
    }

    this->AllocateSpaceForHitList();

    uint32_t hitCount = this->SphereTestModels(start, dir, len, 1);
    M2HeapSort(CM2Scene::SortHitNear, this->m_hitOrder, hitCount, this->m_hitRecs);

    float dirDotStart = start.z * dir.z + start.y * dir.y + start.x * dir.x;
    float t = len;

    if (*dist < 1.0f) {
        t = *dist * len;
    }

    M2HitRec* best = nullptr;
    uint32_t pass = 0;

    while (1) {
        for (uint32_t i = 0; i < hitCount; i++) {
            M2HitRec* rec = &this->m_hitRecs[this->m_hitOrder[i]];

            if (pass) {
                if (best && (best->priority > rec->priority || (best->priority == rec->priority && t <= rec->tNear))) {
                    continue;
                }
            } else if (t <= rec->tNear) {
                break;
            }

            if (rec->model->m_hitTestMode == 3) {
                best = this->HitTestCollision(rec->model, pass, dir, dirDotStart, start, rec, &t, best);
            } else {
                best = this->HitTestGeometry(rec->model, pass, dir, dirDotStart, start, rec, &t, best);
            }
        }

        if (best || !allowSecondPass) {
            break;
        }

        pass++;
        t = len;

        if (pass >= 2) {
            break;
        }
    }

    this->m_flags &= ~0x2u;

    if (!best) {
        return nullptr;
    }

    *dist = t / len;

    CMapBaseObj* owner = nullptr;

    for (CM2Model* model = best->model; model; model = model->m_attachParent) {
        if (model->m_hitTestOwner) {
            owner = model->m_hitTestOwner;
            break;
        }
    }

    this->m_lastHit = *best;
    this->m_lastHitOwner = owner;

    return owner;
}

// OFFSET: 0x81E110
CMapBaseObj* CM2Scene::EndHitTestCollisionWorld(const C3Vector& start, const C3Vector& end, float* dist) {
    C3Vector dir = { 0.0f, 0.0f, 0.0f };
    float len = 0.0f;

    if (!this->ComputeRayDirAndLen(start, end, *dist, &len, &dir)) {
        return nullptr;
    }

    this->AllocateSpaceForHitList();

    C3Vector viewStart = this->m_view.TransformPoint(start);
    C33Matrix rotation(this->m_view);

    C3Vector viewDir;
    viewDir.x = rotation.a0 * dir.x + rotation.b0 * dir.y + rotation.c0 * dir.z;
    viewDir.y = rotation.c1 * dir.z + rotation.b1 * dir.y + rotation.a1 * dir.x;
    viewDir.z = dir.y * rotation.b2 + dir.z * rotation.c2 + dir.x * rotation.a2;

    float viewLen = std::sqrt(this->m_view.a2 * this->m_view.a2 + this->m_view.a1 * this->m_view.a1 + this->m_view.a0 * this->m_view.a0) * len;

    uint32_t hitCount = this->SphereTestModels(viewStart, viewDir, viewLen, 0);
    M2HeapSort(CM2Scene::SortHitNear, this->m_hitOrder, hitCount, this->m_hitRecs);

    float t = len;

    if (*dist < 1.0f) {
        t = len * *dist;
    }

    M2HitRec* best = nullptr;

    for (uint32_t i = 0; i < hitCount; i++) {
        M2HitRec* rec = &this->m_hitRecs[this->m_hitOrder[i]];

        if (t <= rec->tNear) {
            break;
        }

        CM2Model* model = rec->model;
        M2Data* data = model->m_shared->m_data;

        if (model->m_hitTestMode != 3) {
            continue;
        }

        float det = model->m_worldTransform.Determinant();
        C44Matrix inverse = model->m_worldTransform.Inverse(det);

        C3Vector localStart = inverse.TransformPoint(start);
        C3Vector localEnd = inverse.TransformPoint(end);

        float dx = localEnd.x - localStart.x;
        float dy = localEnd.y - localStart.y;
        float dz = localEnd.z - localStart.z;

        float invLen = 1.0f / std::sqrt(dy * dy + dz * dz + dx * dx);

        CRay ray;
        ray.origin = localStart;
        ray.dir.x = dx * invLen;
        ray.dir.y = dy * invLen;
        ray.dir.z = invLen * dz;

        uint16_t* first = data->collisionIndices.Data();
        uint16_t* last = first + data->collisionIndices.count;

        for (uint16_t* index = first; index < last; index += 3) {
            float hitT;

            if (NTempest::Intersect(&ray, data->collisionPositions.Data(), index, &hitT, nullptr, 0.000001f) && hitT >= 0.0f) {
                float scaled = hitT * invLen * len;

                if (t >= scaled) {
                    t = scaled;
                    best = rec;
                }
            }
        }
    }

    this->m_flags &= ~0x2u;

    if (!best) {
        return nullptr;
    }

    *dist = t / len;

    return best->model->m_hitTestOwner;
}
