#ifndef CLIENTOBJECT_CGOBJECT_C_HPP
#define CLIENTOBJECT_CGOBJECT_C_HPP

#include <cstdint>
#include "clientobject/Types.hpp"
#include "clientobject/CHashKeyGUID.hpp"
#include "clientobject/CClientObjCreate.hpp"
#include "storm/Hash.hpp"

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
    //CGObject_C_vtbl* __vftable /*VFT*/;
    //DWORD ukn_0038;
    //DWORD ukn_003C;
    //DWORD ukn_0040;
    //TSList ukn_0044;
    //TSList ukn_0050;
    //TSList ukn_005C;
    //TSList ukn_0068;
    //TSList ukn_0074;
    //TSList ukn_0080;
    //CM2Model* m_model;
    //DWORD ukn090[8];
    //PLAYERNAMEDESC* nameDesc;
    //DWORD ukn091[2];
    //DWORD m_flags;
    //DWORD ukn0C0[4];

    // Member functions
    CGObject_C();
    CGObject_C(CClientObjCreate& objCreate, uint32_t time);

    void SetTypeID(OBJECT_TYPE_ID typeID);
    void AddWorldObject();
    void SetData(uint32_t offset, uint32_t value);

    // Virtual functions
    virtual bool GetModelFileName(const char** fileName);

    // Static functions
    static void SetStorage(CGObject_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr);
};

#endif // CLIENTOBJECT_CGOBJECT_C_HPP
