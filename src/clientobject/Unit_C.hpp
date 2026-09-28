#ifndef CLIENTOBJECT_UNIT_C_HPP
#define CLIENTOBJECT_UNIT_C_HPP

#include <cstdint>
#include "clientobject/CGObject_C.hpp"
#include "db/Db.hpp"
#include <componentcore/CCharacterComponent.hpp>
#include "clientobject/Movement_C.hpp"
#include "clientobject/AnimationTypes.hpp"

class CGPlayer_C;
class CreatureStats_C;

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
    static constexpr uint32_t TotalFields() {
        return CGObject::TotalFields() + 123;
    }

    static constexpr uint32_t GetDataSize() {
        return CGObject::GetDataSize() + sizeof(CGUnitData);
    }

    static constexpr uint32_t GetTotalFieldCount() {
        return CGUnit::GetDataSize() / sizeof(uint32_t);
    }

    static constexpr uint32_t GetFieldCount() {
        return sizeof(CGUnitData) / sizeof(uint32_t);
    }

    static uint32_t MirrorIndexFromFieldIndex(uint32_t fieldIndex);
    static uint32_t DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset);
};

struct CGUnitVirtualItem {
    uint8_t classID;
    uint8_t subclassID;
    uint8_t soundOverrideSubclassID;
    uint8_t material;
    uint8_t inventoryType;
    uint8_t sheatheType;
    uint8_t pad[2];
};

class CGUnit_C : public CGObject_C, public CGUnit {
    public:
    // Static variables
    static WGUID s_activeMover;
    static CVar* s_cvShowFootPrintParticles;
    static CVar* s_cvPathingDistTolerance;
    static int32_t m_trackingType;
    static float m_trackingFacing;

    // Member variables
    /* 00D8 */ CMovementShared* m_passenger = nullptr;
    /* 00D8 */ STORM_EXPLICIT_LIST(CMirrorHandler, m_link) m_unitMirrorLists[CGUnit::GetFieldCount()];
    /* 0784 */
    /* 0788 */ CMovement_C movementData;

    /* 0964 */ CreatureStats_C* m_creatureCacheEntry = nullptr;
    /* 0968 */ CreatureDisplayInfoRec* m_displayInfo = nullptr;
    /* 096C */ CreatureDisplayInfoExtraRec* m_displayInfoExtra = nullptr;
    /* 0970 */ CreatureModelDataRec* m_modelData = nullptr;
    /* 0974 */ CreatureSoundDataRec* m_soundData = nullptr;
    /* 0978 */
    /* 097C */ UnitBloodLevelsRec* m_bloodlevels = nullptr;

    /* 098C */ CM2Model* data98C = nullptr;

    /* 0998 */ uint32_t m_virtualItemDisplayId[3] = { 0, 0, 0 };
    /* 09A4 */ CGUnitVirtualItem m_virtualItem[3];

    /* 09D4 */ uint32_t m_displayId = 0;

    /* 09F8 */ uint32_t unk_09F8 = 0;

    /* 0A30 */ uint32_t unk_0A30 = 0;

    /* 0A38 */ uint32_t m_animationState = 0;

    /* 0A94 */ float m_renderFacing = 0.0f;
    /* 0A98 */ float m_facingVelocity = 0.0f;
    /* 0A9C */ float m_facingBlend = 0.0f;
    /* 0AA0 */ float m_targetFacing = 0.0f;

    /* 0AA8 */ float m_turnDelta[4];
    /* 0AB8 */ float m_scriptedFacing;
    /* 0ABC */ int32_t m_lastTurnTimeMs = 0;

    /* 0B4C */ CCharacterComponent* m_characterComponent = nullptr;

    /* 0B5C */ uint32_t m_sheatheState = 0;

    /* 0B70 */ float float0B70 = 0.0f;
    /* 0B74 */ float float0B74 = 0.0f;
    /* 0B78 */ float float0B78 = 0.0f;
    /* 0B7C */ uint32_t m_mountAnimBehaviorId = ANIM_MOUNT;
    /* 0B80 */ uint32_t m_animTier = 0;
    /* 0B84 */ uint32_t m_torsoKeyBone = -1;
    /* 0B88 */ ANIMATION_ID m_meleeAttackAnimId = ANIM_NONE;
    /* 0B8C */ ANIMATION_ID m_pendingAnimId = ANIM_NONE;
    /* 0B90 */ ANIMATION_ID m_awaitingEndAnimId = ANIM_NONE;
    /* 0B94 */ uint32_t unk_0B94 = 0;

    CGUnit_C();
    CGUnit_C(CClientObjCreate& objCreate, uint32_t time);
    void PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3);
    void SetClientInitData(CClientObjCreate& objCreate, bool a3);
    bool InitializeComponent();
    bool InitializeExtendedDisplay(CGPlayer_C* player, bool hasExtendedData);
    void RefreshDataPointers();
    bool sub_71A430();
    bool sub_71C500();
    float GetMaxCameraHeight();
    bool GetCanFly();
    bool IsClientControlled();
    bool IsRunning();
    bool IsLocalClientControlled();
    bool IsAllowedToSendMessage(NETMESSAGE msgId);
    void ToggleMovementFlag2_0x40(uint8_t flag);
    float GetStandHeight();
    char* GetUnitName(char** a2, bool a3);
    void UpdateUnitNameText();
    bool IsLowPrioritySelection(uint32_t time);
    bool IsActivePlayer();
    bool IsDisarmed(uint8_t a2);
    bool IsLooting();
    bool ShouldKneelForLoot();
    bool CanShuffle();
    bool IsVehicleDriver();
    bool IsBoss();

    bool ProcessLocalMoveEvent(int32_t time, NETMESSAGE msgId, bool needAck, float value, uint32_t index, WGUID transportGuid, uint8_t transportSeat);
    bool SendMovementUpdate(int32_t time, NETMESSAGE msgId, float value, uint32_t index, WGUID transportGuid, uint8_t transportSeat);
    bool BuildMovementUpdate(int32_t time, NETMESSAGE msgId, CDataStore* msg, float value, uint32_t index);
    void SetUpdateInfo(CClientMoveUpdate* moveUpdate, bool localPlayer);

    bool OnMoveEvent(NETMESSAGE msgId, int32_t time, CDataStore* msg); 
    void OnMoveUpdate(int32_t time, bool a3, bool a4);
    void MoveEventHappened(NETMESSAGE msgId);

    bool OnTurnStart(int32_t eventTime, CMovementStatus* update, bool left);

    void OnMoveStartLocal(int32_t eventTime, bool forward);
    void OnMoveStopLocal(int32_t eventTime);
    void OnStrafeStartLocal(int32_t eventTime, bool left);
    void OnStrafeStopLocal(int32_t eventTime);
    void OnAscendDescendStartLocal(int32_t eventTime, bool up);
    void OnAscendDescendStopLocal(int32_t eventTime);
    void OnPitchStartLocal(int32_t eventTime, bool up);
    void OnPitchStopLocal(int32_t eventTime);
    void OnTurnStartLocal(int32_t eventTime, bool left);
    void OnTurnStopLocal(int32_t eventTime);
    void OnSetRawFacingLocal(int32_t eventTime, float facing);
    void OnTurnToAngleLocal(int32_t eventTime, float facing);
    void OnCollideFallLand(uint32_t prevFlags, int32_t fellWithSpeed);
    bool OnCollideFallLandNotify(uint32_t time, uint32_t prevFlags, uint32_t prevFlags2, int32_t wasFalling);
    void OnCollideFalling();

    void OnMovementInitiated();
    void OnMonsterMove(CDataStore* msg, NETMESSAGE msgId, WGUID transportGuid, uint8_t transportFlags, bool a6);
    C3Vector* ComputeTransportRelativeMovement(WGUID guid, C3Vector* position, C3Vector* points, uint32_t* count);
    bool NoStrafe();
    bool IsVehiclePreventingTurning();
    bool HasVehicleTransport();
    bool IsAlteredFormTransitionPreventingMovement();
    bool ClampRawAngleToLegalFacingRange(float* yaw);
    void SmoothFacingAngle(float target);
    void UpdateSmoothFacing(float* facingOffset);

    CreatureModelDataRec* GetModelData();

    // Animation functions
    void UpdateBaseAnimation(uint8_t a2, uint32_t a3);
    void PlayBaseAnimation(ANIMATION_ID animId, uint8_t flags);
    ANIMATION_ID ChooseAnimation(uint32_t a2, uint8_t a3, bool* a4);
    ANIMATION_ID GetCurrentTorsoAnimId();
    bool Uses_A30_Flag_0x40000000(uint32_t a2, ANIMATION_ID* animId);
    bool ChooseDeathAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags);
    bool ChooseSubmergeAnim(uint32_t a2, ANIMATION_ID* animId);
    bool ChooseFallAnim(ANIMATION_ID* animId);
    bool ChooseMovementAnim(uint32_t a2, ANIMATION_ID* animId);
    bool ChooseLootAnim(uint32_t a2, ANIMATION_ID* animId);
    bool ChooseSpellVisualKitAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags);
    bool ChooseCombatAnim(uint32_t a2, ANIMATION_ID* animId, uint8_t flags);
    bool ChooseShuffleAnim(uint32_t a2, ANIMATION_ID* animId);
    bool ChooseRangedLoadAnim(uint32_t a2, ANIMATION_ID* animId);
    bool ChooseStandStateAnim(uint32_t a2, ANIMATION_ID* animId);
    bool ChooseEmoteAnim(uint32_t a2, ANIMATION_ID* animId);
    void ChooseDefaultAnim(ANIMATION_ID* animId, bool a3);
    void PlayFallLandAnimation(uint32_t prevFlags, int32_t fellWithSpeed);

    // Virtual functions
    /* 11 */ void GetPosition(C3Vector& pos) override;
    /* 13 */ float GetFacing() override;
    /* 24 */ bool GetModelFileName(const char** fileName) override;
    /* 32 */ void ModelLoaded(CM2Model* model) override;
    /* 34 */ void PreAnimate(CGWorldFrame* worldFrame) override;
    /* 36 */ void ShouldRender(uint32_t flags, uint32_t* culled, uint32_t* out) override;
    /* 37 */ float GetRenderFacing() override;
    /* 41 */ bool CanHighlight() override;
    /* 42 */ bool CanBeTargetted() override;
    /* 54 */ char* GetObjectName() override;
    /* 66 */ virtual void GetAFKText(char* text, uint32_t textLength);
    /* 67 */ virtual void GetDNDText(char* text, uint32_t textLength);
    /* 68 */ virtual void GetGMText(char* text, uint32_t textLength);
    /* 69 */ virtual void GetDevText(char* text, uint32_t textLength);
    /* 75 */ virtual CGUnitVirtualItem* GetVirtualItem(uint8_t a2, uint32_t a3);
    /* 76 */ virtual uint32_t GetVirtualItemDisplayRec(uint8_t a2, ItemDisplayInfoRec* rec);
    /* 77 */ virtual uint32_t GetVirtualItemDisplayID(uint8_t a2);
    /* 78 */ virtual uint8_t GetClientStandState();
    /* 83 */ virtual float GetPitch();

    // Static functions
    static const char* GetDisplayRaceNameFromRecord(ChrRacesRec* record, uint8_t sexIn, uint8_t* sexOut = nullptr);
    static const char* GetDisplayClassNameFromRecord(ChrClassesRec* record, uint8_t sexIn, uint8_t* sexOut = nullptr);
    static void SetStorage(CGUnit_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
    static void ClientInitialize();
    static void Initialize();
    static void InitActiveMover(WGUID guid);
    static void RegisterMirrorHandlers();
    static int32_t GetTrackingType();
    static float GetTrackingTurn();
    static void UpdateAllSmoothFacing();

    // Packet handlers
    static int32_t HandleMonsterMovePacket(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t HandleMovementPacket(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
};

#endif // CLIENTOBJECT_UNIT_C_HPP
