#ifndef GAMEUI_TYPES_HPP
#define GAMEUI_TYPES_HPP

#include <tempest/Vector.hpp>
#include "clientobject/WGUID.hpp"
#include "event/Types.hpp"

struct CWorldClickEvent {
    C3Vector segStart;
    C3Vector segEnd;
    MOUSEBUTTON button;
};

struct CTerrainClickEvent {
    WGUID guid;
    C3Vector point;
    MOUSEBUTTON button;
};

struct CSpriteClickEvent {
    WGUID guid;
    MOUSEBUTTON button;
};

#endif
