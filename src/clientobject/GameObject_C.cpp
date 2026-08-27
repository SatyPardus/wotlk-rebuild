#include "clientobject/GameObject_C.hpp"

CGGameObject_C::CGGameObject_C() {
}

CGGameObject_C::CGGameObject_C(CClientObjCreate& objCreate, uint32_t time)
    : CGObject_C(objCreate, time) {
    
}

// OFFSET: 0x712F30
void CGGameObject_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    //maybe_CPassenger__PostInit(a3 + 40);
    this->CGObject_C::PostInit(time, objCreate, isUpdate3);
    //m_worldModel = this->m_worldModel;
    //if (m_worldModel) {
    //    ClntObjMgrShowObject(this->m_obj->OBJECT_FIELD_GUID.guid_low, this->m_obj->OBJECT_FIELD_GUID.guid_high);
    //    CWorldScene::LoadModel(m_worldModel, COERCE_FLOAT(maybe_CGGameObject_C__DispatchTypeUse), *&this->m_obj->OBJECT_FIELD_GUID.guid_low, *&this->m_obj->OBJECT_FIELD_GUID.guid_high);
    //    CM2Model::SetSequenceCallback(m_worldModel, maybe_CGGameObject_C__DispatchTypeToggle, this->m_obj->OBJECT_FIELD_GUID.guid_low, this->m_obj->OBJECT_FIELD_GUID.guid_high);
    //    if (CM2Model::IsLoaded(m_worldModel, 0, 0)) {
    //        v6 = this->m_worldModel;
    //        if ((v6->m_shared->m_flags & 1) == 0)
    //            CM2Model::WaitForLoad(v6, 0);
    //        m_data = v6->m_shared->m_data;
    //        z = m_data[15].z;
    //        m_data = (m_data + 188);
    //        v11.b.x = z;
    //        v11.b.y = m_data->y;
    //        v11.b.z = m_data->z;
    //        v11.t = m_data[1];
    //        CWorldMath::TransformAABox(&this[2].m_obj, &v11, &this[2].ukn_0060[0].m_terminator);
    //        if ((*(this[2].Destructor + 1))(this[2].__vftable))
    //            this[2].ukn_0060[3].m_terminator.m_prevlink = !CAaBox::IsEmpty(&this[2].ukn_0060[0].m_terminator);
    //    }
    //}
    //(*(this[2].Destructor + 29))(this[2].__vftable, a4);
    //m_obj = this->m_obj;
    //v12[0] = m_obj->OBJECT_FIELD_GUID.guid_low;
    //v12[1] = m_obj->OBJECT_FIELD_GUID.guid_high;
    //InfoBlockById = DbGameObjectCache_GetInfoBlockById(WDB_CACHE_GAMEOBJECT, m_obj->OBJECT_FIELD_ENTRY, v12, maybe_GameObjectStatsCallback, this, 0);
    //if (InfoBlockById)
    //    CGGameObject_C::LoadBaseObject(this, InfoBlockById);
}

// OFFSET: 0x70CBA0
void CGGameObject_C::SetStorage(CGGameObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_gameObject = reinterpret_cast<CGGameObjectData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_gameObjectMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
