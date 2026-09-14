#include "componentcore/CCharacterComponent.hpp"

#include <algorithm>

#include <common/ObjectAlloc.hpp>
#include <common/Processor.hpp>
#include "console/Types.hpp"
#include "console/CVar.hpp"
#include "model/CM2Model.hpp"
#include "db/Db.hpp"
#include "componentcore/ComponentUtils.hpp"
#include <tempest/Random.hpp>
#include "componentcore/Texture.hpp"
#include <gx/Device.hpp>
#include <gx/Texture.hpp>
#include "gx/texture/CBLPFile.hpp"
#include <event/Event.hpp>
#include "model/CM2Scene.hpp"
#include "model/CM2Shared.hpp"
#include <util/Filesystem.hpp>

CVar* CCharacterComponent::g_componentTextureLevelVar = nullptr;
CVar* CCharacterComponent::g_componentThreadVar = nullptr;
CVar* CCharacterComponent::g_componentCompressVar = nullptr;

CCharacterComponent* CCharacterComponent::m_activePlayerComponent = nullptr;
uint32_t* CCharacterComponent::s_heap = nullptr;
uint32_t CCharacterComponent::s_chrVarArrayLength = 0;
st_race* CCharacterComponent::s_chrVarArray = nullptr;
uint32_t* CCharacterComponent::s_characterFacialHairStylesList = nullptr;
EGxTexFormat CCharacterComponent::s_gxFormatHigh;
EGxTexFormat CCharacterComponent::s_gxFormat;
uint32_t CCharacterComponent::s_textureSize;

char CCharacterComponent::s_path[260];
char CCharacterComponent::s_path2[260];
char CCharacterComponent::s_buffer[260];
char* CCharacterComponent::s_pathEnd;
CStatus CCharacterComponent::s_status;

ITEM_FUNC CCharacterComponent::s_itemFunc[];
uint32_t CCharacterComponent::s_mipLevels;
PREP_FUNC CCharacterComponent::s_prepFunc[];
CompSectionInfo CCharacterComponent::s_sectionInfo[];

bool CCharacterComponent::s_bInRenderPrep;
MipBits* CCharacterComponent::s_textureBuffer;

static STORM_EXPLICIT_LIST(CCharacterComponent, m_link) s_queue;

CompSectionInfo CCharacterComponent::s_sectionInfoRaw[] = {
    { { 0,   0   }, { 256, 128 } }, // SECTION_ARM_UPPER
    { { 0,   128 }, { 256, 128 } }, // SECTION_ARM_LOWER
    { { 0,   256 }, { 256, 64  } }, // SECTION_HAND
    { { 256, 0,  }, { 256, 128 } }, // SECTION_TORSO_UPPER
    { { 256, 128 }, { 256, 64  } }, // SECTION_TORSO_LOWER
    { { 256, 192 }, { 256, 128 } }, // SECTION_LEG_UPPER
    { { 256, 320 }, { 256, 128 } }, // SECTION_LEG_LOWER
    { { 256, 448 }, { 256, 64  } }, // SECTION_FOOT
    { { 0,   320 }, { 256, 64  } }, // SECTION_HEAD_UPPER
    { { 0,   384 }, { 256, 128 } }, // SECTION_HEAD_LOWER
};

int32_t s_itemPriority[NUM_ITEM_SLOT][NUM_COMPONENT_SECTIONS] = {
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 0, -1, 0, 0, -1, -1, -1, -1, -1 },
    { 1, 1, -1, 1, 1, 1, 1, -1, -1, -1 },
    { -1, -1, -1, -1, 5, 2, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, 0, 0, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, 2, 0, -1, -1 },
    { -1, 2, -1, -1, -1, -1, -1, -1, -1, -1 },
    { -1, 3, 0, -1, -1, -1, -1, -1, -1, -1 },
    { -1, -1, -1, 4, 4, -1, -1, -1, -1, -1 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

const char* s_componentSections[] = {
    "ArmUpperTexture",
    "ArmLowerTexture",
    "HandTexture",
    "TorsoUpperTexture",
    "TorsoLowerTexture",
    "LegUpperTexture",
    "LegLowerTexture",
    "FootTexture"
};

const char* s_fileDecorations[] = {
    "M",
    "F",
    "U"
};

// OFFSET: 0x4ED8C0
bool ItemDisplayHasGeosetGroup(uint32_t displayId, int32_t groupIndex, ItemDisplayInfoRec* rec) {
    if (!displayId) {
        return false;
    }

    ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(displayId);

    if (!found) {
        return false;
    }

    *rec = *found;

    return rec->m_geosetGroup[groupIndex] != 0;
}

// OFFSET: 0x4EFB80
bool ItemDisplayHasGeosetGroup(uint32_t displayId, int32_t groupIndex) {
    if (!displayId) {
        return false;
    }

    ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(displayId);

    return found && found->m_geosetGroup[groupIndex] != 0;
}

// OFFSET: 0x4EA9E0
void ReplaceParticleColor(int32_t particleColorId, CM2Model* model) {
    if (!model || !particleColorId) {
        return;
    }

    ParticleColorRec* rec = g_particleColorDB.GetRecord(particleColorId);

    if (!rec) {
        for (uint32_t i = 0; i < 3; i++) {
            model->ReplaceParticleColor(i + 11, CImVector(0xFF00FF00), CImVector(0xFF00FF00), CImVector(0xFF00FF00));
        }

        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        model->ReplaceParticleColor(i + 11, CImVector(rec->m_start[i]), CImVector(rec->m_mid[i]), CImVector(rec->m_end[i]));
    }
}

static bool ComponentVarHandler(CVar*, const char*, const char*, void*) {
    return true;
}

// OFFSET: 0x401FF0
void CCharacterComponent::Initialize() {
    CCharacterComponent::g_componentTextureLevelVar = CVar::Register(
        "componentTextureLevel",
        "Number of mip levels used for character component textures",
        0x1,
        "8",
        &ComponentVarHandler,
        CATEGORY::DEBUG,
        0,
        nullptr,
        false);

    CCharacterComponent::g_componentThreadVar = CVar::Register(
        "componentThread",
        "Multi thread character component processing",
        0x1,
        "1",
        &ComponentVarHandler,
        CATEGORY::DEBUG,
        0,
        nullptr,
        false);

    CCharacterComponent::g_componentCompressVar = CVar::Register(
        "componentCompress",
        "Character component texture compression",
        0x1,
        "1",
        &ComponentVarHandler,
        CATEGORY::DEBUG,
        0,
        nullptr,
        false);

    uint32_t mipLevels = CCharacterComponent::g_componentTextureLevelVar->GetInt();
    int32_t useThreads = CCharacterComponent::g_componentThreadVar->GetInt();
    int32_t useCompression = CCharacterComponent::g_componentCompressVar->GetInt();

    if (!useThreads || OsGetProcessorCount() < 2) {
        useThreads = 0;

        if (mipLevels > 8) {
            mipLevels = 8;
        }

        useCompression = 0;
    }

    if (mipLevels > 9) {
        mipLevels = 9;
    }

    if (mipLevels < 7) {
        mipLevels = 7;
    }

    CCharacterComponent::Initialize(GxTex_Rgb565, mipLevels, useThreads, useCompression);
}

// OFFSET: 0x4F1A20
void CCharacterComponent::Initialize(EGxTexFormat format, uint32_t textureLevel, int32_t useThreads, int32_t useCompression) {
    CCharacterComponent::s_heap = static_cast<uint32_t*>(ALLOC(sizeof(uint32_t)));
    if (CCharacterComponent::s_heap) {
        *CCharacterComponent::s_heap = ObjectAllocAddHeap(sizeof(CCharacterComponent), 32, "CCharacterComponent", true);
    }

    s_pathEnd = s_path;
    //s_pathEnd2 = path;
    CCharacterComponent::s_prepFunc[0] = &CCharacterComponent::RenderPrepAU;
    CCharacterComponent::s_prepFunc[1] = &CCharacterComponent::RenderPrepAL;
    CCharacterComponent::s_prepFunc[2] = &CCharacterComponent::RenderPrepHA;
    CCharacterComponent::s_prepFunc[8] = &CCharacterComponent::RenderPrepHU;
    CCharacterComponent::s_prepFunc[9] = &CCharacterComponent::RenderPrepHL;
    CCharacterComponent::s_prepFunc[3] = &CCharacterComponent::RenderPrepTU;
    CCharacterComponent::s_prepFunc[4] = &CCharacterComponent::RenderPrepTL;
    CCharacterComponent::s_prepFunc[5] = &CCharacterComponent::RenderPrepLU;
    CCharacterComponent::s_prepFunc[6] = &CCharacterComponent::RenderPrepLL;
    CCharacterComponent::s_prepFunc[7] = &CCharacterComponent::RenderPrepFO;
    CCharacterComponent::s_itemFunc[0] = &CCharacterComponent::UpdateItemAU;
    CCharacterComponent::s_itemFunc[1] = &CCharacterComponent::UpdateItemAL;
    CCharacterComponent::s_itemFunc[2] = &CCharacterComponent::UpdateItemHA;
    CCharacterComponent::s_itemFunc[8] = &CCharacterComponent::UpdateItemHU;
    CCharacterComponent::s_itemFunc[9] = &CCharacterComponent::UpdateItemHL;
    CCharacterComponent::s_itemFunc[3] = &CCharacterComponent::UpdateItemTU;
    CCharacterComponent::s_itemFunc[4] = &CCharacterComponent::UpdateItemTL;
    CCharacterComponent::s_itemFunc[5] = &CCharacterComponent::UpdateItemLU;
    CCharacterComponent::s_itemFunc[6] = &CCharacterComponent::UpdateItemLL;
    CCharacterComponent::s_itemFunc[7] = &CCharacterComponent::UpdateItemFO;

    uint32_t mipLevels = std::min(std::max(textureLevel, 6u), 9u);

    // Cap mip levels to 8 if compression isn't enabled
    if (!useCompression && mipLevels > 8) {
        mipLevels = 8;
    }
    CCharacterComponent::s_mipLevels = mipLevels;
    CCharacterComponent::s_textureSize = 1 << mipLevels;

    // Scale section info to match mip levels
    for (int32_t i = 0; i < NUM_COMPONENT_SECTIONS; i++) {
        auto& info = CCharacterComponent::s_sectionInfo[i];
        auto& infoRaw = CCharacterComponent::s_sectionInfoRaw[i];

        info.pos.x = infoRaw.pos.x >> (9 - mipLevels);
        info.pos.y = infoRaw.pos.y >> (9 - mipLevels);
        info.size.x = infoRaw.size.x >> (9 - mipLevels);
        info.size.y = infoRaw.size.y >> (9 - mipLevels);
    }

    //bn_TextureCacheResetLoadCount_0();
    auto v6 = 2 * g_chrRacesDB.GetNumRecords() + 2;
    CCharacterComponent::s_chrVarArrayLength = v6;
    BuildComponentArray(v6, &CCharacterComponent::s_chrVarArray);
    CountFacialFeatures(v6, &CCharacterComponent::s_characterFacialHairStylesList);
    //CCharacterComponent::s_bComponentThread = a3;
    //CCharacterComponent::s_bComponentCompression = 0;
    CCharacterComponent::s_gxFormat = format;
    //if (a3) {
    //    if (a4) {
    //        CCharacterComponent::s_bComponentCompression = 1;
    //        CCharacterComponent::s_gxFormat = 6;
    //    }
    //    _cfltcvt_init_8();
    //} else {
    //    dword_B6B4B8 = 0;
    //    dword_B6B4BC = 0;
    //    dword_B6B4C0 = 0;
    //    dword_B6B4C4 = 0;
    //    dword_B6B4C8 = 0;
    //    dword_B6B4CC = 0;
    //    dword_B6B4D0 = 0;
    //    dword_B6B4D4 = 0;
    //    dword_B6B4D8 = 0;
    //    dword_B6B4DC = 0;
    //}
    CCharacterComponent::s_gxFormatHigh = GxTex_Argb8888;
    CCharacterComponent::s_textureBuffer = nullptr;
    //dword_B6B86C = 0;
    //dword_B6B868 = 0;
    CCharacterComponent::s_textureBuffer = TextureAllocMippedImg(PIXEL_ARGB8888, CCharacterComponent::s_textureSize, CCharacterComponent::s_textureSize);
    //if (CCharacterComponent::s_bComponentCompression)
    //    dword_B6B86C = bn_TextureAllocMippedImg(0, dword_B6B5FC, dword_B6B5FC);
    //if (CCharacterComponent::s_bComponentThread && CCharacterComponent::s_bComponentCompression)
    //    dword_B6B868 = bn_TextureAllocMippedImg(2, dword_B6B5FC, dword_B6B5FC);
    EventRegisterEx(EVENT_ID_POLL, &CCharacterComponent::Update, 0, 0.0);
}

// OFFSET: 0x4F18F0
int32_t CCharacterComponent::Update(const void*, void*) {
    bool hasContext = false;
    if (g_theGxDevicePtr)
        hasContext = g_theGxDevicePtr->CapsHasContext(-1);

    for (CCharacterComponent* component = s_queue.Head(); component;) {
        CCharacterComponent* next = s_queue.Next(component);

        if (component->m_request) {
            //if (!HasContext)
            //    goto LABEL_24;
            //if ((*v1 & 2) == 0)
            //    goto LABEL_24;
            //CCharacterComponent::ProcessFinishedRequest(m_next);
            component->m_link.Unlink();
        } else {
            bool variationsUpdated = component->VariationsLoaded(0);
            bool itemsUpdated = component->ItemsLoaded(0);
            if (!variationsUpdated || !itemsUpdated) {
                component = next;
                continue;
            }

            component->m_flags |= 8;
            //if (CCharacterComponent::s_bComponentThread) {
            //    CCharacterComponent::CreateComponentRequest(m_next);
            //} else {
                if (hasContext) {
                    component->RenderPrepSections();
                    component->m_link.Unlink();
                }
            //}
        }

        component = next;
    }

    //if (CCharacterComponent::s_bComponentThread)
    //    CCharacterComponent::ProcessComponentFinishedList(0);
    return 1;
}

// OFFSET: 0x4F0980
CCharacterComponent* CCharacterComponent::AllocComponent() {
    uint32_t handle;
    void* memory;

    if (ObjectAlloc(*CCharacterComponent::s_heap, &handle, &memory, false) && memory) {
        auto component = new (memory) CCharacterComponent();
        component->m_heapIndex = handle;
        return component;
    }
    return nullptr;
}

// OFFSET: 0x4F16C0
void CCharacterComponent::FreeComponent(CCharacterComponent* component) {
    uint32_t handle = component->m_heapIndex;
    component->~CCharacterComponent();
    ObjectFree(*CCharacterComponent::s_heap, handle);
}

// OFFSET: 0x4F2320
bool CCharacterComponent::Destroy() {
    if (CCharacterComponent::m_activePlayerComponent) {
        CCharacterComponent::FreeComponent(CCharacterComponent::m_activePlayerComponent);
        CCharacterComponent::m_activePlayerComponent = nullptr;
    }

    // if (CCharacterComponent::s_bComponentThread) {
    //     CCharacterComponent::ThreadDestroy();
    // }

    if (CCharacterComponent::s_textureBuffer) {
        TextureFreeMippedImg(
            CCharacterComponent::s_textureBuffer,
            PIXEL_ARGB8888,
            CCharacterComponent::s_textureSize,
            CCharacterComponent::s_textureSize
        );

        CCharacterComponent::s_textureBuffer = nullptr;
    }

    //if (dword_B6B86C) {
    //    TextureFreeMippedImg(dword_B6B86C, 0, s_textureSize, s_textureSize);
    //    dword_B6B86C = 0;
    //}
    //if (dword_B6B868) {
    //    TextureFreeMippedImg(dword_B6B868, 2, s_textureSize, s_textureSize);
    //    dword_B6B868 = 0;
    //}

    if (CCharacterComponent::s_chrVarArray) {
        for (uint32_t race = 0; race < CCharacterComponent::s_chrVarArrayLength; race++) {
            for (int32_t v = 0; v < 5; v++) {
                CharVariationList& list = CCharacterComponent::s_chrVarArray[race].m_variation[v];

                for (int32_t i = 0; i < list.numVariations; i++) {
                    if (list.variation[i].color) {
                        STORM_FREE(list.variation[i].color);
                    }
                }

                if (list.variation) {
                    STORM_FREE(list.variation);
                }
            }
        }

        STORM_FREE(CCharacterComponent::s_chrVarArray);
    }

    CCharacterComponent::s_chrVarArray = nullptr;
    CCharacterComponent::s_chrVarArrayLength = 0;

    TextureCacheDestroy();

    if (CCharacterComponent::s_characterFacialHairStylesList) {
        STORM_FREE(CCharacterComponent::s_characterFacialHairStylesList);
    }

    if (CCharacterComponent::s_heap) {
        STORM_FREE(CCharacterComponent::s_heap);
    }

    CCharacterComponent::s_heap = nullptr;

    // TODO CGPlayer_C::s_displayId = 0;

    EventUnregisterEx(
        EVENT_ID_POLL,
        reinterpret_cast<EVENTHANDLERFUNC>(CCharacterComponent::Update),
        nullptr,
        -1
    );

    return true;
}

// OFFSET: 0x4E9D50
void CCharacterComponent::ValidateComponentData(ComponentData* data, COMPONENT_CONTEXT context) {
    //v3 = a2;
    //SelectionFromContext = GetSelectionFromContext(a2, a1->m_preferences.classId);
    //skinId = a1->m_preferences.skinId;
    //classId = a1->m_preferences.classId;
    //v5 = SelectionFromContext;
    //genderId = a1->m_preferences.genderId;
    //raceId = a1->m_preferences.raceId;
    //v24 = SelectionFromContext;
    //if (!ComponentValidateSkin(CCharacterComponent::s_chrVarArray, raceId, genderId, classId, skinId, a2)) {
    //    NumSkins = CCharacterComponent::GetNumSkins(
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        a1->m_preferences.classId,
    //        a2);
    //    if (NumSkins <= 0)
    //        a1->m_preferences.skinId = 0;
    //    else
    //        a1->m_preferences.skinId = ComponentGetSectionsRecord(
    //            a1->m_preferences.raceId,
    //            a1->m_preferences.genderId,
    //            a1->m_preferences.skinId % NumSkins,
    //            v5);
    //}
    //if (!ComponentValidateFace(
    //        CCharacterComponent::s_chrVarArray,
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        a1->m_preferences.classId,
    //        a1->m_preferences.skinId,
    //        a1->m_preferences.faceId,
    //        a2)) {
    //    NumFacesForSkin = CCharacterComponent::GetNumFacesForSkin(
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        a1->m_preferences.classId,
    //        a1->m_preferences.skinId,
    //        a2);
    //    if (NumFacesForSkin <= 0)
    //        a1->m_preferences.faceId = 0;
    //    else
    //        a1->m_preferences.faceId = ComponentGetNumVariations_0(
    //            a1->m_preferences.raceId,
    //            a1->m_preferences.genderId,
    //            a1->m_preferences.skinId,
    //            a1->m_preferences.faceId % NumFacesForSkin,
    //            v5);
    //}
    //if (!ComponentValidateHair(
    //        CCharacterComponent::s_chrVarArray,
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        a1->m_preferences.classId,
    //        a1->m_preferences.hairColorId,
    //        a1->m_preferences.hairStyleId,
    //        a2)) {
    //    NumHairStylesForColor = CCharacterComponent::GetNumHairStylesForColor(
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        a1->m_preferences.classId,
    //        a1->m_preferences.hairColorId,
    //        a2);
    //    if (NumHairStylesForColor > 0) {
    //        HairStyleVariation = maybe_CCharacterComponent__GetHairStyleVariation(
    //            a1->m_preferences.raceId,
    //            a1->m_preferences.genderId,
    //            a1->m_preferences.hairColorId,
    //            a1->m_preferences.hairStyleId % NumHairStylesForColor,
    //            v5);
//LABEL_25:
    //        a1->m_preferences.hairStyleId = HairStyleVariation;
    //        goto LABEL_26;
    //    }
    //    NumVariations = ComponentGetNumVariations(
    //        CCharacterComponent::s_chrVarArray,
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        3);
    //    v11 = alloca(4 * NumVariations);
    //    v12 = 0;
    //    v23 = &v22;
    //    a1a = 0;
    //    if (NumVariations <= 0)
    //        goto LABEL_24;
    //    do {
    //        NumHairColorsForStyle = CCharacterComponent::GetNumHairColorsForStyle(
    //            a1->m_preferences.raceId,
    //            a1->m_preferences.genderId,
    //            a1->m_preferences.classId,
    //            v12,
    //            a2);
    //        v23[v12] = NumHairColorsForStyle;
    //        if (NumHairColorsForStyle > 0)
    //            a1a = (a1a + 1);
    //        ++v12;
    //    } while (v12 < NumVariations);
    //    if (a1a <= 0) {
//LABEL_24:
    //        v5 = v24;
    //        v3 = a2;
    //        HairStyleVariation = 0;
    //        a1->m_preferences.hairColorId = 0;
    //        goto LABEL_25;
    //    }
    //    v14 = 0;
    //    while (1) {
    //        v15 = v23;
    //        if (v23[v14] > 0 && a1->m_preferences.hairStyleId % a1a == v14)
    //            break;
    //        if (++v14 >= NumVariations)
    //            goto LABEL_23;
    //    }
    //    a1->m_preferences.hairStyleId = v14;
//LABEL_23:
    //    ColorByFlags = maybe_CCharacterComponent__GetColorByFlags(
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        a1->m_preferences.hairStyleId,
    //        a1->m_preferences.hairColorId % *(v15 + 4 * a1->m_preferences.hairStyleId),
    //        v24);
    //    v5 = v24;
    //    v3 = a2;
    //    a1->m_preferences.hairColorId = ColorByFlags;
    //}
//LABEL_26:
    //if (!ComponentValidateFacialFeature(
    //        CCharacterComponent::s_chrVarArray,
    //        CCharacterComponent::s_characterFacialHairStylesList,
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        a1->m_preferences.classId,
    //        a1->m_preferences.hairColorId,
    //        a1->m_preferences.facialHairId,
    //        v3)) {
    //    NumFacialFeaturesForHairColor = CCharacterComponent::GetNumFacialFeaturesForHairColor(
    //        a1->m_preferences.raceId,
    //        a1->m_preferences.genderId,
    //        a1->m_preferences.classId,
    //        a1->m_preferences.hairColorId,
    //        v3);
    //    if (NumFacialFeaturesForHairColor <= 0)
    //        a1->m_preferences.facialHairId = 0;
    //    else
    //        a1->m_preferences.facialHairId = CCharacterComponent::GetNthFacialFeatureIndex(
    //            a1->m_preferences.raceId,
    //            a1->m_preferences.genderId,
    //            a1->m_preferences.classId,
    //            a1->m_preferences.hairColorId,
    //            a1->m_preferences.facialHairId,
    //            a1->m_preferences.facialHairId % NumFacialFeaturesForHairColor,
    //            v5);
    //}
}

void CCharacterComponent::UpdateBaseTexture(EGxTexCommand cmd, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CCharacterComponent* component = reinterpret_cast<CCharacterComponent*>(userArg);

    switch (cmd) {
    case GxTex_Lock:
        if (!CCharacterComponent::s_bInRenderPrep)
            component->RenderPrepAll();
        break;
    case GxTex_Latch:
        if (component->m_gxTexFormat == GxTex_Dxt1) {
            STORM_ASSERT(false);
            //BaseTextureSection = maybe_GetBaseTextureSection(0);
            //v9 = BaseTextureSection * (width >> 2);
            //*texelStrideInBytes = v9;
            //if (v9 < BaseTextureSection)
            //    *texelStrideInBytes = BaseTextureSection;
        } else {
            texelStrideInBytes = 4 * width;
        }
        if (component->m_request) {
            STORM_ASSERT(false);
            //*texels = *(*m_request[2] + 4 * miplevel);
        } else {
            texels = CCharacterComponent::s_textureBuffer->mip[mipLevel];
            //v11 = dword_B6B86C;
            //if (userArg->m_gxTexFormat != GxTex_Dxt1)
            //    v11 = CCharacterComponent::s_textureBuffer;
            //*texels = v11[miplevel];
        }
        break;
    case GxTex_3:
        //if ((userArg->m_data.m_flags & 2) == 0) {
        //    sub_4E8E50(userArg);
        //    sub_4E8E20(userArg, -1);
        //}
        break;
    }
}

// OFFSET: 0x4EFBE0
CCharacterComponent::CCharacterComponent() {
    this->m_link.m_prevlink = nullptr;
    this->m_link.m_next = nullptr;
    this->m_data = ComponentData();
    memset(this->m_baseVariation, 0, sizeof(this->m_baseVariation));
    for (uint32_t i = 0; i < 10; i++) {
        for (uint32_t j = 0; j < 7; j++) {
            this->m_section[i].layerTex[j] = nullptr;
            this->m_section[i].layerItemDisplayId[j] = 0;
        }
        this->m_section[i].layerMask = 0;
    }
    this->m_flags |= 7;
    this->m_dirtySections = -1;
    this->m_request = nullptr;
    this->m_baseSkinTexture = nullptr;
    this->m_gxTexFormat = CCharacterComponent::s_gxFormat;
    memset(this->m_itemDisplayID, 0, sizeof(this->m_itemDisplayID));
    memset(this->m_itemSlotForAttachSlot, -1, sizeof(this->m_itemSlotForAttachSlot));
    this->m_flags &= ~0x20u;
    this->m_data.m_model = nullptr;
}

// OFFSET: 0x4EFCA0
CCharacterComponent::~CCharacterComponent() {
    if (this->m_request) {
        //*m_request &= ~1u;
        this->m_request = nullptr;
    }
    if (this->m_baseSkinTexture) {
        CGxTex* gxTex = TextureGetGxTex(this->m_baseSkinTexture, 1, 0);
        //GxTexSetCannotUpdate(gxTex);
        HandleClose(this->m_baseSkinTexture);
        this->m_baseSkinTexture = nullptr;
    }
    if (this->m_data.m_model) {
        this->m_data.m_model->Release();
        this->m_data.m_model = nullptr;
    }
    this->m_link.Unlink();

    for (int32_t s = NUM_COMPONENT_SECTIONS - 1; s >= 0; s--) {
        CharacterSection& section = this->m_section[s];

        for (int32_t i = 0; i < 7; i++) {
            if (section.layerTex[i]) {
                TextureCacheDestroyTexture(section.layerTex[i]);
            }

            section.layerTex[i] = nullptr;
            section.layerItemDisplayId[i] = 0;
        }
    }

    for (int32_t v = 4; v >= 0; v--) {
        CharacterBaseVariation& variation = this->m_baseVariation[v];

        for (int32_t i = 0; i < 3; i++) {
            if (variation.m_texture[i]) {
                TextureCacheDestroyTexture(variation.m_texture[i]);
            }

            variation.m_texture[i] = nullptr;
        }
    }

    this->m_link.Unlink();
}

// OFFSET: 0x4EA150
void CCharacterComponent::ReplaceHairTexture(uint32_t varitationIndex, const char* a3) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_HAIR, varitationIndex, this->m_data.m_preferences.hairColorID))
        return;

    auto v4 = CCharacterComponent::s_chrVarArray[2 * this->m_data.m_preferences.raceID + this->m_data.m_preferences.sexID].m_variation[3].variation[varitationIndex].color[this->m_data.m_preferences.hairColorID]->m_textureName[0];
    if (!*v4)
        return;

    SStrCopy(s_path, v4);
    auto texture = TextureCreate(s_path, &CCharacterComponent::s_status);
    if (texture) {
        this->m_data.m_model->ReplaceTexture(6, texture);
        HandleClose(texture);
    }
}

// OFFSET: 0x4EA1F0
void CCharacterComponent::LoadBaseVariation(COMPONENT_VARIATIONS variation, uint32_t textureIndex, uint32_t variationIndex, uint32_t colorIndex, COMPONENT_SECTIONS section, const char* a7) {
    auto texture = this->m_baseVariation[variation].m_texture[textureIndex];
    if (texture) {
        //TextureCacheDestroyTexture(texture);
        this->m_baseVariation[variation].m_texture[textureIndex] = nullptr;
    }

    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, variation, variationIndex, colorIndex))
        return;

    auto v4 = CCharacterComponent::s_chrVarArray[2 * this->m_data.m_preferences.raceID + this->m_data.m_preferences.sexID].m_variation[variation].variation[variationIndex].color[colorIndex]->m_textureName[textureIndex];
    if (*v4) {
        SStrCopy(s_path, v4);
        CACHEENTRY* texture = TextureCacheCreateTexture(s_path);
        this->m_baseVariation[variation].m_texture[textureIndex] = texture;
        //if (!Texture)
        //    NOP("**** Unable to Load Texture %s\n");
    }

    this->m_dirtySections |= 1 << section;
    if (this->m_request) {
        //*m_request &= ~1u;
        this->m_request = nullptr;
    }
    this->m_flags &= ~8;
}

// OFFSET: 0x4EA2F0
void CCharacterComponent::SetHairColor(uint32_t colorId, bool a3, const char* a4) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_HAIR, this->m_data.m_preferences.hairStyleID, colorId))
        return;

    this->m_data.m_preferences.hairColorID = colorId;

    auto raceRec = g_chrRacesDB.GetRecord(this->m_data.m_preferences.raceID);
    if (!this->m_data.m_preferences.sexID && !this->m_data.m_preferences.hairStyleID && (raceRec->m_flags & 8) != 0)
        this->ReplaceHairTexture(1, a4);
    this->ReplaceHairTexture(this->m_data.m_preferences.hairStyleID, a4);

    if ((this->m_data.m_flags & 1) != 0)
        return;

    if (a3) {
        this->LoadBaseVariation(VARIATION_HAIR, 1, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_LOWER, a4);
        this->LoadBaseVariation(VARIATION_HAIR, 2, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_UPPER, a4);
    }

    this->LoadBaseVariation(VARIATION_FACIAL_HAIR, 0, this->m_data.m_preferences.facialHairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_LOWER, a4);
    this->LoadBaseVariation(VARIATION_FACIAL_HAIR, 1, this->m_data.m_preferences.facialHairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_UPPER, a4);
}

// OFFSET: 0x4EA3E0
void CCharacterComponent::SetHairStyle(uint32_t colorId, const char* a4) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_HAIR, colorId, this->m_data.m_preferences.hairColorID))
        return;

    this->m_data.m_preferences.hairStyleID = colorId;
    this->m_data.m_geosets[0] = GetConditionalGeoset(&this->m_data.m_preferences);

    if ((this->m_data.m_flags & 1) == 0) {
        this->LoadBaseVariation(VARIATION_HAIR, 1, colorId, this->m_data.m_preferences.hairColorID, SECTION_HEAD_LOWER, a4);
        this->LoadBaseVariation(VARIATION_HAIR, 2, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_UPPER, a4);
    }
    this->SetHairColor(this->m_data.m_preferences.hairColorID, false, a4);
    this->m_flags |= 4;
    this->m_dirtySections |= 0x300;
    if (this->m_request) {
        //*m_request &= ~1u;
        this->m_request = nullptr;
    }
    this->m_flags &= ~8;
}

// OFFSET: 0x4EA490
void CCharacterComponent::SetFace(uint32_t colorId, bool a3, const char* a4) {
    if ((this->m_data.m_flags & 1) != 0)
        return;

    auto rec = this->GetSectionsRecord(VARIATION_SKIN, 0, this->m_data.m_preferences.skinID, nullptr);
    if ((rec && (rec->m_flags & 8) != 0) || !ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_FACE, colorId, this->m_data.m_preferences.skinID))
        return;

    this->m_data.m_preferences.faceID = colorId;
    this->LoadBaseVariation(VARIATION_FACE, 0, colorId, this->m_data.m_preferences.skinID, SECTION_HEAD_LOWER, a4);
    this->LoadBaseVariation(VARIATION_FACE, 1, colorId, this->m_data.m_preferences.skinID, SECTION_HEAD_UPPER, a4);
    if (a3) {
        this->LoadBaseVariation(VARIATION_HAIR, 1, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_LOWER, a4);
        this->LoadBaseVariation(VARIATION_HAIR, 2, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_UPPER, a4);
    }
    this->m_flags |= 4;
    this->m_dirtySections |= 0x300;
    if (this->m_request) {
        //*m_request &= ~1u;
        this->m_request = nullptr;
    }
    this->m_flags &= ~8;
}

// OFFSET: 0x4EA590
void CCharacterComponent::SetBeardStyle(uint32_t colorId, bool a3, const char* a4) {
    if (colorId > CCharacterComponent::s_characterFacialHairStylesList[2 * this->m_data.m_preferences.raceID + this->m_data.m_preferences.sexID])
        return;

    this->m_data.m_preferences.facialHairStyleID = colorId;

    auto ConditionalFacialHairStyle = GetConditionalFacialHairStyle(&this->m_data.m_preferences);
    if (ConditionalFacialHairStyle) {
        this->m_data.m_geosets[1] = ConditionalFacialHairStyle->m_geoset[0] + 100;
        this->m_data.m_geosets[3] = ConditionalFacialHairStyle->m_geoset[1] + 300;
        this->m_data.m_geosets[2] = ConditionalFacialHairStyle->m_geoset[2] + 200;
        this->m_data.m_geosets[16] = ConditionalFacialHairStyle->m_geoset[3] + 1600;
        this->m_data.m_geosets[17] = ConditionalFacialHairStyle->m_geoset[4] + 1700;
        this->m_flags |= 4u;
        if ((ConditionalFacialHairStyle->m_geoset[0] || ConditionalFacialHairStyle->m_geoset[1] || ConditionalFacialHairStyle->m_geoset[2]) && (!this->m_baseVariation[VARIATION_HAIR].m_texture[1] || !this->m_baseVariation[VARIATION_HAIR].m_texture[2])) {
            this->ReplaceHairTexture(1, a4);
        }
    }

    if ((this->m_data.m_flags & 1) == 0) {
        if (a3) {
            this->LoadBaseVariation(VARIATION_FACIAL_HAIR, 0, this->m_data.m_preferences.facialHairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_LOWER, a4);
            this->LoadBaseVariation(VARIATION_FACIAL_HAIR, 1, this->m_data.m_preferences.facialHairStyleID, this->m_data.m_preferences.hairColorID, SECTION_HEAD_UPPER, a4);
        }
        this->m_dirtySections |= 0x300;
        if (this->m_request) {
            //*m_request &= ~1u;
            this->m_request = 0;
        }
        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4EA6B0
void CCharacterComponent::SetSkinColor(uint32_t colorId, bool a3, bool a4, const char* a5) {
    this->m_data.m_preferences.skinID = colorId;

    if (this->m_data.m_flags & 0x1)
        return;

    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_SKIN, 0, colorId)) {
        return;
    }

    uint32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_SKIN, 0);
    auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_SKIN, 0, colorId, nullptr);

    if (colorId < numColors && rec && (rec->m_flags & 0x8) == 0) {
        auto extra = this->GetSectionsRecord(VARIATION_UNDERWEAR, 0, colorId, nullptr);

        if (this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0]) {
            TextureCacheDestroyTexture(this->m_baseVariation[4].m_texture[0]);
            this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0] = nullptr;
        }

        if (this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1]) {
            TextureCacheDestroyTexture(this->m_baseVariation[4].m_texture[1]);
            this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1] = nullptr;
        }

        if (extra) {
            if (*extra->m_textureName[0]) {
                SStrCopy(s_path, extra->m_textureName[0]);
                //if (CopyStringToBuffer(extra->m_textureName[0], s_pathEnd)) {
                    auto texture = TextureCacheCreateTexture(s_path);
                    this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0] = texture;
                    //if (!texture)
                    //    NOP("**** Unable to Load Texture %s\n", s_path);
                //}
            }

            if (*extra->m_textureName[1]) {
                SStrCopy(s_path, extra->m_textureName[1]);
                //if (CopyStringToBuffer(extra->m_textureName[1], s_pathEnd)) {
                    auto texture = TextureCacheCreateTexture(s_path);
                    this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1] = texture;
                    //if (!texture)
                    //    NOP("**** Unable to Load Texture %s\n", s_path);
                //}
            }
        }
    }

    this->ReplaceExtraSkinTexture(a5);
    this->LoadBaseVariation(VARIATION_SKIN, 0, 0, this->m_data.m_preferences.skinID, SECTION_TORSO_UPPER, a5);
    this->SetFace(this->m_data.m_preferences.faceID, a3, a5);

    if (a4) {
        this->m_flags |= 1;
        if (this->m_request) {
            //*this->m_request &= ~1u;
            this->m_request = nullptr;
        }
        this->m_flags &= ~8u;
    }

    this->m_dirtySections = -1;
    if (this->m_request) {
        //*this->m_request &= ~1u;
        this->m_request = nullptr;
    }
    this->m_flags &= ~8u;
}

// OFFSET: 0x4EB290
bool CCharacterComponent::SetPrevSkin(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    int32_t numColors = ComponentGetNumColors(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_SKIN,
        0);

    if (numColors <= 0)
        return false;

    int32_t skinId = this->m_data.m_preferences.skinID;

    int32_t i = skinId - 1;
    if (i < 0)
        i = numColors - 1;
    if (i == skinId)
        return false;

    do {
        auto skinRec = this->GetSectionsRecord(VARIATION_SKIN, 0, i, nullptr);
        auto faceRec = this->GetSectionsRecord(VARIATION_FACE, this->m_data.m_preferences.faceID, i, nullptr);
        auto underRec = this->GetSectionsRecord(VARIATION_UNDERWEAR, 0, i, nullptr);

        if (skinRec && ComponentFlagsMatch(skinRec->m_flags, selection) && faceRec && ComponentFlagsMatch(faceRec->m_flags, selection) && (context == CONTEXT_2 || (underRec && ComponentFlagsMatch(underRec->m_flags, selection)))) {
            this->SetSkinColor(skinRec->m_colorIndex, true, true, nullptr);
            return true;
        }

        if (--i < 0)
            i = numColors - 1;
    } while (i != this->m_data.m_preferences.skinID);

    return false;
}

// OFFSET: 0x4EB150
bool CCharacterComponent::SetNextSkin(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    int32_t numColors = ComponentGetNumColors(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_SKIN,
        0);

    if (numColors <= 0)
        return false;

    int32_t skinId = this->m_data.m_preferences.skinID;

    int32_t i = skinId + 1;
    if (i >= numColors)
        i = 0;
    if (i == skinId)
        return false;

    do {
        auto skinRec = this->GetSectionsRecord(VARIATION_SKIN, 0, i, nullptr);
        auto faceRec = this->GetSectionsRecord(VARIATION_FACE, this->m_data.m_preferences.faceID, i, nullptr);
        auto underRec = this->GetSectionsRecord(VARIATION_UNDERWEAR, 0, i, nullptr);

        if (skinRec && ComponentFlagsMatch(skinRec->m_flags, selection) && faceRec && ComponentFlagsMatch(faceRec->m_flags, selection) && (context == CONTEXT_2 || (underRec && ComponentFlagsMatch(underRec->m_flags, selection)))) {
            this->SetSkinColor(skinRec->m_colorIndex, true, true, nullptr);
            return true;
        }

        if (++i >= numColors)
            i = 0;
    } while (i != this->m_data.m_preferences.skinID);

    return false;
}

// OFFSET: 0x4EB990
bool CCharacterComponent::SetPrevFace(COMPONENT_CONTEXT context, uint32_t skinIndex) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    int32_t numVariations = ComponentGetNumVariations(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_FACE);

    if (numVariations <= 0)
        return false;

    int32_t faceId = this->m_data.m_preferences.faceID;

    int32_t face = faceId - 1;
    if (face < 0)
        face = numVariations - 1;
    if (face == faceId)
        return false;

    int32_t color = 0;
    bool matched = false;

    do {
        int32_t numColors = ComponentGetNumColors(
            CCharacterComponent::s_chrVarArray,
            this->m_data.m_preferences.raceID,
            this->m_data.m_preferences.sexID,
            VARIATION_FACE,
            face);

        if (numColors > 0) {
            int32_t j = 0;

            do {
                color = (j + skinIndex) % numColors;

                auto faceRec = this->GetSectionsRecord(VARIATION_FACE, face, color, nullptr);
                auto skinRec = this->GetSectionsRecord(VARIATION_SKIN, 0, color, nullptr);
                auto underRec = this->GetSectionsRecord(VARIATION_UNDERWEAR, 0, color, nullptr);

                if (faceRec && ComponentFlagsMatch(faceRec->m_flags, selection) && skinRec && ComponentFlagsMatch(skinRec->m_flags, selection) && (context == CONTEXT_2 || (underRec && ComponentFlagsMatch(underRec->m_flags, selection)))) {
                    matched = true;
                    break;
                }

                j = color + 1;
            } while (j < numColors);
        }

        if (matched)
            break;

        if (--face < 0)
            face = numVariations - 1;
    } while (face != this->m_data.m_preferences.faceID);

    if (!matched)
        return false;

    auto faceRec = this->GetSectionsRecord(VARIATION_FACE, face, this->m_data.m_preferences.skinID, nullptr);
    auto skinRec = this->GetSectionsRecord(VARIATION_SKIN, 0, this->m_data.m_preferences.skinID, nullptr);
    auto underRec = this->GetSectionsRecord(VARIATION_UNDERWEAR, 0, this->m_data.m_preferences.skinID, nullptr);

    if (faceRec && ComponentFlagsMatch(faceRec->m_flags, selection) && skinRec && ComponentFlagsMatch(skinRec->m_flags, selection) && (context == CONTEXT_2 || (underRec && ComponentFlagsMatch(underRec->m_flags, selection)))) {
        this->SetFace(face, true, nullptr);
        return true;
    }

    this->SetSkinColor(color, false, true, nullptr);
    this->SetFace(face, true, nullptr);
    return true;
}

// OFFSET: 0x4EB710
bool CCharacterComponent::SetNextFace(COMPONENT_CONTEXT context, uint32_t skinIndex) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    int32_t numVariations = ComponentGetNumVariations(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_FACE);

    if (numVariations <= 0)
        return false;

    int32_t faceId = this->m_data.m_preferences.faceID;

    int32_t face = faceId + 1;
    if (face >= numVariations)
        face = 0;
    if (face == faceId)
        return false;

    int32_t color = 0;
    bool matched = false;

    do {
        int32_t numColors = ComponentGetNumColors(
            CCharacterComponent::s_chrVarArray,
            this->m_data.m_preferences.raceID,
            this->m_data.m_preferences.sexID,
            VARIATION_FACE,
            face);

        if (numColors > 0) {
            int32_t j = 0;

            do {
                color = (j + skinIndex) % numColors;

                auto faceRec = this->GetSectionsRecord(VARIATION_FACE, face, color, nullptr);
                auto skinRec = this->GetSectionsRecord(VARIATION_SKIN, 0, color, nullptr);
                auto underRec = this->GetSectionsRecord(VARIATION_UNDERWEAR, 0, color, nullptr);

                if (faceRec && ComponentFlagsMatch(faceRec->m_flags, selection) && skinRec && ComponentFlagsMatch(skinRec->m_flags, selection) && (context == CONTEXT_2 || (underRec && ComponentFlagsMatch(underRec->m_flags, selection)))) {
                    matched = true;
                    break;
                }

                j = color + 1;
            } while (j < numColors);
        }

        if (matched)
            break;

        if (++face >= numVariations)
            face = 0;
    } while (face != this->m_data.m_preferences.faceID);

    if (!matched)
        return false;

    // 0x4EB8B0 -- does the new face survive with the skin we already have?
    auto faceRec = this->GetSectionsRecord(VARIATION_FACE, face, this->m_data.m_preferences.skinID, nullptr);
    auto skinRec = this->GetSectionsRecord(VARIATION_SKIN, 0, this->m_data.m_preferences.skinID, nullptr);
    auto underRec = this->GetSectionsRecord(VARIATION_UNDERWEAR, 0, this->m_data.m_preferences.skinID, nullptr);

    if (faceRec && ComponentFlagsMatch(faceRec->m_flags, selection) && skinRec && ComponentFlagsMatch(skinRec->m_flags, selection) && (context == CONTEXT_2 || (underRec && ComponentFlagsMatch(underRec->m_flags, selection)))) {
        this->SetFace(face, true, nullptr);
        return true;
    }

    this->SetSkinColor(color, false, true, nullptr);
    this->SetFace(face, true, nullptr);
    return true;
}

// OFFSET: 0x4F0630
bool CCharacterComponent::SetPrevHairStyle(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    int32_t numVariations = ComponentGetNumVariations(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_HAIR);

    if (numVariations <= 0)
        return false;

    int32_t hairStyleId = this->m_data.m_preferences.hairStyleID;

    int32_t style = hairStyleId - 1;
    if (style < 0)
        style = numVariations - 1;
    if (style == hairStyleId)
        return false;

    CharSectionsRec* hit = nullptr;

    do {
        int32_t numColors = ComponentGetNumColors(
            CCharacterComponent::s_chrVarArray,
            this->m_data.m_preferences.raceID,
            this->m_data.m_preferences.sexID,
            VARIATION_HAIR,
            style);

        for (int32_t c = 0; c < numColors; c++) {
            auto rec = this->GetSectionsRecord(VARIATION_HAIR, style, c, nullptr);
            if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
                hit = rec;
                break;
            }
        }

        if (hit)
            break;

        if (--style < 0)
            style = numVariations - 1;
    } while (style != this->m_data.m_preferences.hairStyleID);

    if (!hit)
        return false;

    auto keep = this->GetSectionsRecord(VARIATION_HAIR, style, this->m_data.m_preferences.hairColorID, nullptr);
    if (keep && ComponentFlagsMatch(keep->m_flags, selection)) {
        this->SetHairStyle(keep->m_variationIndex, nullptr);
        return true;
    }

    this->SetHairColor(hit->m_colorIndex, true, nullptr);
    this->SetHairStyle(hit->m_variationIndex, nullptr);

    int32_t facialFeature = this->GetNthFacialFeatureIndex(
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        this->m_data.m_preferences.classID,
        this->m_data.m_preferences.hairColorID,
        this->m_data.m_preferences.facialHairStyleID,
        0,
        GetSelectionFromContext(context, this->m_data.m_preferences.classID));

    if (facialFeature >= 0)
        this->SetBeardStyle(facialFeature, true, nullptr);

    return true;
}

// OFFSET: 0x4F0490
bool CCharacterComponent::SetNextHairStyle(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    int32_t numVariations = ComponentGetNumVariations(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_HAIR);

    if (numVariations <= 0)
        return false;

    int32_t hairStyleId = this->m_data.m_preferences.hairStyleID;

    int32_t style = hairStyleId + 1;
    if (style >= numVariations)
        style = 0;
    if (style == hairStyleId)
        return false;

    CharSectionsRec* hit = nullptr;

    do {
        int32_t numColors = ComponentGetNumColors(
            CCharacterComponent::s_chrVarArray,
            this->m_data.m_preferences.raceID,
            this->m_data.m_preferences.sexID,
            VARIATION_HAIR,
            style);

        for (int32_t c = 0; c < numColors; c++) {
            auto rec = this->GetSectionsRecord(VARIATION_HAIR, style, c, nullptr);
            if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
                hit = rec;
                break;
            }
        }

        if (hit)
            break;

        if (++style >= numVariations)
            style = 0;
    } while (style != this->m_data.m_preferences.hairStyleID);

    if (!hit)
        return false;

    auto keep = this->GetSectionsRecord(VARIATION_HAIR, style, this->m_data.m_preferences.hairColorID, nullptr);
    if (keep && ComponentFlagsMatch(keep->m_flags, selection)) {
        this->SetHairStyle(keep->m_variationIndex, nullptr);
        return true;
    }

    this->SetHairColor(hit->m_colorIndex, true, nullptr);
    this->SetHairStyle(hit->m_variationIndex, nullptr);

    int32_t facialFeature = this->GetNthFacialFeatureIndex(
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        this->m_data.m_preferences.classID,
        this->m_data.m_preferences.hairColorID,
        this->m_data.m_preferences.facialHairStyleID,
        0,
        GetSelectionFromContext(context, this->m_data.m_preferences.classID));

    if (facialFeature >= 0)
        this->SetBeardStyle(facialFeature, true, nullptr);

    return true;
}

// OFFSET: 0x4EB5C0
bool CCharacterComponent::SetPrevHairColor(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    int32_t numColors = ComponentGetNumColors(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_HAIR,
        this->m_data.m_preferences.hairStyleID);

    if (numColors <= 0)
        return false;

    int32_t hairColorId = this->m_data.m_preferences.hairColorID;

    int32_t i = hairColorId - 1;
    if (i < 0)
        i = numColors - 1;
    if (i == hairColorId)
        return false;

    do {
        auto rec = this->GetSectionsRecord(VARIATION_HAIR, this->m_data.m_preferences.hairStyleID, i, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            this->SetHairColor(rec->m_colorIndex, true, nullptr);
            return true;
        }

        if (--i < 0)
            i = numColors - 1;
    } while (i != this->m_data.m_preferences.hairColorID);

    return false;
}

// OFFSET: 0x4EB500
bool CCharacterComponent::SetNextHairColor(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    int32_t numColors = ComponentGetNumColors(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_HAIR,
        this->m_data.m_preferences.hairStyleID);

    if (numColors <= 0)
        return false;

    int32_t hairColorId = this->m_data.m_preferences.hairColorID;

    int32_t i = hairColorId + 1;
    if (i >= numColors)
        i = 0;
    if (i == hairColorId)
        return false;

    do {
        auto rec = this->GetSectionsRecord(VARIATION_HAIR, this->m_data.m_preferences.hairStyleID, i, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            this->SetHairColor(rec->m_colorIndex, true, nullptr);
            return true;
        }

        if (++i >= numColors)
            i = 0;
    } while (i != this->m_data.m_preferences.hairColorID);

    return false;
}

// OFFSET: 0x4EBE80
bool CCharacterComponent::SetPrevFacialFeature(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    bool found;
    this->GetSectionsRecord(
        VARIATION_FACIAL_HAIR,
        this->m_data.m_preferences.facialHairStyleID,
        this->m_data.m_preferences.hairColorID,
        &found);

    if (!found) {
        uint32_t count = CCharacterComponent::s_characterFacialHairStylesList[2 * this->m_data.m_preferences.raceID + this->m_data.m_preferences.sexID];

        int32_t prev = this->m_data.m_preferences.facialHairStyleID - 1;
        if (prev < 0)
            prev = static_cast<int32_t>(count) - 1;

        this->SetBeardStyle(prev, true, nullptr);
        return true;
    }

    int32_t numVariations = ComponentGetNumVariations(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_FACIAL_HAIR);

    if (numVariations <= 0)
        return false;

    int32_t facialHairId = this->m_data.m_preferences.facialHairStyleID;

    int32_t variation = facialHairId - 1;
    if (variation < 0)
        variation = numVariations - 1;
    if (variation == facialHairId)
        return false;

    CharSectionsRec* hit = nullptr;

    do {
        int32_t numColors = ComponentGetNumColors(
            CCharacterComponent::s_chrVarArray,
            this->m_data.m_preferences.raceID,
            this->m_data.m_preferences.sexID,
            VARIATION_FACIAL_HAIR,
            variation);

        for (int32_t c = 0; c < numColors; c++) {
            auto rec = this->GetSectionsRecord(VARIATION_FACIAL_HAIR, variation, c, nullptr);
            if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
                hit = rec;
                break;
            }
        }

        if (hit)
            break;

        if (--variation < 0)
            variation = numVariations - 1;
    } while (variation != this->m_data.m_preferences.facialHairStyleID);

    if (!hit)
        return false;

    auto keep = this->GetSectionsRecord(VARIATION_FACIAL_HAIR, variation, this->m_data.m_preferences.hairColorID, nullptr);
    if (keep && ComponentFlagsMatch(keep->m_flags, selection)) {
        this->SetBeardStyle(keep->m_variationIndex, true, nullptr);
        return true;
    }

    this->SetHairColor(hit->m_colorIndex, false, nullptr);
    this->SetBeardStyle(hit->m_variationIndex, true, nullptr);
    return true;
}

// OFFSET: 0x4EBCA0
bool CCharacterComponent::SetNextFacialFeature(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);

    bool found;
    this->GetSectionsRecord(
        VARIATION_FACIAL_HAIR,
        this->m_data.m_preferences.facialHairStyleID,
        this->m_data.m_preferences.hairColorID,
        &found);

    if (!found) {
        uint32_t count = CCharacterComponent::s_characterFacialHairStylesList
            [2 * this->m_data.m_preferences.raceID + this->m_data.m_preferences.sexID];

        int32_t next = this->m_data.m_preferences.facialHairStyleID + 1;
        if (next >= static_cast<int32_t>(count))
            next = 0;

        this->SetBeardStyle(next, true, nullptr);
        return true;
    }

    int32_t numVariations = ComponentGetNumVariations(
        CCharacterComponent::s_chrVarArray,
        this->m_data.m_preferences.raceID,
        this->m_data.m_preferences.sexID,
        VARIATION_FACIAL_HAIR);

    if (numVariations <= 0)
        return false;

    int32_t facialHairId = this->m_data.m_preferences.facialHairStyleID;

    int32_t variation = facialHairId + 1;
    if (variation >= numVariations)
        variation = 0;
    if (variation == facialHairId)
        return false;

    CharSectionsRec* hit = nullptr;

    do {
        int32_t numColors = ComponentGetNumColors(
            CCharacterComponent::s_chrVarArray,
            this->m_data.m_preferences.raceID,
            this->m_data.m_preferences.sexID,
            VARIATION_FACIAL_HAIR,
            variation);

        for (int32_t c = 0; c < numColors; c++) {
            auto rec = this->GetSectionsRecord(VARIATION_FACIAL_HAIR, variation, c, nullptr);
            if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
                hit = rec;
                break;
            }
        }

        if (hit)
            break;

        if (++variation >= numVariations)
            variation = 0;
    } while (variation != this->m_data.m_preferences.facialHairStyleID);

    if (!hit)
        return false;

    auto keep = this->GetSectionsRecord(VARIATION_FACIAL_HAIR, variation, this->m_data.m_preferences.hairColorID, nullptr);
    if (keep && ComponentFlagsMatch(keep->m_flags, selection)) {
        this->SetBeardStyle(keep->m_variationIndex, true, nullptr);
        return true;
    }

    this->SetHairColor(hit->m_colorIndex, false, nullptr);
    this->SetBeardStyle(hit->m_variationIndex, true, nullptr);
    return true;
}


// OFFSET: 0x4EB3E0
void CCharacterComponent::SetRandomSkin(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);
    uint32_t numSkins = this->GetNumSkins(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.classID, context);
    uint32_t rndSelection = 0;

    if (numSkins > 0) {
        rndSelection = CRandom::dice(numSkins, g_rndSeed);
    }
    auto rec = ComponentGetSkinColor(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, rndSelection, selection);
    if (rec >= 0)
        this->SetSkinColor(rec, true, true, nullptr);
}

// OFFSET: 0x4EB680
void CCharacterComponent::SetRandomHairColor(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);
    uint32_t numHairStyles = this->GetNumHairColorsForStyle(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.classID, this->m_data.m_preferences.hairStyleID, context);
    uint32_t rndSelection = 0;
    if (numHairStyles)
        rndSelection = CRandom::dice(numHairStyles, g_rndSeed);
    uint32_t hairVariation = ComponentGetHairColor(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.hairStyleID, rndSelection, selection);
    if (hairVariation >= 0)
        this->SetHairColor(hairVariation, 1, nullptr);
}

// OFFSET: 0x4EB470
void CCharacterComponent::SetRandomHairStyle(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);
    uint32_t numHairStyles = this->GetNumHairStylesForColor(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.classID, this->m_data.m_preferences.hairColorID, context);
    uint32_t rndSelection = 0;
    if (numHairStyles)
        rndSelection = CRandom::dice(numHairStyles, g_rndSeed);
    uint32_t hairVariation = ComponentGetHairVariation(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.hairColorID, rndSelection, selection);
    if (hairVariation >= 0)
        this->SetHairStyle(hairVariation, nullptr);
}

// OFFSET: 0x4EBC10
void CCharacterComponent::SetRandomFace(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);
    uint32_t numFaces = this->GetNumFacesForSkin(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.classID, this->m_data.m_preferences.skinID, context);
    uint32_t rndSelection = 0;
    if (numFaces)
        rndSelection = CRandom::dice(numFaces, g_rndSeed);
    uint32_t hairVariation = ComponentGetFaceVariation(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.skinID, rndSelection, selection);
    if (hairVariation >= 0)
        this->SetFace(hairVariation, 1, nullptr);
}

// OFFSET: 0x4EC050
void CCharacterComponent::SetRandomFacialFeature(COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, this->m_data.m_preferences.classID);
    uint32_t numFacialFeatures = this->GetNumFacialFeaturesForHairColor(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.classID, this->m_data.m_preferences.hairColorID, context);
    uint32_t rndSelection = 0;
    if (numFacialFeatures)
        rndSelection = CRandom::dice(numFacialFeatures, g_rndSeed);
    uint32_t hairVariation = this->GetNthFacialFeatureIndex(this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, this->m_data.m_preferences.classID, this->m_data.m_preferences.hairColorID, this->m_data.m_preferences.facialHairStyleID, rndSelection, selection);
    if (hairVariation >= 0)
        this->SetBeardStyle(hairVariation, 1, nullptr);
}

// OFFSET: 0x4E7B80
uint32_t CCharacterComponent::GetNumSkins(uint32_t raceId, uint32_t sexId, uint32_t classId, COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, classId);
    uint32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_SKIN, 0);
    uint32_t skins = 0;

    for (uint32_t i = 0; i < numColors; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_SKIN, 0, i, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            skins++;
        }
    }
    return skins;
}

// OFFSET: 0x4E7C10
uint32_t CCharacterComponent::GetNumHairStylesForColor(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t colorId, COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, classId);
    uint32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_HAIR);
    uint32_t skins = 0;

    for (uint32_t i = 0; i < numVariations; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_HAIR, i, colorId, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            skins++;
        }
    }
    return skins;
}

// OFFSET: 0x4E7DF0
uint32_t CCharacterComponent::GetNumFacialFeaturesForHairColor(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t colorId, COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, classId);
    uint32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_FACIAL_HAIR);
    uint32_t result = 0;

    for (uint32_t i = 0; i < numVariations; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_FACIAL_HAIR, i, colorId, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            result++;
        }
    }
    return result;
}

// OFFSET: 0x4E7CB0
uint32_t CCharacterComponent::GetNumHairColorsForStyle(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t index, COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, classId);
    uint32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_HAIR, index);
    uint32_t result = 0;

    for (uint32_t i = 0; i < numColors; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_HAIR, index, i, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            result++;
        }
    }
    return result;
}

// OFFSET: 0x4E7D50
uint32_t CCharacterComponent::GetNumFacesForSkin(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t index, COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, classId);
    uint32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_FACE);
    uint32_t result = 0;

    for (uint32_t i = 0; i < numVariations; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_FACE, i, index, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            result++;
        }
    }
    return result;
}

// OFFSET: 0x4E80E0
int32_t CCharacterComponent::GetNthFacialFeatureIndex(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t hairColorId, uint32_t facialHairId, uint32_t index, uint32_t selection) {
    bool found;
    ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_FACIAL_HAIR, facialHairId, hairColorId, &found);
    if (!found) {
        if (index >= CCharacterComponent::s_characterFacialHairStylesList[raceId * 2 + sexId])
            return -1;
        return index;
    }

    auto context = GetContextFromSelection(selection);
    auto numFacialFeatures = this->GetNumFacialFeaturesForHairColor(raceId, sexId, classId, hairColorId, context);

    if (!numFacialFeatures)
        return -1;

    uint32_t match = 0;
    for (uint32_t i = 0; i < numFacialFeatures; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, VARIATION_FACIAL_HAIR, i, hairColorId, nullptr);
        if (!rec || !ComponentFlagsMatch(rec->m_flags, selection))
            continue;
        if (match == index)
            return rec->m_variationIndex;
        match++;
    }
    return -1;
}

// OFFSET: 0x4E76D0
CharSectionsRec* CCharacterComponent::GetSectionsRecord(COMPONENT_VARIATIONS variation, uint32_t variationIndex, uint32_t colorId, bool* found) {
    return ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, variation, variationIndex, colorId, found);
}

// OFFSET: 0x4F24D0
bool CCharacterComponent::Init(ComponentData* data, const char* a3) {
    if (this->m_data.m_model)
        this->m_data.m_model->Release();
    memcpy(&this->m_data, data, sizeof(this->m_data));
    memset(this->m_baseVariation, 0, sizeof(this->m_baseVariation));
    memset(this->m_section, 0, sizeof(this->m_section));
    this->m_handItemDisplayID[0] = 0;
    this->m_handItemDisplayID[1] = 0;
    this->m_handItemDisplayID[2] = 0;
    if ((data->m_preferences.raceID & 0x80000000) != 0
          || data->m_preferences.raceID > g_chrRacesDB.m_maxID
          || !ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.skinID)) {
        return 0;
    }
    this->m_flags = this->m_flags & 0xFFFFFFBA | 5;
    this->m_dirtySections = -1;
    this->m_request = nullptr;
    if ((this->m_data.m_flags & 1) == 0 && this->m_baseSkinTexture) {
        HandleClose(this->m_baseSkinTexture);
        this->m_baseSkinTexture = 0;
    }
    if ((this->m_data.m_flags & 2) != 0)
        this->m_gxTexFormat = CCharacterComponent::s_gxFormatHigh;
    ChrRacesRec* chrRaces = g_chrRacesDB.GetRecord(this->m_data.m_preferences.raceID);
    this->m_flags ^= (this->m_flags ^ (16 * ((chrRaces->m_flags & 2) != 0))) & 0x10;
    if ((this->m_data.m_flags & 1) != 0)
        this->SkinNPC(a3);
    this->SetSkinColor(this->m_data.m_preferences.skinID, 0, 1, a3);
    this->SetHairStyle(this->m_data.m_preferences.hairStyleID, a3);
    this->SetBeardStyle(this->m_data.m_preferences.facialHairStyleID, 0, a3);
    return true;
}

// OFFSET: 0x4F1FC0
bool CCharacterComponent::SkinNPC(const char* a2) {
    this->m_data.m_flags &= ~1;
    if (!this->m_data.m_npcSkinTexture[0])
        return false;

    CStatus status;
    SStrCopy(s_path, this->m_data.m_npcSkinTexture);
    CGxTexFlags flags = CGxTexFlags(GxTex_LinearMipNearest, 0, 0, 0, 0, 0, 1);
    auto tex = TextureCreate(s_path, flags, &status, 0);
    if (tex) {
        this->m_data.m_model->ReplaceTexture(1, tex);
        HandleClose(tex);
        this->ReplaceExtraSkinTexture(a2);
        this->m_data.m_flags |= 1;
        this->m_flags |= 5;
        if (this->m_request) {
            //*m_request &= ~1u;
            this->m_request = nullptr;
        }
        this->m_flags &= ~8u;
        //CStatus::Destroy(v11);
        return 1;
    } else {
        //CStatus::Destroy(v11);
        return false;
    }
}

// OFFSET: 0x4EA0B0
void CCharacterComponent::ReplaceExtraSkinTexture(const char* a2) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.skinID)) {
        return;
    }

    auto rec = CCharacterComponent::s_chrVarArray[2 * this->m_data.m_preferences.raceID + this->m_data.m_preferences.sexID]
                   .m_variation[0]
                   .variation[0]
                   .color[this->m_data.m_preferences.skinID];

    auto name = rec->m_textureName[1];
    if (!*name)
        return;

    SStrCopy(s_path, name);
    //CopyStringToBuffer(name, s_pathEnd);

    auto texture = TextureCreate(s_path, &s_status);
    if (texture) {
        this->m_data.m_model->ReplaceTexture(8, texture);
        HandleClose(texture);
    }
}

// OFFSET: 0x4EFED0
void CCharacterComponent::Prep() {
    if ((this->m_data.m_flags & 1) != 0) {
        if ((this->m_flags & 4) == 0)
            return;
        this->GeosRenderPrep();
    }
    if (this->m_dirtySections || (this->m_flags & 1) != 0) {
        if (!this->m_link.IsLinked())
            s_queue.LinkToTail(this);
    } else if ((this->m_flags & 4) != 0) {
        this->GeosRenderPrep();
    }
}

// OFFSET: 0x4F1520
bool CCharacterComponent::RenderPrep(int32_t a2) {
    if ((this->m_data.m_flags & 0x1) != 0) {
        if ((this->m_flags & 0x4) != 0) {
            this->GeosRenderPrep();
        }
        return true;
    }

    if (!this->m_dirtySections && (this->m_flags & 0x1) == 0) {
        if ((this->m_flags & 0x4) != 0) {
            this->GeosRenderPrep();
        }
        return true;
    }

    if ((this->m_flags & 0x1) == 0) {
        if (!this->m_link.IsLinked())
            s_queue.LinkToTail(this);
        return true;
    }

    if (a2) {
        if (this->m_request) {
            //*m_request &= ~1u;
            this->m_request = nullptr;
        }
        this->VariationsLoaded(1);
        this->ItemsLoaded(1);
        this->m_flags |= 8u;
        this->RenderPrepSections();
        this->m_link.Unlink();
        return true;
    }

    if (!this->m_link.IsLinked())
        s_queue.LinkToTail(this);
    return false;
}

// OFFSET: 0x4F14A0
void CCharacterComponent::RenderPrepSections() {
    CCharacterComponent::s_bInRenderPrep = true;
    if ((this->m_flags & 4) != 0)
        this->GeosRenderPrep();
    if (!this->m_baseSkinTexture)
        this->CreateBaseTexture();
    this->PrepSections();
    this->m_flags &= ~1u;
    this->m_dirtySections = 0;
    this->DestroyRenderTextures();
    this->m_link.Unlink();
    CCharacterComponent::s_bInRenderPrep = false;
}

// OFFSET: 0x4EE0D0
void CCharacterComponent::PrepSections() {
    CGxTex* tex = nullptr;
    for (int32_t i = 0; i < 10; i++) {
        if (((1 << i) & this->m_dirtySections) != 0) {
            (this->*CCharacterComponent::s_prepFunc[i])();
            if ((this->m_flags & 1) == 0 && this->m_gxTexFormat != GxTex_Dxt1) {
                tex = TextureGetGxTex(this->m_baseSkinTexture, 1, nullptr);
                GxTexUpdate(tex, s_sectionInfo[i].pos.x, s_sectionInfo[i].pos.y, s_sectionInfo[i].pos.x + s_sectionInfo[i].size.x, s_sectionInfo[i].pos.y + s_sectionInfo[i].size.y, 1);
            }
        }
    }

    if ((this->m_flags & 1) != 0 || this->m_gxTexFormat == GxTex_Dxt1) {
        //if (this->m_gxTexFormat == GxTex_Dxt1)
        //    CCharacterComponent__ComputeMipLevelsSIMD(CCharacterComponent::s_textureBuffer, dword_B6B86C);
        if ((this->m_flags & 1) != 0) {
            tex = TextureGetGxTex(this->m_baseSkinTexture, 1, 0);
            GxTexUpdate(tex, 0, 0, s_textureSize, s_textureSize, 1);
        } else {
            for (int32_t i = 0; i < 10; i++) {
                if (((1 << i) & this->m_dirtySections) != 0) {
                    tex = TextureGetGxTex(this->m_baseSkinTexture, 1, nullptr);
                    GxTexUpdate(tex, s_sectionInfo[i].pos.x, s_sectionInfo[i].pos.y, s_sectionInfo[i].pos.x + s_sectionInfo[i].size.x, s_sectionInfo[i].pos.y + s_sectionInfo[i].size.y, 1);
                }
            }
        }
    }
}

// OFFSET: 0x4EE2A0
void CCharacterComponent::RenderPrepAll() {
    this->m_flags |= 1u;
    if (this->m_request) {
        //*this->m_request &= ~1u;
        this->m_request = 0;
    }
    this->m_flags &= ~8u;
    this->m_dirtySections = -1;
    if (this->m_request) {
        //*this->m_request &= ~1u;
        this->m_request = 0;
    }
    this->m_flags &= ~8u;
    this->VariationsLoaded(1);
    this->ItemsLoaded(1);
    for (uint32_t i = 0; i < NUM_COMPONENT_SECTIONS; i++) {
        (this->*CCharacterComponent::s_prepFunc[i])();
    }
    //if (this->m_gxTexFormat == GxTex_Dxt1)
    //    CCharacterComponent__ComputeMipLevelsSIMD(dword_B6B870, dword_B6B86C);
    this->m_flags &= ~1u;
    this->m_dirtySections = 0;
    this->DestroyRenderTextures();
    this->m_link.Unlink();
}

// OFFSET: 0x4E7650
void CCharacterComponent::DestroyRenderTextures() {
    for (int32_t s = 0; s < NUM_COMPONENT_SECTIONS; s++) {
        CharacterSection& section = this->m_section[s];

        for (int32_t i = 0; i < 7; i++) {
            if (section.layerItemDisplayId[i] == 0) {
                continue;
            }

            if (section.layerTex[i]) {
                TextureCacheDestroyTexture(section.layerTex[i]);
                section.layerTex[i] = nullptr;
            }
        }
    }
}

// OFFSET: 0x4ED640
bool CCharacterComponent::VariationsLoaded(bool a2) {
    TCTEXTUREINFO info;
    for (uint32_t i = 0; i < 5; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            if (this->m_baseVariation[i].m_texture[j] && !TextureCacheGetInfo(this->m_baseVariation[i].m_texture[j], &info, a2)) {
                return false;
            }
        }
    }
    this->m_flags &= ~2;
    return true;
}

// OFFSET: 0x4F0A30
void CCharacterComponent::RenderPrepAL() {
    this->PasteFromSkin(SECTION_ARM_LOWER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);

    CharacterSection* section = &this->m_section[SECTION_ARM_LOWER];
    for (int32_t i = 0; i < 7; i++) {
        if (((1 << i) & section->layerMask) != 0) {
            this->PasteToSection(SECTION_ARM_LOWER, section->layerTex[i], CCharacterComponent::s_textureBuffer);
        }
    }
}

// OFFSET: 0x4F09D0
void CCharacterComponent::RenderPrepAU() {
    this->PasteFromSkin(SECTION_ARM_UPPER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);

    CharacterSection* section = &this->m_section[SECTION_ARM_UPPER];
    for (int32_t i = 0; i < 2; i++) {
        if (((1 << i) & section->layerMask) != 0) {
            this->PasteToSection(SECTION_ARM_UPPER, section->layerTex[i], CCharacterComponent::s_textureBuffer);
        }
    }
}

// OFFSET: 0x4F0E40
void CCharacterComponent::RenderPrepFO() {
    this->PasteFromSkin(SECTION_FOOT, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);

    if ((this->m_section[SECTION_FOOT].layerMask & 1) != 0)
        this->PasteToSection(SECTION_FOOT, this->m_section[SECTION_FOOT].layerTex[0], CCharacterComponent::s_textureBuffer);
}

// OFFSET: 0x4F0A90
void CCharacterComponent::RenderPrepHA() {
    this->PasteFromSkin(SECTION_HAND, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);
    if ((this->m_section[SECTION_HAND].layerMask & 1) != 0)
        this->PasteToSection(SECTION_HAND, this->m_section[SECTION_HAND].layerTex[0], CCharacterComponent::s_textureBuffer);
}

// OFFSET: 0x4F0B70
void CCharacterComponent::RenderPrepHL() {
    auto sectionRecord = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.skinID, nullptr);
    if (sectionRecord && (sectionRecord->m_flags & 8) != 0)
        this->PasteFromSkin(SECTION_HEAD_LOWER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);
    auto v3 = this->m_baseVariation[VARIATION_FACE].m_texture[0];
    if (v3)
        this->PasteToSection(SECTION_HEAD_LOWER, v3, CCharacterComponent::s_textureBuffer);
    auto v4 = this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[0];
    if (v4)
        this->PasteToSection(SECTION_HEAD_LOWER, v4, CCharacterComponent::s_textureBuffer);
    auto v5 = this->m_baseVariation[VARIATION_HAIR].m_texture[1];
    if (v5)
        this->PasteToSection(SECTION_HEAD_LOWER, v5, CCharacterComponent::s_textureBuffer);
}

// OFFSET: 0x4F0AD0
void CCharacterComponent::RenderPrepHU() {
    auto sectionRecord = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.skinID, nullptr);
    if (sectionRecord && (sectionRecord->m_flags & 8) != 0)
        this->PasteFromSkin(SECTION_HEAD_UPPER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);
    auto v3 = this->m_baseVariation[VARIATION_FACE].m_texture[1];
    if (v3)
        this->PasteToSection(SECTION_HEAD_UPPER, v3, CCharacterComponent::s_textureBuffer);
    auto v4 = this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[1];
    if (v4)
        this->PasteToSection(SECTION_HEAD_UPPER, v4, CCharacterComponent::s_textureBuffer);
    auto v5 = this->m_baseVariation[VARIATION_HAIR].m_texture[2];
    if (v5)
        this->PasteToSection(SECTION_HEAD_UPPER, v5, CCharacterComponent::s_textureBuffer);
}

// OFFSET: 0x4F0DB0
void CCharacterComponent::RenderPrepLL() {
    this->PasteFromSkin(SECTION_LEG_LOWER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);

    CharacterSection* section = &this->m_section[SECTION_LEG_LOWER];

    int32_t first = (this->m_flags & 0x20) != 0 ? 1 : 0;
    int32_t last = (this->m_flags & 0x20) != 0 ? 4 : 6;

    for (int32_t i = first; i < last; i++) {
        if (((1 << i) & section->layerMask) != 0) {
            this->PasteToSection(SECTION_LEG_LOWER, section->layerTex[i], CCharacterComponent::s_textureBuffer);
        }
    }
}

// OFFSET: 0x4F0D00
void CCharacterComponent::RenderPrepLU() {
    this->PasteFromSkin(SECTION_LEG_UPPER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);

    CharacterSection* section = &this->m_section[SECTION_LEG_UPPER];

    if ((this->m_flags & 0x20) != 0 || (section->layerMask & 3) == 0) {
        auto v2 = this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0];
        if (v2)
            this->PasteToSection(SECTION_LEG_UPPER, v2, CCharacterComponent::s_textureBuffer);
    }
    int32_t first = (this->m_flags & 0x20) != 0 ? 1 : 0;
    int32_t last = (this->m_flags & 0x20) != 0 ? 2 : 3;

    for (int32_t i = first; i < last; i++) {
        if (((1 << i) & section->layerMask) != 0) {
            this->PasteToSection(SECTION_LEG_UPPER, section->layerTex[i], CCharacterComponent::s_textureBuffer);
        }
    }
}

// OFFSET: 0x4F0CA0
void CCharacterComponent::RenderPrepTL() {
    this->PasteFromSkin(SECTION_TORSO_LOWER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);
    CharacterSection* section = &this->m_section[SECTION_TORSO_LOWER];
    for (int32_t i = 0; i < 7; i++) {
        if (((1 << i) & section->layerMask) != 0) {
            this->PasteToSection(SECTION_TORSO_LOWER, section->layerTex[i], CCharacterComponent::s_textureBuffer);
        }
    }
}

// OFFSET: 0x4F0C10
void CCharacterComponent::RenderPrepTU() {
    this->PasteFromSkin(SECTION_TORSO_UPPER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer);
    if ((this->m_section[3].layerMask & 7) == 0 && this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1]) {
        this->PasteToSection(SECTION_TORSO_UPPER, this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1], CCharacterComponent::s_textureBuffer);
    }
    CharacterSection* section = &this->m_section[SECTION_TORSO_UPPER];
    for (int32_t i = 0; i < 5; i++) {
        if (((1 << i) & section->layerMask) != 0) {
            this->PasteToSection(SECTION_TORSO_UPPER, section->layerTex[i], CCharacterComponent::s_textureBuffer);
        }
    }
}


// OFFSET: 0x4EFF10
void CCharacterComponent::CreateBaseTexture() {
    auto dataFormat = this->m_gxTexFormat == GxTex_Dxt1
                          ? GxTex_Dxt1
                          : GxTex_Argb8888;

    CGxTexFlags flags = CGxTexFlags(GxTex_LinearMipLinear, 0, 0, 0, 0, 0, 1);
    if (GxDevApi() == GxApi_GLL) {
        flags.m_bit14 = 1;
    }

    auto baseTexture = TextureCreate(
        CCharacterComponent::s_textureSize,
        CCharacterComponent::s_textureSize,
        this->m_gxTexFormat,
        dataFormat,
        flags,
        this,
        &CCharacterComponent::UpdateBaseTexture,
        "CharacterBaseSkin",
        1);

    this->m_baseSkinTexture = baseTexture;

    this->m_data.m_model->ReplaceTexture(1, this->m_baseSkinTexture);
}

// OFFSET: 0x4ED900
void CCharacterComponent::GeosRenderPrep() {
    bool eyeGlowFlag = false;

    // Death Knight
    auto sectionRec = this->GetSectionsRecord(VARIATION_FACE, this->m_data.m_preferences.faceID, this->m_data.m_preferences.skinID, nullptr);
    if (this->m_data.m_preferences.classID == 6 || (sectionRec && (sectionRec->m_flags & 4) != 0)) {
        eyeGlowFlag = true;
    }

    auto model = this->m_data.m_model;

    model->SetGeometryVisible(0, 2000, 0);
    model->SetGeometryVisible(0, 0, 1);
    for (int32_t i = 0; i < 19; i++) {
        if (i == 17 && eyeGlowFlag)
            model->SetGeometryVisible(1703, 1703, 1);
        else
            model->SetGeometryVisible(this->m_data.m_geosets[i], this->m_data.m_geosets[i], 1);
    }

    ItemDisplayInfoRec* rec = this->m_itemDisplayID[8] ? g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[8]) : nullptr;

    if (rec && rec->m_geosetGroup[0]) {
        model->SetGeometryVisible(401, 499, 0);
        model->SetGeometryVisible(rec->m_geosetGroup[0] + 401, rec->m_geosetGroup[0] + 401, 1);
    } else if (this->m_itemDisplayID[3]) {
        rec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[3]);

        if (rec && rec->m_geosetGroup[0]) {
            model->SetGeometryVisible(rec->m_geosetGroup[0] + 801, rec->m_geosetGroup[0] + 801, 1);
        }
    }

    bool armLowerCovered = false;

    for (int32_t i = 1; i < 7; i++) {
        if (((1 << i) & this->m_section[SECTION_ARM_LOWER].layerMask) != 0) {
            armLowerCovered = true;
            break;
        }
    }

    if (!armLowerCovered && this->m_itemDisplayID[2]) {
        rec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[2]);

        if (rec && rec->m_geosetGroup[0]) {
            model->SetGeometryVisible(rec->m_geosetGroup[0] + 801, rec->m_geosetGroup[0] + 801, 1);
        }
    }

    bool chestRobe = false;
    bool legsRobe = false;
    bool group12Applied = false;

    rec = this->m_itemDisplayID[3] ? g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[3]) : nullptr;

    if (rec && rec->m_geosetGroup[2]) {
        model->SetGeometryVisible(501, 599, 0);
        model->SetGeometryVisible(902, 999, 0);
        model->SetGeometryVisible(1100, 1199, 0);
        model->SetGeometryVisible(1300, 1399, 0);
        model->SetGeometryVisible(rec->m_geosetGroup[2] + 1301, rec->m_geosetGroup[2] + 1301, 1);
        chestRobe = true;
    } else {
        rec = this->m_itemDisplayID[5] ? g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[5]) : nullptr;

        if (rec && rec->m_geosetGroup[2] && (this->m_flags & 0x20) == 0) {
            model->SetGeometryVisible(501, 599, 0);
            model->SetGeometryVisible(902, 999, 0);
            model->SetGeometryVisible(1100, 1199, 0);
            model->SetGeometryVisible(1300, 1399, 0);
            model->SetGeometryVisible(rec->m_geosetGroup[2] + 1301, rec->m_geosetGroup[2] + 1301, 1);
            legsRobe = true;
        } else {
            rec = this->m_itemDisplayID[6] ? g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[6]) : nullptr;

            if (rec && rec->m_geosetGroup[0]) {
                model->SetGeometryVisible(501, 599, 0);
                model->SetGeometryVisible(901, 901, 1);
                model->SetGeometryVisible(rec->m_geosetGroup[0] + 501, rec->m_geosetGroup[0] + 501, 1);
            } else {
                rec = this->m_itemDisplayID[5] ? g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[5]) : nullptr;

                if (!rec || !rec->m_geosetGroup[1] || (this->m_flags & 0x20) != 0) {
                    model->SetGeometryVisible(901, 901, 1);
                } else {
                    model->SetGeometryVisible(rec->m_geosetGroup[1] + 901, rec->m_geosetGroup[1] + 901, 1);
                }
            }

            rec = this->m_itemDisplayID[9] ? g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[9]) : nullptr;

            if (rec && rec->m_geosetGroup[0]) {
                model->SetGeometryVisible(rec->m_geosetGroup[0] + 1201, rec->m_geosetGroup[0] + 1201, 1);
                group12Applied = true;
            }
        }
    }

    bool skipTail = false;

    if ((this->m_flags & 0x40) != 0) {
        model->SetGeometryVisible(1201, 1201, 1);

        if (chestRobe) {
            skipTail = true;
        } else if (!legsRobe) {
            model->SetGeometryVisible(1202, 1202, 1);
        }
    } else if (chestRobe) {
        skipTail = true;
    }

    if (!skipTail) {
        if (!group12Applied && this->m_itemDisplayID[2]) {
            rec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[2]);

            if (rec && rec->m_geosetGroup[1]) {
                model->SetGeometryVisible(rec->m_geosetGroup[1] + 1001, rec->m_geosetGroup[1] + 1001, 1);
            }
        }

        if ((this->m_flags & 0x20) == 0 && this->m_itemDisplayID[5]) {
            rec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[5]);

            if (rec && rec->m_geosetGroup[0]) {
                if (rec->m_geosetGroup[0] > 2) {
                    model->SetGeometryVisible(1300, 1399, 0);
                    model->SetGeometryVisible(rec->m_geosetGroup[0] + 1101, rec->m_geosetGroup[0] + 1101, 1);
                } else if (!group12Applied) {
                    model->SetGeometryVisible(rec->m_geosetGroup[0] + 1101, rec->m_geosetGroup[0] + 1101, 1);
                }
            }
        }
    }

    if (this->m_itemDisplayID[10]) {
        rec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[10]);

        if (rec && rec->m_geosetGroup[0]) {
            model->SetGeometryVisible(1500, 1599, 0);
            model->SetGeometryVisible(rec->m_geosetGroup[0] + 1501, rec->m_geosetGroup[0] + 1501, 1);
        }
    }

    if (this->m_itemDisplayID[4]) {
        rec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[4]);

        if (rec && rec->m_geosetGroup[0]) {
            model->SetGeometryVisible(1800, 1899, 0);
            model->SetGeometryVisible(rec->m_geosetGroup[0] + 1801, rec->m_geosetGroup[0] + 1801, 1);
        }
    }

    //this->m_data.m_model->OptimizeVisibleGeometry();
    this->m_flags &= ~4;
}

void CCharacterComponent::GetPreferences(CHARACTER_PREFERENCES* info) {
    if (info) {
        *info = this->m_data.m_preferences;
    }
}

// OFFSET: 0x4F07D0
void CCharacterComponent::PasteFromSkin(COMPONENT_SECTIONS section, CACHEENTRY* entry, MipBits* bits) {
    if (!TextureCacheHasMips(entry))
        return;

    TCTEXTUREINFO info;
    TextureCacheGetInfo(entry, &info, true);

    CompSectionInfo sectionInfo = s_sectionInfo[section];

    info.alphaSize = 0;
    if (info.width >= s_textureSize || info.height >= s_textureSize) {
        uint32_t v4 = 0;
        uint32_t width = info.width;
        while (width != s_textureSize) {
            width >>= 1;
            v4++;
        }
        this->Paste(entry, bits, sectionInfo.pos, sectionInfo.pos, sectionInfo.size, info, v4);
    } else {
        this->PasteScale(entry, bits, sectionInfo.pos, sectionInfo.pos, sectionInfo.size, info);
    }
}

// OFFSET: 0x4F08A0
void CCharacterComponent::PasteToSection(COMPONENT_SECTIONS section, CACHEENTRY* entry, MipBits* bits) {
    if (!TextureCacheHasMips(entry))
        return;

    TCTEXTUREINFO info;
    TextureCacheGetInfo(entry, &info, true);

    C2iVector srcPos = C2iVector(0, 0);
    CompSectionInfo sectionInfo = s_sectionInfo[section];

    if (info.width >= sectionInfo.size.x || info.height >= sectionInfo.size.y) {
        uint32_t v4 = 0;
        uint32_t width = info.width;
        while (width != sectionInfo.size.x) {
            width >>= 1;
            v4++;
        }
        this->Paste(entry, bits, sectionInfo.pos, srcPos, sectionInfo.size, info, v4);
    } else {
        this->PasteScale(entry, bits, sectionInfo.pos, srcPos, sectionInfo.size, info);
    }
}

// OFFSET: 0x4EC550
void CCharacterComponent::Paste(CACHEENTRY* entry, MipBits* dstMips, const C2iVector& dstPos, const C2iVector& srcPos, const C2iVector& srcSize, TCTEXTUREINFO& srcInfo, int32_t srcMipLevel) {
    auto pal = TextureCacheGetPal(entry);

    C2iVector src = srcPos;
    C2iVector dst = dstPos;

    if (!pal) {
        this->PasteCrappyGreen(dstMips, dst, 4 * s_textureSize, src, srcInfo, srcMipLevel, -srcMipLevel);
    } else {
        switch (srcInfo.alphaSize) {
        case 0:
            this->PasteOpaque(entry, pal, dstMips, dst, 4 * s_textureSize, src, srcSize, srcInfo, srcMipLevel, -srcMipLevel);
            break;
        case 1:
            this->PasteTransparent1Bit(entry, pal, dstMips, dst, 4 * s_textureSize, src, srcSize, srcInfo, srcMipLevel, -srcMipLevel);
            break;
        case 4:
            this->PasteTransparent4Bit(entry, pal, dstMips, dst, 4 * s_textureSize, src, srcSize, srcInfo, srcMipLevel, -srcMipLevel);
            break;
        case 8:
            this->PasteTransparent8Bit(entry, pal, dstMips, dst, 4 * s_textureSize, src, srcSize, srcInfo, srcMipLevel, -srcMipLevel);
            break;
        }
    }
}

// OFFSET: 0x4E82D0
void CCharacterComponent::PasteCrappyGreen(MipBits* dstMips, C2iVector dstPos, uint32_t pixelStrideInBytes, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel) {
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    while (srcMipLevel < srcInfo.mipCount) {
        auto dstMip = reinterpret_cast<uint8_t*>(dstMips->mip[invSrcMipLevel + srcMipLevel]);
        auto dstRow = dstMip + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;
        auto dstEnd = dstRow + pixelStrideInBytes * copyHeight;

        for (; dstRow < dstEnd; dstRow += pixelStrideInBytes) {
            auto dst = reinterpret_cast<uint32_t*>(dstRow);
            for (uint32_t x = 0; x < copyWidth; x++) {
                dst[x] = 0xFF00FF00; // 0x4E8339 -- memset32
            }
        }

        copyWidth = (copyWidth >> 1) ? (copyWidth >> 1) : 1;
        copyHeight = (copyHeight >> 1) ? (copyHeight >> 1) : 1;

        dstPos.x >>= 1;
        dstPos.y >>= 1;
        pixelStrideInBytes >>= 1;

        srcMipLevel++;
    }
}

// OFFSET: 0x4E84F0
void CCharacterComponent::PasteTransparent1Bit(CACHEENTRY* entry, BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel) {
    uint32_t srcRowPitch = srcInfo.width >> srcMipLevel;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    while (srcMipLevel < srcInfo.mipCount) {
        auto dstMip = reinterpret_cast<uint8_t*>(dstMips->mip[invSrcMipLevel + srcMipLevel]);

        auto src = static_cast<const uint8_t*>(TextureCacheGetMip(entry, srcMipLevel));
        auto alpha = src + srcRowPitch * copyHeight;
        auto srcEnd = alpha;

        auto dstRow = dstMip + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;

        while (src < srcEnd) {
            auto dst = reinterpret_cast<C4Pixel*>(dstRow);

            for (uint32_t x = 0; x < copyWidth; x++) {
                if ((alpha[x >> 3] & (1 << (x & 7))) == 0)
                    continue;

                const BlpPalPixel& texel = pal[src[x]];

                dst[x].b = texel.b;
                dst[x].g = texel.g;
                dst[x].r = texel.r;
                dst[x].a = 0xFF;
            }

            src += srcRowPitch;
            dstRow += pixelStrideInBytes;
            alpha += srcRowPitch >> 3;
        }

        copyWidth = (copyWidth >> 1) ? (copyWidth >> 1) : 1;
        copyHeight = (copyHeight >> 1) ? (copyHeight >> 1) : 1;

        dstPos.x >>= 1;
        dstPos.y >>= 1;
        srcPos.x >>= 1;
        srcPos.y >>= 1;

        srcRowPitch >>= 1;
        pixelStrideInBytes >>= 1;

        srcMipLevel++;
    }
}

// OFFSET: 0x4E8660
void CCharacterComponent::PasteTransparent4Bit(CACHEENTRY* entry, BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel) {
    uint32_t srcRowPitch = srcInfo.width >> srcMipLevel;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    while (srcMipLevel < srcInfo.mipCount) {
        auto dstMip = reinterpret_cast<uint8_t*>(dstMips->mip[invSrcMipLevel + srcMipLevel]);

        auto src = static_cast<const uint8_t*>(TextureCacheGetMip(entry, srcMipLevel));
        auto srcEnd = src + srcRowPitch * copyHeight;
        auto alpha = srcEnd; // 0x4E86D4

        auto dstRow = dstMip + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;

        while (src < srcEnd) {
            auto dst = reinterpret_cast<C4Pixel*>(dstRow);

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t shift = 4 * (x & 1);
                uint32_t nibble = (alpha[x] & (15 << shift)) >> shift;
                uint32_t a = (nibble << 4) | nibble;

                const BlpPalPixel& texel = pal[src[x]];

                dst[x].b = ((255 - a) * dst[x].b + a * texel.b) >> 8;
                dst[x].g = ((255 - a) * dst[x].g + a * texel.g) >> 8;
                dst[x].r = ((255 - a) * dst[x].r + a * texel.r) >> 8;
                dst[x].a = 0xFF;
            }

            src += srcRowPitch;
            dstRow += pixelStrideInBytes;
            alpha += srcRowPitch >> 1;
        }

        copyWidth = (copyWidth >> 1) ? (copyWidth >> 1) : 1;
        copyHeight = (copyHeight >> 1) ? (copyHeight >> 1) : 1;

        dstPos.x >>= 1;
        dstPos.y >>= 1;
        srcPos.x >>= 1;
        srcPos.y >>= 1;

        srcRowPitch >>= 1;
        pixelStrideInBytes >>= 1;

        srcMipLevel++;
    }
}

// OFFSET: 0x4E8840
void CCharacterComponent::PasteTransparent8Bit(CACHEENTRY* entry, BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel) {
    uint32_t srcRowPitch = srcInfo.width >> srcMipLevel;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    while (srcMipLevel < srcInfo.mipCount) {
        auto dstMip = reinterpret_cast<uint8_t*>(dstMips->mip[invSrcMipLevel + srcMipLevel]);

        auto src = static_cast<const uint8_t*>(TextureCacheGetMip(entry, srcMipLevel));
        auto srcEnd = src + srcRowPitch * copyHeight;
        auto alpha = srcEnd;

        auto dstRow = dstMip + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;

        while (src < srcEnd) {
            auto dst = reinterpret_cast<C4Pixel*>(dstRow);

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t a = alpha[x];

                const BlpPalPixel& texel = pal[src[x]];

                dst[x].b = ((255 - a) * dst[x].b + a * texel.b) >> 8;
                dst[x].g = ((255 - a) * dst[x].g + a * texel.g) >> 8;
                dst[x].r = ((255 - a) * dst[x].r + a * texel.r) >> 8;
                dst[x].a = 0xFF;
            }

            src += srcRowPitch;
            dstRow += pixelStrideInBytes;
            alpha += srcRowPitch;
        }

        copyWidth = (copyWidth >> 1) ? (copyWidth >> 1) : 1;
        copyHeight = (copyHeight >> 1) ? (copyHeight >> 1) : 1;

        dstPos.x >>= 1;
        dstPos.y >>= 1;
        srcPos.x >>= 1;
        srcPos.y >>= 1;

        srcRowPitch >>= 1;
        pixelStrideInBytes >>= 1;

        srcMipLevel++;
    }
}

// OFFSET: 0x4E83A0
void CCharacterComponent::PasteOpaque(CACHEENTRY* entry, BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel) {
    uint32_t srcRowPitch = srcInfo.width >> srcMipLevel;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    while (srcMipLevel < srcInfo.mipCount) {
        auto dstMip = reinterpret_cast<uint8_t*>(dstMips->mip[invSrcMipLevel + srcMipLevel]);

        auto src = static_cast<const uint8_t*>(TextureCacheGetMip(entry, srcMipLevel)) + srcPos.x + srcRowPitch * srcPos.y;
        auto srcEnd = src + srcRowPitch * copyHeight;

        auto dstRow = dstMip + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;

        while (src < srcEnd) {
            auto dst = reinterpret_cast<C4Pixel*>(dstRow);

            for (uint32_t x = 0; x < copyWidth; x++) {
                const BlpPalPixel& texel = pal[src[x]];

                dst[x].b = texel.b;
                dst[x].g = texel.g;
                dst[x].r = texel.r;
                dst[x].a = 0xFF;
            }

            src += srcRowPitch;
            dstRow += pixelStrideInBytes;
        }
        copyWidth = (copyWidth >> 1) ? (copyWidth >> 1) : 1;
        copyHeight = (copyHeight >> 1) ? (copyHeight >> 1) : 1;

        dstPos.x >>= 1;
        dstPos.y >>= 1;
        srcPos.x >>= 1;
        srcPos.y >>= 1;

        srcRowPitch >>= 1;
        pixelStrideInBytes >>= 1;

        srcMipLevel++;
    }
}

// OFFSET: 0x4EF9D0
void CCharacterComponent::PasteScale(CACHEENTRY* entry, MipBits* dstMips, const C2iVector& dstPos, const C2iVector& srcPos, const C2iVector& srcSize, TCTEXTUREINFO& srcInfo) {
    C2iVector dst = dstPos;
    C2iVector src = srcPos;
    C2iVector size = srcSize;

    switch (srcInfo.alphaSize) {
    case 0:
        this->PasteOpaqueScale(entry, dstMips, dst, 4 * s_textureSize, src, size, srcInfo);
        break;
    case 1:
        this->PasteTransparent1BitScale(entry, dstMips, dst, 4 * s_textureSize, src, size, srcInfo);
        break;
    case 4:
        this->PasteTransparent4BitScale(entry, dstMips, dst, 4 * s_textureSize, src, size, srcInfo);
        break;
    case 8:
        this->PasteTransparent8BitScale(entry, dstMips, dst, 4 * s_textureSize, src, size, srcInfo);
        break;
    }
}

// OFFSET: 0x4E89F0
void CCharacterComponent::PasteOpaqueScale(CACHEENTRY* entry, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, C2iVector& srcSize, const TCTEXTUREINFO& srcInfo) {
    uint32_t srcRowPitch = srcInfo.width;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    BlpPalPixel* pal = TextureCacheGetPal(entry);

    if (!pal) {
        return;
    }

    srcPos.x >>= 1;
    srcPos.y >>= 1;

    auto srcBase = static_cast<const uint8_t*>(TextureCacheGetMip(entry, 0)) + srcPos.x + srcRowPitch * srcPos.y;

    auto dstRow = reinterpret_cast<uint8_t*>(dstMips->mip[0]) + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;

    for (uint32_t y = 0; y < copyHeight; y++) {
        uint32_t y0 = y >> 1;
        uint32_t y1 = y0 + 1;

        if (y1 >= (copyHeight >> 1) - 1) {
            y1 = (copyHeight >> 1) - 1;
        }

        auto dst = reinterpret_cast<C4Pixel*>(dstRow);

        if (y & 1) {
            auto row0 = srcBase + srcRowPitch * y0;
            auto row1 = srcBase + srcRowPitch * y1;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];
                const BlpPalPixel& p10 = pal[row1[x0]];

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];
                    const BlpPalPixel& p11 = pal[row1[x1]];

                    dst[x].b = (p00.b + p01.b + p10.b + p11.b) >> 2;
                    dst[x].g = (p00.g + p01.g + p10.g + p11.g) >> 2;
                    dst[x].r = (p00.r + p01.r + p10.r + p11.r) >> 2;
                    dst[x].a = 0xFF;
                } else {
                    dst[x].b = (p00.b + p10.b) >> 1;
                    dst[x].g = (p00.g + p10.g) >> 1;
                    dst[x].r = (p00.r + p10.r) >> 1;
                    dst[x].a = 0xFF;
                }
            }
        } else {
            auto row0 = srcBase + srcRowPitch * y0;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];

                    dst[x].b = (p00.b + p01.b) >> 1;
                    dst[x].g = (p00.g + p01.g) >> 1;
                    dst[x].r = (p00.r + p01.r) >> 1;
                    dst[x].a = 0xFF;
                } else {
                    dst[x].b = p00.b;
                    dst[x].g = p00.g;
                    dst[x].r = p00.r;
                    dst[x].a = 0xFF;
                }
            }
        }

        dstRow += pixelStrideInBytes;
    }

    srcSize.x = (srcSize.x >> 1) < 1 ? 1 : (srcSize.x >> 1);
    srcSize.y = (srcSize.y >> 1) < 1 ? 1 : (srcSize.y >> 1);

    dstPos.x >>= 1;
    dstPos.y >>= 1;

    this->PasteOpaque(entry, pal, dstMips, dstPos, pixelStrideInBytes >> 1, srcPos, srcSize, srcInfo, 0, 1);
}

// OFFSET: 0x4EC690
void CCharacterComponent::PasteTransparent1BitScale(CACHEENTRY* entry, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, C2iVector& srcSize, const TCTEXTUREINFO& srcInfo) {
    uint32_t srcRowPitch = srcInfo.width;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    BlpPalPixel* pal = TextureCacheGetPal(entry);

    if (!pal) {
        return;
    }

    srcPos.x >>= 1;
    srcPos.y >>= 1;

    auto srcBase = static_cast<const uint8_t*>(TextureCacheGetMip(entry, 0));
    auto alphaBase = srcBase + srcRowPitch * srcInfo.height;

    auto dstRow = reinterpret_cast<uint8_t*>(dstMips->mip[0]) + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;

    for (uint32_t y = 0; y < copyHeight; y++) {
        uint32_t y0 = y >> 1;
        uint32_t y1 = y0 + 1;

        if (y1 >= (copyHeight >> 1) - 1) {
            y1 = (copyHeight >> 1) - 1;
        }

        uint32_t rowOff0 = srcRowPitch * y0;
        uint32_t rowOff1 = srcRowPitch * y1;

        auto dst = reinterpret_cast<C4Pixel*>(dstRow);

        if (y & 1) {
            auto row0 = srcBase + rowOff0;
            auto row1 = srcBase + rowOff1;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];
                const BlpPalPixel& p10 = pal[row1[x0]];

                uint32_t a00 = (alphaBase[(rowOff0 + x0) >> 3] & (1 << (x0 & 7))) ? 255 : 0;
                uint32_t a10 = (alphaBase[(rowOff1 + x0) >> 3] & (1 << (x0 & 7))) ? 255 : 0;

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];
                    const BlpPalPixel& p11 = pal[row1[x1]];

                    uint32_t a01 = (alphaBase[(rowOff0 + x1) >> 3] & (1 << (x1 & 7))) ? 255 : 0;
                    uint32_t a11 = (alphaBase[(rowOff1 + x1) >> 3] & (1 << (x1 & 7))) ? 255 : 0;

                    a = (a00 + a10 + a01 + a11) >> 2;
                    srcB = (p00.b + p01.b + p10.b + p11.b) >> 2;
                    srcG = (p00.g + p01.g + p10.g + p11.g) >> 2;
                    srcR = (p00.r + p01.r + p10.r + p11.r) >> 2;
                } else {
                    a = (a00 + a10) >> 1;
                    srcB = (p00.b + p10.b) >> 1;
                    srcG = (p00.g + p10.g) >> 1;
                    srcR = (p00.r + p10.r) >> 1;
                }

                dst[x].b = ((255 - a) * dst[x].b + a * srcB) >> 8;
                dst[x].g = ((255 - a) * dst[x].g + a * srcG) >> 8;
                dst[x].r = ((255 - a) * dst[x].r + a * srcR) >> 8;
                dst[x].a = 0xFF;
            }
        } else {
            auto row0 = srcBase + rowOff0;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];

                uint32_t a00 = (alphaBase[(rowOff0 + x0) >> 3] & (1 << (x0 & 7))) ? 255 : 0;

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];

                    uint32_t a01 = (alphaBase[(rowOff0 + x1) >> 3] & (1 << (x1 & 7))) ? 255 : 0;

                    a = (a00 + a01) >> 1;
                    srcB = (p00.b + p01.b) >> 1;
                    srcG = (p00.g + p01.g) >> 1;
                    srcR = (p00.r + p01.r) >> 1;
                } else {
                    a = a00;
                    srcB = p00.b;
                    srcG = p00.g;
                    srcR = p00.r;
                }

                dst[x].b = ((255 - a) * dst[x].b + a * srcB) >> 8;
                dst[x].g = ((255 - a) * dst[x].g + a * srcG) >> 8;
                dst[x].r = ((255 - a) * dst[x].r + a * srcR) >> 8;
                dst[x].a = 0xFF;
            }
        }

        dstRow += pixelStrideInBytes;
    }

    srcSize.x = (srcSize.x >> 1) < 1 ? 1 : (srcSize.x >> 1);
    srcSize.y = (srcSize.y >> 1) < 1 ? 1 : (srcSize.y >> 1);

    dstPos.x >>= 1;
    dstPos.y >>= 1;

    this->PasteTransparent1Bit(entry, pal, dstMips, dstPos, pixelStrideInBytes >> 1, srcPos, srcSize, srcInfo, 0, 1);
}

// OFFSET: 0x4ECC20
void CCharacterComponent::PasteTransparent4BitScale(CACHEENTRY* entry, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, C2iVector& srcSize, const TCTEXTUREINFO& srcInfo) {
    uint32_t srcRowPitch = srcInfo.width;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    BlpPalPixel* pal = TextureCacheGetPal(entry);

    if (!pal) {
        return;
    }

    srcPos.x >>= 1;
    srcPos.y >>= 1;

    auto srcBase = static_cast<const uint8_t*>(TextureCacheGetMip(entry, 0));
    auto alphaBase = srcBase + srcRowPitch * srcInfo.height;

    auto dstRow = reinterpret_cast<uint8_t*>(dstMips->mip[0]) + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;

    for (uint32_t y = 0; y < copyHeight; y++) {
        uint32_t y0 = y >> 1;
        uint32_t y1 = y0 + 1;

        if (y1 >= (copyHeight >> 1) - 1) {
            y1 = (copyHeight >> 1) - 1;
        }

        uint32_t rowOff0 = srcRowPitch * y0;
        uint32_t rowOff1 = srcRowPitch * y1;

        auto dst = reinterpret_cast<C4Pixel*>(dstRow);

        if (y & 1) {
            auto row0 = srcBase + rowOff0;
            auto row1 = srcBase + rowOff1;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                uint32_t shift0 = 4 * (x0 & 1);

                const BlpPalPixel& p00 = pal[row0[x0]];
                const BlpPalPixel& p10 = pal[row1[x0]];

                uint32_t n00 = (alphaBase[(rowOff0 + x0) >> 1] & (15 << shift0)) >> shift0;
                uint32_t n10 = (alphaBase[(rowOff1 + x0) >> 1] & (15 << shift0)) >> shift0;

                uint32_t a00 = (n00 << 4) | n00;
                uint32_t a10 = (n10 << 4) | n10;

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    uint32_t shift1 = 4 * (x1 & 1);

                    const BlpPalPixel& p01 = pal[row0[x1]];
                    const BlpPalPixel& p11 = pal[row1[x1]];

                    uint32_t n01 = (alphaBase[(rowOff0 + x1) >> 1] & (15 << shift1)) >> shift1;
                    uint32_t n11 = (alphaBase[(rowOff1 + x1) >> 1] & (15 << shift1)) >> shift1;

                    uint32_t a01 = (n01 << 4) | n01;
                    uint32_t a11 = (n11 << 4) | n11;

                    a = (a00 + a01 + a10 + a11) >> 2;
                    srcB = (p00.b + p01.b + p10.b + p11.b) >> 2;
                    srcG = (p00.g + p01.g + p10.g + p11.g) >> 2;
                    srcR = (p00.r + p01.r + p10.r + p11.r) >> 2;
                } else {
                    a = (a00 + a10) >> 1;
                    srcB = (p00.b + p10.b) >> 1;
                    srcG = (p00.g + p10.g) >> 1;
                    srcR = (p00.r + p10.r) >> 1;
                }

                dst[x].b = ((255 - a) * dst[x].b + a * srcB) >> 8;
                dst[x].g = ((255 - a) * dst[x].g + a * srcG) >> 8;
                dst[x].r = ((255 - a) * dst[x].r + a * srcR) >> 8;
                dst[x].a = 0xFF;
            }
        } else {
            auto row0 = srcBase + rowOff0;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                uint32_t shift0 = 4 * (x0 & 1);

                const BlpPalPixel& p00 = pal[row0[x0]];

                uint32_t n00 = (alphaBase[(rowOff0 + x0) >> 1] & (15 << shift0)) >> shift0;
                uint32_t a00 = (n00 << 4) | n00;

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    uint32_t shift1 = 4 * (x1 & 1);

                    const BlpPalPixel& p01 = pal[row0[x1]];

                    uint32_t n01 = (alphaBase[(rowOff0 + x1) >> 1] & (15 << shift1)) >> shift1;
                    uint32_t a01 = (n01 << 4) | n01;

                    a = (a00 + a01) >> 1;
                    srcB = (p00.b + p01.b) >> 1;
                    srcG = (p00.g + p01.g) >> 1;
                    srcR = (p00.r + p01.r) >> 1;
                } else {
                    a = a00;
                    srcB = p00.b;
                    srcG = p00.g;
                    srcR = p00.r;
                }

                dst[x].b = ((255 - a) * dst[x].b + a * srcB) >> 8;
                dst[x].g = ((255 - a) * dst[x].g + a * srcG) >> 8;
                dst[x].r = ((255 - a) * dst[x].r + a * srcR) >> 8;
                dst[x].a = 0xFF;
            }
        }

        dstRow += pixelStrideInBytes;
    }

    srcSize.x = (srcSize.x >> 1) < 1 ? 1 : (srcSize.x >> 1);
    srcSize.y = (srcSize.y >> 1) < 1 ? 1 : (srcSize.y >> 1);

    dstPos.x >>= 1;
    dstPos.y >>= 1;

    this->PasteTransparent4Bit(entry, pal, dstMips, dstPos, pixelStrideInBytes >> 1, srcPos, srcSize, srcInfo, 0, 1);
}

// OFFSET: 0x4ED200
void CCharacterComponent::PasteTransparent8BitScale(CACHEENTRY* entry, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, C2iVector& srcSize, const TCTEXTUREINFO& srcInfo) {
    uint32_t srcRowPitch = srcInfo.width;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    BlpPalPixel* pal = TextureCacheGetPal(entry);

    if (!pal) {
        return;
    }

    srcPos.x >>= 1;
    srcPos.y >>= 1;

    auto srcBase = static_cast<const uint8_t*>(TextureCacheGetMip(entry, 0));
    auto alphaBase = srcBase + srcRowPitch * srcInfo.height;

    auto dstRow = reinterpret_cast<uint8_t*>(dstMips->mip[0]) + pixelStrideInBytes * dstPos.y + 4 * dstPos.x;

    for (uint32_t y = 0; y < copyHeight; y++) {
        uint32_t y0 = y >> 1;
        uint32_t y1 = y0 + 1;

        if (y1 >= (copyHeight >> 1) - 1) {
            y1 = (copyHeight >> 1) - 1;
        }

        uint32_t rowOff0 = srcRowPitch * y0;
        uint32_t rowOff1 = srcRowPitch * y1;

        auto dst = reinterpret_cast<C4Pixel*>(dstRow);

        if (y & 1) {
            auto row0 = srcBase + rowOff0;
            auto row1 = srcBase + rowOff1;

            auto alphaRow0 = alphaBase + rowOff0;
            auto alphaRow1 = alphaBase + rowOff1;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];
                const BlpPalPixel& p10 = pal[row1[x0]];

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];
                    const BlpPalPixel& p11 = pal[row1[x1]];

                    a = (alphaRow1[x0] + alphaRow1[x1] + alphaRow0[x0] + alphaRow0[x1]) >> 2;
                    srcB = (p00.b + p01.b + p10.b + p11.b) >> 2;
                    srcG = (p00.g + p01.g + p10.g + p11.g) >> 2;
                    srcR = (p00.r + p01.r + p10.r + p11.r) >> 2;
                } else {
                    a = (alphaRow1[x0] + alphaRow0[x0]) >> 1;
                    srcB = (p00.b + p10.b) >> 1;
                    srcG = (p00.g + p10.g) >> 1;
                    srcR = (p00.r + p10.r) >> 1;
                }

                dst[x].b = ((255 - a) * dst[x].b + a * srcB) >> 8;
                dst[x].g = ((255 - a) * dst[x].g + a * srcG) >> 8;
                dst[x].r = ((255 - a) * dst[x].r + a * srcR) >> 8;
                dst[x].a = 0xFF;
            }
        } else {
            auto row0 = srcBase + rowOff0;

            auto alphaRow0 = alphaBase + rowOff0;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];

                    a = (alphaRow0[x0] + alphaRow0[x1]) >> 1;
                    srcB = (p00.b + p01.b) >> 1;
                    srcG = (p00.g + p01.g) >> 1;
                    srcR = (p00.r + p01.r) >> 1;
                } else {
                    a = alphaRow0[x0];
                    srcB = p00.b;
                    srcG = p00.g;
                    srcR = p00.r;
                }

                dst[x].b = ((255 - a) * dst[x].b + a * srcB) >> 8;
                dst[x].g = ((255 - a) * dst[x].g + a * srcG) >> 8;
                dst[x].r = ((255 - a) * dst[x].r + a * srcR) >> 8;
                dst[x].a = 0xFF;
            }
        }

        dstRow += pixelStrideInBytes;
    }

    srcSize.x = (srcSize.x >> 1) < 1 ? 1 : (srcSize.x >> 1);
    srcSize.y = (srcSize.y >> 1) < 1 ? 1 : (srcSize.y >> 1);

    dstPos.x >>= 1;
    dstPos.y >>= 1;

    this->PasteTransparent8Bit(entry, pal, dstMips, dstPos, pixelStrideInBytes >> 1, srcPos, srcSize, srcInfo, 0, 1);
}

// OFFSET: 0x4E6FB0
CACHEENTRY* CCharacterComponent::LoadItemComponentTexture(ItemDisplayInfoRec* itemDisplayInfo, COMPONENT_SECTIONS section) {
    SStrPrintf(CCharacterComponent::s_path, sizeof(CCharacterComponent::s_path), "Item\\TextureComponents\\%s\\%s_%s.blp", s_componentSections[section], itemDisplayInfo->m_texture[section], s_fileDecorations[2]);

    if (!SFile::FileExists(CCharacterComponent::s_path)) {
        CCharacterComponent::s_path[SStrLen(CCharacterComponent::s_path) - 5] = s_fileDecorations[this->m_data.m_preferences.sexID][0];
    }

    return TextureCacheCreateTexture(CCharacterComponent::s_path);
}

// OFFSET: 0x4ED6D0
bool CCharacterComponent::ItemsLoaded(bool a2) {
    TCTEXTUREINFO info;
    bool loaded = true;

    for (uint32_t i = 0; i < NUM_COMPONENT_SECTIONS; i++) {
        if (((1 << i) & this->m_dirtySections) == 0) {
            continue;
        }

        CharacterSection* section = &this->m_section[i];

        for (uint32_t j = 0; j < 7; j++) {
            if (section->layerItemDisplayId[j]) {
                if (!section->layerTex[j]) {
                    ItemDisplayInfoRec* displayRec = g_itemDisplayInfoDB.GetRecord(section->layerItemDisplayId[j]);

                    if (displayRec) {
                        section->layerTex[j] = this->LoadItemComponentTexture(displayRec, (COMPONENT_SECTIONS)i);
                    }
                }
            } else if (!section->layerTex[j]) {
                continue;
            }

            if (a2) {
                if (!section->layerTex[j]) {
                    continue;
                }

                TextureCacheGetInfo(section->layerTex[j], &info, 1);
            } else if (!TextureCacheGetInfo(section->layerTex[j], &info, 0)) {
                loaded = false;
                continue;
            }

            if (TextureCacheHasMips(section->layerTex[j])) {
                section->layerMask |= 1 << j;
            } else {
                section->layerMask &= ~(1 << j);
            }
        }
    }

    return loaded;
}

// OFFSET: 0x4EE380
bool CCharacterComponent::UpdateTextureSlot(COMPONENT_SECTIONS section, const ItemDisplayInfoRec* displayRec, int32_t layer) {
    if ((this->m_data.m_flags & 1) != 0) {
        return false;
    }

    CharacterSection* characterSection = &this->m_section[section];

    if (characterSection->layerItemDisplayId[layer]) {
        ItemDisplayInfoRec* current = g_itemDisplayInfoDB.GetRecord(characterSection->layerItemDisplayId[layer]);

        if (current && current->m_texture[section] == displayRec->m_texture[section]) {
            return false;
        }
    }

    characterSection->layerMask &= ~(1 << layer);

    if (characterSection->layerTex[layer]) {
        TextureCacheDestroyTexture(characterSection->layerTex[layer]);
        characterSection->layerTex[layer] = nullptr;
    }

    characterSection->layerItemDisplayId[layer] = displayRec->m_ID;

    return true;
}

// OFFSET: 0x4EA880
void CCharacterComponent::FreeSectionTexture(COMPONENT_SECTIONS section, int32_t layer) {
    if (layer == -1) {
        return;
    }

    CharacterSection* characterSection = &this->m_section[section];

    if (characterSection->layerTex[layer]) {
        TextureCacheDestroyTexture(characterSection->layerTex[layer]);
        characterSection->layerTex[layer] = nullptr;
    }

    characterSection->layerItemDisplayId[layer] = 0;
    characterSection->layerMask &= ~(1 << layer);
}

// OFFSET: 0x4EFFA0
void CCharacterComponent::GetItemDisplayPriority(ITEM_SLOT itemSlot, COMPONENT_SECTIONS section, const ItemDisplayInfoRec* displayRec, bool update) {
    int32_t layer = s_itemPriority[itemSlot][section];
    bool handled = false;

    if (section == SECTION_ARM_LOWER) {
        if (displayRec && displayRec->m_geosetGroup[0]) {
            if (itemSlot == ITEMSLOT_3) {
                layer = 5;
            } else if (itemSlot == ITEMSLOT_8) {
                layer = 6;
            }
        }
    } else if (section == SECTION_LEG_LOWER) {
        if (itemSlot == ITEMSLOT_3) {
            if (displayRec && displayRec->m_geosetGroup[2]) {
                layer = 4;
                this->m_flags |= 4;
            }
        } else if (itemSlot == ITEMSLOT_5) {
            if (displayRec && displayRec->m_geosetGroup[2]) {
                layer = ItemDisplayHasGeosetGroup(this->m_itemDisplayID[3], 2) ? 3 : 4;
                this->m_flags |= 4;
            }
        } else if (itemSlot == ITEMSLOT_6 && displayRec && displayRec->m_geosetGroup[0]) {
            this->m_flags |= 4;
            layer = 3;
        }
    } else if (section == SECTION_FOOT && (this->m_flags & 0x10) != 0) {
        update = false;
        this->FreeSectionTexture(section, s_itemPriority[itemSlot][section]);
        handled = true;
    }

    if (!handled) {
        if (update) {
            if (!this->UpdateTextureSlot(section, displayRec, layer)) {
                return;
            }
        } else {
            this->FreeSectionTexture(section, layer);
        }
    }

    if (section == SECTION_LEG_LOWER && !update) {
        ItemDisplayInfoRec rec;

        if (itemSlot == ITEMSLOT_3) {
            if (ItemDisplayHasGeosetGroup(this->m_itemDisplayID[5], 2, &rec)) {
                this->FreeSectionTexture(SECTION_LEG_LOWER, 3);
                this->UpdateTextureSlot(SECTION_LEG_LOWER, &rec, 4);
            }

            if (ItemDisplayHasGeosetGroup(this->m_itemDisplayID[6], 0, &rec)) {
                this->UpdateTextureSlot(SECTION_LEG_LOWER, &rec, 3);
            }
        } else if (itemSlot == ITEMSLOT_5 && ItemDisplayHasGeosetGroup(this->m_itemDisplayID[5], 2, &rec)) {
            this->UpdateTextureSlot(SECTION_LEG_LOWER, &rec, 4);
        }
    }

    if (layer != -1) {
        this->m_dirtySections |= 1 << section;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4F01A0
void CCharacterComponent::UpdateItemAU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_ARM_UPPER];

    if (update) {
        if (!this->UpdateTextureSlot(SECTION_ARM_UPPER, displayRec, layer)) {
            return;
        }
    } else {
        this->FreeSectionTexture(SECTION_ARM_UPPER, layer);
    }

    if (layer != -1) {
        this->m_dirtySections |= 1 << SECTION_ARM_UPPER;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4F0200
void CCharacterComponent::UpdateItemAL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_ARM_LOWER];

    if (displayRec && displayRec->m_geosetGroup[0]) {
        if (itemSlot == ITEMSLOT_3) {
            layer = 5;
        } else if (itemSlot == ITEMSLOT_8) {
            layer = itemSlot - 2;
        }
    }

    if (update) {
        if (!this->UpdateTextureSlot(SECTION_ARM_LOWER, displayRec, layer)) {
            return;
        }
    } else {
        this->FreeSectionTexture(SECTION_ARM_LOWER, layer);
    }

    if (layer != -1) {
        this->m_dirtySections |= 1 << SECTION_ARM_LOWER;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4F0280
void CCharacterComponent::UpdateItemHA(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_HAND];

    if (update) {
        if (!this->UpdateTextureSlot(SECTION_HAND, displayRec, layer)) {
            return;
        }
    } else {
        this->FreeSectionTexture(SECTION_HAND, layer);
    }

    if (layer != -1) {
        this->m_dirtySections |= 1 << SECTION_HAND;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

void CCharacterComponent::UpdateItemHU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
}

void CCharacterComponent::UpdateItemHL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
}

// OFFSET: 0x4F02E0
void CCharacterComponent::UpdateItemTU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_TORSO_UPPER];

    if (update) {
        if (!this->UpdateTextureSlot(SECTION_TORSO_UPPER, displayRec, layer)) {
            return;
        }
    } else {
        this->FreeSectionTexture(SECTION_TORSO_UPPER, layer);
    }

    if (layer != -1) {
        this->m_dirtySections |= 1 << SECTION_TORSO_UPPER;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4F0340
void CCharacterComponent::UpdateItemTL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_TORSO_LOWER];

    if (update) {
        if (!this->UpdateTextureSlot(SECTION_TORSO_LOWER, displayRec, layer)) {
            return;
        }
    } else {
        this->FreeSectionTexture(SECTION_TORSO_LOWER, layer);
    }

    if (layer != -1) {
        this->m_dirtySections |= 1 << SECTION_TORSO_LOWER;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4F03A0
void CCharacterComponent::UpdateItemLU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_LEG_UPPER];

    if (update) {
        if (!this->UpdateTextureSlot(SECTION_LEG_UPPER, displayRec, layer)) {
            return;
        }
    } else {
        this->FreeSectionTexture(SECTION_LEG_UPPER, layer);
    }

    if (layer != -1) {
        this->m_dirtySections |= 1 << SECTION_LEG_UPPER;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4F0400
void CCharacterComponent::UpdateItemLL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
    this->GetItemDisplayPriority(itemSlot, SECTION_LEG_LOWER, displayRec, update);
}

// OFFSET: 0x4F0420
void CCharacterComponent::UpdateItemFO(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_FOOT];

    if ((this->m_flags & 0x10) == 0 && update) {
        if (!this->UpdateTextureSlot(SECTION_FOOT, displayRec, layer)) {
            return;
        }
    } else {
        this->FreeSectionTexture(SECTION_FOOT, layer);
    }

    if (layer != -1) {
        this->m_dirtySections |= 1 << SECTION_FOOT;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4EC0E0
void CCharacterComponent::ClearGuildTexture(int32_t layer) {
    for (int32_t i = layer; i > 1; i--) {
        CharacterSection* section = &this->m_section[SECTION_TORSO_UPPER];

        if (section->layerTex[i]) {
            TextureCacheDestroyTexture(section->layerTex[i]);
            section->layerTex[i] = nullptr;
        }

        section->layerMask &= ~(1 << i);
        section->layerItemDisplayId[i] = 0;

        this->m_dirtySections |= 1 << SECTION_TORSO_UPPER;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }

    for (int32_t i = layer; i > 1; i--) {
        CharacterSection* section = &this->m_section[SECTION_TORSO_LOWER];

        if (section->layerTex[i]) {
            TextureCacheDestroyTexture(section->layerTex[i]);
            section->layerTex[i] = nullptr;
        }

        section->layerMask &= ~(1 << i);
        section->layerItemDisplayId[i] = 0;

        this->m_dirtySections |= 1 << SECTION_TORSO_LOWER;

        if (this->m_request) {
            *static_cast<uint32_t*>(this->m_request) &= ~1u;
            this->m_request = nullptr;
        }

        this->m_flags &= ~8u;
    }
}

// OFFSET: 0x4EE460
void CCharacterComponent::RemoveItem(ITEM_SLOT itemSlot) {
    if (!this->m_itemDisplayID[itemSlot]) {
        return;
    }

    this->m_flags |= 4;

    switch (itemSlot) {
    case ITEMSLOT_0: {
        this->m_data.m_geosets[0] = GetConditionalGeoset(&this->m_data.m_preferences);

        CM2Model* model = this->m_data.m_model;

        if (model && model->IsLoaded(0, 0) && model->HasAttachment(11)) {
            model->DetachAllChildrenById(11);
        }

        CharacterFacialHairStylesRec* facialHair = GetConditionalFacialHairStyle(&this->m_data.m_preferences);

        if (facialHair) {
            this->m_data.m_geosets[1] = facialHair->m_variationID + 100;
            this->m_data.m_geosets[3] = facialHair->m_geoset[0] + 300;
            this->m_data.m_geosets[2] = facialHair->m_geoset[1] + 200;
            this->m_data.m_geosets[7] = 702;
            this->m_data.m_geosets[16] = facialHair->m_geoset[2] + 1600;
            this->m_data.m_geosets[17] = facialHair->m_geoset[3] + 1700;
        }

        this->m_itemDisplayID[itemSlot] = 0;

        return;
    }

    case ITEMSLOT_1: {
        CM2Model* model = this->m_data.m_model;

        if (model && model->IsLoaded(0, 0) && model->HasAttachment(6)) {
            model->DetachAllChildrenById(6);
        }

        model = this->m_data.m_model;

        if (model && model->IsLoaded(0, 0) && model->HasAttachment(5)) {
            model->DetachAllChildrenById(5);
        }

        this->m_itemDisplayID[itemSlot] = 0;

        return;
    }

    case ITEMSLOT_3:
        this->m_flags &= ~0x20u;

        break;

    case ITEMSLOT_9:
        if ((this->m_flags & 0x40) != 0) {
            this->m_itemDisplayID[itemSlot] = 0;

            return;
        }

        this->m_data.m_geosets[15] = 1501;
        this->ClearGuildTexture(4);

        break;

    case ITEMSLOT_10:
        this->m_data.m_geosets[15] = 1501;
        this->m_itemDisplayID[itemSlot] = 0;

        return;

    case ITEMSLOT_11: {
        if (this->m_itemSlotForAttachSlot[26] == 11) {
            CM2Model* model = this->m_data.m_model;

            if (model && model->IsLoaded(0, 0) && model->HasAttachment(26)) {
                model->DetachAllChildrenById(26);
            }

            this->m_itemSlotForAttachSlot[26] = -1;
        }

        this->m_itemDisplayID[itemSlot] = 0;

        return;
    }

    default:
        break;
    }

    ItemDisplayInfoRec* displayRec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[itemSlot]);

    this->m_itemDisplayID[itemSlot] = 0;

    for (int32_t i = 0; i < 8; i++) {
        (this->*CCharacterComponent::s_itemFunc[i])(itemSlot, displayRec, false);
    }
}

// OFFSET: 0x4EEB30
void CCharacterComponent::RemoveItemByInventoryType(uint32_t inventoryType) {
    ITEM_SLOT itemSlot;
    uint32_t clearMask = 0;

    switch (inventoryType) {
    case 1:
        this->RemoveItem(ITEMSLOT_0);

        return;

    case 3:
        this->RemoveItem(ITEMSLOT_1);

        return;

    case 4:
        itemSlot = ITEMSLOT_2;

        break;

    case 5:
    case 20:
        itemSlot = ITEMSLOT_3;
        clearMask = 0x20;

        break;

    case 6:
        itemSlot = ITEMSLOT_4;

        break;

    case 7:
        itemSlot = ITEMSLOT_5;

        break;

    case 8:
        itemSlot = ITEMSLOT_6;

        break;

    case 9:
        itemSlot = ITEMSLOT_7;

        break;

    case 10:
        itemSlot = ITEMSLOT_8;

        break;

    case 16:
        if (this->m_itemDisplayID[ITEMSLOT_10]) {
            this->m_flags |= 4;
            this->m_data.m_geosets[15] = 1501;
            this->m_itemDisplayID[ITEMSLOT_10] = 0;
        }

        return;

    case 19:
        if (this->m_itemDisplayID[ITEMSLOT_9]) {
            this->m_flags |= 4;

            if ((this->m_flags & 0x40) != 0) {
                this->m_itemDisplayID[ITEMSLOT_9] = 0;
            } else {
                this->m_data.m_geosets[15] = 1501;
                this->ClearGuildTexture(4);

                ItemDisplayInfoRec* displayRec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_9]);

                this->m_itemDisplayID[ITEMSLOT_9] = 0;

                for (int32_t i = 0; i < 8; i++) {
                    (this->*CCharacterComponent::s_itemFunc[i])(ITEMSLOT_9, displayRec, false);
                }
            }
        }

        return;

    default:
        return;
    }

    if (!this->m_itemDisplayID[itemSlot]) {
        return;
    }

    this->m_flags = (this->m_flags & ~clearMask) | 4;

    ItemDisplayInfoRec* displayRec = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[itemSlot]);

    this->m_itemDisplayID[itemSlot] = 0;

    for (int32_t i = 0; i < 8; i++) {
        (this->*CCharacterComponent::s_itemFunc[i])(itemSlot, displayRec, false);
    }
}

// OFFSET: 0x4E7700
void CCharacterComponent::ComponentCloseFingers(CM2Model* model, bool rightHand) {
    uint32_t first = rightHand ? 13 : 8;
    uint32_t last = rightHand ? 17 : 12;

    for (uint32_t i = first; i <= last; i++) {
        model->SetBoneSequence(i, 15, -1, 0, 1.0f, 0, 1);
    }
}

// OFFSET: 0x4E7800
bool CCharacterComponent::ComposeHelmModelFilePath(const ChrRacesRec* racesRec, const ItemDisplayInfoRec* displayRec, char* dest, uint32_t destSize) {
    char path[260];

    strcpy(path, "Item\\ObjectComponents\\Head\\");
    strcat(path, displayRec->m_modelName[0]);

    uint32_t length = strlen(path);
    char* cursor = &path[length];

    while (cursor > path && *cursor != '.') {
        cursor--;
    }

    if (cursor == path) {
        cursor = &path[length];
    }

    *cursor = '_';
    strcpy(cursor + 1, racesRec->m_clientPrefix);
    strcat(path, s_fileDecorations[this->m_data.m_preferences.sexID]);
    strcat(path, ".mdx");
    strcpy(dest, path);

    return 1;
}

// OFFSET: 0x4EEFA0
bool CCharacterComponent::GetHelmModelFilePath(ItemDisplayInfoRec* rec, ItemDisplayInfoRec** displayRec) {
    if (!this->m_itemDisplayID[ITEMSLOT_0]) {
        return 0;
    }

    ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_0]);

    if (!found) {
        return 0;
    }

    *rec = *found;

    if (!rec->m_modelName[0][0]) {
        return 0;
    }

    *displayRec = rec;

    ChrRacesRec* racesRec = g_chrRacesDB.GetRecord(this->m_data.m_preferences.raceID);

    if (!racesRec) {
        return 0;
    }

    return this->ComposeHelmModelFilePath(racesRec, rec, CCharacterComponent::s_pathEnd, 260);
}

// OFFSET: 0x4EA8F0
void CCharacterComponent::ComponentUtilAddItemVisual(CM2Model* model, int32_t itemVisualId) {
    while (model->m_attachList) {
        model->m_attachList->DetachFromParent();
    }

    ItemVisualsRec* visuals = g_itemVisualsDB.GetRecord(itemVisualId);

    if (!visuals) {
        return;
    }

    for (int32_t i = 0; i < 5; i++) {
        ItemVisualEffectsRec* effect = g_itemVisualEffectsDB.GetRecord(visuals->m_slot[i]);

        if (!effect || !effect->m_model[0]) {
            continue;
        }

        strcpy(CCharacterComponent::s_pathEnd, effect->m_model);

        CM2Model* visual = model->m_scene->CreateModel(CCharacterComponent::s_path, 0);

        if (visual) {
            visual->AttachToParent(model, i, nullptr, 0);
            visual->Release();
        }
    }
}

// OFFSET: 0x4EAA70
void CCharacterComponent::AddLink(CM2Model* parent, uint32_t attachmentId, const char* modelPath, const char* texturePath, int32_t itemVisualId, const ItemDisplayInfoRec* displayRec) {
    if (!parent) {
        return;
    }

    CM2Model* model = parent->m_scene->CreateModel(modelPath, 0);

    if (!model) {
        return;
    }

    HTEXTURE texture = TextureCreate(texturePath, CGxTexFlags(GxTex_LinearMipNearest, 0, 0, 0, 0, 0, 1), &CCharacterComponent::s_status, 0);

    if (!texture) {
        model->Release();

        return;
    }

    model->ReplaceTexture(2, texture);
    HandleClose(texture);

    if (itemVisualId > 0) {
        this->ComponentUtilAddItemVisual(model, itemVisualId);
    }

    parent->DetachAllChildrenById(attachmentId);
    model->AttachToParent(parent, attachmentId, nullptr, 0);

    if (attachmentId == 1) {
        this->ComponentCloseFingers(parent, 0);
    } else if (attachmentId == 2) {
        this->ComponentCloseFingers(parent, 1);
    }

    ReplaceParticleColor(displayRec->m_particleColorID, model);
    model->Release();
}

// OFFSET: 0x4EF020
bool CCharacterComponent::IsHelmModelCorrect() {
    CM2Model* child = this->m_data.m_model->m_attachList;

    while (child && child->m_attachmentId != 11) {
        child = child->m_attachNext;
    }

    if (!child) {
        return false;
    }

    ItemDisplayInfoRec rec;
    ItemDisplayInfoRec* displayRec;

    if (!this->GetHelmModelFilePath(&rec, &displayRec)) {
        return false;
    }

    strcpy(OsPathRemoveExtension(CCharacterComponent::s_path), "m2");

    return SStrCmpI(child->m_shared->m_filePath, CCharacterComponent::s_path, 0x7FFFFFFF) == 0;
}

// OFFSET: 0x4EF0D0
void CCharacterComponent::AddHelm(int32_t itemVisualId) {
    ItemDisplayInfoRec rec;
    ItemDisplayInfoRec* displayRec;

    if (!this->GetHelmModelFilePath(&rec, &displayRec)) {
        return;
    }

    strcpy(CCharacterComponent::s_buffer, "Item\\ObjectComponents\\Head\\");
    strcat(CCharacterComponent::s_buffer, displayRec->m_modelTexture[0]);
    strcat(CCharacterComponent::s_buffer, ".blp");
    strcpy(CCharacterComponent::s_path2, CCharacterComponent::s_buffer);

    this->AddLink(this->m_data.m_model, 11, CCharacterComponent::s_path, CCharacterComponent::s_path2, itemVisualId, displayRec);

    HelmetGeosetVisDataRec* visData = g_helmetGeosetVisDataDB.GetRecord(displayRec->m_helmetGeosetVisID[this->m_data.m_preferences.sexID]);

    if (!visData) {
        return;
    }

    uint32_t raceMask = 1 << this->m_data.m_preferences.raceID;

    if (raceMask & visData->m_hideGeoset[0]) {
        this->m_data.m_geosets[0] = 1;
    }

    if (raceMask & visData->m_hideGeoset[1]) {
        this->m_data.m_geosets[1] = 101;
    }

    if (raceMask & visData->m_hideGeoset[2]) {
        this->m_data.m_geosets[2] = 201;
    }

    if (raceMask & visData->m_hideGeoset[3]) {
        this->m_data.m_geosets[3] = 301;
    }

    if (raceMask & visData->m_hideGeoset[4]) {
        this->m_data.m_geosets[7] = 701;
    }

    if (raceMask & visData->m_hideGeoset[5]) {
        this->m_data.m_geosets[16] = 1601;
    }

    if (raceMask & visData->m_hideGeoset[6]) {
        this->m_data.m_geosets[17] = 1701;
    }
}

// OFFSET: 0x4EF250
int32_t CCharacterComponent::GetQuiverModelFilePath(ItemDisplayInfoRec* rec, ItemDisplayInfoRec** displayRec) {
    if (!this->m_itemDisplayID[ITEMSLOT_11]) {
        return 0;
    }

    ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_11]);

    if (!found) {
        return 0;
    }

    *rec = *found;

    if (!rec->m_modelName[0][0]) {
        return 0;
    }

    *displayRec = rec;

    strcpy(CCharacterComponent::s_buffer, "Item\\ObjectComponents\\Quiver\\");
    strcat(CCharacterComponent::s_buffer, (*displayRec)->m_modelName[0]);
    strcpy(CCharacterComponent::s_pathEnd, CCharacterComponent::s_buffer);

    return 1;
}

// OFFSET: 0x4EF300
bool CCharacterComponent::IsQuiverModelCorrect() {
    CM2Model* child = this->m_data.m_model->m_attachList;

    while (child && child->m_attachmentId != 26) {
        child = child->m_attachNext;
    }

    if (!child) {
        return false;
    }

    ItemDisplayInfoRec rec;
    ItemDisplayInfoRec* displayRec;

    if (!this->GetQuiverModelFilePath(&rec, &displayRec)) {
        return false;
    }

    strcpy(OsPathRemoveExtension(CCharacterComponent::s_path), "m2");

    return SStrCmpI(child->m_shared->m_filePath, CCharacterComponent::s_path, 0x7FFFFFFF) == 0;
}

// OFFSET: 0x4EF3B0
void CCharacterComponent::AddQuiver(int32_t itemVisualId) {
    ItemDisplayInfoRec rec;
    ItemDisplayInfoRec* displayRec;

    if (!this->GetQuiverModelFilePath(&rec, &displayRec)) {
        return;
    }

    strcpy(CCharacterComponent::s_buffer, "Item\\ObjectComponents\\Quiver\\");
    strcat(CCharacterComponent::s_buffer, displayRec->m_modelTexture[0]);
    strcat(CCharacterComponent::s_buffer, ".blp");
    strcpy(CCharacterComponent::s_path2, CCharacterComponent::s_buffer);

    this->m_itemSlotForAttachSlot[26] = ITEMSLOT_11;

    this->AddLink(this->m_data.m_model, 26, CCharacterComponent::s_path, CCharacterComponent::s_path2, itemVisualId, displayRec);
}

// OFFSET: 0x4EF4B0
int32_t CCharacterComponent::BuildShoulderItemPaths(ItemDisplayInfoRec* rec, ItemDisplayInfoRec** displayRec, char* leftModel, char* rightModel, char* leftTexture, char* rightTexture) {
    *leftModel = 0;
    *rightModel = 0;

    if (leftTexture) {
        *leftTexture = 0;
    }

    if (rightTexture) {
        *rightTexture = 0;
    }

    if (!this->m_itemDisplayID[ITEMSLOT_1]) {
        return 0;
    }

    ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_1]);

    if (!found) {
        return 0;
    }

    *rec = *found;
    *displayRec = rec;

    if ((*displayRec)->m_modelName[0][0]) {
        strcpy(CCharacterComponent::s_buffer, "Item\\ObjectComponents\\Shoulder\\");
        strcat(CCharacterComponent::s_buffer, (*displayRec)->m_modelName[0]);
        strcpy(leftModel, CCharacterComponent::s_buffer);

        if (leftTexture) {
            strcpy(CCharacterComponent::s_buffer, "Item\\ObjectComponents\\Shoulder\\");
            strcat(CCharacterComponent::s_buffer, (*displayRec)->m_modelTexture[0]);
            strcat(CCharacterComponent::s_buffer, ".blp");
            strcpy(leftTexture, CCharacterComponent::s_buffer);
        }
    }

    if ((*displayRec)->m_modelName[1][0]) {
        strcpy(CCharacterComponent::s_buffer, "Item\\ObjectComponents\\Shoulder\\");
        strcat(CCharacterComponent::s_buffer, (*displayRec)->m_modelName[1]);
        strcpy(rightModel, CCharacterComponent::s_buffer);

        if (rightTexture) {
            strcpy(CCharacterComponent::s_buffer, "Item\\ObjectComponents\\Shoulder\\");
            strcat(CCharacterComponent::s_buffer, (*displayRec)->m_modelTexture[1]);
            strcat(CCharacterComponent::s_buffer, ".blp");
            strcpy(rightTexture, CCharacterComponent::s_buffer);
        }
    }

    return 1;
}

// OFFSET: 0x4EF710
int32_t CCharacterComponent::AreShoulderModelsCorrect() {
    CM2Model* left = this->m_data.m_model->m_attachList;

    while (left && left->m_attachmentId != 6) {
        left = left->m_attachNext;
    }

    if (!left) {
        return 0;
    }

    CM2Model* right = this->m_data.m_model->m_attachList;

    while (right && right->m_attachmentId != 5) {
        right = right->m_attachNext;
    }

    if (!right) {
        return 0;
    }

    ItemDisplayInfoRec rec;
    ItemDisplayInfoRec* displayRec;

    if (!this->BuildShoulderItemPaths(&rec, &displayRec, CCharacterComponent::s_path, CCharacterComponent::s_path2, nullptr, nullptr)) {
        return 0;
    }

    strcpy(OsPathRemoveExtension(CCharacterComponent::s_path), "m2");
    strcpy(OsPathRemoveExtension(CCharacterComponent::s_path2), "m2");

    if (SStrCmpI(left->m_shared->m_filePath, CCharacterComponent::s_path, 0x7FFFFFFF)) {
        return 0;
    }

    if (SStrCmpI(right->m_shared->m_filePath, CCharacterComponent::s_path2, 0x7FFFFFFF)) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x4EF840
void CCharacterComponent::AddShoulders(int32_t itemVisualId) {
    char leftModel[260];
    char rightModel[260];
    char leftTexture[260];
    char rightTexture[260];

    ItemDisplayInfoRec rec;
    ItemDisplayInfoRec* displayRec;

    if (!this->BuildShoulderItemPaths(&rec, &displayRec, leftModel, rightModel, leftTexture, rightTexture)) {
        return;
    }

    if (leftModel[0]) {
        this->AddLink(this->m_data.m_model, 6, leftModel, leftTexture, itemVisualId, displayRec);
    }

    if (rightModel[0]) {
        this->AddLink(this->m_data.m_model, 5, rightModel, rightTexture, itemVisualId, displayRec);
    }
}

// OFFSET: 0x4F21E0
void CCharacterComponent::AddCape() {
    if (!this->m_itemDisplayID[ITEMSLOT_10]) {
        return;
    }

    ItemDisplayInfoRec rec;
    ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_10]);

    if (!found) {
        return;
    }

    rec = *found;

    if (!rec.m_modelTexture[0][0]) {
        return;
    }

    strcpy(CCharacterComponent::s_buffer, "Item\\ObjectComponents\\Cape\\");
    strcat(CCharacterComponent::s_buffer, rec.m_modelTexture[0]);
    strcat(CCharacterComponent::s_buffer, ".blp");
    strcpy(CCharacterComponent::s_path, CCharacterComponent::s_buffer);

    CStatus status;

    HTEXTURE texture = TextureCreate(CCharacterComponent::s_path, &status);

    if (texture) {
        this->m_data.m_model->ReplaceTexture(2, texture);
        HandleClose(texture);
    }
}

// OFFSET: 0x4F2640
void CCharacterComponent::AddItem(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, int32_t itemVisualId) {
    this->m_flags |= 4;
    this->m_itemDisplayID[itemSlot] = displayRec->m_ID;

    switch (itemSlot) {
    case ITEMSLOT_0:
        if (!this->IsHelmModelCorrect()) {
            if (this->m_itemDisplayID[ITEMSLOT_0]) {
                CM2Model* model = this->m_data.m_model;

                if (model && model->IsLoaded(0, 0) && model->HasAttachment(11)) {
                    model->DetachAllChildrenById(11);
                }
            }

            this->AddHelm(itemVisualId);
        }

        return;

    case ITEMSLOT_1:
        if (!this->AreShoulderModelsCorrect()) {
            if (this->m_itemDisplayID[ITEMSLOT_1]) {
                CM2Model* model = this->m_data.m_model;

                if (model && model->IsLoaded(0, 0) && model->HasAttachment(6)) {
                    model->DetachAllChildrenById(6);
                }

                model = this->m_data.m_model;

                if (model && model->IsLoaded(0, 0) && model->HasAttachment(5)) {
                    model->DetachAllChildrenById(5);
                }
            }

            this->AddShoulders(itemVisualId);
        }

        return;

    case ITEMSLOT_3:
        this->m_flags = (this->m_flags & ~0x20u) | (displayRec->m_flags & 4 ? 0x20 : 0);

        break;

    case ITEMSLOT_10:
        this->AddCape();

        return;

    case ITEMSLOT_11:
        if (!this->IsQuiverModelCorrect()) {
            if (this->m_itemDisplayID[ITEMSLOT_11]) {
                CM2Model* model = this->m_data.m_model;

                if (model && model->IsLoaded(0, 0) && model->HasAttachment(26)) {
                    model->DetachAllChildrenById(26);
                }
            }

            this->AddQuiver(itemVisualId);
        }

        return;

    default:
        break;
    }

    if ((this->m_data.m_flags & 1) != 0) {
        return;
    }

    for (int32_t i = 0; i < 8; i++) {
        if (displayRec->m_texture[i][0] && s_itemPriority[itemSlot][i] != -1) {
            (this->*CCharacterComponent::s_itemFunc[i])(itemSlot, displayRec, true);
        }
    }
}

// OFFSET: 0x4F2830
void CCharacterComponent::AddItem(ITEM_SLOT itemSlot, int32_t itemDisplayId, int32_t itemVisualId) {
    if (itemDisplayId <= 0) {
        return;
    }

    ItemDisplayInfoRec* displayRec = g_itemDisplayInfoDB.GetRecord(itemDisplayId);

    if (!displayRec) {
        return;
    }

    this->AddItem(itemSlot, displayRec, itemVisualId);
}

// OFFSET: 0x4E79A0
void CCharacterComponent::RemoveLinkpt(CM2Model* model, uint32_t attachmentId) {
    if (attachmentId == -1) {
        return;
    }

    if (attachmentId == 0 || attachmentId == 2) {
        for (uint32_t i = 13; i <= 17; i++) {
            model->UnsetBoneSequence(i, 0, 1);
        }
    } else if (attachmentId == 1) {
        for (uint32_t i = 8; i <= 12; i++) {
            model->UnsetBoneSequence(i, 0, 1);
        }
    }

    if (model && model->IsLoaded(0, 0) && model->HasAttachment(attachmentId)) {
        model->DetachAllChildrenById(attachmentId);
    }
}

// OFFSET: 0x4EACD0
uint32_t CCharacterComponent::AddHandItem(CM2Model* model, const ItemDisplayInfoRec* displayRec, uint32_t attachmentId, int32_t sheatheType, bool useSheathed, bool isShield, bool treatAsMainHand, int32_t itemVisualId) {
    if (!model || !displayRec || attachmentId > 0x12) {
        return -1;
    }

    const char* prefix = "Item\\ObjectComponents\\Weapon\\";

    uint32_t hand;
    uint32_t sheathed;

    if (attachmentId == 15 || (attachmentId == 17 && treatAsMainHand)) {
        hand = 1;

        switch (sheatheType) {
        case 1:
            sheathed = 26;
            break;

        case 2:
            sheathed = 30;
            break;

        case 3:
            sheathed = 32;
            break;

        case 4:
            sheathed = 28;
            break;

        default:
            sheathed = -1;
            break;
        }
    } else if (attachmentId == 16 || attachmentId == 17) {
        hand = 2;

        switch (sheatheType) {
        case 1:
            sheathed = 27;
            break;

        case 2:
            sheathed = 31;
            break;

        case 3:
            sheathed = 33;
            break;

        case 4:
            sheathed = 28;
            break;

        default:
            sheathed = -1;
            break;
        }

        if (attachmentId == 16 && isShield) {
            prefix = "Item\\ObjectComponents\\Shield\\";
            hand = 0;
        }
    } else {
        return -1;
    }

    this->RemoveLinkpt(model, hand);
    this->RemoveLinkpt(model, sheathed);

    uint32_t attachment = useSheathed ? sheathed : hand;

    if (model->IsLoaded(0, 0) && !model->HasAttachment(attachment)) {
        return -1;
    }

    strcpy(CCharacterComponent::s_buffer, prefix);
    strcat(CCharacterComponent::s_buffer, displayRec->m_modelName[0]);
    strcpy(CCharacterComponent::s_path, CCharacterComponent::s_buffer);

    strcpy(CCharacterComponent::s_buffer, prefix);
    strcat(CCharacterComponent::s_buffer, displayRec->m_modelTexture[0]);
    strcat(CCharacterComponent::s_buffer, ".blp");
    strcpy(CCharacterComponent::s_path2, CCharacterComponent::s_buffer);

    this->AddLink(model, attachment, CCharacterComponent::s_path, CCharacterComponent::s_path2, itemVisualId ? itemVisualId : displayRec->m_itemVisual, displayRec);

    return attachment;
}

// OFFSET: 0x4EF900
void CCharacterComponent::SetHandItemDisplay(int32_t itemDisplayId, uint32_t handIndex, uint32_t attachmentId, bool isShield) {
    ItemDisplayInfoRec* displayRec = g_itemDisplayInfoDB.GetRecord(itemDisplayId);

    if (!displayRec) {
        return;
    }

    this->m_handItemDisplayID[handIndex] = itemDisplayId;

    this->AddHandItem(this->m_data.m_model, displayRec, attachmentId, 0, 0, isShield, 0, 0);
}

// OFFSET: 0x4EF970
void CCharacterComponent::SetMainHandItemDisplay(int32_t itemDisplayId) {
    this->SetHandItemDisplay(itemDisplayId, 0, 15, 0);
}

// OFFSET: 0x4EF990
void CCharacterComponent::SetOffHandItemDisplay(int32_t itemDisplayId) {
    this->SetHandItemDisplay(itemDisplayId, 1, 16, 0);
}

// OFFSET: 0x4EF9B0
void CCharacterComponent::SetShieldItemDisplay(int32_t itemDisplayId) {
    this->SetHandItemDisplay(itemDisplayId, 1, 16, 1);
}

// OFFSET: 0x4F29C0
void CCharacterComponent::AddItemByType(uint32_t inventoryType, int32_t itemDisplayId) {
    this->RemoveItemByInventoryType(inventoryType);

    if (itemDisplayId <= 0) {
        return;
    }

    switch (inventoryType) {
    case 1:
        this->AddItem(ITEMSLOT_0, itemDisplayId, 0);
        break;

    case 3:
        this->AddItem(ITEMSLOT_1, itemDisplayId, 0);
        break;

    case 4:
        this->AddItem(ITEMSLOT_2, itemDisplayId, 0);
        break;

    case 5:
    case 20:
        this->AddItem(ITEMSLOT_3, itemDisplayId, 0);
        break;

    case 6:
        this->AddItem(ITEMSLOT_4, itemDisplayId, 0);
        break;

    case 7:
        this->AddItem(ITEMSLOT_5, itemDisplayId, 0);
        break;

    case 8:
        this->AddItem(ITEMSLOT_6, itemDisplayId, 0);
        break;

    case 9:
        this->AddItem(ITEMSLOT_7, itemDisplayId, 0);
        break;

    case 10:
        this->AddItem(ITEMSLOT_8, itemDisplayId, 0);
        break;

    case 13:
    case 17:
    case 21:
        this->SetMainHandItemDisplay(itemDisplayId);
        break;

    case 14:
        this->SetShieldItemDisplay(itemDisplayId);
        break;

    case 16:
        this->AddItem(ITEMSLOT_10, itemDisplayId, 0);
        break;

    case 19:
        this->AddItem(ITEMSLOT_9, itemDisplayId, 0);
        break;

    case 22:
        this->SetOffHandItemDisplay(itemDisplayId);
        break;

    default:
        break;
    }
}

// OFFSET: 0x4F2880
void CCharacterComponent::AddItemBySlot(int32_t itemSlot, int32_t itemDisplayId, int32_t itemVisualId) {
    switch (itemSlot) {
    case 0:
        this->AddItem(ITEMSLOT_0, itemDisplayId, itemVisualId);
        break;

    case 2:
        this->AddItem(ITEMSLOT_1, itemDisplayId, itemVisualId);
        break;

    case 3:
        this->AddItem(ITEMSLOT_2, itemDisplayId, itemVisualId);
        break;

    case 4:
        this->AddItem(ITEMSLOT_3, itemDisplayId, itemVisualId);
        break;

    case 5:
        this->AddItem(ITEMSLOT_4, itemDisplayId, itemVisualId);
        break;

    case 6:
        this->AddItem(ITEMSLOT_5, itemDisplayId, itemVisualId);
        break;

    case 7:
        this->AddItem(ITEMSLOT_6, itemDisplayId, itemVisualId);
        break;

    case 8:
        this->AddItem(ITEMSLOT_7, itemDisplayId, itemVisualId);
        break;

    case 9:
        this->AddItem(ITEMSLOT_8, itemDisplayId, itemVisualId);
        break;

    case 14:
        this->AddItem(ITEMSLOT_10, itemDisplayId, itemVisualId);
        break;

    case 18:
        this->AddItem(ITEMSLOT_9, itemDisplayId, itemVisualId);
        break;

    default:
        break;
    }
}

// OFFSET: 0x4EB070
void CCharacterComponent::RemoveHandItem(CM2Model* model, uint32_t attachmentId, int32_t sheatheType, bool isShield) {
    if (!model || attachmentId - 14 > 4) {
        return;
    }

    uint32_t hand = -1;
    uint32_t sheathed = -1;

    if (attachmentId == 15) {
        hand = 1;

        switch (sheatheType) {
        case 1:
            sheathed = 26;
            break;

        case 2:
            sheathed = 30;
            break;

        case 3:
            sheathed = 32;
            break;

        case 4:
            sheathed = 28;
            break;

        default:
            sheathed = -1;
            break;
        }
    } else if (attachmentId > 15 && attachmentId <= 17) {
        hand = 2;

        switch (sheatheType) {
        case 1:
            sheathed = 27;
            break;

        case 2:
            sheathed = 31;
            break;

        case 3:
            sheathed = 33;
            break;

        case 4:
            sheathed = 28;
            break;

        default:
            sheathed = -1;
            break;
        }

        if (isShield) {
            hand = 0;
        }
    }

    this->RemoveLinkpt(model, hand);
    this->RemoveLinkpt(model, sheathed);
}
