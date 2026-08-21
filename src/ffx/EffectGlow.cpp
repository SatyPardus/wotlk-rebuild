#include "ffx/EffectGlow.hpp"
#include "console/CVar.hpp"

EffectGlow::EffectGlow()
    : FFX::Effect() {
    //v1 = this;
    //v24 = this;
    //FFX::Effect::Effect(this);
    //p_unk_001C = &v1->unk_001C;
    //v1->unk_0000 = &off_A941C8;
    //v1->unk_001C = 0;
    //v1->unk_0020 = 0;
    //v1->unk_0024 = 0;
    //v1->unk_0028 = 0;
    auto v3 = "1";
    if (!this->unk_0018)
        v3 = "0";
    this->m_cvar = CVar::Register("ffxGlow", "full screen glow effect", 1, v3, FFX::CVarCallback, 1, 0, 0, 0);
    //v22 = &dword_D457A0;
    //v21 = &dword_D45810;
    //v20[0] = &dword_D457A0;
    //v20[1] = &dword_D45810;
    //v19[0] = &dword_D45C70;
    //v19[1] = &dword_D457A0;
    //v19[2] = &dword_D45810;
    //if (SMemAlloc(60, ".\\EffectGlow.cpp", 23, 0))
    //    v23 = bn_PassBox4_constructor(&v22, 1, &dword_D45810, 0, 0);
    //else
    //    v23 = 0;
    //bn_TSGrowableArray_FFX_Pass_New(&v1->unk_0008, &v23);
    //if (SMemAlloc(60, ".\\EffectGlow.cpp", 24, 0))
    //    v23 = bn_PassGauss4_constructor(&v21, 1, &dword_D45810, 0, 0);
    //else
    //    v23 = 0;
    //bn_TSGrowableArray_FFX_Pass_New(&v1->unk_0008, &v23);
    //if (SMemAlloc(64, ".\\EffectGlow.cpp", 25, 0))
    //    v23 = bn_PassGlow_constructor(v20, 2, &dword_D45784, 0, 1);
    //else
    //    v23 = 0;
    //bn_TSGrowableArray_FFX_Pass_New(&v1->unk_0008, &v23);
    //if (SMemAlloc(60, ".\\EffectGlow.cpp", 27, 0))
    //    v23 = bn_PassBox4_constructor(&v22, 1, &dword_D45810, 0, 0);
    //else
    //    v23 = 0;
    //v4 = v1->unk_0020 + 1;
    //if (v4 > *p_unk_001C) {
    //    unk_0028 = v1->unk_0028;
    //    if (!unk_0028)
    //        unk_0028 = TSGrowableArray_4__CalcChunkSize(&v1->unk_001C, v1->unk_0020 + 1);
    //    if (v4 % unk_0028)
    //        v4 += unk_0028 - v4 % unk_0028;
    //    bn_TSGrowableArray_FFX_Pass_ReallocData(&v1->unk_001C, v4);
    //}
    //unk_0020 = v1->unk_0020;
    //v7 = (v1->unk_0024 + 4 * unk_0020);
    //v1->unk_0020 = unk_0020 + 1;
    //*v7 = v23;
    //if (SMemAlloc(60, ".\\EffectGlow.cpp", 28, 0))
    //    v23 = bn_PassGauss4_constructor(&v21, 1, &dword_D45810, 0, 0);
    //else
    //    v23 = 0;
    //v8 = v1->unk_0020 + 1;
    //if (v8 > *p_unk_001C) {
    //    v9 = v1->unk_0028;
    //    if (!v9)
    //        v9 = TSGrowableArray_4__CalcChunkSize(&v1->unk_001C, v1->unk_0020 + 1);
    //    if (v8 % v9)
    //        v8 += v9 - v8 % v9;
    //    bn_TSGrowableArray_FFX_Pass_ReallocData(&v1->unk_001C, v8);
    //    v1 = v24;
    //}
    //v10 = p_unk_001C[1];
    //v11 = (p_unk_001C[2] + 4 * v10);
    //p_unk_001C[1] = v10 + 1;
    //*v11 = v23;
    //if (byte_D45768 && CGxDevice::Caps(g_theGxDevicePtr)->m_texNonPow2) {
    //    if (SMemAlloc(64, ".\\EffectGlow.cpp", 30, 0)) {
    //        v12 = bn_PassGlow_constructor(v20, 2, &dword_D45784, 0, 1);
    //        *maybe_TSGrowableArray_FFXPass__New(p_unk_001C) = v12;
    //        result = v1;
    //        v1->unk_002C = 0;
    //    } else {
    //        *maybe_TSGrowableArray_FFXPass__New(p_unk_001C) = 0;
    //        v1->unk_002C = 0;
    //        return v1;
    //    }
    //} else {
    //    if (SMemAlloc(72, ".\\EffectGlow.cpp", 32, 0))
    //        v23 = maybe_PassGlowWave__PassGlowWave(v19, 3, &dword_D45784, 0, 1);
    //    else
    //        v23 = 0;
    //    v14 = p_unk_001C[1] + 1;
    //    if (v14 > *p_unk_001C) {
    //        v15 = p_unk_001C[3];
    //        if (!v15)
    //            v15 = TSGrowableArray_4__CalcChunkSize(p_unk_001C, p_unk_001C[1] + 1);
    //        if (v14 % v15)
    //            v14 += v15 - v14 % v15;
    //        bn_TSGrowableArray_FFX_Pass_ReallocData(p_unk_001C, v14);
    //        v1 = v24;
    //    }
    //    v16 = p_unk_001C[1];
    //    v17 = v23;
    //    v18 = (p_unk_001C[2] + 4 * v16);
    //    p_unk_001C[1] = v16 + 1;
    //    *v18 = v17;
    //    result = v1;
    //    v1->unk_002C = 0;
    //}
}
