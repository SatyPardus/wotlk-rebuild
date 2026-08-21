#include "ffx/FFX.hpp"

CVar* FFX::s_cvarFFXRectangle;
CVar* FFX::s_cvarFFX;

void FFX::Init() {
    FFX::s_cvarFFX = CVar::Register("ffx", "full screen effects", 1, "1", FFX::CVarCallback, 1, 0, 0, 0);
    FFX::s_cvarFFXRectangle = CVar::Register("ffxRectangle", "use rectangle texture for full screen effects", 1, "1", FFX::CVarCallback, 1, 0, 0, 0);
    // TODO
}

bool FFX::CVarCallback(CVar*, const char*, const char*, void*) {
    // TODO
    return true;
}

FFX::Effect::Effect() {
    //this->unk_0000 = &off_A418A8;
    this->m_cvar = 0;
    this->unk_0008 = 0;
    this->unk_000C = 0;
    this->unk_0010 = 0;
    this->unk_0014 = 0;
    this->unk_0018 = 1;
}
