#ifndef WORLD_BSP_QUERY_HPP
#define WORLD_BSP_QUERY_HPP

#include "world/map/Types.hpp"
#include <cstdint>
#include <tempest/ray/CRay.hpp>
#include <world/map/CFrustum.hpp>
#include "tempest/Segment.hpp"
#include "bsp/AaBsp.hpp"

bool QueryCull(const CAaBox& box, const C3Vector* v0, const C3Vector* v1, const C3Vector* v2);
bool QueryCull(CFrustum* frustum, C3Vector& v0, C3Vector& v1, C3Vector& v2);

enum { BSPQUERY_MAX_FACES = 0x2000 };
enum { BSPQUERY_FACE_TESTED = 0x80 };

class BspQuery {
    public:
    // OFFSET: 0x7C7610
    void ClearTestFaces() {
        while (BspQuery::testFaceSub) {
            BspQuery::testFaceSub--;
            uint16_t face = BspQuery::testFaces[BspQuery::testFaceSub];
            this->faces[face].flags &= ~BSPQUERY_FACE_TESTED;
        }

        BspQuery::hitFaceSub = 0;
        BspQuery::testFaceSub = 0;
    }

    int* overflowFlags;

    SMOPoly* faces;       // +0x04
    C3Vector* vertexList; // +0x08
    uint16_t* indices;    // +0x0C

    static uint16_t testFaces[BSPQUERY_MAX_FACES]; // 0xD25BF8
    static uint16_t hitFaces[BSPQUERY_MAX_FACES];  // 0xD29BF8
    static int testFaceSub;                        // 0xD2DBF8
    static int hitFaceSub;                         // 0xD2DBFC
};

template <class T>
class BspQuery_Volume : public BspQuery {
    public:
    // 0x007C9B10 / 0x007C7660.
    void operator()(uint16_t faceIndex);
    bool GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node);

    const T* volume;          // +0x10
    uint16_t faceIgnoreFlags; // +0x14
};

enum {
    BSPQUERY_IGNORE_TRANSPARENT = 0x100, // material blendMode != 0
    BSPQUERY_IGNORE_OPAQUE = 0x200       // no material, or blendMode == 0
};

class BspQuery_Segment : public BspQuery {
    public:
    void operator()(uint16_t faceIndex); // 0x007C6C30
    bool GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node); // 0x007C6D50

    float* hitT;            // +0x10
    float origHitT;         // +0x14
    C3Segment seg;          // +0x18 
    CRay ray;              // +0x30 
    float oosegMag;         // +0x48
    float maxT;             // +0x4C 
    SMOMaterial* materials; // +0x50 
    int m_unk54; // +0x54

    uint16_t faceIgnoreFlags; // +0x58
};

enum {
    BSPQUERY_LINK_BOTH = 0x20,  // may update both hit slots
    BSPQUERY_LINK_FIRST = 0x08, // may update slot 0 only
    BSPQUERY_LINK_SECOND = 0x04 // may update slot 1 only
};

class BspQuery_SegmentLink : public BspQuery {
    public:
    void operator()(uint16_t faceIndex); // 0x007C6600
    bool GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node); // 0x007C6790

    CRay ray;                // +0x10
    C3Segment seg;            // +0x28
    float oosegMag;           // +0x40
    float tMin;               // +0x44
    float tMax;               // +0x48
    float bestT0;             // +0x4C
    float bestT1;             // +0x50
    int bestFace0;            // +0x54
    int bestFace1;            // +0x58
    uint32_t faceIgnoreFlags; // +0x5C
};

void BuildTriQuery(BspQuery_Segment* q, SMOPoly* polyList, C3Vector* vertexList, uint16_t* indices, const C3Segment* seg, float* hitT, uint16_t faceIgnoreFlags, SMOMaterial* materials);

#endif
