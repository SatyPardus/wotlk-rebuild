#include "componentcore/ComponentData.hpp"

// OFFSET: 0x4DFDA0
ComponentData::ComponentData() {
    this->m_flags &= 0xFFFFFFFC;
    this->m_preferences.raceID = 0;
    this->m_preferences.sexID = 0;
    this->m_preferences.classID = 0;
    this->m_preferences.hairColorID = 0;
    this->m_preferences.skinID = 0;
    this->m_preferences.faceID = 0;
    this->m_preferences.facialHairStyleID = 0;
    this->m_preferences.hairStyleID = 0;
    this->m_npcSkinTexture[0] = 0;
    this->m_model = nullptr;

    DefaultGeosets();
}

ComponentData::ComponentData(const CHARACTER_INFO& info) {
    this->m_flags &= 0xFFFFFFFC;
    this->m_model = nullptr;
    this->m_npcSkinTexture[0] = 0;

    this->m_preferences.raceID = info.raceID;
    this->m_preferences.sexID = info.sexID;
    this->m_preferences.classID = info.classID;
    this->m_preferences.skinID = info.skinID;
    this->m_preferences.faceID = info.faceID;
    this->m_preferences.hairStyleID = info.hairStyleID;
    this->m_preferences.hairColorID = info.hairColorID;
    this->m_preferences.facialHairStyleID = info.facialHairStyleID;

    DefaultGeosets();
}

// OFFSET: none (inlined)
void ComponentData::DefaultGeosets() {
    this->m_geosets[0] = 1;
    this->m_geosets[1] = 101;
    this->m_geosets[2] = 201;
    this->m_geosets[3] = 301;
    this->m_geosets[7] = 702;
    this->m_geosets[4] = 401;
    this->m_geosets[5] = 501;
    this->m_geosets[6] = 601;
    this->m_geosets[8] = 801;
    this->m_geosets[9] = 901;
    this->m_geosets[10] = 1001;
    this->m_geosets[11] = 1101;
    this->m_geosets[12] = 1201;
    this->m_geosets[13] = 1301;
    this->m_geosets[14] = 1401;
    this->m_geosets[15] = 1501;
    this->m_geosets[16] = 1601;
    this->m_geosets[17] = 1701;
    this->m_geosets[18] = 1801;
}
