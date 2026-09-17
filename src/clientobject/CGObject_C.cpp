#include "clientobject/CGObject_C.hpp"
#include "clientobject/ObjectMgrClient.hpp"
#include <world/CWorldScene.hpp>
#include <model/CM2Scene.hpp>
#include <world/map/CMap.hpp>
#include <gameui/CGWorldFrame.hpp>
#include <gameui/CGGameUI.hpp>
#include "model/CM2Shared.hpp"
#include <client/FrameTime.hpp>

static const uint32_t s_objectMirrorIndex[CGObject::TotalFields()] = {
    3,
    4,
    5,
};

// OFFSET: 0x4F4A20
uint32_t CGObject::MirrorIndexFromFieldIndex(uint32_t fieldIndex) {
    for (uint32_t i = 0; i < CGObject::TotalFields(); i++) {
        if (s_objectMirrorIndex[i] == fieldIndex) {
            return i;
        }
    }

    return CGObject::GetFieldCount();
}

CGObject_C::CGObject_C() {
    
}

CGObject_C::CGObject_C(CClientObjCreate& objCreate, uint32_t time) {
    //a1->hashObject.m_linktoslot.m_prevlink = 0;
    //a1->hashObject.m_linktoslot.m_next = 0;
    //a1->hashObject.m_linktofull.m_prevlink = 0;
    //a1->hashObject.m_linktofull.m_next = 0;
    //a1->hashObject.m_key.m_guid = 0i64;
    //a1->__vftable = off_9F3A70;
    //a1->ukn_0054 = 0;
    //a1->ukn_0058 = 0;
    //a1->ukn_0060[0].m_terminator.m_next = 0;
    //a1->ukn_0060[0].m_linkoffset = 0;
    //p_m_terminator = &a1->ukn_0060[0].m_terminator;
    //p_m_terminator->m_prevlink = p_m_terminator;
    //a1->ukn_0060[0].m_terminator.m_next = (p_m_terminator | 1);
    //a1->ukn_0060[1].m_terminator.m_next = 0;
    //a1->ukn_0060[1].m_linkoffset = 0;
    //a1->ukn_0060[1].m_terminator.m_prevlink = &a1->ukn_0060[1].m_terminator;
    //a1->ukn_0060[1].m_terminator.m_next = (&a1->ukn_0060[1].m_terminator | 1);
    //a1->ukn_0060[2].m_terminator.m_next = 0;
    //a1->ukn_0060[2].m_linkoffset = 0;
    //a1->ukn_0060[2].m_terminator.m_prevlink = &a1->ukn_0060[2].m_terminator;
    //a1->ukn_0060[2].m_terminator.m_next = (&a1->ukn_0060[2].m_terminator | 1);
    //a1->ukn_0060[3].m_terminator.m_next = 0;
    //a1->ukn_0060[3].m_linkoffset = 0;
    //a1->ukn_0060[3].m_terminator.m_prevlink = &a1->ukn_0060[3].m_terminator;
    //a1->ukn_0060[3].m_terminator.m_next = (&a1->ukn_0060[3].m_terminator | 1);
    //a1->ukn_0060[4].m_terminator.m_next = 0;
    //a1->ukn_0060[4].m_linkoffset = 0;
    //a1->ukn_0060[4].m_terminator.m_prevlink = &a1->ukn_0060[4].m_terminator;
    //a1->ukn_0060[4].m_terminator.m_next = (&a1->ukn_0060[4].m_terminator | 1);
    //a1->ukn_0060[5].m_terminator.m_next = 0;
    //a1->ukn_0060[5].m_linkoffset = 0;
    //a1->ukn_0060[5].m_terminator.m_prevlink = &a1->ukn_0060[5].m_terminator;
    //a1->ukn_0060[5].m_terminator.m_next = (&a1->ukn_0060[5].m_terminator | 1);
    this->m_scale = 1.0;
    this->unk_009C = 1.0;
    //*&a1->ukn_00A4 = 1.0;
    //this->m_model = 0;
    this->m_height = 1.0;
    //a1->ukn_0090 = 0;
    //a1->ukn_0094 = 0;
    //a1->ukn_00A0 = 0;
    //a1->ukn_00A8 = 0;
    //a1->ukn_00B0 = 0;
    this->m_worldModel = 0;
    this->m_worldObject = 0;
    this->m_modelFlags = 0;
    //a1->ukn_00C0 = 0;
    //a1->ukn_00C4 = 0;
    //a1->ukn_00C8 = 0xFF000000;
    //*(&a1->ukn_00C8 + 1) = 0;
    ClntObjMgrLinkInNewObject(this);
    this->m_scale = this->m_obj->m_scale;
}

void CGObject_C::SetTypeID(OBJECT_TYPE_ID typeID) {
    this->m_typeID = typeID;

    switch (typeID) {
    case ID_OBJECT:
        this->m_obj->m_type = HIER_TYPE_OBJECT;
        break;

    case ID_ITEM:
        this->m_obj->m_type = HIER_TYPE_ITEM;
        break;

    case ID_CONTAINER:
        this->m_obj->m_type = HIER_TYPE_CONTAINER;
        break;

    case ID_UNIT:
        this->m_obj->m_type = HIER_TYPE_UNIT;
        break;

    case ID_PLAYER:
        this->m_obj->m_type = HIER_TYPE_PLAYER;
        break;

    case ID_GAMEOBJECT:
        this->m_obj->m_type = HIER_TYPE_GAMEOBJECT;
        break;

    case ID_DYNAMICOBJECT:
        this->m_obj->m_type = HIER_TYPE_DYNAMICOBJECT;
        break;

    case ID_CORPSE:
        this->m_obj->m_type = HIER_TYPE_CORPSE;
        break;

    default:
        break;
    }
}

// OFFSET: 0x743760
void CGObject_C::AddWorldObject() {
    const char* modelFileName;
    if (!this->m_worldModel && this->GetModelFileName(&modelFileName)) {
        CM2Model* model = CWorldScene::s_m2Scene->CreateModel(modelFileName, 0);
        if (model != this->m_worldModel) {
            if (model)
                model->m_refCount++;
            CM2Model* prevModel = this->m_worldModel;
            this->m_worldModel = model;
            this->SetModelFinish(prevModel);
        }
        model->Release();
    }

    if (!this->m_worldModel)
        return;
    //if (!ClntObjMgrGetPlayerType())
    //    return;

    if (this->m_worldObject) {
        //SysMsgPrintf_0(1, 2, "OBJECTALREADYACTIVE|0x%016I64X", *&this->m_obj->OBJECT_FIELD_GUID);
        return;
    }

    uint32_t v6 = 0;
    if ((this->m_obj->m_type & OBJECT_TYPE::TYPE_GAMEOBJECT) != 0) {
        v6 = 11;
    } else if ((this->m_obj->m_type & OBJECT_TYPE::TYPE_DYNAMICOBJECT) != 0) {
        v6 = 10;
    } else if ((this->m_obj->m_type & OBJECT_TYPE::TYPE_CORPSE) != 0) {
        //if ((this[1].ukn27 & 1) != 0)
        //    v6 = 2;
    } else if ((this->m_obj->m_type & OBJECT_TYPE::TYPE_UNIT) != 0) {
        //if (CGUnit_C::IsLinkAll(this))
        //    v6 = 8;
        //if (CGUnit_C::HasNoShadowBlob(this))
        //    v6 |= 2u;
        v6 |= 0x10u;
        if ((this->m_obj->m_type & 0x10) != 0)
            v6 |= 0x20u;
    }
    CM2Model* v7 = this->GetObjectModel();
    this->m_worldObject = CMap::ObjectCreate(v7, CGWorldFrame::ObjectEnumProc, nullptr, (uint64_t)this->m_obj->m_guid, 0, v6);
    if ((this->m_modelFlags & 0x40000) != 0 && (this->m_modelFlags & 0x20000) == 0)
        this->UpdateWorldObject(0);
}

// OFFSET: 0x743680
void CGObject_C::SetModelFinish(CM2Model* model) {
    //m_effectList = this->m_effectList;
    //if (m_effectList) {
    //    do {
    //        v4 = *(m_effectList + 264);
    //        bn_CEffect_DetachFromParent(m_effectList);
    //        if (CM2Model::IsLoaded(this->m_worldModel, 0, 0))
    //            CEffect::UpdateAttachment(m_effectList);
    //        m_effectList = v4;
    //    } while (v4);
    //}
    if (model) {
        model->SetLoadedCallback(nullptr, nullptr);
        if (model->m_attachParent)
            model->DetachFromParent();
        model->Release();
    }

    if (this->m_worldModel)
        this->m_worldModel->SetLoadedCallback(CGObject_C::ModelLoadedCallback, this);

    if (this->m_worldObject && this->m_worldModel == this->GetObjectModel()) {
        World::ObjectSetModel(this->m_worldObject, this->m_worldModel);
    }
}

// OFFSET: 0x744230
void CGObject_C::ModelChanged() {
    CM2Model* model = this->GetObjectModel();
    bool isLoaded = model->IsLoaded(0, 1);
    CAaBox boundingBox = model->GetBoundingBox();
    float height = 0.0f;
    if (boundingBox.t.x > boundingBox.b.x && boundingBox.t.y > boundingBox.b.y && boundingBox.t.z > boundingBox.b.z)
        height = boundingBox.t.z - boundingBox.b.z;
    this->m_height = height;
    if ((this->m_modelFlags & 0x10000) == 0)
        this->UpdateWorldObject(false);
    if (isLoaded)
        this->m_modelFlags &= ~0x200000;
    else
        this->m_modelFlags |= 0x200000;
}

// OFFSET: 0x743450
bool CGObject_C::IsReadyToDraw() {
    CM2Model* model = this->GetObjectModel();
    if (model && model->IsDrawable(0, 0)) {
        return true;
    }
    return false;
}

// OFFSET: 0x743420
void CGObject_C::SetDisablePending(bool pending) {
    if (pending)
        this->m_modelFlags |= 0x100000u;
    else
        this->m_modelFlags &= ~0x100000u;
}

// OFFSET: 0x7433D0
bool CGObject_C::IsObjectLocked() {
    return (this->m_modelFlags & 0xFFFF) != 0;
}

// OFFSET: 0x743BA0
void CGObject_C::SetData(uint32_t offset, uint32_t value) {
    reinterpret_cast<uint32_t*>(this->m_obj)[offset] = value;
}

// OFFSET: 0x744A50
void CGObject_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    this->m_modelFlags |= 0x40000u;
    //this->DoFade(this->GetBaseAlpha(), this->ShouldFadeIn() != 0 ? 1000 : 0);
    this->RefreshInteractIcon();
    //this->ukn_00A0 = 0;
    if (this->m_worldObject)
        this->UpdateWorldObject(0);
}

CGUnit_C* CGObject_C::AsUnit() {
    STORM_ASSERT(this->m_obj->m_type & TYPEMASK_UNIT);
    return reinterpret_cast<CGUnit_C*>(this);
}

CGPlayer_C* CGObject_C::AsPlayer() {
    STORM_ASSERT(this->m_obj->m_type & TYPEMASK_PLAYER);
    return reinterpret_cast<CGPlayer_C*>(this);
}

CGItem_C* CGObject_C::AsItem() {
    STORM_ASSERT(this->m_obj->m_type & TYPEMASK_ITEM);
    return reinterpret_cast<CGItem_C*>(this);
}

CGContainer_C* CGObject_C::AsContainer() {
    STORM_ASSERT(this->m_obj->m_type & TYPEMASK_CONTAINER);
    return reinterpret_cast<CGContainer_C*>(this);
}

CGGameObject_C* CGObject_C::AsGameObject() {
    STORM_ASSERT(this->m_obj->m_type & TYPEMASK_GAMEOBJECT);
    return reinterpret_cast<CGGameObject_C*>(this);
}

CGDynamicObject_C* CGObject_C::AsDynamicObject() {
    STORM_ASSERT(this->m_obj->m_type & TYPEMASK_DYNAMICOBJECT);
    return reinterpret_cast<CGDynamicObject_C*>(this);
}

CGCorpse_C* CGObject_C::AsCorpse() {
    STORM_ASSERT(this->m_obj->m_type & TYPEMASK_CORPSE);
    return reinterpret_cast<CGCorpse_C*>(this);
}


// OFFSET: 0x744D20
void CGObject_C::Disable() {
    //ActiveCamera = CGWorldFrame::GetActiveCamera();
    //if (ActiveCamera) {
    //    m_obj = this->m_obj;
    //    if (ActiveCamera->m_relativeTo.guid_low == m_obj->m_guid.guid_low && ActiveCamera->m_relativeTo.guid_high == m_obj->m_guid.guid_high)
    //        CGCamera::MakeRelativeTo(ActiveCamera, 0.0);
    //}
    //this->Fadeout();
    if (this->m_model) {
        if (this->m_model->m_attachParent)
            this->m_model->DetachFromParent();
        this->m_model->Release();
        this->m_model = 0;
    }
    //this->ukn_00C4 = 0;
    //BYTE2(this->ukn_00C8) = 0;
    //LOBYTE(this->ukn_00C8) = 0;
    this->m_modelFlags = this->m_modelFlags & 0xF8FEFFFF | 0x10000;
    this->m_disableTime = FrameTime::s_curTimeMs;
}

// OFFSET: 0x744DB0
void CGObject_C::Reenable() {
    this->m_modelFlags = this->m_modelFlags & 0xFFFCFFFF | 0x20000;
    this->m_scale = this->GetScale();
    //this->ukn_00A0 = 0;
    //this->DoFade(this->GetBaseAlpha(), this->ShouldFadeIn() != 0 ? 1000 : 0);
}

// OFFSET: 0x743FF0
void CGObject_C::PostReenable() {
    this->RefreshInteractIcon();
    this->m_modelFlags &= ~0x20000u;
    if (!this->m_worldObject) {
        if (this->GetObjectModel())
            this->AddWorldObject();
    }
}

// OFFSET: 0x632050 (NOP)
void CGObject_C::HandleOutOfRange() {
    
}

// OFFSET: 0x7438E0
void CGObject_C::UpdateWorldObject(bool a2) {
    if (!this->m_worldObject)
        return;

    C44Matrix mat;
    C3Vector pos;
    this->GetPosition(pos);
    float facing = this->GetFacing();
    float scale = this->GetTrueScale();

    mat.Translate(pos);
    mat.RotateAroundZ(facing);
    mat.Scale(scale);

    C3Vector vec;
    CAaBox box;
    CAaSphere sphere;

    if (this->m_worldModel && this->m_worldModel->IsLoaded(0, 0)) {
        if (!this->m_worldModel->m_shared->m_m2DataLoaded)
            this->m_worldModel->WaitForLoad(nullptr);

        M2Bounds* collisionBounds = &this->m_worldModel->m_shared->m_data->collisionBounds;
        vec.x = (collisionBounds->extent.t.x + collisionBounds->extent.b.x) * 0.5;
        vec.y = (collisionBounds->extent.t.y + collisionBounds->extent.b.y) * 0.5;
        vec.z = (collisionBounds->extent.t.z + collisionBounds->extent.b.z) * 0.5;
        box = this->m_worldModel->GetBoundingBox();
        sphere = this->m_worldModel->GetBoundingSphere();
    }
    CMap::ObjectUpdate(this->m_worldObject, mat, box, sphere, vec, a2, 0xFFFFFFFF);
}

// OFFSET: 0x7451B0
void CGObject_C::GetNamePosition(C3Vector& pos) {
    if (this->m_worldModel->HasAttachment(18)) {
        pos = this->m_worldModel->GetAttachmentPosition(18);
    } else {
        this->GetPosition(pos);
        pos.z += this->m_height * this->m_scale * 1.25f;
    }
}

// OFFSET: 0x4D5EA0
void CGObject_C::GetPosition(C3Vector& pos) {
    pos = C3Vector();
}

// OFFSET: 0x4D5EC0
void CGObject_C::GetRawPosition(C3Vector& pos) {
    this->GetPosition(pos);
}

// OFFSET: 0x4D5EE0
float CGObject_C::GetFacing() {
    return 0.0f;
}

// OFFSET: 0x4D5EF0
float CGObject_C::GetRawFacing() {
    return this->GetFacing();
}

// OFFSET: 0x4D5F00
float CGObject_C::GetScale() {
    return this->m_obj->m_scale;
}

// OFFSET: 0x4D5F10
WGUID CGObject_C::GetTransportGUID() {
    return 0;
}

// OFFSET: 0x5EEB70 (NOP)
void CGObject_C::RefreshInteractIcon() {

}

// OFFSET: 0x4899F0
bool CGObject_C::GetModelFileName(const char** fileName) {
    *fileName = nullptr;
    return false;
}

// OFFSET: 0x4D5F70
bool CGObject_C::GetSelectionHighlightColor(CImVector& color) {
    color = { 0xFF, 0xFF, 0xFF, 0xFF };
    return true;
}

// OFFSET: 0x4D5F90
float CGObject_C::GetTrueScale() {
    return this->unk_009C * this->m_scale;
}

// OFFSET: 0x7442E0
void CGObject_C::ModelLoaded(CM2Model* model) {
    if (model != this->GetObjectModel())
        return;

    this->ModelChanged();
    //m_effectList = this->m_effectList;
    //if (m_effectList) {
    //    do {
    //        v4 = *(m_effectList + 264);
    //        CEffect::UpdateAttachment(m_effectList);
    //        m_effectList = v4;
    //    } while (v4);
    //}
}

// OFFSET: 0x743EC0
void CGObject_C::PreAnimate(CGWorldFrame* worldFrame) {
    //if (CGGameUI::m_lockedTarget == *&this->m_obj->m_guid) {
    //    WGUID activePlayerGUID = ClntObjMgrGetActivePlayer();
    //    if (this->m_obj->m_guid != activePlayerGUID) {
    //        if (s_cvObjectSelectionCircle->m_intValue) {
    //            worldFrame->m_trackedEffectGuidA = this->m_obj->m_guid;
    //        }
    //    }
    //}
    //if (maybe_CGPetInfo__GetTarget() == this->m_obj->m_guid) {
    //    WGUID activePlayerGUID = ClntObjMgrGetActivePlayer();
    //    if (this->m_obj->m_guid != activePlayerGUID) {
    //        if (s_cvObjectSelectionCircle->m_intValue) {
    //            worldFrame->m_trackedEffectGuidB = this->m_obj->m_guid;
    //        }
    //    }
    //}
    //this->ApplyAlpha(FrameTime::s_curTimeMs);
    //if (this->ukn_00A0 && FrameTime::s_curTimeMs > this->ukn_00A0) {
    //    auto v20 = (FrameTime::s_curTimeMs - this->ukn_00A0);
    //    if (v20 >= 2000) {
    //        this->m_scale = this->m_obj->m_scale;
    //        this->ukn_00A0 = 0;
    //        this->ukn26();
    //    } else {
    //        this->m_scale = (this->GetScale() - this->ukn_00A4) * (-cos(v20 * 0.00050000002 * 3.1415927) * 0.5 + 0.5) + this->ukn_00A4;
    //        this->ukn25();
    //    }
    //}
}

// OFFSET: 0x743330
bool CGObject_C::Animate(float a2) {
    CM2Model* model = this->GetObjectModel();
    if (!model)
        return true;

    float scale = this->GetTrueScale();
    float facing = this->GetRenderFacing();
    C3Vector position;
    this->GetPosition(position);
    model->SetWorldTransform(position, facing, scale);
    return true;
}

void CGObject_C::ShouldRender(uint32_t flags, uint32_t* culled, uint32_t* out) {
    // 0x1 = should bypass cull?
    if ((flags & 1) == 0)
        *culled = 1;
}

// OFFSET: 0x4D5EF0
float CGObject_C::GetRenderFacing() {
    return this->GetRawFacing();
}

// OFFSET: 0x8E5250
bool CGObject_C::CanHighlight() {
    return false;
}

// OFFSET: 0x8E5250
bool CGObject_C::CanBeTargetted() {
    return false;
}

// OFFSET: 0x4D5FA0
void CGObject_C::GetMatrix(C44Matrix& mat) {
    mat.a0 = 1.0;
    mat.a1 = 0.0;
    mat.a2 = 0.0;
    mat.a3 = 0.0;
    mat.b0 = 0.0;
    mat.b2 = 0.0;
    mat.b3 = 0.0;
    mat.c0 = 0.0;
    mat.c1 = 0.0;
    mat.c3 = 0.0;
    mat.d0 = 0.0;
    mat.d1 = 0.0;
    mat.d2 = 0.0;
    mat.b1 = 1.0;
    mat.c2 = 1.0;
    mat.d3 = 1.0;
}

// OFFSET: 0x7434E0
uint32_t CGObject_C::UpdateObjectNameString(uint32_t mask, char* text, uint32_t textSize) {
    char* name = this->GetObjectName();
    if (name) {
        SStrPrintf(text, textSize, "%s", name);
    } else {
        *text = 0;
    }
    return 1;
}

// OFFSET: 0x743530
bool CGObject_C::ShouldRenderObjectName(uint32_t mask) {
    if (this->m_model)
        return 0;
    return (mask >> 11) & 1;
}

// OFFSET: 0x4D5FE0
CM2Model* CGObject_C::GetObjectModel() {
    return this->m_worldModel;
}

// OFFSET: 0x8E5250
char* CGObject_C::GetObjectName() {
    return nullptr;
}

// OFFSET: 0x427A90
bool CGObject_C::IsTransport() {
    return false;
}

// OFFSET: 0x743640
void CGObject_C::SetStorage(CGObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    obj->m_obj = reinterpret_cast<CGObjectData*>(descriptorPtr);
    obj->m_objMirror = reinterpret_cast<void*>(mirrorPtr);
}

// OFFSET: 0x743110
void CGObject_C::ModelLoadedCallback(CM2Model* model, void* arg) {
    CGObject_C* obj = reinterpret_cast<CGObject_C*>(arg);
    if (obj)
        obj->ModelLoaded(model);
}
