#ifndef CLIENTOBJECT_ITEM_C_HPP
#define CLIENTOBJECT_ITEM_C_HPP

#include "clientobject/CGObject_C.hpp"
#include <cstdint>

struct CGItemData {
    WGUID ITEM_FIELD_OWNER;
    WGUID ITEM_FIELD_CONTAINED;
    WGUID ITEM_FIELD_CREATOR;
    WGUID ITEM_FIELD_GIFTCREATOR;
    uint32_t ITEM_FIELD_STACK_COUNT;
    uint32_t ITEM_FIELD_DURATION;
    uint32_t ITEM_FIELD_SPELL_CHARGES[5];
    uint32_t ITEM_FIELD_FLAGS;
    uint32_t ITEM_FIELD_ENCHANTMENT_1_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_1_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_2_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_2_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_3_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_3_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_4_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_4_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_5_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_5_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_6_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_6_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_7_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_7_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_8_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_8_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_9_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_9_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_10_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_10_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_11_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_11_3;
    uint32_t ITEM_FIELD_ENCHANTMENT_12_1[2];
    uint32_t ITEM_FIELD_ENCHANTMENT_12_3;
    uint32_t ITEM_FIELD_PROPERTY_SEED;
    uint32_t ITEM_FIELD_RANDOM_PROPERTIES_ID;
    uint32_t ITEM_FIELD_DURABILITY;
    uint32_t ITEM_FIELD_MAXDURABILITY;
    uint32_t ITEM_FIELD_CREATE_PLAYED_TIME;
    uint32_t ITEM_FIELD_PAD;
};

class CGItem {
    public:
    uint32_t unk_00D0;
    CGItemData* m_item;
    void* m_itemMirror;

    // OFFSET: 0x4F51C0
    static constexpr uint32_t TotalFields() {
        return CGObject::TotalFields() + 47;
    }

    static constexpr uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGItemData);
    }

    static constexpr uint32_t GetTotalFieldCount() {
        return CGItem::GetDataSize() / sizeof(uint32_t);
    }

    static constexpr uint32_t GetFieldCount() {
        return sizeof(CGItemData) / sizeof(uint32_t);
    }

    static uint32_t MirrorIndexFromFieldIndex(uint32_t fieldIndex);
    static uint32_t DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset);
};

class CGItem_C : public CGObject_C, public CGItem {
    public:

    STORM_EXPLICIT_LIST(CMirrorHandler, m_link) m_itemMirrorLists[CGItem::GetFieldCount()];

    CGItem_C();
    CGItem_C(CClientObjCreate& objCreate, uint32_t time);
    void PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3);

    static void SetStorage(CGItem_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_ITEM_C_HPP
