#ifndef WORLD_MAP_C_MAP_OBJ_DEF_GROUP_HPP
#define WORLD_MAP_C_MAP_OBJ_DEF_GROUP_HPP

#include "storm/list/TSLink.hpp"
#include "tempest/Plane.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include <tempest/Vector.hpp>
#include "world/map/CFrustum.hpp"
#include "model/CM2Lighting.hpp"

class CMapObjDefGroup : public CMapBaseObj {
    public:
    CAaBox bbox;
    CAaSphere sphere;
    float distanceToCamera;
    uint32_t groupNum;
    uint32_t unkFlags;
    int32_t unk_58;
    CImVector ambientColor;
    int32_t unk_60;
    int32_t unk_64;
    int32_t unk_68;
    STORM_EXPLICIT_LIST(CFrustum, sceneLink) frustumList;
    STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) doodadDefLinkList;
    //DWORD unk_84;
    //DWORD unk_88;
    //DWORD unk_8C;
    //DWORD unk_90;
    //DWORD unk_94;
    //DWORD unk_98;
    //DWORD unk_9C;
    //DWORD unk_A0;
    //DWORD unk_A4;
    TSLink<CMapObjDefGroup> sortEntryLink;
    TSLink<CMapObjDefGroup> sortTableLink;
    //DWORD unk_B8;
    //DWORD unk_BC;

    virtual void SelectLights(CM2Lighting* lighting);

    void MarkPrepared();
};

#endif
