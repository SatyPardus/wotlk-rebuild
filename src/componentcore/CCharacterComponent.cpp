#include "componentcore/CCharacterComponent.hpp"

#include <algorithm>

#include <common/ObjectAlloc.hpp>
#include <common/Processor.hpp>
#include "console/Types.hpp"
#include "console/CVar.hpp"
#include "model/CM2Model.hpp"
#include "db/Db.hpp"
#include "componentcore/ComponentUtils.hpp"

CVar* CCharacterComponent::g_componentTextureLevelVar = nullptr;
CVar* CCharacterComponent::g_componentThreadVar = nullptr;
CVar* CCharacterComponent::g_componentCompressVar = nullptr;

uint32_t* CCharacterComponent::s_heap = nullptr;
uint32_t CCharacterComponent::s_chrVarArrayLength = 0;
st_race* CCharacterComponent::s_chrVarArray = nullptr;


static bool ComponentVarHandler(CVar*, const char*, const char*, void*) {
    return true;
}

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

void CCharacterComponent::Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThreads, int32_t useCompression) {
    CCharacterComponent::s_heap = static_cast<uint32_t*>(ALLOC(sizeof(uint32_t)));
    if (CCharacterComponent::s_heap) {
        *CCharacterComponent::s_heap = ObjectAllocAddHeap(sizeof(CCharacterComponent), 32, "CCharacterComponent", true);
    }

    //s_pathEnd = s_path;
    //s_pathEnd2 = path;
    //dword_B6B834 = sub_4F09D0;
    //dword_B6B838 = CCharacterComponent::sub_4F0A30;
    //dword_B6B83C = CCharacterComponent::sub_4F0A90;
    //dword_B6B854 = maybe_CCharacterComponent__ClearTextureCache;
    //dword_B6B858 = CCharacterComponent::sub_4F0B70;
    //dword_B6B840 = CCharacterComponent::sub_4F0C10;
    //dword_B6B844 = CCharacterComponent::sub_4F0CA0;
    //dword_B6B848 = CCharacterComponent::sub_4F0D00;
    //dword_B6B84C = CCharacterComponent::sub_4F0DB0;
    //dword_B6B850 = CCharacterComponent::sub_4F0E40;
    //CCharacterComponent::m_itemFunc[0] = CCharacterComponent::sub_4F01A0;
    //dword_B6B810 = CCharacterComponent::sub_4F0200;
    //dword_B6B814 = CCharacterComponent::sub_4F0280;
    //dword_B6B82C = NOP_0;
    //dword_B6B830 = NOP_0;
    //dword_B6B818 = CCharacterComponent::sub_4F02E0;
    //dword_B6B81C = CCharacterComponent::sub_4F0340;
    //dword_B6B820 = CCharacterComponent::sub_4F03A0;
    //dword_B6B824 = CCharacterComponent::sub_4F0400;
    //dword_B6B828 = CCharacterComponent::sub_4F0420;
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
    //bnl_CCharacterComponent__s_gxFormatHigh = 2;
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

void CCharacterComponent::FreeComponent(CCharacterComponent* component) {
    component->~CCharacterComponent();
    // TODO: ObjectFree()
}

void CCharacterComponent::ValidateComponentData(ComponentData* data, COMPONENT_CONTEXT context) {
}

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

CCharacterComponent::~CCharacterComponent() {
}

void CCharacterComponent::SetRandomSkin(COMPONENT_CONTEXT context) {
}

void CCharacterComponent::SetRandomHairColor(COMPONENT_CONTEXT context) {
}

void CCharacterComponent::SetRandomHairStyle(COMPONENT_CONTEXT context) {
}

void CCharacterComponent::SetRandomFace(COMPONENT_CONTEXT context) {
}

void CCharacterComponent::SetRandomFacialFeature(COMPONENT_CONTEXT context) {
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
    //if ((data->m_preferences.raceId & 0x80000000) != 0
    //      || data->m_preferences.raceId > g_ChrRacesDB.maxIndex
    //      || !ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.raceId, this->m_data.m_preferences.genderId, 0, 0, this->m_data.m_preferences.skinId)) {
    //    return 0;
    //}
    this->m_flags = this->m_flags & 0xFFFFFFBA | 5;
    this->m_dirtySections = -1;
    this->m_request = nullptr;
    if ((this->m_data.m_flags & 1) == 0 && this->m_baseSkinTexture) {
        HandleClose(this->m_baseSkinTexture);
        this->m_baseSkinTexture = 0;
    }
    //if ((this->m_data.m_flags & 2) != 0)
    //    this->m_gxTexFormat = CCharacterComponent::s_gxFormatHigh;
    ChrRacesRec* chrRaces = g_chrRacesDB.GetRecord(this->m_data.m_preferences.raceID);
    this->m_flags ^= (this->m_flags ^ (16 * ((chrRaces->m_flags & 2) != 0))) & 0x10;
    //if ((this->m_data.m_flags & 1) != 0)
    //    this->SkinNpc(a3);
    //this->SetSkinColor(this->m_data.m_preferences.skinID, 0, 1, a3);
    //this->SetHairStyle(this->m_data.m_preferences.hairStyleID, a3);
    //this->SetBeardStyle(this->m_data.m_preferences.facialHairStyleID, 0, a3);
    return false;
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

}

void CCharacterComponent::GeosRenderPrep(int32_t a2) {

    bool eyeGlowFlag = false;

    // Death Knight
    if (this->m_data.m_preferences.classID == 6) {
        eyeGlowFlag = true;
    } else {
        // TODO: ComponentGetSectionsRecord
    }

    this->m_data.m_model->SetGeometryVisible(0, 2000, 0);
    this->m_data.m_model->SetGeometryVisible(0, 0, 1);
    for (int i = 0; i < 19; ++i) {
        if (i == 17 && eyeGlowFlag) {
            this->m_data.m_model->SetGeometryVisible(1703, 1703, 1);
        } else {
            this->m_data.m_model->SetGeometryVisible(this->m_data.m_geosets[i], this->m_data.m_geosets[i], 1);
        }
    }
}

void CCharacterComponent::GetPreferences(CHARACTER_PREFERENCES* info) {
    if (info) {
        *info = this->m_data.m_preferences;
    }
}
