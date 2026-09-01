#include "util/Network.hpp"

// OFFSET: 0x990370
bool AckMessageNeedsIndex(NETMESSAGE msgId) {
    return msgId == CMSG_FORCE_MOVE_ROOT_ACK
        || msgId == CMSG_FORCE_MOVE_UNROOT_ACK
        || msgId == MSG_MOVE_TELEPORT_ACK
        || msgId == CMSG_FORCE_RUN_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_SWIM_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_WALK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_TURN_RATE_CHANGE_ACK
        || msgId == CMSG_FORCE_PITCH_RATE_CHANGE_ACK
        || msgId == CMSG_MOVE_HOVER_ACK
        || msgId == CMSG_MOVE_SET_CAN_FLY_ACK
        || msgId == CMSG_MOVE_FEATHER_FALL_ACK
        || msgId == CMSG_MOVE_WATER_WALK_ACK
        || msgId == CMSG_MOVE_KNOCK_BACK_ACK
        || msgId == CMSG_MOVE_GRAVITY_DISABLE_ACK
        || msgId == CMSG_MOVE_GRAVITY_ENABLE_ACK
        || msgId == CMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY_ACK
        || msgId == CMSG_MOVE_SET_COLLISION_HGT_ACK;
}

// OFFSET: 0x990420
bool IsAckMessage(NETMESSAGE msgId) {
    return msgId == CMSG_FORCE_MOVE_ROOT_ACK
        || msgId == CMSG_FORCE_MOVE_UNROOT_ACK
        || msgId == MSG_MOVE_TELEPORT_ACK
        || msgId == CMSG_FORCE_RUN_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_SWIM_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_WALK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_TURN_RATE_CHANGE_ACK
        || msgId == CMSG_FORCE_PITCH_RATE_CHANGE_ACK
        || msgId == CMSG_MOVE_HOVER_ACK
        || msgId == CMSG_MOVE_SET_CAN_FLY_ACK
        || msgId == CMSG_MOVE_FEATHER_FALL_ACK
        || msgId == CMSG_MOVE_WATER_WALK_ACK
        || msgId == CMSG_MOVE_KNOCK_BACK_ACK
        || msgId == CMSG_MOVE_NOT_ACTIVE_MOVER
        || msgId == CMSG_MOVE_GRAVITY_DISABLE_ACK
        || msgId == CMSG_MOVE_GRAVITY_ENABLE_ACK
        || msgId == CMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY_ACK
        || msgId == CMSG_MOVE_SET_COLLISION_HGT_ACK;
}

// OFFSET: 0x9904E0
bool IsMessageAllowedWhileOnSpline(NETMESSAGE msgId) {
    if (msgId > CMSG_MOVE_SET_FLY) {
        if (msgId == CMSG_DISMISS_CONTROLLED_VEHICLE || msgId == CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE)
            return 1;
    } else {
        if (msgId == CMSG_MOVE_SET_FLY)
            return 1;
        if (msgId <= MSG_MOVE_TOGGLE_COLLISION_CHEAT)
            return msgId == MSG_MOVE_TOGGLE_COLLISION_CHEAT || msgId >= MSG_MOVE_START_SWIM && msgId <= MSG_MOVE_STOP_SWIM;
        if (msgId >= CMSG_MOVE_START_SWIM_CHEAT)
            return msgId <= CMSG_MOVE_STOP_SWIM_CHEAT;
    }
    return 0;
}

// OFFSET: 0x7151F0
bool sub_7151F0(NETMESSAGE msgId) {
    return msgId == CMSG_FORCE_RUN_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_SWIM_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_WALK_SPEED_CHANGE_ACK
        || msgId == CMSG_FORCE_TURN_RATE_CHANGE_ACK
        || msgId == CMSG_FORCE_PITCH_RATE_CHANGE_ACK
        || msgId == CMSG_MOVE_HOVER_ACK
        || msgId == CMSG_MOVE_SET_CAN_FLY_ACK
        || msgId == CMSG_MOVE_FEATHER_FALL_ACK
        || msgId == CMSG_MOVE_WATER_WALK_ACK
        || msgId == CMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY_ACK
        || msgId == CMSG_MOVE_SET_COLLISION_HGT_ACK;
}
