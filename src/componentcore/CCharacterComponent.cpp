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

CVar* CCharacterComponent::g_componentTextureLevelVar = nullptr;
CVar* CCharacterComponent::g_componentThreadVar = nullptr;
CVar* CCharacterComponent::g_componentCompressVar = nullptr;

uint32_t* CCharacterComponent::s_heap = nullptr;
uint32_t CCharacterComponent::s_chrVarArrayLength = 0;
st_race* CCharacterComponent::s_chrVarArray = nullptr;
EGxTexFormat CCharacterComponent::s_gxFormatHigh;


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

// OFFSET: 0x401FF0
void CCharacterComponent::Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThreads, int32_t useCompression) {
    CCharacterComponent::s_heap = static_cast<uint32_t*>(ALLOC(sizeof(uint32_t)));
    if (CCharacterComponent::s_heap) {
        *CCharacterComponent::s_heap = ObjectAllocAddHeap(sizeof(CCharacterComponent), 32, "CCharacterComponent", true);
    }

    //s_pathEnd = s_path;
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
    //CCharacterComponent::InitializeCharacterHairStylesLookup(v6, &CCharacterComponent::s_characterFacialHairStylesList);
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
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, 3, varitationIndex, this->m_data.m_preferences.hairColorID))
        return;

    auto v4 = CCharacterComponent::s_chrVarArray[2 * this->m_data.m_preferences.raceID + this->m_data.m_preferences.sexID].m_variation[3].variation[varitationIndex].color[this->m_data.m_preferences.hairColorID]->m_textureName[0];
    if (!v4)
        return;

    //v5 = s_pathEnd;
    //do {
    //    v6 = *v4;
    //    *v5++ = *v4++;
    //} while (v6);
    //Texture = CreateTexture(s_path, &status);
    //v8 = Texture;
    //if (Texture) {
    //    CM2Model::ReplaceTexture(this->m_data.model, 6, Texture);
    //    HandleClose(v8);
    //}
}

void CCharacterComponent::LoadBaseVariation(uint32_t variation, uint32_t textureIndex, uint32_t variationIndex, uint32_t colorIndex, uint32_t section, const char* a7) {
    auto texture = this->m_baseVariation[variation].m_texture[textureIndex];
    if (texture) {
        //TextureCacheDestroyTexture(texture);
        this->m_baseVariation[variation].m_texture[textureIndex] = nullptr;
    }

    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, variation, variationIndex, colorIndex))
        return;

    auto v4 = CCharacterComponent::s_chrVarArray[2 * this->m_data.m_preferences.raceID + this->m_data.m_preferences.sexID].m_variation[variation].variation[variationIndex].color[colorIndex]->m_textureName[textureIndex];
    if (v4) {
        //v13 = s_pathEnd;
        //do {
        //    v14 = *v12;
        //    *v13++ = *v12++;
        //} while (v14);
        //Texture = TextureCacheCreateTexture(s_path);
        //this->m_baseVariation[variation].m_texture[textureIndex] = Texture;
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
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, 3, this->m_data.m_preferences.hairStyleID, colorId))
        return;

    this->m_data.m_preferences.hairColorID = colorId;

    auto raceRec = g_chrRacesDB.GetRecord(this->m_data.m_preferences.raceID);
    if (!this->m_data.m_preferences.sexID && !this->m_data.m_preferences.hairStyleID && (raceRec->m_flags & 8) != 0)
        this->ReplaceHairTexture(1, a4);
    this->ReplaceHairTexture(this->m_data.m_preferences.hairStyleID, a4);

    if ((this->m_data.m_flags & 1) != 0)
        return;

    if (a3) {
        this->LoadBaseVariation(3, 1, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, 9, a4);
        this->LoadBaseVariation(3, 2, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, 8, a4);
    }

    this->LoadBaseVariation(2, 0, this->m_data.m_preferences.facialHairStyleID, this->m_data.m_preferences.hairColorID, 9, a4);
    this->LoadBaseVariation(2, 1, this->m_data.m_preferences.facialHairStyleID, this->m_data.m_preferences.hairColorID, 8, a4);
}

// OFFSET: 0x4EA3E0
void CCharacterComponent::SetHairStyle(uint32_t colorId, const char* a4) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, 3, colorId, this->m_data.m_preferences.hairColorID))
        return;

    this->m_data.m_preferences.hairStyleID = colorId;
    this->m_data.m_geosets[0] = GetConditionalGeoset(&this->m_data.m_preferences);

    if ((this->m_data.m_flags & 1) == 0) {
        this->LoadBaseVariation(3, 1, colorId, this->m_data.m_preferences.hairColorID, 9, a4);
        this->LoadBaseVariation(3, 2, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, 8, a4);
    }
    this->SetHairColor(this->m_data.m_preferences.hairColorID, false, a4);
    this->m_flags |= 4;
    this->m_dirtySections |= 300;
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

    auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, 0, 0, this->m_data.m_preferences.skinID, nullptr);
    if ((!rec || (rec->m_flags & 8) == 0) && !ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, 1, colorId, this->m_data.m_preferences.skinID))
        return;

    this->m_data.m_preferences.faceID = colorId;
    this->LoadBaseVariation(1, 0, colorId, this->m_data.m_preferences.skinID, 9, a4);
    this->LoadBaseVariation(1, 1, colorId, this->m_data.m_preferences.skinID, 8, a4);
    if (a3) {
        this->LoadBaseVariation(3, 1, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, 9, a4);
        this->LoadBaseVariation(3, 2, this->m_data.m_preferences.hairStyleID, this->m_data.m_preferences.hairColorID, 8, a4);
    }
    this->m_flags |= 4;
    this->m_dirtySections |= 300;
    if (this->m_request) {
        //*m_request &= ~1u;
        this->m_request = nullptr;
    }
    this->m_flags &= ~8;
}

// OFFSET: 0x4EA590
void CCharacterComponent::SetBeardStyle(uint32_t colorId, bool a3, const char* a4) {

}

// OFFSET: 0x4EA6B0
void CCharacterComponent::SetSkinColor(uint32_t colorId, bool a3, bool a4, const char* a5) {

}

// OFFSET: 0x4EB290
void CCharacterComponent::SetPrevSkin(COMPONENT_CONTEXT context) {

}

// OFFSET: 0x4EB150
void CCharacterComponent::SetNextSkin(COMPONENT_CONTEXT context) {

}

// OFFSET: 0x4EB990
void CCharacterComponent::SetPrevFace(COMPONENT_CONTEXT context, uint32_t skinIndex) {

}

// OFFSET: 0x4EB710
void CCharacterComponent::SetNextFace(COMPONENT_CONTEXT context, uint32_t skinIndex) {

}

// OFFSET: 0x4F0630
void CCharacterComponent::SetPrevHairStyle(COMPONENT_CONTEXT context) {

}

// OFFSET: 0x4F0490
void CCharacterComponent::SetNextHairStyle(COMPONENT_CONTEXT context) {

}

// OFFSET: 0x4EB5C0
void CCharacterComponent::SetPrevHairColor(COMPONENT_CONTEXT context) {

}

// OFFSET: 0x4EB500
void CCharacterComponent::SetNextHairColor(COMPONENT_CONTEXT context) {

}

// OFFSET: 0x4EBE80
void CCharacterComponent::SetPrevFacialFeature(COMPONENT_CONTEXT context) {

}

// OFFSET: 0x4EBCA0
void CCharacterComponent::SetNextFacialFeature(COMPONENT_CONTEXT context) {

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
    //SelectionFromContext = GetSelectionFromContext(a2, this->m_data.classId);
    //NumHairColorsForStyle = CCharacterComponent::GetNumHairColorsForStyle(
    //    this->m_data.raceId,
    //    this->m_data.genderId,
    //    this->m_data.classId,
    //    this->m_data.hairStyleId,
    //    a2);
    //if (NumHairColorsForStyle > 0) {
    //    Hash = SecureRandom::GetHash(g_rndSeed);
    //    v5 = bn_aullshr(0x20u, (NumHairColorsForStyle * Hash) >> 32);
    //} else {
    //    v5 = 0;
    //}
    //HairColor = ComponentGetHairColor(
    //    this->m_data.raceId,
    //    this->m_data.genderId,
    //    this->m_data.hairStyleId,
    //    v5,
    //    SelectionFromContext);
    //if (HairColor >= 0)
    //    CCharacterComponent::SetHairColor(this, HairColor, 1, 0);
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
    //SelectionFromContext = GetSelectionFromContext(a2, this->m_data.classId);
    //NumFacesForSkin = CCharacterComponent::GetNumFacesForSkin(
    //    this->m_data.raceId,
    //    this->m_data.genderId,
    //    this->m_data.classId,
    //    this->m_data.skinId,
    //    a2);
    //if (NumFacesForSkin > 0) {
    //    Hash = SecureRandom::GetHash(g_rndSeed);
    //    v5 = bn_aullshr(0x20u, (NumFacesForSkin * Hash) >> 32);
    //} else {
    //    v5 = 0;
    //}
    //FaceVariation = ComponentGetFaceVariation(
    //    this->m_data.raceId,
    //    this->m_data.genderId,
    //    this->m_data.skinId,
    //    v5,
    //    SelectionFromContext);
    //if (FaceVariation >= 0)
    //    CCharacterComponent::SetFace(this, FaceVariation, 1, 0);
}

// OFFSET: 0x4EC050
void CCharacterComponent::SetRandomFacialFeature(COMPONENT_CONTEXT context) {
    //SelectionFromContext = GetSelectionFromContext(a2, this->m_data.classId);
    //NumFacialFeaturesForHairColor = CCharacterComponent::GetNumFacialFeaturesForHairColor(
    //    this->m_data.raceId,
    //    this->m_data.genderId,
    //    this->m_data.classId,
    //    this->m_data.hairColorId,
    //    a2);
    //if (NumFacialFeaturesForHairColor > 0) {
    //    Hash = SecureRandom::GetHash(g_rndSeed);
    //    v5 = bn_aullshr(0x20u, (NumFacialFeaturesForHairColor * Hash) >> 32);
    //} else {
    //    v5 = 0;
    //}
    //FacialFeatureVariation = ComponentGetFacialFeatureVariation(
    //    this->m_data.raceId,
    //    this->m_data.genderId,
    //    this->m_data.classId,
    //    this->m_data.hairColorId,
    //    this->m_data.facialHairId,
    //    v5,
    //    SelectionFromContext);
    //if (FacialFeatureVariation >= 0)
    //    CCharacterComponent::SetBeardStyle(this, FacialFeatureVariation, 1, 0);
}

// OFFSET: 0x4E7B80
uint32_t CCharacterComponent::GetNumSkins(uint32_t raceId, uint32_t sexId, uint32_t classId, COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, classId);
    uint32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, raceId, sexId, 0, 0);
    uint32_t skins = 0;

    for (uint32_t i = 0; i < numColors; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, 0, 0, i, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            skins++;
        }
    }
    return skins;
}

// OFFSET: 0x4E7C10
uint32_t CCharacterComponent::GetNumHairStylesForColor(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t colorId, COMPONENT_CONTEXT context) {
    uint32_t selection = GetSelectionFromContext(context, classId);
    uint32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceId, sexId, 3);
    uint32_t skins = 0;

    for (uint32_t i = 0; i < numVariations; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, 3, i, colorId, nullptr);
        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            skins++;
        }
    }
    return skins;
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
          || !ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, 0, 0, this->m_data.m_preferences.skinID)) {
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

    if ((this->m_data.m_flags & 0x1) != 0) {
        //if (!this->m_link.Next())
        //    TSList::LinkToTail_0(&stru_AC46E4, this);
        return true;
    }

    if (a2) {
        if (this->m_request) {
            //*m_request &= ~1u;
            //this->m_request = 0;
        }
        //CCharacterComponent::sub_4ED640(this, 1);
        //CCharacterComponent::ItemsLoaded(this, 1);
        this->m_flags |= 8u;
        //CCharacterComponent::RenderPrepSections(this);
        this->m_link.Unlink();
    }

    // if (!this->m_link.Next())
    //     TSList::LinkToTail_0(&stru_AC46E4, this);
    return false;
}

// OFFSET: 0x4ED900
void CCharacterComponent::GeosRenderPrep() {
    bool eyeGlowFlag = false;

    // Death Knight
    auto sectionRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceID, this->m_data.m_preferences.sexID, 1, this->m_data.m_preferences.faceID, this->m_data.m_preferences.skinID, nullptr);
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


}

void CCharacterComponent::GetPreferences(CHARACTER_PREFERENCES* info) {
    if (info) {
        *info = this->m_data.m_preferences;
    }
}
