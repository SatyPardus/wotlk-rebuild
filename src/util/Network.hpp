#ifndef UTIL_NETWORK_HPP
#define UTIL_NETWORK_HPP

#include <cstdint>
#include "net/Types.hpp"

bool AckMessageNeedsIndex(NETMESSAGE msgId);

bool IsAckMessage(NETMESSAGE msgId);

bool IsMessageAllowedWhileOnSpline(NETMESSAGE msgId);

bool sub_7151F0(NETMESSAGE msgId);

#endif
