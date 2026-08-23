#ifndef COMPONENT_CORE_COMPONENT_UTILS_HPP
#define COMPONENT_CORE_COMPONENT_UTILS_HPP

#include "componentcore/Types.hpp"

class CharSectionsRec;
class CharacterFacialHairStylesRec;
struct CHARACTER_PREFERENCES;

void BuildComponentArray(uint32_t numRaceSexPairs, st_race** out);
CharSectionsRec* ComponentGetSectionsRecord(st_race* lookup, uint32_t raceId, uint32_t genderId, uint32_t variation, uint32_t variationIndex, uint32_t colorIndex, bool* found);
bool ComponentValidateBase(st_race* lookup, uint32_t raceId, uint32_t genderId, uint32_t variation, uint32_t variationIndex, uint32_t colorIndex);
uint32_t GetConditionalGeoset(CHARACTER_PREFERENCES* preferences);
CharacterFacialHairStylesRec* GetConditionalFacialHairStyle(CHARACTER_PREFERENCES* preferences);
uint32_t GetSelectionFromContext(COMPONENT_CONTEXT context, uint32_t a2);
uint32_t ComponentGetNumColors(st_race* lookup, uint32_t raceId, uint32_t genderId, uint32_t variation, uint32_t variationIndex);
bool ComponentFlagsMatch(uint32_t flags, uint32_t selection);
int32_t ComponentGetSkinColor(uint32_t raceId, uint32_t sexId, uint32_t index, uint32_t selection);
uint32_t ComponentGetNumVariations(st_race* lookup, uint32_t raceId, uint32_t genderId, uint32_t variation);
uint32_t ComponentGetHairVariation(uint32_t raceId, uint32_t sexId, uint32_t colorId, uint32_t index, uint32_t selection);

#endif // COMPONENT_CORE_COMPONENT_UTILS_HPP
