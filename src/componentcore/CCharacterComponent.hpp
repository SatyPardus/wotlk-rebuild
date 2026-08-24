#ifndef COMPONENT_CORE_C_CHARACTER_COMPONENT_HPP
#define COMPONENT_CORE_C_CHARACTER_COMPONENT_HPP

#include "net/Types.hpp"
#include "componentcore/Types.hpp"
#include "gx/Types.hpp"
#include <storm/Array.hpp>
#include "componentcore/ComponentData.hpp"
#include "tempest/vector/C2iVector.hpp"

class CSimpleModelFFX;
class CM2Model;
class CVar;
class ItemDisplayInfoRec;
class CCharacterComponent;
struct TCTEXTUREINFO;
class BlpPalPixel;

struct CompSectionInfo {
    C2iVector pos;
    C2iVector size;
};

typedef void (CCharacterComponent::*ITEM_FUNC)(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
typedef void (CCharacterComponent::*PREP_FUNC)();

class CCharacterComponent {
    public:
    // Static variables
    static CVar* g_componentTextureLevelVar;
    static CVar* g_componentThreadVar;
    static CVar* g_componentCompressVar;

    static uint32_t* s_heap;
    static uint32_t s_chrVarArrayLength;
    static st_race* s_chrVarArray;
    static uint32_t* s_characterFacialHairStylesList;
    static EGxTexFormat s_gxFormatHigh;
    static EGxTexFormat s_gxFormat;
    static uint32_t s_textureSize;

    static char s_path[260];
    static char* s_pathEnd;
    static CStatus s_status;

    static ITEM_FUNC s_itemFunc[NUM_COMPONENT_SECTIONS];
    static uint32_t s_mipLevels;
    static PREP_FUNC s_prepFunc[NUM_COMPONENT_SECTIONS];
    static CompSectionInfo s_sectionInfo[NUM_COMPONENT_SECTIONS];
    static CompSectionInfo s_sectionInfoRaw[NUM_COMPONENT_SECTIONS];

    static bool s_bInRenderPrep;
    static MipBits* s_textureBuffer;

    // Static functions
    static void Initialize();
    static void Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThreads, int32_t useCompression);
    static int32_t Update(const void*, void*);
    static CCharacterComponent* AllocComponent();
    static void FreeComponent(CCharacterComponent* component);
    static void ValidateComponentData(ComponentData* data, COMPONENT_CONTEXT context);
    static void UpdateBaseTexture(EGxTexCommand cmd, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);

    CCharacterComponent();
    ~CCharacterComponent();

    void ReplaceHairTexture(uint32_t varitationIndex, const char* a3);
    void LoadBaseVariation(COMPONENT_VARIATIONS variation, uint32_t textureIndex, uint32_t variationIndex, uint32_t colorIndex, COMPONENT_SECTIONS section, const char* a7);

    void SetHairColor(uint32_t colorId, bool a3, const char* a4);
    void SetHairStyle(uint32_t colorId, const char* a4);
    void SetFace(uint32_t colorId, bool a3, const char* a4);
    void SetBeardStyle(uint32_t colorId, bool a3, const char* a4);
    void SetSkinColor(uint32_t colorId, bool a3, bool a4, const char* a5);

    bool SetPrevSkin(COMPONENT_CONTEXT context);
    bool SetNextSkin(COMPONENT_CONTEXT context);
    bool SetPrevFace(COMPONENT_CONTEXT context, uint32_t skinIndex);
    bool SetNextFace(COMPONENT_CONTEXT context, uint32_t skinIndex);
    bool SetPrevHairStyle(COMPONENT_CONTEXT context);
    bool SetNextHairStyle(COMPONENT_CONTEXT context);
    bool SetPrevHairColor(COMPONENT_CONTEXT context);
    bool SetNextHairColor(COMPONENT_CONTEXT context);
    bool SetPrevFacialFeature(COMPONENT_CONTEXT context);
    bool SetNextFacialFeature(COMPONENT_CONTEXT context);

    void SetRandomSkin(COMPONENT_CONTEXT context);
    void SetRandomHairColor(COMPONENT_CONTEXT context);
    void SetRandomHairStyle(COMPONENT_CONTEXT context);
    void SetRandomFace(COMPONENT_CONTEXT context);
    void SetRandomFacialFeature(COMPONENT_CONTEXT context);

    uint32_t GetNumSkins(uint32_t raceId, uint32_t sexId, uint32_t classId, COMPONENT_CONTEXT context);
    uint32_t GetNumHairStylesForColor(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t colorId, COMPONENT_CONTEXT context);
    uint32_t GetNumFacialFeaturesForHairColor(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t colorId, COMPONENT_CONTEXT context);
    uint32_t GetNumHairColorsForStyle(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t index, COMPONENT_CONTEXT context);
    uint32_t GetNumFacesForSkin(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t index, COMPONENT_CONTEXT context);

    int32_t GetNthFacialFeatureIndex(uint32_t raceId, uint32_t sexId, uint32_t classId, uint32_t hairColorId, uint32_t facialHairId, uint32_t index, uint32_t selection);

    CharSectionsRec* GetSectionsRecord(COMPONENT_VARIATIONS variation, uint32_t variationIndex, uint32_t colorId, bool* found);

    bool Init(ComponentData* data, const char* a3);
    void SkinNPC(const char* a2);
    void ReplaceExtraSkinTexture(const char* a2);
    bool RenderPrep(int32_t a2);
    void RenderPrepSections();
    void PrepSections();
    void RenderPrepAll();
    void DestroyRenderTextures();
    bool VariationsLoaded(bool a2);
    void RenderPrepAL();
    void RenderPrepAU();
    void RenderPrepFO();
    void RenderPrepHA();
    void RenderPrepHL();
    void RenderPrepHU();
    void RenderPrepLL();
    void RenderPrepLU();
    void RenderPrepTL();
    void RenderPrepTU();
    void CreateBaseTexture();
    void GeosRenderPrep();
    void GetPreferences(CHARACTER_PREFERENCES* info);

    void PasteFromSkin(COMPONENT_SECTIONS section, CACHEENTRY* entry, MipBits* bits);
    void PasteToSection(COMPONENT_SECTIONS section, CACHEENTRY* entry, MipBits* bits);
    void Paste(CACHEENTRY* entry, MipBits* dstMips, const C2iVector& dstPos, const C2iVector& srcPos, const C2iVector& srcSize, TCTEXTUREINFO& srcInfo, int32_t srcMipLevel);
    void PasteCrappyGreen(MipBits* dstMips, C2iVector dstPos, uint32_t pixelStrideInBytes, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel);
    void PasteTransparent1Bit(CACHEENTRY* entry, BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel);
    void PasteTransparent4Bit(CACHEENTRY* entry, BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel);
    void PasteTransparent8Bit(CACHEENTRY* entry, BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel);
    void PasteOpaque(CACHEENTRY* entry, BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, const C2iVector& srcSize, const TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t invSrcMipLevel);
    void PasteScale(CACHEENTRY* entry, MipBits* dstMips, const C2iVector& dstPos, const C2iVector& srcPos, const C2iVector& srcSize, TCTEXTUREINFO& srcInfo);

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
