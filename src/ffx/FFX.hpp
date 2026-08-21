#ifndef FFX_FFX_HPP
#define FFX_FFX_HPP

#include <cstdint>
#include "console/CVar.hpp"

namespace FFX {
    extern CVar* s_cvarFFXRectangle;
    extern CVar* s_cvarFFX;

    void Init();
    bool CVarCallback(CVar*, const char*, const char*, void*);

    class Effect {
        public:
        /* 0000 */ // vftable
        /* 0004 */ CVar* m_cvar;
        /* 0008 */ uint32_t unk_0008;
        /* 000C */ uint32_t unk_000C;
        /* 0010 */ uint32_t unk_0010;
        /* 0014 */ uint32_t unk_0014;
        /* 0018 */ uint8_t unk_0018;
        /* 0019 */ uint8_t unk_0019;
        /* 001A */ uint8_t unk_001A;
        /* 001B */ uint8_t unk_001B;

        Effect();
    };
}

#endif
