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
class ChrRacesRec;
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
    static char s_path2[260];
    static char s_buffer[260];
    static char* s_pathEnd;
    static CStatus s_status;

    static ITEM_FUNC s_itemFunc[NUM_COMPONENT_SECTIONS];
    static uint32_t s_mipLevels;
    static PREP_FUNC s_prepFunc[NUM_COMPONENT_SECTIONS];
    static CompSectionInfo s_sectionInfo[NUM_COMPONENT_SECTIONS];
    static CompSectionInfo s_sectionInfoRaw[NUM_COMPONENT_SECTIONS];

    static bool s_bInRenderPrep;
    static MipBits* s_textureBuffer;

    static CCharacterComponent* m_activePlayerComponent;

    // Static functions
    static void Initialize();
    static void Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThreads, int32_t useCompression);
    static bool Destroy();
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
    bool SkinNPC(const char* a2);
    void ReplaceExtraSkinTexture(const char* a2);
    void Prep();
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
    void PasteOpaqueScale(CACHEENTRY* entry, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, C2iVector& srcSize, const TCTEXTUREINFO& srcInfo);
    void PasteTransparent1BitScale(CACHEENTRY* entry, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, C2iVector& srcSize, const TCTEXTUREINFO& srcInfo);
    void PasteTransparent4BitScale(CACHEENTRY* entry, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, C2iVector& srcSize, const TCTEXTUREINFO& srcInfo);
    void PasteTransparent8BitScale(CACHEENTRY* entry, MipBits* dstMips, C2iVector& dstPos, uint32_t pixelStrideInBytes, C2iVector& srcPos, C2iVector& srcSize, const TCTEXTUREINFO& srcInfo);

    CACHEENTRY* LoadItemComponentTexture(ItemDisplayInfoRec* itemDisplayInfo, COMPONENT_SECTIONS section);
    bool ItemsLoaded(bool a2);
    bool UpdateTextureSlot(COMPONENT_SECTIONS section, const ItemDisplayInfoRec* displayRec, int32_t layer);
    void FreeSectionTexture(COMPONENT_SECTIONS section, int32_t layer);
    void GetItemDisplayPriority(ITEM_SLOT itemSlot, COMPONENT_SECTIONS section, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemAU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemAL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemHA(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemHU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemHL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemTU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemTL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemLU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemLL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void UpdateItemFO(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool update);
    void ClearGuildTexture(int32_t layer);
    void RemoveItem(ITEM_SLOT itemSlot);
    void RemoveItemByInventoryType(uint32_t inventoryType);
    void ComponentCloseFingers(CM2Model* model, bool rightHand); 
    bool ComposeHelmModelFilePath(const ChrRacesRec* racesRec, const ItemDisplayInfoRec* displayRec, char* dest, uint32_t destSize);
    bool GetHelmModelFilePath(ItemDisplayInfoRec* rec, ItemDisplayInfoRec** displayRec);
    void ComponentUtilAddItemVisual(CM2Model* model, int32_t itemVisualId);
    void AddLink(CM2Model* parent, uint32_t attachmentId, const char* modelPath, const char* texturePath, int32_t itemVisualId, const ItemDisplayInfoRec* displayRec);
    bool IsHelmModelCorrect();
    void AddHelm(int32_t itemVisualId);
    int32_t GetQuiverModelFilePath(ItemDisplayInfoRec* rec, ItemDisplayInfoRec** displayRec);
    bool IsQuiverModelCorrect();
    void AddQuiver(int32_t itemVisualId);
    int32_t BuildShoulderItemPaths(ItemDisplayInfoRec* rec, ItemDisplayInfoRec** displayRec, char* leftModel, char* rightModel, char* leftTexture, char* rightTexture);
    int32_t AreShoulderModelsCorrect();
    void AddShoulders(int32_t itemVisualId);
    void AddCape();
    void AddItem(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, int32_t itemVisualId);
    void AddItem(ITEM_SLOT itemSlot, int32_t itemDisplayId, int32_t itemVisualId);
    void RemoveLinkpt(CM2Model* model, uint32_t attachmentId);
    uint32_t AddHandItem(CM2Model* model, const ItemDisplayInfoRec* displayRec, uint32_t attachmentId, int32_t sheatheType, bool useSheathed, bool isShield, bool treatAsMainHand, int32_t itemVisualId);
    void SetHandItemDisplay(int32_t itemDisplayId, uint32_t handIndex, uint32_t attachmentId, bool isShield);
    void SetMainHandItemDisplay(int32_t itemDisplayId);
    void SetOffHandItemDisplay(int32_t itemDisplayId);
    void SetShieldItemDisplay(int32_t itemDisplayId);
    void AddItemByType(uint32_t inventoryType, int32_t itemDisplayId);
    void AddItemBySlot(int32_t itemSlot, int32_t itemDisplayId, int32_t itemVisualId);
    void RemoveHandItem(CM2Model* model, uint32_t attachmentId, int32_t sheatheType, bool isShield);

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
