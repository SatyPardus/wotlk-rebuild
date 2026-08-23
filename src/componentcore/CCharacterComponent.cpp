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

CVar* CCharacterComponent::g_componentTextureLevelVar = nullptr;
CVar* CCharacterComponent::g_componentThreadVar = nullptr;
CVar* CCharacterComponent::g_componentCompressVar = nullptr;

uint32_t* CCharacterComponent::s_heap = nullptr;
uint32_t CCharacterComponent::s_chrVarArrayLength = 0;
st_race* CCharacterComponent::s_chrVarArray = nullptr;
uint32_t* CCharacterComponent::s_characterFacialHairStylesList = nullptr;
EGxTexFormat CCharacterComponent::s_gxFormatHigh;

char CCharacterComponent::s_path[260];
char* CCharacterComponent::s_pathEnd;
CStatus CCharacterComponent::s_status;


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
void CCharacterComponent::Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThreads, int32_t useCompression) {
    CCharacterComponent::s_heap = static_cast<uint32_t*>(ALLOC(sizeof(uint32_t)));
    if (CCharacterComponent::s_heap) {
        *CCharacterComponent::s_heap = ObjectAllocAddHeap(sizeof(CCharacterComponent), 32, "CCharacterComponent", true);
    }

    s_pathEnd = s_path;
    //s_pathEnd2 = path;
    //CCharacterComponent::m_prepFunc[0] = CCharacterComponent::RenderPrepAU;
    //CCharacterComponent::m_prepFunc[1] = CCharacterComponent::RenderPrepAL;
    //CCharacterComponent::m_prepFunc[2] = CCharacterComponent::RenderPrepHA;
    //CCharacterComponent::m_prepFunc[8] = CCharacterComponent::RenderPrepHU;
    //CCharacterComponent::m_prepFunc[9] = CCharacterComponent::RenderPrepHL;
    //CCharacterComponent::m_prepFunc[3] = CCharacterComponent::RenderPrepTU;
    //CCharacterComponent::m_prepFunc[4] = CCharacterComponent::RenderPrepTL;
    //CCharacterComponent::m_prepFunc[5] = CCharacterComponent::RenderPrepLU;
    //CCharacterComponent::m_prepFunc[6] = CCharacterComponent::RenderPrepLL;
    //CCharacterComponent::m_prepFunc[7] = CCharacterComponent::RenderPrepFO;
    //CCharacterComponent::m_itemFunc[0] = CCharacterComponent::UpdateItemAU;
    //CCharacterComponent::m_itemFunc[1] = CCharacterComponent::UpdateItemAL;
    //CCharacterComponent::m_itemFunc[2] = CCharacterComponent::UpdateItemHA;
    //CCharacterComponent::m_itemFunc[8] = NOP_0;
    //CCharacterComponent::m_itemFunc[9] = NOP_0;
    //CCharacterComponent::m_itemFunc[3] = CCharacterComponent::UpdateItemTU;
    //CCharacterComponent::m_itemFunc[4] = CCharacterComponent::UpdateItemTL;
    //CCharacterComponent::m_itemFunc[5] = CCharacterComponent::UpdateItemLU;
    //CCharacterComponent::m_itemFunc[6] = CCharacterComponent::UpdateItemLL;
    //CCharacterComponent::m_itemFunc[7] = CCharacterComponent::UpdateItemFO;
    //if (a2 <= 9) {
    //    if (a2 < 6)
    //        v5 = 6;
    //} else {
    //    v5 = 9;
    //}
    //if (!a4 && v5 > 8)
    //    v5 = 8;
    //bnl_CCharacterComponent__s_mipLevels = v5;
    //dword_B6B5FC = 1 << v5;
    //dword_B6B888[0] = dword_B6B928 >> (9 - v5);
    //dword_B6B88C = dword_B6B92C >> (9 - v5);
    //dword_B6B890[0] = dword_B6B930 >> (9 - v5);
    //dword_B6B894[0] = dword_B6B934 >> (9 - v5);
    //dword_B6B898 = dword_B6B938 >> (9 - v5);
    //dword_B6B89C = dword_B6B93C >> (9 - v5);
    //dword_B6B8A0 = dword_B6B940 >> (9 - v5);
    //dword_B6B8A4 = dword_B6B944 >> (9 - v5);
    //dword_B6B8A8 = dword_B6B948 >> (9 - v5);
    //dword_B6B8AC = dword_B6B94C >> (9 - v5);
    //dword_B6B8B0 = dword_B6B950 >> (9 - v5);
    //dword_B6B8B4 = dword_B6B954 >> (9 - v5);
    //dword_B6B8B8 = dword_B6B958 >> (9 - v5);
    //dword_B6B8BC = dword_B6B95C >> (9 - v5);
    //dword_B6B8C0 = dword_B6B960 >> (9 - v5);
    //dword_B6B8C4 = dword_B6B964 >> (9 - v5);
    //dword_B6B8C8 = dword_B6B968 >> (9 - v5);
    //dword_B6B8CC = dword_B6B96C >> (9 - v5);
    //dword_B6B8D0 = dword_B6B970 >> (9 - v5);
    //dword_B6B8D4 = dword_B6B974 >> (9 - v5);
    //dword_B6B8D8 = dword_B6B978 >> (9 - v5);
    //dword_B6B8DC = dword_B6B97C >> (9 - v5);
    //dword_B6B8E0 = dword_B6B980 >> (9 - v5);
    //dword_B6B8E4 = dword_B6B984 >> (9 - v5);
    //dword_B6B8E8 = dword_B6B988 >> (9 - v5);
    //dword_B6B8EC = dword_B6B98C >> (9 - v5);
    //dword_B6B8F0 = dword_B6B990 >> (9 - v5);
    //dword_B6B8F4 = dword_B6B994 >> (9 - v5);
    //dword_B6B8F8 = dword_B6B998 >> (9 - v5);
    //dword_B6B8FC = dword_B6B99C >> (9 - v5);
    //dword_B6B900 = dword_B6B9A0 >> (9 - v5);
    //dword_B6B904 = dword_B6B9A4 >> (9 - v5);
    //dword_B6B908 = dword_B6B9A8 >> (9 - v5);
    //dword_B6B90C = dword_B6B9AC >> (9 - v5);
    //dword_B6B910 = dword_B6B9B0 >> (9 - v5);
    //dword_B6B914 = dword_B6B9B4 >> (9 - v5);
    //dword_B6B918 = dword_B6B9B8 >> (9 - v5);
    //dword_B6B91C = dword_B6B9BC >> (9 - v5);
    //dword_B6B920 = dword_B6B9C0 >> (9 - v5);
    //dword_B6B924 = dword_B6B9C4 >> (9 - v5);
    //bn_TextureCacheResetLoadCount_0();
    auto v6 = 2 * g_chrRacesDB.GetNumRecords() + 2;
    CCharacterComponent::s_chrVarArrayLength = v6;
    BuildComponentArray(v6, &CCharacterComponent::s_chrVarArray);
    CountFacialFeatures(v6, &CCharacterComponent::s_characterFacialHairStylesList);
    //CCharacterComponent::s_bComponentThread = a3;
    //CCharacterComponent::s_bComponentCompression = 0;
    //CCharacterComponent::s_gxFormat = a1;
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
    //dword_B6B870 = 0;
    //dword_B6B86C = 0;
    //dword_B6B868 = 0;
    //dword_B6B870 = bn_TextureAllocMippedImg(2, dword_B6B5FC, dword_B6B5FC);
    //if (CCharacterComponent::s_bComponentCompression)
    //    dword_B6B86C = bn_TextureAllocMippedImg(0, dword_B6B5FC, dword_B6B5FC);
    //if (CCharacterComponent::s_bComponentThread && CCharacterComponent::s_bComponentCompression)
    //    dword_B6B868 = bn_TextureAllocMippedImg(2, dword_B6B5FC, dword_B6B5FC);
    //EventRegisterEx(EVENT_ON_POLL, bn_CCharacterComponent_Update, 0, 0.0);
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
    //this->m_gxTexFormat = CCharacterComponent::s_gxFormat;
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
    //m_itemDisplayID = this->m_itemDisplayID;
    //v17 = 9;
    //while (1) {
    //    m_itemDisplayID -= 15;
    //    do {
    //        if (m_itemDisplayID[v3])
    //            TextureCacheDestroyTexture(m_itemDisplayID[v3]);
    //        m_itemDisplayID[v3] = 0;
    //        m_itemDisplayID[v3++ + 7] = 0;
    //    } while (v3 < 7);
    //    if (--v17 < 0)
    //        break;
    //    v3 = 0;
    //}
    //m_section = this->m_section;
    //for (i = 4; i >= 0; --i) {
    //    m_section = (m_section - 12);
    //    for (j = 0; j < 7; ++j) {
    //        result = m_section->layerTex[j];
    //        if (result)
    //            result = TextureCacheDestroyTexture(m_section->layerTex[j]->gap0);
    //        m_section->layerTex[j] = 0;
    //    }
    //}
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
    if (v4) {
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
void CCharacterComponent::SkinNPC(const char* a2) {

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
        //if (!this->m_link.Next())
        //    TSList::LinkToTail_0(&stru_AC46E4, this);
        return true;
    }

    if (a2) {
        if (this->m_request) {
            //*m_request &= ~1u;
            this->m_request = nullptr;
        }
        //CCharacterComponent::sub_4ED640(this, 1);
        //CCharacterComponent::ItemsLoaded(this, 1);
        this->m_flags |= 8u;
        this->RenderPrepSections();
        this->m_link.Unlink();
        return true;
    }

    // if (!this->m_link.Next())
    //     TSList::LinkToTail_0(&stru_AC46E4, this);
    return false;
}

// OFFSET: 0x4F14A0
void CCharacterComponent::RenderPrepSections() {
    //dword_B6B884 = 1;
    if ((this->m_flags & 4) != 0)
        this->GeosRenderPrep();
    //if (!this->m_baseSkinTexture)
    //    this->CreateBaseTexture();
    //this->PrepSections();
    //this->m_flags &= ~1u;
    //this->m_dirtySections = 0;
    //this->DestroyRenderTextures();
    this->m_link.Unlink();
    //dword_B6B884 = 0;
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

    // TODO item stuff

    //this->m_data.m_model->OptimizeVisibleGeometry();
    this->m_flags &= ~4;
}

void CCharacterComponent::GetPreferences(CHARACTER_PREFERENCES* info) {
    if (info) {
        *info = this->m_data.m_preferences;
    }
}
