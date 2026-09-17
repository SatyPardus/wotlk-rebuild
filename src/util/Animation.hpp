#ifndef UTIL_ANIMATION_HPP
#define UTIL_ANIMATION_HPP

#include <cstdint>
#include <clientobject/AnimationTypes.hpp>

bool IsAirboneAnim(uint32_t sequenceId);
uint32_t GetAnimBehaviorId(ANIMATION_ID animId);
bool IsEmoteAnim(ANIMATION_ID animId);
bool IsSpellCastAnim(ANIMATION_ID animId);
bool IsSpellPrecastAnim(ANIMATION_ID animId);
bool IsSpellCastOrPrecastAnim(ANIMATION_ID animId);
bool IsThrownWeaponAnim(ANIMATION_ID animId);
bool IsBowAnim(ANIMATION_ID animId);
bool IsRifleAnim(ANIMATION_ID animId);

#endif
