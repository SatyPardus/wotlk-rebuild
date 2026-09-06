#include "clientobject/CGObject_C.hpp"
#include "clientobject/ObjectMgrClient.hpp"
#include <world/CWorldScene.hpp>
#include <model/CM2Scene.hpp>
#include <world/map/CMap.hpp>
#include <gameui/CGWorldFrame.hpp>
#include "model/CM2Shared.hpp"

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
        //v6 |= 0x10u;
        //if ((this->m_obj->m_type & 0x10) != 0)
        //    v6 |= 0x20u;
    }
    CM2Model* v7 = this->GetObjectModel();
    this->m_worldObject = CMap::ObjectCreate(v7, CGWorldFrame::ObjectEnumProc, nullptr, (uint64_t)this->m_obj->m_guid, 0, v6);
    if ((this->m_modelFlags & 0x40000) != 0 && (this->m_modelFlags & 0x20000) == 0)
        this->UpdateWorldObject(0);
}

// OFFSET: 0x743680
void CGObject_C::SetModelFinish(CM2Model* model) {
    //m_model = this->ukn_00A8;
    //if (m_model) {
    //    do {
    //        ukn48 = m_model->ukn48;
    //        bn_CEffect_DetachFromParent(m_model);
    //        if (CM2Model::IsLoaded(this->m_worldModel, 0, 0))
    //            CEffect::UpdateAttachment(m_model);
    //        m_model = ukn48;
    //    } while (ukn48);
    //}
    if (model) {
        model->SetLoadedCallback(nullptr, nullptr);
        //if (a2->ukn19)
        //    CM2Model::DetachFromParent(a2);
        model->Release();
    }

    if (this->m_worldModel)
        this->m_worldModel->SetLoadedCallback(CGObject_C::ModelLoadedCallback, this);

    //if (this->m_worldObject && this->m_worldModel == this->GetObjectModel()) {
    //    World::ObjectSetModel(this->m_worldObject, this->m_worldModel);
    //}
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

// OFFSET: 0x743BA0
void CGObject_C::SetData(uint32_t offset, uint32_t value) {
    reinterpret_cast<uint32_t*>(this->m_obj)[offset] = value;
}

// OFFSET: 0x744A50
void CGObject_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    //ukn57 = this->ukn57;
    this->m_modelFlags |= 0x40000u;
    //v8 = ukn57() != 0 ? 0x3E8 : 0;
    //alpha = (this->ukn58)(this);
    //CGObject_C::DoFade(this, alpha, v8);
    //(this->ukn20)();
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

// OFFSET: 0x744DB0
void CGObject_C::Reenable() {
    //v2 = this->__vftable;
    //this->ukn_00BC = this->ukn_00BC & 0xFFFCFFFF | 0x20000;
    //*&this->ukn_0090[2] = (v2->GetScale)();
    //SetFrameOfReference = this->__vftable[1].SetFrameOfReference;
    //this->ukn_0090[4] = 0;
    //v5 = (SetFrameOfReference)(this) != 0 ? 1000 : 0;
    //alpha = (this->__vftable[1].ukn19)(this);
    //CGObject_C::DoFade(this, alpha, v5);
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

// OFFSET: 0x4899F0
bool CGObject_C::GetModelFileName(const char** fileName) {
    *fileName = nullptr;
    return false;
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
    //ukn_00A8 = this->ukn_00A8;
    //if (ukn_00A8) {
    //    do {
    //        v4 = *(ukn_00A8 + 264);
    //        CEffect::UpdateAttachment(ukn_00A8);
    //        ukn_00A8 = v4;
    //    } while (v4);
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

// OFFSET: 0x4D5FE0
CM2Model* CGObject_C::GetObjectModel() {
    return this->m_worldModel;
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
