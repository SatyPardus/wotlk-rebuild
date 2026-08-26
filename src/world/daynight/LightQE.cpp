#include "world/daynight/LightQE.hpp"
#include "db/rec/LightRec.hpp"
#include "tempest/Math.hpp"

namespace DayNight {

    int32_t LightQE::s_localSort = 0;

    // OFFSET: 0x7ED0A0
    bool LightQE::HasHigherPriority(const LightQE& a, const LightQE& b) {
        if (LightQE::s_localSort) {
            return b.m_key >= a.m_key;
        }

        static const float tolerance = 0.3333333432674408f;

        const LightRec* lightA = *a.m_light;
        const LightRec* lightB = *b.m_light;

        const float dx = lightA->m_gameCoords[0] - lightB->m_gameCoords[0];
        const float dy = lightA->m_gameCoords[1] - lightB->m_gameCoords[1];
        const float dz = lightA->m_gameCoords[2] - lightB->m_gameCoords[2];

        if (sqrt(dx * dx + dy * dy + dz * dz) > tolerance) {
            return b.m_key <= a.m_key;
        }

        return lightB->m_gameFalloffStart <= lightA->m_gameFalloffStart;
    }

} // namespace DayNight
