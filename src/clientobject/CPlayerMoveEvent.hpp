#ifndef CLIENTOBJECT_C_PLAYER_MOVE_EVENT_HPP
#define CLIENTOBJECT_C_PLAYER_MOVE_EVENT_HPP

#include <cstdint>
#include <tempest/Vector.hpp>
#include <storm/List.hpp>
#include "clientobject/WGUID.hpp"

class CPlayerMoveEvent {
    public:
    TSLink<CPlayerMoveEvent> m_link = {};
    uint32_t m_eventTime;
    uint32_t m_eventId;
    C3Vector m_position;
    float m_facing;
    float m_pitch;
    uint32_t unk_0024;
    WGUID m_transportGuid;
    uint32_t m_moveFlags;
    uint16_t m_moveExtraFlags;
    uint16_t unk_0036;
    uint32_t m_fallTime;
    float m_value;
    uint32_t unk_0040;
    uint32_t unk_0044;
    uint32_t unk_0048;
    uint32_t m_ackCounter;
    uint8_t unk_0050;
    uint8_t m_needAck;
    uint8_t m_seat;
    uint8_t unk_0053;
    uint32_t unk_0054;
};

#endif // CLIENTOBJECT_DYNAMIC_OBJECT_C_HPP
