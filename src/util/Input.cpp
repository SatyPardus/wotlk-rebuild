#include "util/Input.hpp"
#include <storm/String.hpp>

// OFFSET: 0x7DC010
uint32_t StringToMouseButton(const char* name) {
    if (!name || !*name) {
        return 0;
    }

    if (!SStrCmpI(name, "LeftButton", 0x7FFFFFFF)) {
        return 0x00000001;
    }
    if (!SStrCmpI(name, "RightButton", 0x7FFFFFFF)) {
        return 0x00000004;
    }
    if (!SStrCmpI(name, "MiddleButton", 0x7FFFFFFF)) {
        return 0x00000002;
    }
    if (!SStrCmpI(name, "Button4", 0x7FFFFFFF)) {
        return 0x00000008;
    }
    if (!SStrCmpI(name, "Button5", 0x7FFFFFFF)) {
        return 0x00000010;
    }
    if (!SStrCmpI(name, "Button6", 0x7FFFFFFF)) {
        return 0x00000020;
    }
    if (!SStrCmpI(name, "Button7", 0x7FFFFFFF)) {
        return 0x00000040;
    }
    if (!SStrCmpI(name, "Button8", 0x7FFFFFFF)) {
        return 0x00000080;
    }
    if (!SStrCmpI(name, "Button9", 0x7FFFFFFF)) {
        return 0x00000100;
    }
    if (!SStrCmpI(name, "Button10", 0x7FFFFFFF)) {
        return 0x00000200;
    }
    if (!SStrCmpI(name, "Button11", 0x7FFFFFFF)) {
        return 0x00000400;
    }
    if (!SStrCmpI(name, "Button12", 0x7FFFFFFF)) {
        return 0x00000800;
    }
    if (!SStrCmpI(name, "Button13", 0x7FFFFFFF)) {
        return 0x00001000;
    }
    if (!SStrCmpI(name, "Button14", 0x7FFFFFFF)) {
        return 0x00002000;
    }
    if (!SStrCmpI(name, "Button15", 0x7FFFFFFF)) {
        return 0x00004000;
    }
    if (!SStrCmpI(name, "Button16", 0x7FFFFFFF)) {
        return 0x00008000;
    }
    if (!SStrCmpI(name, "Button17", 0x7FFFFFFF)) {
        return 0x00010000;
    }
    if (!SStrCmpI(name, "Button18", 0x7FFFFFFF)) {
        return 0x00020000;
    }
    if (!SStrCmpI(name, "Button19", 0x7FFFFFFF)) {
        return 0x00040000;
    }
    if (!SStrCmpI(name, "Button20", 0x7FFFFFFF)) {
        return 0x00080000;
    }
    if (!SStrCmpI(name, "Button21", 0x7FFFFFFF)) {
        return 0x00100000;
    }
    if (!SStrCmpI(name, "Button22", 0x7FFFFFFF)) {
        return 0x00200000;
    }
    if (!SStrCmpI(name, "Button23", 0x7FFFFFFF)) {
        return 0x00400000;
    }
    if (!SStrCmpI(name, "Button24", 0x7FFFFFFF)) {
        return 0x00800000;
    }
    if (!SStrCmpI(name, "Button25", 0x7FFFFFFF)) {
        return 0x01000000;
    }
    if (!SStrCmpI(name, "Button26", 0x7FFFFFFF)) {
        return 0x02000000;
    }
    if (!SStrCmpI(name, "Button27", 0x7FFFFFFF)) {
        return 0x04000000;
    }
    if (!SStrCmpI(name, "Button28", 0x7FFFFFFF)) {
        return 0x08000000;
    }
    if (!SStrCmpI(name, "Button29", 0x7FFFFFFF)) {
        return 0x10000000;
    }
    if (!SStrCmpI(name, "Button30", 0x7FFFFFFF)) {
        return 0x20000000;
    }
    if (!SStrCmpI(name, "Button31", 0x7FFFFFFF)) {
        return 0x40000000;
    }

    return 0;
}

// OFFSET: 0x55DF30
uint32_t MouseButtonToIndex(uint32_t button) {
    switch (button) {
    case 0x00000001:
        return 1; // LeftButton
    case 0x00000004:
        return 2; // RightButton
    case 0x00000002:
        return 3; // MiddleButton
    case 0x00000008:
        return 4;
    case 0x00000010:
        return 5;
    case 0x00000020:
        return 6;
    case 0x00000040:
        return 7;
    case 0x00000080:
        return 8;
    case 0x00000100:
        return 9;
    case 0x00000200:
        return 10;
    case 0x00000400:
        return 11;
    case 0x00000800:
        return 12;
    case 0x00001000:
        return 13;
    case 0x00002000:
        return 14;
    case 0x00004000:
        return 15;
    case 0x00008000:
        return 16;
    case 0x00010000:
        return 17;
    case 0x00020000:
        return 18;
    case 0x00040000:
        return 19;
    case 0x00080000:
        return 20;
    case 0x00100000:
        return 21;
    case 0x00200000:
        return 22;
    case 0x00400000:
        return 23;
    case 0x00800000:
        return 24;
    case 0x01000000:
        return 25;
    case 0x02000000:
        return 26;
    case 0x04000000:
        return 27;
    case 0x08000000:
        return 28;
    case 0x10000000:
        return 29;
    case 0x20000000:
        return 30;
    case 0x40000000:
        return 31;
    default:
        return 0;
    }
}
