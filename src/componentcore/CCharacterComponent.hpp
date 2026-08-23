#ifndef COMPONENT_CORE_C_CHARACTER_COMPONENT_HPP
#define COMPONENT_CORE_C_CHARACTER_COMPONENT_HPP

#include "net/Types.hpp"
#include "componentcore/Types.hpp"
#include "gx/Types.hpp"
#include <storm/Array.hpp>
#include "componentcore/ComponentData.hpp"

class CSimpleModelFFX;
class CM2Model;
class CVar;

class CCharacterComponent {
    public:
    // Static variables
    static CVar* g_componentTextureLevelVar;
    static CVar* g_componentThreadVar;
    static CVar* g_componentCompressVar;

    static uint32_t* s_heap;
    static uint32_t s_chrVarArrayLength;
    static st_race* s_chrVarArray;
    static EGxTexFormat s_gxFormatHigh;

    // Static functions
    static void Initialize();
    static void Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThreads, int32_t useCompression);
    static CCharacterComponent* AllocComponent();
    static void FreeComponent(CCharacterComponent* component);
    static void ValidateComponentData(ComponentData* data, COMPONENT_CONTEXT context = DEFAULT_CONTEXT);

    CCharacterComponent();
    ~CCharacterComponent();

    void ReplaceHairTexture(uint32_t varitationIndex, const char* a3);
    void LoadBaseVariation(uint32_t variation, uint32_t textureIndex, uint32_t variationIndex, uint32_t colorIndex, uint32_t section, const char* a7);

    void SetHairColor(uint32_t colorId, bool a3, const char* a4);
    void SetHairStyle(uint32_t colorId, const char* a4);
    void SetFace(uint32_t colorId, bool a3, const char* a4);
    void SetBeardStyle(uint32_t colorId, bool a3, const char* a4);
    void SetSkinColor(uint32_t colorId, bool a3, bool a4, const char* a5);

    void SetPrevSkin(COMPONENT_CONTEXT context);
    void SetNextSkin(COMPONENT_CONTEXT context);
    void SetPrevFace(COMPONENT_CONTEXT context, uint32_t skinIndex);
    void SetNextFace(COMPONENT_CONTEXT context, uint32_t skinIndex);
    void SetPrevHairStyle(COMPONENT_CONTEXT context);
    void SetNextHairStyle(COMPONENT_CONTEXT context);
    void SetPrevHairColor(COMPONENT_CONTEXT context);
    void SetNextHairColor(COMPONENT_CONTEXT context);
    void SetPrevFacialFeature(COMPONENT_CONTEXT context);
    void SetNextFacialFeature(COMPONENT_CONTEXT context);

    void SetRandomSkin(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    void SetRandomHairColor(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    void SetRandomHairStyle(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    void SetRandomFace(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    void SetRandomFacialFeature(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);

    uint32_t GetNumSkins(uint32_t raceId, uint32_t sexId, uint32_t classId, COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    uint32_t GetNumHairStylesForColor(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t colorId, COMPONENT_CONTEXT context = DEFAULT_CONTEXT);

    bool Init(ComponentData* data, const char* a3);
    void SkinNPC(const char* a2);
    bool RenderPrep(int32_t a2);
    void GeosRenderPrep();
    void GetPreferences(CHARACTER_PREFERENCES* info);

    public:
    TSLink<CCharacterComponent> m_link;
    uint32_t m_flags = 0;
    int32_t m_dirtySections = -1;
    uint32_t m_heapIndex;
    EGxTexFormat m_gxTexFormat;
    ComponentData m_data;
    HTEXTURE m_baseSkinTexture = nullptr;
    CharacterBaseVariation m_baseVariation[5];
    CharacterSection m_section[10];
    uint32_t m_itemDisplayID[12];
    uint32_t m_handItemDisplayID[3];
    uint32_t m_itemSlotForAttachSlot[50];
    void* m_request;
};

#endif // COMPONENT_CORE_C_CHARACTER_COMPONENT_HPP
