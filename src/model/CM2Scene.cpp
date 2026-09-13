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
#include <util/Unimplemented.hpp>
#include "model/CParticleEmitter2.hpp"
#include <util/Byte.hpp>

uint32_t CM2Scene::s_optFlags = 0xFFFFFFFF;

static const int32_t s_m2BlendToGxBlend[7] = { 2, 2, 2, 10, 3, 4, 5 };

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
bool CM2Scene::Animate(const C3Vector& cameraPos) {
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
        this->m_cache->BeginThread(CM2Scene::AnimateThread, this);

        CM2Model* nextModel;

        for (auto model = this->m_animateList; model; model = nextModel->m_animateNext) {
            if (!model->m_attachParent) {
                C3Vector zero = { 0.0f, 0.0f, 0.0f };
                C3Vector one = { 1.0f, 1.0f, 1.0f };

                if (model->m_flag1000) {
                    model->AnimateMTSimple(&this->m_view, one, zero, 1.0f, 1.0f);
                } else {
                    model->AnimateMT(&this->m_view, one, zero, 1.0f, 1.0f);
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
                C3Vector zero = { 0.0f, 0.0f, 0.0f };
                C3Vector one = { 1.0f, 1.0f, 1.0f };

                if (model->m_flag1000) {
                    model->AnimateMTSimple(&this->m_view, one, zero, 1.0f, 1.0f);
                } else {
                    model->AnimateMT(&this->m_view, one, zero, 1.0f, 1.0f);
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

    uint32_t elementIndex = 0;

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

        auto lighting = model->m_currentLighting;
        auto data = model->m_shared->m_data;

        int32_t aboveWater = lighting->m_flags & 0x20;
        int32_t belowWater = lighting->m_flags & 0x40;

        if (aboveWater && belowWater) {
            C3Vector center;
            center.x = (data->bounds.extent.b.x + data->bounds.extent.t.x) * 0.5f;
            center.y = (data->bounds.extent.t.y + data->bounds.extent.b.y) * 0.5f;
            center.z = 0.5f * (data->bounds.extent.t.z + data->bounds.extent.b.z);

            float scale = sqrt((model->matrixF4.a2 * model->matrixF4.a2) + (model->matrixF4.a1 * model->matrixF4.a1) + (model->matrixF4.a0 * model->matrixF4.a0));
            float radius = scale * data->bounds.radius;

            C3Vector world = center * model->matrixF4;
            float distance = (lighting->m_liquidPlane.n.y * world.y) + (lighting->m_liquidPlane.n.z * world.z) + (world.x * lighting->m_liquidPlane.n.x) + lighting->m_liquidPlane.d;

            aboveWater = -radius <= distance;
            belowWater = radius >= distance;

            if ((this->m_cache->m_flags & 0x2) == 0 && aboveWater && belowWater) {
                int32_t submerged = this->m_liquidTypeId != 0;
                aboveWater = this->m_liquidTypeId == 0;
                belowWater = submerged;
            }
        }

        auto skinProfile = model->m_shared->m_skinData;
        int32_t optGeo = model->ptr2D0 != nullptr;

        int32_t allowShadowPass;

        if ((this->m_cache->m_flags & 0x1) == 0 || (model->m_flags & 0x1) != 0 || !model->m_flag40) {
            allowShadowPass = 0;
        } else {
            allowShadowPass = 1;
        }

        uint32_t batchCount;

        if (optGeo) {
            WHOA_UNIMPLEMENTED(0);
        } else {
            batchCount = skinProfile->batches.Count();
        }

        for (uint32_t batchIndex = 0; batchIndex < batchCount; batchIndex++) {
            M2Batch* batch;
            M2SkinSection* skinSection;

            if (optGeo) {
                WHOA_UNIMPLEMENTED(0);
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
                alpha = alpha * model->m_colors[batch->colorIndex].alphaTrack.currentValue;
            }

            if (batch->textureCount) {
                alpha = alpha * model->m_textureWeights[data->textureWeightCombos[batch->textureWeightComboIndex]].weightTrack.currentValue;
            }

            if (alpha < 0.000099999997f) {
                continue;
            }

            M2Material* material = &data->materials[batch->materialIndex];

            int32_t projected;

            if ((batch->flags & 0x4) == 0 || this->m_projectTextureCallback == nullptr) {
                projected = 0;
            } else {
                projected = 1;
            }

            M2Material* layerMaterial = batch->materialLayer ? &data->materials[batch->materialIndex - batch->materialLayer] : &data->materials[batch->materialIndex];

            int32_t transparent;

            if (layerMaterial->blendMode > 1 || alpha < 0.99998999f) {
                transparent = 1;
            } else {
                transparent = 0;
            }

            CShaderEffect* effect;

            if (optGeo) {
                WHOA_UNIMPLEMENTED(0);
            } else {
                effect = model->m_shared->m_batchShaders[batchIndex];
            }

            if (!effect) {
                continue;
            }

            auto element = this->m_elements.New();

            if (!element) {
                return 0;
            }

            if (projected) {
                element->type = 1;
            } else if (!model->IsBatchDoodadCompatible(batch) || transparent) {
                element->type = 0;
            } else {
                element->type = 2;
            }

            element->model = model;
            element->flags = 0x0;

            if (transparent == 1 && aboveWater && belowWater && !projected) {
                element->flags |= 0x2;
            }

            if (optGeo) {
                element->flags |= 0x4;
            }

            element->alpha = alpha;
            element->index = batchIndex;
            element->priorityPlane = batch->priorityPlane;
            element->batch = batch;
            element->skinSection = skinSection;
            element->effect = effect;

            CM2Scene::ComputeElementShaders(element);

            float sortDepth;

            if (transparent < 1) {
                element->float14 = model->float88;
                sortDepth = model->float88;
            } else if (data->flags & 0x10) {
                C3Vector center = skinSection->sortCenterPosition * model->m_boneMatrices[skinSection->centerBoneIndex];
                element->float14 = (center.z * center.z) + (center.y * center.y) + (center.x * center.x);
                sortDepth = model->float88;
            } else {
                C44Matrix& bone = model->m_boneMatrices[skinSection->centerBoneIndex];
                float depth;

                if (batch->flags & 0x1) {
                    C3Vector p = skinSection->sortCenterPosition * bone;
                    C3Vector n = p;
                    float lengthSq = (p.x * p.x) + (p.z * p.z) + (p.y * p.y);

                    if (lengthSq > 2.384185791015625e-07f) {
                        float invLength = 1.0f / sqrt(lengthSq);
                        n.x = p.x * invLength;
                        n.y = invLength * p.y;
                        n.z = invLength * p.z;
                    }

                    float boneScale = sqrt((bone.a2 * bone.a2) + (bone.a1 * bone.a1) + (bone.a0 * bone.a0)) * skinSection->sortRadius;

                    p.x = p.x - (n.x * boneScale);
                    p.y = p.y - (n.y * boneScale);
                    p.z = p.z - (n.z * boneScale);

                    depth = (p.y * p.y) + (p.x * p.x) + (p.z * p.z);

                    if (p.z < 0.0f) {
                        depth = -depth;
                    }
                } else if (batch->flags & 0x2) {
                    C3Vector p = skinSection->sortCenterPosition * bone;
                    C3Vector n = p;
                    float lengthSq = (p.z * p.z) + (p.y * p.y) + (p.x * p.x);

                    if (lengthSq > 2.384185791015625e-07f) {
                        float invLength = 1.0f / sqrt(lengthSq);
                        n.x = p.x * invLength;
                        n.y = invLength * p.y;
                        n.z = invLength * p.z;
                    }

                    float boneScale = sqrt((bone.a2 * bone.a2) + (bone.a1 * bone.a1) + (bone.a0 * bone.a0)) * skinSection->sortRadius;

                    p.x = (n.x * boneScale) + p.x;
                    p.y = (n.y * boneScale) + p.y;
                    p.z = (n.z * boneScale) + p.z;

                    depth = (p.y * p.y) + (p.z * p.z) + (p.x * p.x);

                    if (p.z < 0.0f) {
                        depth = -depth;
                    }
                } else {
                    C3Vector p = skinSection->sortCenterPosition * bone;
                    depth = (p.z * p.z) + (p.y * p.y) + (p.x * p.x);
                }

                element->float14 = depth;

                if (!allowShadowPass || projected || (material->flags & 0x10) != 0) {
                    sortDepth = element->float14;
                } else {
                    sortDepth = model->float88;
                }
            }

            element->float10 = sortDepth;

            if (element->type == 2) {
                *this->m_doodadElements.New() = elementIndex;
            } else if (transparent == 1) {
                if (projected) {
                    if (belowWater) {
                        *this->m_passElements[2].New() = elementIndex;
                    } else {
                        *this->m_passElements[1].New() = elementIndex;
                    }
                } else {
                    if (aboveWater) {
                        *this->m_passElements[1].New() = elementIndex;
                    }

                    if (belowWater) {
                        *this->m_passElements[2].New() = elementIndex;
                    }
                }
            } else {
                *this->m_passElements[transparent].New() = elementIndex;
            }

            elementIndex++;

            if (allowShadowPass && !projected && transparent >= 1 && (material->flags & 0x10) == 0) {
                element->float14 = 3.4028235e38f;

                auto shadow = this->m_elements.New();

                if (!shadow) {
                    return 0;
                }

                memcpy(shadow, &this->m_elements[elementIndex - 1], sizeof(M2Element));
                shadow->flags |= 0x1;

                if (aboveWater) {
                    *this->m_passElements[1].New() = elementIndex;
                }

                if (belowWater) {
                    *this->m_passElements[2].New() = elementIndex;
                }

                elementIndex++;
            }
        }

        //for (uint32_t ribbonIndex = 0; ribbonIndex < data->ribbons.Count(); ribbonIndex++) {
        //    if (model->m_ribbonEmitters[ribbonIndex]->IsDead()) {
        //        continue;
        //    }
        //
        //    auto& ribbon = data->ribbons[ribbonIndex];
        //    auto& modelRibbon = model->m_ribbons[ribbonIndex];
        //
        //    float alpha = model->float198;
        //
        //    if (ribbon.alphaTrack.sequenceTimes.Count()) {
        //        alpha = alpha * modelRibbon.alphaTrack.currentValue;
        //    }
        //
        //    M2Material* material = &data->materials[data->materialLookup[ribbon.materialIndices[0]]];
        //
        //    auto element = this->m_elements.New();
        //
        //    if (!element) {
        //        continue;
        //    }
        //
        //    element->alpha = alpha;
        //    element->index = ribbonIndex;
        //    element->type = 3;
        //    element->model = model;
        //    element->flags = 0x0;
        //    element->priorityPlane = ribbon.priorityPlane;
        //    element->float10 = model->float88;
        //    element->float14 = model->float88;
        //    element->effect = nullptr;
        //    element->vertexPermute = -1;
        //    element->pixelPermute = -1;
        //    element->uint3C = 0;
        //
        //    if (material->blendMode > 1 || alpha < 0.99998999f) {
        //        if (aboveWater) {
        //            *this->m_passElements[1].New() = elementIndex;
        //        } else {
        //            *this->m_passElements[2].New() = elementIndex;
        //        }
        //    } else {
        //        *this->m_passElements[0].New() = elementIndex;
        //    }
        //
        //    elementIndex++;
        //}

        //if (model->m_drawCallback) {
        //    auto element = this->m_elements.New();
        //
        //    if (element) {
        //        element->type = 5;
        //        element->alpha = 1.0f;
        //        element->model = model;
        //        element->flags = 0x0;
        //        element->index = 0;
        //        element->priorityPlane = 0;
        //        element->float10 = model->float88;
        //        element->float14 = model->float88;
        //        element->effect = nullptr;
        //        element->vertexPermute = -1;
        //        element->pixelPermute = -1;
        //        element->uint3C = 0;
        //
        //        if (model->f_flags & 0x20) {
        //            *this->m_passElements[0].New() = elementIndex;
        //        } else if (aboveWater) {
        //            *this->m_passElements[1].New() = elementIndex;
        //        } else {
        //            *this->m_passElements[2].New() = elementIndex;
        //        }
        //
        //        elementIndex++;
        //    }
        //}
    }

    uint32_t particleElementCount = 0;

    while (this->m_particleList) {
        auto model = this->m_particleList;
        this->m_particleList = model->m_particleNext;
        model->m_particlePrev = nullptr;
        model->m_particleNext = nullptr;

        if (!model->IsDrawable(0, 0)) {
            continue;
        }

        auto lighting = model->m_currentLighting;
        auto data = model->m_shared->m_data;

        int32_t aboveWater = lighting->m_flags & 0x20;

        if (aboveWater && (lighting->m_flags & 0x40) != 0) {
            C3Vector center;
            center.x = (data->bounds.extent.t.x + data->bounds.extent.b.x) * 0.5f;
            center.y = (data->bounds.extent.t.y + data->bounds.extent.b.y) * 0.5f;
            center.z = 0.5f * (data->bounds.extent.t.z + data->bounds.extent.b.z);

            float scale = sqrt((model->matrixF4.a2 * model->matrixF4.a2) + (model->matrixF4.a1 * model->matrixF4.a1) + (model->matrixF4.a0 * model->matrixF4.a0));
            float radius = scale * data->bounds.radius;

            C3Vector world = center * model->matrixF4;
            float distance = (lighting->m_liquidPlane.n.y * world.y) + (lighting->m_liquidPlane.n.z * world.z) + (lighting->m_liquidPlane.n.x * world.x) + lighting->m_liquidPlane.d;

            aboveWater = -radius <= distance;
        }

        for (uint32_t i = 0; i < data->particles.Count(); i++) {
            auto emitter = model->m_particleEmitters[i];

            if ((model->f_flags & 0x2000) != 0 && (emitter->m_flags & 0x200) != 0) {
                continue;
            }

            if ((emitter->m_flags & 0x2000000) != 0) {
                continue;
            }

            if (!model->m_particles[i].m_active) {
                continue;
            }

            float alpha = model->float198;

            if (alpha < 0.000099999997f) {
                continue;
            }

            auto& particle = data->particles[i];
            C3Vector world = particle.position * model->m_boneMatrices[particle.boneIndex];
            float depth = (world.z * world.z) + (world.y * world.y) + (world.x * world.x);

            this->QueueParticleElement(emitter, model, depth, alpha, aboveWater, &elementIndex, &particleElementCount);

            for (uint32_t c = 0; c < emitter->m_childEmitterCount; c++) {
                this->QueueParticleElement(emitter->m_childEmitters[c], model, depth, alpha, aboveWater, &elementIndex, &particleElementCount);
            }
        }
    }

    uint32_t doodadCount = this->m_doodadElements.Count();

    //if (doodadCount > 1) {
    //    memset(CM2Scene::s_doodadHashSlots, 0xFF, sizeof(CM2Scene::s_doodadHashSlots));
    //
    //    for (uint32_t i = 0; i < doodadCount; i++) {
    //        uint32_t index = this->m_doodadElements[i];
    //        M2Element* element = &this->m_elements[index];
    //        uint32_t start = CM2Scene::HashElement(element) % 0xFB;
    //        uint32_t slot = start;
    //
    //        while (true) {
    //            slot++;
    //
    //            if (slot >= 0xFB) {
    //                slot = 0;
    //            }
    //
    //            int32_t occupant = CM2Scene::s_doodadHashSlots[slot];
    //
    //            if (occupant == -1 || slot == start) {
    //                CM2Scene::s_doodadHashSlots[slot] = index;
    //                break;
    //            }
    //
    //            if (!CM2Scene::InterpolateAnimationFrame(index, occupant, this)) {
    //                break;
    //            }
    //        }
    //
    //        element->doodadKey = CM2Scene::s_doodadHashSlots[slot];
    //    }
    //}

    //M2HeapSort(CM2Scene::SortDoodadProxy, this->m_doodadElements.Ptr(), doodadCount, this);

    uint32_t writeIndex = 0;
    uint32_t readIndex = 0;

    //while (readIndex < doodadCount) {
    //    uint32_t first = this->m_doodadElements[readIndex];
    //    this->m_doodadElements[writeIndex] = first;
    //    writeIndex++;
    //
    //    uint32_t scan = readIndex + 1;
    //
    //    while (scan < doodadCount) {
    //        uint32_t candidate = this->m_doodadElements[scan];
    //
    //        if (this->m_elements[first].doodadKey != this->m_elements[candidate].doodadKey) {
    //            break;
    //        }
    //
    //        this->m_doodadElements[writeIndex] = candidate;
    //        writeIndex++;
    //        scan++;
    //    }
    //
    //    if (scan - readIndex <= 1) {
    //        this->m_elements[first].type = 0;
    //        CM2Scene::ComputeElementShaders(&this->m_elements[first]);
    //        *this->m_passElements[0].New() = first;
    //        writeIndex--;
    //        readIndex = readIndex + 1;
    //    } else {
    //        this->m_elements[first].doodadRunLength = scan - readIndex;
    //        readIndex = scan;
    //    }
    //}

    if (writeIndex > this->m_doodadElements.Count() && writeIndex > this->m_doodadElements.m_alloc) {
        this->m_doodadElements.ReallocData(writeIndex);
    }

    this->m_doodadElements.m_count = writeIndex;

    M2HeapSort(CM2Scene::SortOpaque, this->m_passElements[0].Ptr(), this->m_passElements[0].Count(), this);
    M2HeapSort(CM2Scene::SortTransparent, this->m_passElements[1].Ptr(), this->m_passElements[1].Count(), this);
    M2HeapSort(CM2Scene::SortTransparent, this->m_passElements[2].Ptr(), this->m_passElements[2].Count(), this);

    if (LOBYTE(this->m_cache->m_flags) < 0 && particleElementCount > 1) {
        this->SortAdditiveParticleElements(1);
        this->SortAdditiveParticleElements(2);
    }

    return 1;
}

// OFFSET: 0x81CA20
int32_t GxBlendToM2Blend(int32_t gxBlend) {
    switch (gxBlend) {
    case 1:
        return 1;
    case 2:
        return 2;
    case 10:
        return 3;
    case 3:
        return 4;
    case 4:
        return 5;
    case 5:
        return 6;
    default:
        return 0;
    }
}

// OFFSET: 0x81F9E0
void CM2Scene::SortAdditiveParticleElements(int32_t pass) {
    auto& passElements = this->m_passElements[pass];

    uint32_t group = 0;
    int32_t previousAdditive = 0;

    for (uint32_t i = 0; i < passElements.Count(); i++) {
        M2Element* element = &this->m_elements[passElements[i]];
        M2Data* data = element->model->m_shared->m_data;

        int32_t blendMode = 0;

        switch (element->type) {
        case 0:
        case 1:
        case 2: {
            blendMode = data->materials[element->batch->materialIndex].blendMode;
            break;
        }

        case 3: {
            blendMode = data->materials[data->ribbons[element->index].materialIndices[0]].blendMode;
            break;
        }

        case 4: {
            if (this->m_cache->m_flags & 0x100) {
                blendMode = 4;
            } else {
                blendMode = GxBlendToM2Blend(element->emitter->m_materialBlend);
            }

            break;
        }

        default: {
            break;
        }
        }

        int32_t blend = s_m2BlendToGxBlend[blendMode];
        int32_t additive = blend == 3 || blend == 10;

        if (!additive || !previousAdditive) {
            group++;
        }

        previousAdditive = additive;
        element->additiveGroup = group;
    }

    M2HeapSort(CM2Scene::SortAdditiveParticles, passElements.Ptr(), passElements.Count(), this);
}

// OFFSET: 0x821930
void CM2Scene::QueueParticleElement(CParticleEmitter2* emitter, CM2Model* model, float depth, float alpha, int32_t aboveWater, uint32_t* elementIndex, uint32_t* particleCount) {
    if (!emitter->HasLiveParticles()) {
        return;
    }

    if (emitter->m_hasModel == 1) {
        return;
    }

    M2Element* element = this->m_elements.New();

    if (!element) {
        return;
    }

    element->alpha = alpha;
    element->model = model;
    element->type = 4;
    element->flags = 0x0;
    element->emitter = emitter;
    element->priorityPlane = emitter->m_priorityPlane;
    element->float10 = model->float88;
    element->effect = nullptr;
    element->float14 = depth;
    element->vertexPermute = -1;
    element->pixelPermute = -1;
    element->uint3C = 0;

    int32_t blend = emitter->m_materialBlend;

    if (blend == 10 || blend == 3) {
        (*particleCount)++;
    }

    if (blend <= 1 && alpha >= 0.999989986f) {
        *this->m_passElements[0].New() = *elementIndex;
        (*elementIndex)++;
        return;
    }

    if (aboveWater == 0 || (emitter->m_flags & 0x40000) != 0) {
        *this->m_passElements[2].New() = *elementIndex;
    } else {
        *this->m_passElements[1].New() = *elementIndex;
    }

    (*elementIndex)++;
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

// OFFSET: 0x47BF20
int32_t PointerDiff4(const void* a, const void* b) {
    return (reinterpret_cast<intptr_t>(a) - reinterpret_cast<intptr_t>(b)) >> 2;
}

// OFFSET: 0x81CA80
int32_t ParticleRenderStateKey(const CParticleMaterial* material) {
    int32_t key = 4;

    if ((material->flags & 0x1) == 0) {
        key = 5;
    }

    if ((material->flags & 0x2) == 0) {
        key |= 0x2;
    }

    if ((material->flags & 0x4) == 0) {
        key |= 0x10;
    }

    return key;
}

// OFFSET: 0x81F0E0
int32_t CM2Scene::SortAdditiveParticles(uint32_t a, uint32_t b, const void* userArg) {
    auto scene = static_cast<const CM2Scene*>(userArg);

    const M2Element* left = &scene->m_elements[a];
    const M2Element* right = &scene->m_elements[b];

    if (left->additiveGroup > right->additiveGroup) {
        return 1;
    }

    if (left->additiveGroup < right->additiveGroup) {
        return -1;
    }

    if (left->type == 4 || right->type == 4) {
        if (left->type > right->type) {
            return -1;
        }

        if (left->type < right->type) {
            return 1;
        }
    }

    if (left->type != 4) {
        return CM2Scene::SortTransparent(a, b, userArg);
    }

    CParticleEmitter2* leftEmitter = left->emitter;
    CParticleEmitter2* rightEmitter = right->emitter;

    CParticleMaterial leftMaterial;
    leftMaterial.blend = leftEmitter->m_materialBlend;
    leftMaterial.flags = leftEmitter->m_materialFlags;

    CParticleMaterial rightMaterial;
    rightMaterial.blend = rightEmitter->m_materialBlend;
    rightMaterial.flags = rightEmitter->m_materialFlags;

    if (leftMaterial.blend < rightMaterial.blend) {
        return -1;
    }

    if (leftMaterial.blend > rightMaterial.blend) {
        return 1;
    }

    uint32_t leftKey = ParticleRenderStateKey(&leftMaterial);
    uint32_t rightKey = ParticleRenderStateKey(&rightMaterial);

    if (leftKey < rightKey) {
        return -1;
    }

    if (leftKey > rightKey) {
        return 1;
    }

    return PointerDiff4(leftEmitter->m_texture, rightEmitter->m_texture);
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
            bounds = &data->sequences[model->m_bones[0].sequence.m_sequenceIndex].bounds;
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
void* CM2Scene::EndHitTest(const C3Vector& start, const C3Vector& end, float* dist, int32_t allowSecondPass) {
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

    void* owner = nullptr;

    for (CM2Model* model = best->model; model; model = model->m_attachParent) {
        if (reinterpret_cast<intptr_t>(model->m_hitTestOwner) != -1) {
            owner = model->m_hitTestOwner;
            break;
        }
    }

    this->m_lastHit = *best;
    this->m_lastHitOwner = owner;

    return owner;
}

// OFFSET: 0x81E110
void* CM2Scene::EndHitTestCollisionWorld(const C3Vector& start, const C3Vector& end, float* dist) {
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
