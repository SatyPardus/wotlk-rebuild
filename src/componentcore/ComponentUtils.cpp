#include "componentcore/ComponentUtils.hpp"
#include "componentcore/CCharacterComponent.hpp"
#include "db/Db.hpp"
#include "net/Types.hpp"

// OFFSET: 0x4F3DD0
void BuildComponentArray(uint32_t numRaceSexPairs, st_race** out) {
    st_race* lookup;

    if (!out)
        return;

    lookup = reinterpret_cast<st_race*>(STORM_ALLOC(numRaceSexPairs > 0x6666666 ? -1 : sizeof(st_race) * numRaceSexPairs));
    if (lookup) {
        for (uint32_t i = 0; i < numRaceSexPairs; i++) {
            // This is in a extra function at 0x4011D0
            // Put it here cause lazy to make that function
            for (uint32_t j = 0; j < 5; j++) {
                lookup[i].m_variation[j].numVariations = 0;
                lookup[i].m_variation[j].variation = nullptr;
            }
        }
    }

    uint32_t prevRace, prevSex, prevSection;
    uint32_t maxVariation = 0;
    CharVariationList* cell;
    uint32_t count;

    CharSectionsRec* rec = g_charSectionsDB.GetRecordByIndex(0);
    prevRace = rec->m_raceID;
    prevSex = rec->m_sexID;
    prevSection = rec->m_baseSection;

    for (uint32_t i = 0; i < g_charSectionsDB.GetNumRecords(); ++i) {
        rec = g_charSectionsDB.GetRecordByIndex(i);

        if (rec->m_baseSection < 5) {
            if (prevRace != rec->m_raceID || prevSex != rec->m_sexID || prevSection != rec->m_baseSection) {
                count = maxVariation + 1;
                cell = &lookup[prevSex + 2 * prevRace].m_variation[prevSection];
                cell->numVariations = maxVariation + 1;

                if (count > 0) {
                    CharColorList* cl = reinterpret_cast<CharColorList*>(STORM_ALLOC(count > 0x1FFFFFFF ? -1 : sizeof(CharColorList) * count));
                    if (cl)
                        for (int j = 0; j < count; ++j) {
                            cl[j].numColors = 0;
                            cl[j].color = nullptr;
                        }
                    cell->variation = cl;
                }
                maxVariation = 0;
            }

            if (maxVariation <= rec->m_variationIndex)
                maxVariation = rec->m_variationIndex;

            prevRace = rec->m_raceID;
            prevSex = rec->m_sexID;
            prevSection = rec->m_baseSection;
        }
    }

    count = maxVariation + 1;
    cell = &lookup[prevSex + 2 * prevRace].m_variation[prevSection];
    cell->numVariations = count;
    if (count > 0) {
        CharColorList* cl = reinterpret_cast<CharColorList*>(STORM_ALLOC(count > 0x1FFFFFFF ? -1 : sizeof(CharColorList) * count));
        if (cl)
            for (int j = 0; j < count; ++j) {
                cl[j].numColors = 0;
                cl[j].color = NULL;
            }
        cell->variation = cl;
    }

    uint32_t prevVariation;
    uint32_t maxColor = 0;

    prevRace = 1;
    prevSex = 0;
    prevSection = 0;
    prevVariation = 0;

    for (uint32_t i = 0; i < g_charSectionsDB.GetNumRecords(); ++i) {
        rec = g_charSectionsDB.GetRecordByIndex(i);

        if (rec->m_baseSection < 5) {
            if (prevRace != rec->m_raceID || prevSex != rec->m_sexID || prevSection != rec->m_baseSection || prevVariation != rec->m_variationIndex) {
                cell = &lookup[prevSex + 2 * prevRace].m_variation[prevSection];

                if (cell->numVariations > 0)
                {
                    count = maxColor + 1;
                    cell->variation[prevVariation].numColors = count;

                    if (cell->variation[prevVariation].numColors > 0) {
                        CharSectionsRec** rows = reinterpret_cast<CharSectionsRec**>(STORM_ALLOC(count > 0x3FFFFFFF ? -1 : sizeof(void*) * count));
                        if (rows)
                            memset(rows, 0, sizeof(void*) * count);
                        cell->variation[prevVariation].color = rows;
                    }
                }
                maxColor = 0;
            }

            if (maxColor <= rec->m_colorIndex)
                maxColor = rec->m_colorIndex;

            prevRace = rec->m_raceID;
            prevSex = rec->m_sexID;
            prevSection = rec->m_baseSection;
            prevVariation = rec->m_variationIndex;
        }
    }

    cell = &lookup[prevSex + 2 * prevRace].m_variation[prevSection];
    if (cell->numVariations > 0) {
        count = maxColor + 1;
        cell->variation[prevVariation].numColors = count;
        if (cell->variation[prevVariation].numColors > 0) {
            CharSectionsRec** rows = reinterpret_cast<CharSectionsRec**>(STORM_ALLOC(count > 0x3FFFFFFF ? -1 : sizeof(void*) * count));
            if (rows)
                memset(rows, 0, sizeof(void*) * count);
            cell->variation[prevVariation].color = rows;
        }
    }

    CharColorList* colorList;
    for (uint32_t i = 0; i < g_charSectionsDB.GetNumRecords(); ++i) {
        rec = g_charSectionsDB.GetRecordByIndex(i);

        if (rec->m_baseSection < 5) {
            int vi = rec->m_variationIndex;
            int ci = rec->m_colorIndex;

            cell = &lookup[rec->m_sexID + 2 * rec->m_raceID].m_variation[rec->m_baseSection];

            if (cell->numVariations > 0 && vi < cell->numVariations) {
                colorList = &cell->variation[vi];
                if (colorList->numColors > 0 && ci < colorList->numColors)
                    colorList->color[ci] = rec;
            }
        }
    }

    *out = lookup;
}

// OFFSET: 0x4F3BA0
CharSectionsRec* ComponentGetSectionsRecord(st_race* lookup, uint32_t raceId, uint32_t genderId, uint32_t variation, uint32_t variationIndex, uint32_t colorIndex, bool* found) {
    if (found)
        *found = false;

    if (variation > 4)
        return nullptr;

    auto v7 = &lookup[2 * raceId + genderId].m_variation[variation];

    if (!v7->numVariations || variationIndex >= v7->numVariations)
        return nullptr;

    auto v8 = &v7->variation[variationIndex];
    if (!v8->numColors || colorIndex >= v8->numColors)
        return nullptr;

    if (found)
        *found = true;
    return v8->color[colorIndex];
}

// OFFSET: 0x4F3B50
bool ComponentValidateBase(st_race* lookup, uint32_t raceId, uint32_t genderId, uint32_t variation, uint32_t variationIndex, uint32_t colorIndex) {
    if (variation > 4)
        return false;

    auto v7 = &lookup[2 * raceId + genderId].m_variation[variation];

    if (!v7->numVariations || variationIndex >= v7->numVariations)
        return false;

    auto v8 = &v7->variation[variationIndex];
    if (!v8->numColors || colorIndex >= v8->numColors)
        return false;

    return true;
}

// OFFSET: 0x4EA050
uint32_t GetConditionalGeoset(CHARACTER_PREFERENCES* preferences) {
    for (uint32_t i = 0; i < g_charHairGeosetsDB.GetNumRecords(); i++) {
        auto rec = g_charHairGeosetsDB.GetRecordByIndex(i);
        if (preferences->raceID == rec->m_raceID && preferences->sexID == rec->m_sexID && preferences->hairStyleID == rec->m_variationID) {
            if (rec->m_geosetID)
                return rec->m_geosetID;
            break;
        }
    }
    return 1;
}

// OFFSET: 0x4EA000
CharacterFacialHairStylesRec* GetConditionalFacialHairStyle(CHARACTER_PREFERENCES* preferences) {
    for (uint32_t i = 0; i < g_characterFacialHairStylesDB.GetNumRecords(); i++) {
        auto rec = g_characterFacialHairStylesDB.GetRecordByIndex(i);
        if (preferences->raceID == rec->m_raceID && preferences->sexID == rec->m_sexID && preferences->hairStyleID == rec->m_variationID) {
            return rec;
        }
    }
    return nullptr;
}

// OFFSET: 0x4F3A40
uint32_t GetSelectionFromContext(COMPONENT_CONTEXT context, uint32_t a2) {
    switch (context) {
    case CONTEXT_1:
        return (a2 == 6 ? 1 : 0) + 2;
    case CONTEXT_2:
        return 4;
    case CONTEXT_3:
        return (a2 == 6 ? 1 : 0) + 5;
    }
    return a2 == 6 ? 1 : 0;
}

// OFFSET: 0x4F3B10
uint32_t ComponentGetNumColors(st_race* lookup, uint32_t raceId, uint32_t genderId, uint32_t variation, uint32_t variationIndex) {
    if (variation > 4)
        return 0;

    auto v7 = &lookup[2 * raceId + genderId].m_variation[variation];

    if (!v7->numVariations || variationIndex >= v7->numVariations)
        return 0;

    auto v8 = &v7->variation[variationIndex];
    if (!v8->numColors || !v8->color)
        return 0;

    return v8->numColors;
}

// OFFSET: 0x4F39A0
bool ComponentFlagsMatch(uint32_t flags, uint32_t selection) {
    switch (selection) {
    case 0:
        if ((flags & 1) == 0)
            return 0;
        return (flags & 0xC) == 0;
    case 1:
        return (flags & 1) != 0 && (flags & 0x14) != 0 && (flags & 8) == 0;
    case 2:
        if ((flags & 3) == 0)
            return 0;
        return (flags & 0xC) == 0;
    case 3:
        if ((flags & 3) == 0 || (flags & 0x14) == 0)
            return 0;
        return (flags & 8) == 0;
    case 4:
        return 1;
    case 5:
        return (flags & 0xC) == 0;
    case 6:
        if ((flags & 0x14) == 0)
            return 0;
        return (flags & 8) == 0;
    default:
        return 0;
    }
}

// OFFSET: 0x4E7E90
int32_t ComponentGetSkinColor(uint32_t raceId, uint32_t sexId, uint32_t index, uint32_t selection) {
    uint32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, raceId, sexId, 0, 0);
    if (!numColors)
        return -1;

    uint32_t match = 0;
    for (uint32_t i = 0; i < numColors; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, 0, 0, i, nullptr);
        if (!rec || !ComponentFlagsMatch(rec->m_flags, selection))
            continue;
        if (match == index)
            return rec->m_colorIndex;
        match++;
    }
    return -1;
}

// OFFSET: 0x4F3AE0
uint32_t ComponentGetNumVariations(st_race* lookup, uint32_t raceId, uint32_t genderId, uint32_t variation) {
    if (variation > 4)
        return 0;

    auto v7 = &lookup[2 * raceId + genderId].m_variation[variation];

    if (!v7->numVariations || !v7->variation)
        return 0;

    return v7->numVariations;
}

// OFFSET: 0x4E7F20
uint32_t ComponentGetHairVariation(uint32_t raceId, uint32_t sexId, uint32_t colorId, uint32_t index, uint32_t selection) {
    uint32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceId, sexId, 3);
    if (!numVariations)
        return -1;

    uint32_t match = 0;
    for (uint32_t i = 0; i < numVariations; i++) {
        auto rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceId, sexId, 3, i, colorId, nullptr);
        if (!rec || !ComponentFlagsMatch(rec->m_flags, selection))
            continue;
        if (match == index)
            return rec->m_variationIndex;
        match++;
    }
    return -1;
}
