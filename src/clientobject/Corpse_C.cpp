#include "clientobject/Corpse_C.hpp"

static const uint32_t s_corpseMirrorIndex[CGCorpse::TotalFields() - CGObject::TotalFields()] = {
    27,
    28,
    30,
};

// OFFSET: 0x4F5690
uint32_t CGCorpse::MirrorIndexFromFieldIndex(uint32_t fieldIndex) {
    for (uint32_t i = 0; i < CGCorpse::TotalFields() - CGObject::TotalFields(); i++) {
        if (s_corpseMirrorIndex[i] == fieldIndex) {
            return i;
        }
    }

    return CGCorpse::GetFieldCount();
}

// OFFSET: 0x4D4380
uint32_t CGCorpse::DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset) {
    uint32_t mirrorIndex = CGCorpse::MirrorIndexFromFieldIndex((fieldByteOffset - baseByteOffset) >> 2);
    return (fieldByteOffset & 3) + 4 * (CGObject::TotalFields() + mirrorIndex);
}

CGCorpse_C::CGCorpse_C() {
}

CGCorpse_C::CGCorpse_C(CClientObjCreate& objCreate, uint32_t time) : CGObject_C(objCreate, time) {
    
}

// OFFSET: 0x705B20
void CGCorpse_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    //v4 = LODWORD(arg4);
    //maybe_CPassenger__PostInit(LODWORD(arg4) + 40);
    //if (!(*(*this + 64))(this))
    //    this[62] = *(v4 + 52);
    this->CGObject_C::PostInit(time, objCreate, isUpdate3);
    //v6 = *(this + 52);
    //v7 = *(v6 + 16);
    //v8 = *(this + 45);
    //LODWORD(arg4) = 1;
    //if (v7 < g_CreatureDisplayInfoDB.minIndex || v7 > g_CreatureDisplayInfoDB.maxIndex)
    //    v9 = 0;
    //else
    //    v9 = g_CreatureDisplayInfoDB.Rows[v7 - g_CreatureDisplayInfoDB.minIndex];
    //v10 = 0;
    //if (v9) {
    //    v11 = *(v9 + 4);
    //    if (v11 < g_CreatureModelDataDB.minIndex || v11 > g_CreatureModelDataDB.maxIndex) {
    //        v10 = 0;
    //    } else {
    //        v10 = g_CreatureModelDataDB.Rows[v11 - g_CreatureModelDataDB.minIndex];
    //        if (v10 && (*(v10 + 4) & 4) == 0)
    //            arg4 = 0.0;
    //    }
    //}
    //if ((*(v6 + 108) & 1) == 0) {
    //    if (arg4 == 0.0) {
    //        maybe_CCharacterComponent__ReplaceMonsterSkin(v8, v9, v10);
    //    } else {
    //        *(this + 156) = CCharacterComponent::AllocComponent();
    //        ComponentData::ComponentData(&v32);
    //        v12 = *(this + 52);
    //        v32.m_preferences.raceID = *(v12 + 97);
    //        v32.m_preferences.sexID = *(v12 + 98);
    //        v32.m_preferences.skinID = *(v12 + 99);
    //        v32.m_preferences.faceId = *(v12 + 100);
    //        v32.m_preferences.hairStyleId = *(v12 + 101);
    //        v32.m_preferences.hairColorId = *(v12 + 102);
    //        v32.m_preferences.facialHairId = *(v12 + 103);
    //        v32.m_model = v8;
    //        v13 = *v12;
    //        v35 = v12[1];
    //        v14 = __PAIR64__(v35, v13) == ClntObjMgrGetActivePlayer();
    //        ++v8->m_refCount;
    //        v15 = *(this + 156);
    //        v32.m_flags ^= (LOBYTE(v32.m_flags) ^ (2 * v14)) & 2;
    //        CCharacterComponent::Init(v15, &v32, 0);
    //    }
    //}
    //v16 = *(this + 46);
    //v31 = fmt;
    //a4 = (*(*this + 44))(this);
    //if (World::QueryObjectLiquid(v16, &v35, &arg4, &a2) && arg4 - *(a4 + 8) > 0.66666669) {
    //    *(this + 160) |= 2u;
    //    CM2Model::SetBoneSequence(v8, 0xFFFFFFFF, 132, -1, 0, 1.0, 1, 1);
    //} else {
    //    *(this + 160) &= ~2u;
    //    CM2Model::SetBoneSequence(v8, 0xFFFFFFFF, 6, -1, 0, 1.0, 1, 1);
    //}
    //if (*(this + 156)) {
    //    v17 = 0.0;
    //    arg4 = 0.0;
    //    do {
    //        WowClientDB::GetRow(&a3);
    //        v18 = *(*(this + 52) + 4 * LODWORD(v17) + 20) & 0xFFFFFF;
    //        if (v18 >= bnl_g_itemDisplayInfoDB.minIndex && v18 <= bnl_g_itemDisplayInfoDB.maxIndex) {
    //            v19 = v18 - bnl_g_itemDisplayInfoDB.minIndex;
    //            v20 = bnl_g_itemDisplayInfoDB.Rows[v19] == 0;
    //            v21 = &bnl_g_itemDisplayInfoDB.Rows[v19];
    //            if (!v20) {
    //                if (g_clientDbPerformCompress) {
    //                    ClientDB::DecompressRow(*v21, 100, &a3);
    //                } else {
    //                    qmemcpy(&a3, *v21, sizeof(a3));
    //                    v17 = arg4;
    //                }
    //                if (LODWORD(v17) != 17) {
    //                    if (LODWORD(v17) == 15 || LODWORD(v17) == 16) {
    //                        v22 = ClntObjMgrObjectPtr(*(*(this + 52) + 4 * LODWORD(v17) + 20), TYPEMASK_ITEM);
    //                        v23 = v22;
    //                        if (v22) {
    //                            arg4 = this[45];
    //                            v30 = CGItem_C::GetInventoryType(v22) == 14;
    //                            SheatheType = bn_CGItem_C_GetSheatheType(v23);
    //                            CCharacterComponent::AddHandItem(LODWORD(arg4), &a3, LODWORD(v17), SheatheType, 0, v30, 0, 0);
    //                        }
    //                        goto LABEL_43;
    //                    }
    //                    if (v17 == 0.0) {
    //                        if ((*(*(this + 52) + 108) & 8) == 0)
    //                            goto LABEL_38;
    //                    } else if (LODWORD(v17) != 14 || (*(*(this + 52) + 108) & 0x10) == 0) {
//LABEL_38:
    //                        CCharacterComponent::AddItemBySlot(*(this + 156), SLODWORD(v17), a3.m_ID, 0);
    //                        if (LODWORD(v17) == 18 && (a3.m_flags & 1) != 0)
    //                            maybe_CGCorpse_C__ApplyGuildColor(1);
    //                    }
    //                }
    //            }
    //        }
//LABEL_43:
    //        NOP(v31);
    //        ++LODWORD(v17);
    //        arg4 = v17;
    //    } while (LODWORD(v17) < 0x13);
    //}
    //if ((*(*(this + 52) + 112) & 1) != 0 && !*(this + 162)) {
    //    SpecialSpellVisualEffectNameRec = bn_GetSpecialSpellVisualEffectNameRec(4);
    //    Model = CM2Scene::CreateModel(*(*(this + 45) + 40), *(SpecialSpellVisualEffectNameRec + 8), 0);
    //    *(this + 162) = Model;
    //    if (Model)
    //        CM2Model::AttachToParent(Model, *(this + 45), 0x13u, 0, 0);
    //}
    //v27 = *(this + 52);
    //if ((v27[27] & 1) == 0) {
    //    v28 = *v27;
    //    v29 = v27[1];
    //    if (__PAIR64__(v29, v28) == ClntObjMgrGetActivePlayer())
    //        bn_CGGameUI_SetActiveCorpse(**(this + 2), *(*(this + 2) + 4));
    //}
}

// OFFSET: 0x70CBA0
void CGCorpse_C::SetStorage(CGCorpse_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_corpse = reinterpret_cast<CGCorpseData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_corpseMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
