#include "db/Db.hpp"
#include "db/WowClientDB_Base.hpp"

#include <cstdio>

void LoadDB(WowClientDB_Base* db, const char* filename, int32_t linenumber) {
    db->Load(filename, linenumber);
    printf("load %s:%d\n", filename, linenumber);
};

// OFFSET: 0x7EAF60
void TransformMapCoord(LightRec* rec, bool a2) {
    if ((0.0 != rec->m_gameCoords[0] || 0.0 != rec->m_gameCoords[1] || 0.0 != rec->m_gameCoords[2]) && a2) {
        rec->m_gameCoords[0] = rec->m_gameCoords[0] * 0.027777778f;
        rec->m_gameCoords[1] = rec->m_gameCoords[1] * 0.027777778f;
        rec->m_gameCoords[2] = rec->m_gameCoords[2] * 0.027777778f;
        rec->m_gameFalloffStart = rec->m_gameFalloffStart * 0.027777778f;
        rec->m_gameFalloffEnd = 0.027777778 * rec->m_gameFalloffEnd;
        auto v4 = 0.0;
        auto v5 = 0.0;
        //if (!bn_World_MapIsDungeon()) {
            v4 = 17066.666;
            v5 = 17066.666;
        //}
        const float x = rec->m_gameCoords[0];
        const float y = rec->m_gameCoords[1];
        const float z = rec->m_gameCoords[2];

        rec->m_gameCoords[0] = -(z - v4);
        rec->m_gameCoords[1] = -(x - v5);
        rec->m_gameCoords[2] = y;
    }
}

void sub_7EB060(bool a1) {
    for (int32_t i = 0; i < g_lightDB.GetNumRecords(); i++) {
        auto rec = g_lightDB.GetRecordByIndex(i);
        TransformMapCoord(rec, a1);
    }
}

void ClientDBInitialize() {
    // TODO

    StaticDBLoadAll(LoadDB);

    // TODO
    sub_7EB060(true);
}
