#ifndef CLIENTOBJECT_UNIT_C_HPP
#define CLIENTOBJECT_UNIT_C_HPP

#include <cstdint>
#include "clientobject/CGObject_C.hpp"
#include "db/Db.hpp"

class ChrRacesRec;
class ChrClassesRec;

struct CGUnitData {
    WGUID UNIT_FIELD_CHARM;
    WGUID UNIT_FIELD_SUMMON;
    WGUID UNIT_FIELD_CRITTER;
    WGUID UNIT_FIELD_CHARMEDBY;
    WGUID UNIT_FIELD_SUMMONEDBY;
    WGUID UNIT_FIELD_CREATEDBY;
    WGUID UNIT_FIELD_TARGET;
    WGUID UNIT_FIELD_CHANNEL_OBJECT;
    uint32_t UNIT_CHANNEL_SPELL;
    uint32_t UNIT_FIELD_BYTES_0;
    uint32_t UNIT_FIELD_HEALTH;
    uint32_t UNIT_FIELD_POWER1;
    uint32_t UNIT_FIELD_POWER2;
    uint32_t UNIT_FIELD_POWER3;
    uint32_t UNIT_FIELD_POWER4;
    uint32_t UNIT_FIELD_POWER5;
    uint32_t UNIT_FIELD_POWER6;
    uint32_t UNIT_FIELD_POWER7;
    uint32_t UNIT_FIELD_MAXHEALTH;
    uint32_t UNIT_FIELD_MAXPOWER1;
    uint32_t UNIT_FIELD_MAXPOWER2;
    uint32_t UNIT_FIELD_MAXPOWER3;
    uint32_t UNIT_FIELD_MAXPOWER4;
    uint32_t UNIT_FIELD_MAXPOWER5;
    uint32_t UNIT_FIELD_MAXPOWER6;
    uint32_t UNIT_FIELD_MAXPOWER7;
    float UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER[7];
    float UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER[7];
    uint32_t UNIT_FIELD_LEVEL;
    uint32_t UNIT_FIELD_FACTIONTEMPLATE;
    uint32_t UNIT_VIRTUAL_ITEM_SLOT_ID[3];
    uint32_t UNIT_FIELD_FLAGS;
    uint32_t UNIT_FIELD_FLAGS_2;
    uint32_t UNIT_FIELD_AURASTATE;
    uint32_t UNIT_FIELD_BASEATTACKTIME[2];
    uint32_t UNIT_FIELD_RANGEDATTACKTIME;
    float UNIT_FIELD_BOUNDINGRADIUS;
    float UNIT_FIELD_COMBATREACH;
    uint32_t UNIT_FIELD_DISPLAYID;
    uint32_t UNIT_FIELD_NATIVEDISPLAYID;
    uint32_t UNIT_FIELD_MOUNTDISPLAYID;
    float UNIT_FIELD_MINDAMAGE;
    float UNIT_FIELD_MAXDAMAGE;
    float UNIT_FIELD_MINOFFHANDDAMAGE;
    float UNIT_FIELD_MAXOFFHANDDAMAGE;
    uint32_t UNIT_FIELD_BYTES_1;
    uint32_t UNIT_FIELD_PETNUMBER;
    uint32_t UNIT_FIELD_PET_NAME_TIMESTAMP;
    uint32_t UNIT_FIELD_PETEXPERIENCE;
    uint32_t UNIT_FIELD_PETNEXTLEVELEXP;
    uint32_t UNIT_DYNAMIC_FLAGS;
    float UNIT_MOD_CAST_SPEED;
    uint32_t UNIT_CREATED_BY_SPELL;
    uint32_t UNIT_NPC_FLAGS;
    uint32_t UNIT_NPC_EMOTESTATE;
    uint32_t UNIT_FIELD_STAT0;
    uint32_t UNIT_FIELD_STAT1;
    uint32_t UNIT_FIELD_STAT2;
    uint32_t UNIT_FIELD_STAT3;
    uint32_t UNIT_FIELD_STAT4;
    uint32_t UNIT_FIELD_POSSTAT0;
    uint32_t UNIT_FIELD_POSSTAT1;
    uint32_t UNIT_FIELD_POSSTAT2;
    uint32_t UNIT_FIELD_POSSTAT3;
    uint32_t UNIT_FIELD_POSSTAT4;
    uint32_t UNIT_FIELD_NEGSTAT0;
    uint32_t UNIT_FIELD_NEGSTAT1;
    uint32_t UNIT_FIELD_NEGSTAT2;
    uint32_t UNIT_FIELD_NEGSTAT3;
    uint32_t UNIT_FIELD_NEGSTAT4;
    uint32_t UNIT_FIELD_RESISTANCES[7];
    uint32_t UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE[7];
    uint32_t UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE[7];
    uint32_t UNIT_FIELD_BASE_MANA;
    uint32_t UNIT_FIELD_BASE_HEALTH;
    uint32_t UNIT_FIELD_BYTES_2;
    uint32_t UNIT_FIELD_ATTACK_POWER;
    uint32_t UNIT_FIELD_ATTACK_POWER_MODS;
    float UNIT_FIELD_ATTACK_POWER_MULTIPLIER;
    uint32_t UNIT_FIELD_RANGED_ATTACK_POWER;
    uint32_t UNIT_FIELD_RANGED_ATTACK_POWER_MODS;
    float UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;
    float UNIT_FIELD_MINRANGEDDAMAGE;
    float UNIT_FIELD_MAXRANGEDDAMAGE;
    uint32_t UNIT_FIELD_POWER_COST_MODIFIER[7];
    float UNIT_FIELD_POWER_COST_MULTIPLIER[7];
    float UNIT_FIELD_MAXHEALTHMODIFIER;
    float UNIT_FIELD_HOVERHEIGHT;
    uint32_t UNIT_FIELD_PADDING;
};

class CGUnit {
    public:
    CGUnitData* m_unit;
    void* m_unitMirror;

    // OFFSET: 0x4F52C0
    static uint32_t TotalFields() {
        return CGObject::TotalFields() + 123;
    }

    static uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGUnitData);
    }
};

class CGUnit_C : public CGObject_C, public CGUnit {
    public:
    C3Vector tempPosition;
    float tempFacing;

    CGUnit_C();
    CGUnit_C(CClientObjCreate& objCreate, uint32_t time);
    void PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3);

    CreatureModelDataRec* GetModelData();

    // Virtual functions
    /* 11 */ void GetPosition(C3Vector& pos) override;
    /* 13 */ float GetFacing() override;
    /* 24 */ bool GetModelFileName(const char** fileName) override;

    // Static functions
    static const char* GetDisplayRaceNameFromRecord(ChrRacesRec* record, uint8_t sexIn, uint8_t* sexOut = nullptr);
    static const char* GetDisplayClassNameFromRecord(ChrClassesRec* record, uint8_t sexIn, uint8_t* sexOut = nullptr);
    static void SetStorage(CGUnit_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_UNIT_C_HPP
