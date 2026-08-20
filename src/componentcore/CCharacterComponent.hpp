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
    enum COMPONENT_CONTEXT
    {
        DEFAULT_CONTEXT = 0,
        CONTEXT_1,
        CONTEXT_2,
        CONTEXT_3
    };

    public:
    // Static variables
    static CVar* g_componentTextureLevelVar;
    static CVar* g_componentThreadVar;
    static CVar* g_componentCompressVar;

    static uint32_t* s_heap;
    static uint32_t s_chrVarArrayLength;
    static st_race* s_chrVarArray;

    // Static functions
    static void Initialize();
    static void Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThreads, int32_t useCompression);
    static CCharacterComponent* AllocComponent();
    static void FreeComponent(CCharacterComponent* component);
    static void ValidateComponentData(ComponentData* data, COMPONENT_CONTEXT context = DEFAULT_CONTEXT);

    CCharacterComponent();
    ~CCharacterComponent();

    void SetRandomSkin(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    void SetRandomHairColor(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    void SetRandomHairStyle(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    void SetRandomFace(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);
    void SetRandomFacialFeature(COMPONENT_CONTEXT context = DEFAULT_CONTEXT);

    bool Init(ComponentData* data, const char* a3);
    bool RenderPrep(int32_t a2);
    void GeosRenderPrep(int32_t a2);
    void GetPreferences(CHARACTER_PREFERENCES* info);

    public:
    TSLink<CCharacterComponent> m_link;
    uint32_t m_flags;
    uint32_t m_dirtySections;
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
