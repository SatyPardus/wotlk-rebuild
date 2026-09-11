#ifndef CLIENTOBJECT_CGOBJECT_C_HPP
#define CLIENTOBJECT_CGOBJECT_C_HPP

#include <cstdint>
#include "clientobject/Types.hpp"
#include "clientobject/CHashKeyGUID.hpp"
#include "clientobject/CClientObjCreate.hpp"
#include "model/CM2Model.hpp"
#include "storm/Hash.hpp"
#include "world/map/CMapEntity.hpp"

class CGUnit_C;
class CGPlayer_C;
class CGWorldFrame;
class PLAYERNAMEDESC;

struct CGObjectData {
    WGUID m_guid;
    OBJECT_TYPE m_type;
    uint32_t m_entryID;
    float m_scale;
    uint32_t pad;
};

class CGObject {
    public:
    // uint32_t unk_0000;
    CGObjectData* m_obj;
    void* m_objMirror;
    uint32_t m_heapIndex;
    OBJECT_TYPE_ID m_typeID;

    // OFFSET: 0x4F4A10
    static uint32_t TotalFields() {
        return 3;
    };

    static uint32_t GetDataSize() {
        return sizeof(CGObjectData);
    }
};

class CGObject_C : public CGObject, public TSHashObject<CGObject_C, CHashKeyGUID> {
    public:
    // Member variables
    /* 0x0038 */ TSLink<CGObject_C> m_link;
    /* 0x0098 */ float m_scale = 1.0f;
    /* 0x009C */ float unk_009C = 1.0f;
    /* 0x00AC */ float m_height = 1.0f;
    /* 0x00B0 */ PLAYERNAMEDESC* m_nameDesc = nullptr;
    /* 0x00B4 */ CM2Model* m_worldModel = nullptr;
    /* 0x00B8 */ CMapEntity* m_worldObject = nullptr;
    /* 0x00BC */ uint32_t m_modelFlags = 0;

    // Member functions
    CGObject_C();
    CGObject_C(CClientObjCreate& objCreate, uint32_t time);

    void SetTypeID(OBJECT_TYPE_ID typeID);
    void AddWorldObject();
    void SetModelFinish(CM2Model* model);
    void ModelChanged();
    bool IsReadyToDraw();
    void SetData(uint32_t offset, uint32_t value);
    void PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3);

    CGUnit_C* AsUnit();
    CGPlayer_C* AsPlayer();

    // Virtual functions
    /* 02 */ virtual void Reenable();
    /* 05 */ virtual void UpdateWorldObject(bool a2);
    /* 08 */ virtual void GetNamePosition(C3Vector& pos);
    /* 11 */ virtual void GetPosition(C3Vector& pos);
    /* 12 */ virtual void GetRawPosition(C3Vector& pos);
    /* 13 */ virtual float GetFacing();
    /* 14 */ virtual float GetRawFacing();
    /* 15 */ virtual float GetScale();
    /* 16 */ virtual WGUID GetTransportGUID();
    /* 24 */ virtual bool GetModelFileName(const char** fileName);
    /* 31 */ virtual bool GetSelectionHighlightColor(CImVector& color);
    /* 31 */ virtual float GetTrueScale();
    /* 32 */ virtual void ModelLoaded(CM2Model* model);
    /* 34 */ virtual void PreAnimate(CGWorldFrame* worldFrame);
    /* 35 */ virtual bool Animate(float a2);
    /* 36 */ virtual void ShouldRender(uint32_t flags, uint32_t* culled, uint32_t* out);
    /* 37 */ virtual float GetRenderFacing();
    /* 49 */ virtual void GetMatrix(C44Matrix& pos);
    /* 51 */ virtual uint32_t UpdateObjectNameString(uint32_t mask, char* text, uint32_t textSize);
    /* 52 */ virtual bool ShouldRenderObjectName(uint32_t mask);
    /* 53 */ virtual CM2Model* GetObjectModel();
    /* 54 */ virtual char* GetObjectName();

    // Static functions
    static void SetStorage(CGObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
    static void ModelLoadedCallback(CM2Model* model, void* arg);
};

#endif // CLIENTOBJECT_CGOBJECT_C_HPP
