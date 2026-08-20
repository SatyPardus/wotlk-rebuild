#include "componentcore/ComponentUtils.hpp"
#include "db/Db.hpp"

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
