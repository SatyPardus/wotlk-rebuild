#ifndef CLIENTOBJECT_CGOBJECT_C_HPP
#define CLIENTOBJECT_CGOBJECT_C_HPP

#include <cstdint>
#include "clientobject/Types.hpp"
#include "clientobject/CHashKeyGUID.hpp"
#include "storm/Hash.hpp"

struct ObjectFields {
    WGUID OBJECT_FIELD_GUID;
    OBJECT_TYPE OBJECT_FIELD_TYPE;
    uint32_t OBJECT_FIELD_ENTRY;
    float OBJECT_FIELD_SCALE_X;
    uint32_t OBJECT_FIELD_PADDING;
};

class CGObject {
    public:
    // uint32_t unk_0000;
    ObjectFields* ObjectData;
    // uint32_t ukn_0008;
    // uint32_t ukn_000C;
    OBJECT_TYPE_ID m_typeID;
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

    void SetTypeID(OBJECT_TYPE_ID typeID);
};

#endif // CLIENTOBJECT_CGOBJECT_C_HPP
