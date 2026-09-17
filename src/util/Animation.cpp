#include "util/Animation.hpp"
#include "db/Db.hpp"

// OFFSET: 0x7176B0
uint32_t GetAnimBehaviorId(ANIMATION_ID animId) {
    auto animationDataRec = g_animationDataDB.GetRecord(animId);
    if (animationDataRec)
        return animationDataRec->m_behaviorID;
    return ANIM_COUNT;
}

// OFFSET: 0x71DC20
bool IsAirboneAnim(uint32_t sequenceId) {
    auto animationDataRec = g_animationDataDB.GetRecord(sequenceId);
    if (!animationDataRec)
        return false;

    return animationDataRec->m_behaviorID >= 37 && (animationDataRec->m_behaviorID <= 40 || animationDataRec->m_behaviorID == 467);
}

// OFFSET: 0x71D940
bool IsEmoteAnim(ANIMATION_ID animId) {
    switch (GetAnimBehaviorId(animId)) {
    case ANIM_EMOTE_TALK:
    case ANIM_EMOTE_EAT:
    case ANIM_EMOTE_WORK:
    case ANIM_EMOTE_USE_STANDING:
    case ANIM_EMOTE_TALK_EXCLAMATION:
    case ANIM_EMOTE_TALK_QUESTION:
    case ANIM_EMOTE_BOW:
    case ANIM_EMOTE_WAVE:
    case ANIM_EMOTE_CHEER:
    case ANIM_EMOTE_DANCE:
    case ANIM_EMOTE_LAUGH:
    case ANIM_EMOTE_SLEEP:
    case ANIM_EMOTE_SIT_GROUND:
    case ANIM_EMOTE_RUDE:
    case ANIM_EMOTE_ROAR:
    case ANIM_EMOTE_KISS:
    case ANIM_EMOTE_CRY:
    case ANIM_EMOTE_CHICKEN:
    case ANIM_EMOTE_APPLAUD:
    case ANIM_EMOTE_SHOUT:
    case ANIM_EMOTE_FLEX:
    case ANIM_EMOTE_SHY:
    case ANIM_EMOTE_POINT:
    case ANIM_EMOTE_SALUTE:
    case ANIM_EMOTE_WORK_NO_SHEATHE:
    case ANIM_EMOTE_STUN_NO_SHEATHE:
    case ANIM_EMOTE_USE_STANDING_NO_SHEATHE:
    case ANIM_DRUID_BEAR_MAUL:
    case ANIM_EMOTE_YES:
    case ANIM_EMOTE_NO:
    case ANIM_EMOTE_TRAIN:
        return true;
    }
    return false;
}

// OFFSET: 0x71D380
bool IsSpellCastAnim(ANIMATION_ID animId) {
    switch (GetAnimBehaviorId(animId)) {
    case ANIM_SPELL:
    case ANIM_SPELL_CAST:
    case ANIM_SPELL_CAST_AREA:
    case ANIM_SPELL_CAST_DIRECTED:
    case ANIM_SPELL_CAST_OMNI:
        return true;
    }
    return false;
}

// OFFSET: 0x71D410
bool IsSpellPrecastAnim(ANIMATION_ID animId) {
    switch (GetAnimBehaviorId(animId)) {
    case ANIM_READY_SPELL_DIRECTED:
    case ANIM_READY_SPELL_OMNI:
        return true;
    }
    return false;
}

// OFFSET: 0x71DC70
bool IsSpellCastOrPrecastAnim(ANIMATION_ID animId) {
    if (!IsSpellCastAnim(animId) || !IsSpellPrecastAnim(animId))
        return false;

    return false;
}

// OFFSET: 0x71DA20
bool IsThrownWeaponAnim(ANIMATION_ID animId) {
    switch (GetAnimBehaviorId(animId)) {
    case ANIM_ATTACK_THROWN:
    case ANIM_HOLD_THROWN:
    case ANIM_LOAD_THROWN:
        return true;
    }
    return false;
}

// OFFSET: 0x71DA60
bool IsBowAnim(ANIMATION_ID animId) {
    switch (GetAnimBehaviorId(animId)) {
    case ANIM_ATTACK_BOW:
    case ANIM_LOAD_BOW:
    case ANIM_HOLD_BOW:
        return true;
    }
    return false;
}

// OFFSET: 0x71DAA0
bool IsRifleAnim(ANIMATION_ID animId) {
    switch (GetAnimBehaviorId(animId)) {
    case ANIM_ATTACK_RIFLE:
    case ANIM_LOAD_RIFLE:
    case ANIM_HOLD_RIFLE:
        return true;
    }
    return false;
}

