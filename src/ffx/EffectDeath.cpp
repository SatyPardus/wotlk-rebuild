#include "ffx/EffectDeath.hpp"
#include "console/CVar.hpp"

EffectDeath::EffectDeath()
    : FFX::Effect() {
    //this->unk_0000 = &off_A418D8;
    auto v3 = "1";
    if (this->unk_0018 == 0)
        v3 = "0";
    this->m_cvar = CVar::Register("ffxDeath", "full screen death effect", 1, v3, FFX::CVarCallback, 1, 0, 0, 0);
    //if (CGxDevice::Caps(g_theGxDevicePtr)->m_shaderTargets[4] == 7) {
    //    v6 = &dword_D457A0;
    //    if (SMemAlloc(52, ".\\FFXEffects.cpp", 1022, 0)) {
    //        v7 = maybe_EffectDeath__EffectDeath(&v6, 1, &dword_D45784, 0, 1);
    //        bn_TSGrowableArray_FFX_Pass_New(&this->unk_0008, &v7);
    //    } else {
    //        v7 = 0;
    //        bn_TSGrowableArray_FFX_Pass_New(&this->unk_0008, &v7);
    //    }
    //    return this;
    //} else {
    //    if (CGxDevice::Caps(g_theGxDevicePtr)->m_shaderTargets[4] > 0) {
    //        v5 = &dword_D457A0;
    //        v6 = &dword_D45810;
    //        if (SMemAlloc(60, ".\\FFXEffects.cpp", 1032, 0))
    //            v7 = bn_PassBox4_constructor(&v5, 1, v6, 0, 0);
    //        else
    //            v7 = 0;
    //        bn_TSGrowableArray_FFX_Pass_New(&this->unk_0008, &v7);
    //        if (SMemAlloc(60, ".\\FFXEffects.cpp", 1033, 0))
    //            v7 = bn_PassGauss4_constructor(&v6, 1, v6, 0, 0);
    //        else
    //            v7 = 0;
    //        bn_TSGrowableArray_FFX_Pass_New(&this->unk_0008, &v7);
    //        if (SMemAlloc(60, ".\\FFXEffects.cpp", 1034, 0)) {
    //            v7 = PassDeath::PassDeath(&v5, 2, &dword_D45784, 0, 1);
    //            bn_TSGrowableArray_FFX_Pass_New(&this->unk_0008, &v7);
    //            return this;
    //        }
    //        v7 = 0;
    //        bn_TSGrowableArray_FFX_Pass_New(&this->unk_0008, &v7);
    //    }
    //    return this;
    //}
}
