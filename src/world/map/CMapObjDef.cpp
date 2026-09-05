#include "world/map/CMapObjDef.hpp"
#include <world/CWorldMath.hpp>

CMapObjDef::CMapObjDef() {
    //CMapBaseObj::CMapBaseObj(this);
    //this->unk_28 = 0;
    //this->unk_2C = 0;
    //this->unk_30 = 0;
    //this->unk_34 = 0;
    //this->__vftable = &off_A3FFA8;
    this->position.x = 0.0;
    this->position.y = 0.0;
    this->position.z = 0.0;
    this->bbox.b.x = 0.0;
    this->bbox.b.y = 0.0;
    this->bbox.b.z = 0.0;
    this->bbox.t.x = 0.0;
    this->bbox.t.y = 0.0;
    this->bbox.t.z = 0.0;
    this->sphere.c.x = 0.0;
    this->sphere.r = 0.0;
    this->sphere.c.y = 0.0;
    this->sphere.c.z = 0.0;
    this->mat.a0 = 1.0;
    this->mat.b1 = 1.0;
    this->unk_10C = 0;
    this->mat.c2 = 1.0;
    this->unk_110 = 0;
    this->mat.d3 = 1.0;
    this->mapObjDefGroupLinkList.m_terminator.m_next = 0;
    this->mat.a1 = 0.0;
    this->mat.a2 = 0.0;
    this->mat.a3 = 0.0;
    this->mat.b0 = 0.0;
    this->mat.b2 = 0.0;
    this->mat.b3 = 0.0;
    this->mat.c0 = 0.0;
    this->mat.c1 = 0.0;
    this->mat.c3 = 0.0;
    this->mat.d0 = 0.0;
    this->mat.d1 = 0.0;
    this->mat.d2 = 0.0;
    this->invMat.a0 = 1.0;
    this->invMat.b1 = 1.0;
    this->invMat.c2 = 1.0;
    this->invMat.d3 = 1.0;
    this->invMat.a1 = 0.0;
    this->invMat.a2 = 0.0;
    this->invMat.a3 = 0.0;
    this->invMat.b0 = 0.0;
    this->invMat.b2 = 0.0;
    this->invMat.b3 = 0.0;
    this->invMat.c0 = 0.0;
    this->invMat.c1 = 0.0;
    this->invMat.c3 = 0.0;
    this->invMat.d0 = 0.0;
    this->invMat.d1 = 0.0;
    this->invMat.d2 = 0.0;
    //this->mapObjDefGroupLinkList.m_terminator.m_prevLink = &this->mapObjDefGroupLinkList.m_terminator;
    //this->mapObjDefGroupLinkList.m_linkoffset = 12;
    //this->mapObjDefGroupLinkList.m_terminator.m_next = (&this->mapObjDefGroupLinkList.m_terminator | 1);
    this->groupCount = 0;
    //this->lightsArray.m_alloc = 0;
    //this->lightsArray.m_count = 0;
    //this->lightsArray.m_data = 0;
    //this->lightsArray.m_chunk = 0;
    this->argbColor = { 0, 0, 0, 0 };
    this->nameId = 0;
    this->owner = 0;
    this->unk_F8 = 0;
    this->type |= 8u;
    this->doodadSet = 0;
    this->nameSet = 0;
    this->unk_108 = 0;
    this->unk_148 = 0;
    this->unkFlags = 0xFFFF;
}

// OFFSET: 0x7B3B70
void CMapObjDef::ConvertInlineGroupsToArray() {
    int32_t n = this->groupCount;
    if (n > 4)
        n = 4;

    CMapObjDefGroup* saved[4];
    if (n)
        memcpy(saved, this->defGroups.m_inline, sizeof(void*) * n);

    memset(&this->defGroups.m_array, 0, sizeof(this->defGroups.m_array));
    this->defGroups.m_array.Add(n, saved);
    // TSGrowableArray_CMapObjDefGroup__Add(this, ADJ(this)->groupCount - v4, &ADJ(this)->defGroups.m_alloc + v4);

    this->groupCount = -1;
}

CMapObjDefGroup** CMapObjDef::Groups() {
    return (this->groupCount == -1) ? this->defGroups.m_array.Ptr() : this->defGroups.m_inline;
}

int32_t CMapObjDef::GroupCount() const {
    return (this->groupCount == -1) ? this->defGroups.m_array.Count() : this->groupCount;
}

void CMapObjDef::ReserveGroups(int32_t n) {
    if (this->groupCount == -1)
        this->defGroups.m_array.SetCount(n);
    else if (n <= 4)
        this->groupCount = n;
    else {
        this->ConvertInlineGroupsToArray();
        this->defGroups.m_array.SetCount(n);
    }
}

// OFFSET: 0x7B3990
bool CMapObjDef::TestAABox(C3Vector& start, C3Vector& end) {
    return CWorldMath::VectorIntersectAABox2(this->bbox, start, end);
}
