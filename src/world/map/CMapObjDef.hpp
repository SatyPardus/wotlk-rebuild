#ifndef WORLD_MAP_C_MAP_OBJ_DEF_HPP
#define WORLD_MAP_C_MAP_OBJ_DEF_HPP

#include "storm/list/TSLink.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/CMapObj.hpp"
#include "world/map/CMapObjDefGroup.hpp"
#include "world/map/Types.hpp"
#include "storm/Hash.hpp"
#include "tempest/vector/CImVector.hpp"
#include "clientobject/WGUID.hpp"

union CMapObjDefGroupStorage {
    CMapObjDefGroup* m_inline[4];
    TSGrowableArray<CMapObjDefGroup*> m_array;

    CMapObjDefGroupStorage()
        : m_inline {} {}         // activate the trivial member
    ~CMapObjDefGroupStorage() {} // deliberately empty
    CMapObjDefGroupStorage(const CMapObjDefGroupStorage&) = delete;
    CMapObjDefGroupStorage& operator=(const CMapObjDefGroupStorage&) = delete;
};

class CMapObjDef : public CMapBaseObj, public TSHashObject<CMapObjDef, uint32_t> {
    public:
    C3Vector position;
    CAaBox bbox;
    CAaSphere sphere;
    C44Matrix mat;
    C44Matrix invMat;
    int32_t nameId;
    CMapObj* owner;
    int32_t unk_F8;
    uint32_t unkFlags;
    int32_t doodadSet;
    int32_t nameSet;
    int32_t unk_108;
    int32_t unk_10C;
    int32_t unk_110;
    STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) mapObjDefGroupLinkList;
    CMapObjDefGroupStorage defGroups;
    uint32_t groupCount;
    //TSGrowableArray unk;
    CImVector argbColor;
    WGUID unk_148;
    int16_t doodadSetOverrides[3];

    CMapObjDef();
    void ConvertInlineGroupsToArray();
    CMapObjDefGroup** Groups();
    int32_t GroupCount() const;
    void ReserveGroups(int32_t n);
    bool TestAABox(C3Vector& start, C3Vector& end);
    CMapObjDefGroup** GroupSlot(uint32_t index);
};

#endif
