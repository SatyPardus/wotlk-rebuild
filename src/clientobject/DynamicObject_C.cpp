#include "clientobject/DynamicObject_C.hpp"

CGDynamicObject_C::CGDynamicObject_C() {
}

CGDynamicObject_C::CGDynamicObject_C(CClientObjCreate& objCreate, uint32_t time)
    : CGObject_C(objCreate, time) {
    
}

// OFFSET: 0x705230
void CGDynamicObject_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    //maybe_CPassenger__PostInit(a3 + 40);
    //if (!(this->GetTransportGUID)(this))
    //    this[1].hashObject.m_linktofull.m_next = *(a3 + 52);
    this->CGObject_C::PostInit(time, objCreate, isUpdate3);
    //IsSpellInTransit = bn_Spell_C_IsSpellInTransit(this[1].Destructor, this[1].Disable, this[1].PostReenable, this[1].UpdateWorldObject);
    //v6 = this->__vftable;
    //this[1].ukn_0060[5].m_linkoffset ^= (this[1].ukn_0060[5].m_linkoffset ^ (2 * IsSpellInTransit)) & 2;
    //if (v6->GetObjectModel(this)) {
    //    m_obj = this->m_obj;
    //    v22 = *&m_obj->OBJECT_FIELD_GUID.guid_high;
    //    v20 = *&m_obj->OBJECT_FIELD_GUID.guid_low;
    //    v8 = this->GetObjectModel(this);
    //    CWorldScene::LoadModel(v8, COERCE_FLOAT(bn_AnimEventCallback_0), v20, v22);
    //    v9 = this->m_obj;
    //    guid_high = v9->OBJECT_FIELD_GUID.guid_high;
    //    guid_low = v9->OBJECT_FIELD_GUID.guid_low;
    //    v10 = this->GetObjectModel(this);
    //    CM2Model::SetSequenceCallback(v10, bn_AnimFinishedCallback, guid_low, guid_high);
    //    if ((this[1].ukn_0060[5].m_linkoffset & 2) != 0) {
    //        v11 = this->GetObjectModel(this);
    //        CM2Model::SetEmittersEnabled(v11, 0);
    //        v12 = this->GetObjectModel(this);
    //        CM2Model::SetRibbonsEnabled(v12, 0);
    //    }
    //}
    //maybe_CGDynamicObject_C__ObjectVisKitProc(this);
    //v13 = this[1].__vftable;
    //if (LOBYTE(v13->Reenable) == 2) {
    //    Destructor = v13->Destructor;
    //    Disable = v13->Disable;
    //    if (__PAIR64__(Disable, Destructor) == ClntObjMgrGetActivePlayer()) {
    //        ActivePlayer = ClntObjMgrGetActivePlayer();
    //        v17 = ClntObjMgrObjectPtr(ActivePlayer, TYPEMASK_PLAYER);
    //        if (v17) {
    //            v18 = this->m_obj;
    //            v19 = v18->OBJECT_FIELD_GUID.guid_low;
    //            v24 = v18->OBJECT_FIELD_GUID.guid_high;
    //            if (__PAIR64__(v24, v19) == CGPlayer_C::GetFarSightGuid(v17))
    //                bn_CGPlayer_C_SetFarSightFocus(this);
    //        }
    //    }
    //}
}

// OFFSET: 0x70CBA0
void CGDynamicObject_C::SetStorage(CGDynamicObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_dynamicObject = reinterpret_cast<CGDynamicObjectData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_dynamicObjectMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
