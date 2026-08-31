#include "bsp/BspQuery.hpp"
#include "bsp/AaBsp.hpp"
#include "bsp/CAaBspNodeDigest.hpp"
#include "bsp/CAaBspDigestCache.hpp"
#include <tempest/segment/C3Segment.hpp>
#include <tempest/Intersect.hpp>

uint16_t BspQuery::testFaces[BSPQUERY_MAX_FACES];
uint16_t BspQuery::hitFaces[BSPQUERY_MAX_FACES];
int BspQuery::testFaceSub;
int BspQuery::hitFaceSub;

// OFFSET: 0x7C7A00
bool QueryCull(const CAaBox& box, const C3Vector* v0, const C3Vector* v1, const C3Vector* v2) {
    for (uint32_t i = 0; i < 3; i++) {
        bool allAbove = (box.t[i] - (*v0)[i] < 0.0f) && (box.t[i] - (*v1)[i] < 0.0f) && (box.t[i] - (*v2)[i] < 0.0f);

        bool allBelow = ((*v0)[i] - box.b[i] < 0.0f) && ((*v1)[i] - box.b[i] < 0.0f) && ((*v2)[i] - box.b[i] < 0.0f);

        if (allAbove || allBelow)
            return true;
    }

    return false;
}

// OFFSET: 0x7C71E0
bool QueryCull(CFrustum* frustum, C3Vector& v0, C3Vector& v1, C3Vector& v2) {
    uint8_t c0, c1, c2;

    frustum->Cull(v0, &c0);
    frustum->Cull(v1, &c1);
    frustum->Cull(v2, &c2);

    return (c0 & (c1 & c2)) != 0;
}

// OFFSET: 0x7C78E0
void BuildTriQuery(BspQuery_Segment* q, SMOPoly* polyList, C3Vector* vertexList, uint16_t* indices, const C3Segment* seg, float* hitT, uint16_t faceIgnoreFlags, SMOMaterial* materials) {
    q->overflowFlags = 0;
    q->faces = polyList;
    q->vertexList = vertexList;
    q->indices = indices;
    q->hitT = hitT;

    q->materials = materials;
    q->faceIgnoreFlags = faceIgnoreFlags | BSPQUERY_FACE_TESTED;
    //q->m_unk54 = g_unk_CF08F8;

    q->ray.origin = seg->b;

    q->ray.dir.x = seg->t.x - seg->b.x;
    q->ray.dir.y = seg->t.y - seg->b.y;
    q->ray.dir.z = seg->t.z - seg->b.z;

    float mag = (float)sqrt(q->ray.dir.x * q->ray.dir.x + q->ray.dir.y * q->ray.dir.y + q->ray.dir.z * q->ray.dir.z);

    q->maxT = *hitT * mag;

    q->seg = *seg;
    q->origHitT = *hitT;
    q->oosegMag = 1.0f / mag;

    q->ray.dir.x *= q->oosegMag;
    q->ray.dir.y *= q->oosegMag;
    q->ray.dir.z *= q->oosegMag;
}

template <>
void BspQuery_Volume<CAaBox>::operator()(uint16_t faceIndex) {
    if (this->faces[faceIndex].flags & this->faceIgnoreFlags)
        return;

    if (BspQuery::testFaceSub >= BSPQUERY_MAX_FACES) {
        if (this->overflowFlags)
            *this->overflowFlags |= 1;
        return;
    }

    BspQuery::testFaces[BspQuery::testFaceSub] = faceIndex;
    BspQuery::testFaceSub++;

    this->faces[faceIndex].flags |= BSPQUERY_FACE_TESTED;

    if (!QueryCull(*this->volume,
                   &this->vertexList[this->indices[3 * faceIndex + 0]],
                   &this->vertexList[this->indices[3 * faceIndex + 1]],
                   &this->vertexList[this->indices[3 * faceIndex + 2]])) {
        BspQuery::hitFaces[BspQuery::hitFaceSub] = faceIndex;
        BspQuery::hitFaceSub++;
    }
}

// OFFSET: 0x7C7230
template <>
bool BspQuery_Volume<CAaBox>::GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node) {
    if (!g_BspDigestCache)
        return false;

    CAaBspNodeDigest* digest = g_BspDigestCache->GetDigest(aaBsp, node, this->faces, this->vertexList, this->indices);
    if (!digest)
        return false;

    unsigned char outcodes[CAaBspNodeDigest_MaxVertices];
    const CAaBox& box = *this->volume;

    for (int i = 0; i < digest->nVertices; i++) {
        const C3Vector& v = digest->vertices[i];
        unsigned char oc = 0;

        if (box.b.x >= v.x)
            oc |= 0x20;
        if (box.t.x <= v.x)
            oc |= 0x10;
        if (box.b.y >= v.y)
            oc |= 0x08;
        if (box.t.y <= v.y)
            oc |= 0x04;
        if (box.b.z >= v.z)
            oc |= 0x02;
        if (box.t.z <= v.z)
            oc |= 0x01;

        outcodes[i] = oc;
    }

    for (int i = 0; i < digest->nFaces; i++) {
        uint16_t ignore = this->faceIgnoreFlags;

        if (digest->faceFlags[i] & ignore)
            continue;

        uint16_t face = digest->faceIndices[i];

        // The digest's copy of the flags has the traversal marker stripped,
        // so this second test is what catches a face already visited through
        // another node during this same query.
        if (this->faces[face].flags & ignore)
            continue;

        if (BspQuery::testFaceSub >= BSPQUERY_MAX_FACES) {
            if (this->overflowFlags)
                *this->overflowFlags |= 1;
            break;
        }

        BspQuery::testFaces[BspQuery::testFaceSub] = face;
        BspQuery::testFaceSub++;

        this->faces[face].flags |= BSPQUERY_FACE_TESTED;

        unsigned char oc = outcodes[digest->faceVertexIndices[3 * i + 1]] & outcodes[digest->faceVertexIndices[3 * i + 2]] & outcodes[digest->faceVertexIndices[3 * i + 0]];

        if ((oc & 0x3F) == 0) {
            BspQuery::hitFaces[BspQuery::hitFaceSub] = face;
            BspQuery::hitFaceSub++;
        }
    }

    return true;
}

template <>
bool BspQuery_Volume<CFrustum>::GetFaceIndicesUsingCache(const CAaBsp&, const CAaBspNode*) {
    return false;
}

// OFFSET: 0x7C6C30
void BspQuery_Segment::operator()(uint16_t faceIndex) {
    uint16_t ignore = this->faceIgnoreFlags;
    SMOPoly* poly = &this->faces[faceIndex];

    if (poly->flags & ignore)
        return;

    if (poly->materialId < 0xFF && this->materials[poly->materialId].blendMode != 0) {
        if (ignore & BSPQUERY_IGNORE_TRANSPARENT)
            return;
    } else {
        if (ignore & BSPQUERY_IGNORE_OPAQUE)
            return;
    }

    if (BspQuery::testFaceSub >= BSPQUERY_MAX_FACES) {
        if (this->overflowFlags)
            *this->overflowFlags |= 1;
        return;
    }

    BspQuery::testFaces[BspQuery::testFaceSub] = faceIndex;
    BspQuery::testFaceSub++;

    poly->flags |= BSPQUERY_FACE_TESTED;

    float t = 0.0f;

    if (!NTempest::Intersect(&this->ray, this->vertexList, &this->indices[3 * faceIndex], &t, nullptr, 0.002f))
        return;

    if (t < 0.0f)
        return;

    if (t > this->maxT)
        return;

    this->maxT = t;

    BspQuery::hitFaces[0] = faceIndex;
    BspQuery::hitFaceSub = 1;

    *this->hitT = t * this->oosegMag;

    if (*this->hitT > this->origHitT)
        *this->hitT = this->origHitT;
    else
        *this->hitT = *this->hitT;
}

// OFFSET: 0x7C6D50
bool BspQuery_Segment::GetFaceIndicesUsingCache(const CAaBsp& aaBsp,
                                               const CAaBspNode* node) {
    if (!g_BspDigestCache)
        return false;

    CAaBspNodeDigest* digest = g_BspDigestCache->GetDigest(aaBsp, node, this->faces, this->vertexList, this->indices);
    if (!digest)
        return false;

    C3Vector lo;
    C3Vector hi;

    for (uint32_t k = 0; k < 3; k++) {
        if (this->seg.b[k] > this->seg.t[k]) {
            lo[k] = this->seg.t[k];
            hi[k] = this->seg.b[k];
        } else {
            lo[k] = this->seg.b[k];
            hi[k] = this->seg.t[k];
        }

        lo[k] -= 0.01f;
        hi[k] += 0.01f;
    }

    unsigned char outcodes[CAaBspNodeDigest_MaxVertices];

    for (int i = 0; i < digest->nVertices; i++) {
        const C3Vector& v = digest->vertices[i];
        unsigned char oc = 0;

        if (lo.x > v.x)
            oc |= 0x20;
        if (hi.x < v.x)
            oc |= 0x10;
        if (lo.y > v.y)
            oc |= 0x08;
        if (hi.y < v.y)
            oc |= 0x04;
        if (lo.z > v.z)
            oc |= 0x02;
        if (hi.z < v.z)
            oc |= 0x01;

        outcodes[i] = oc;
    }

    for (int i = 0; i < digest->nFaces; i++) {
        uint16_t ignore = this->faceIgnoreFlags;

        if (digest->faceFlags[i] & ignore)
            continue;

        uint16_t face = digest->faceIndices[i];
        SMOPoly* poly = &this->faces[face];

        if (poly->flags & ignore)
            continue;

        if (poly->materialId < 0xFF && this->materials[poly->materialId].blendMode != 0) {
            if (ignore & BSPQUERY_IGNORE_TRANSPARENT)
                continue;
        } else {
            if (ignore & BSPQUERY_IGNORE_OPAQUE)
                continue;
        }

        if (BspQuery::testFaceSub >= BSPQUERY_MAX_FACES) {
            if (this->overflowFlags)
                *this->overflowFlags |= 1;
            break;
        }

        BspQuery::testFaces[BspQuery::testFaceSub] = face;
        BspQuery::testFaceSub++;

        poly->flags |= BSPQUERY_FACE_TESTED;

        unsigned char oc = outcodes[digest->faceVertexIndices[3 * i + 1]] & outcodes[digest->faceVertexIndices[3 * i + 2]] & outcodes[digest->faceVertexIndices[3 * i + 0]];

        if (oc & 0x3F)
            continue;

        float t = 0.0f;

        if (!NTempest::Intersect(&this->ray, digest->vertices, &digest->faceVertexIndices[3 * i], &t, nullptr, 0.002f))
            continue;

        if (t < 0.0f)
            continue;

        if (t > this->maxT)
            continue;

        this->maxT = t;

        BspQuery::hitFaces[0] = face;
        BspQuery::hitFaceSub = 1;

        *this->hitT = t * this->oosegMag;

        if (*this->hitT > this->origHitT)
            *this->hitT = this->origHitT;
        else
            *this->hitT = *this->hitT;
    }

    return true;
}

// OFFSET: 0x7C6600
void BspQuery_SegmentLink::operator()(uint16_t faceIndex) {
    uint32_t flags = this->faces[faceIndex].flags;

    if (flags & this->faceIgnoreFlags)
        return;

    if (BspQuery::testFaceSub >= BSPQUERY_MAX_FACES)
        return;

    BspQuery::testFaces[BspQuery::testFaceSub] = faceIndex;
    BspQuery::testFaceSub++;

    this->faces[faceIndex].flags |= BSPQUERY_FACE_TESTED;

    float t = 0.0f;

    if (!NTempest::Intersect(&this->ray, this->vertexList, &this->indices[3 * faceIndex], &t, nullptr, 0.002f))
        return;

    if (flags & BSPQUERY_LINK_BOTH) {
        if (t < 0.0f)
            return;

        if (t <= this->bestT0) {
            this->bestT0 = t;
            this->bestFace0 = faceIndex;
        }
    } else if (flags & BSPQUERY_LINK_FIRST) {
        if (t < 0.0f)
            return;

        if (t > this->bestT0)
            return;

        this->bestFace0 = faceIndex;
        this->bestT0 = t;
        return;
    } else if (flags & BSPQUERY_LINK_SECOND) {
        if (t < 0.0f)
            return;
    } else {
        return;
    }

    if (t > this->bestT1)
        return;

    this->bestFace1 = faceIndex;
    this->bestT1 = t;
}

// OFFSET: 0x7C6790
bool BspQuery_SegmentLink::GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node) {
    if (!g_BspDigestCache)
        return false;

    CAaBspNodeDigest* digest = g_BspDigestCache->GetDigest(aaBsp, node, this->faces, this->vertexList, this->indices);
    if (!digest)
        return false;

    C3Vector lo;
    C3Vector hi;

    for (uint32_t k = 0; k < 3; k++) {
        if (this->seg.b[k] > this->seg.t[k]) {
            lo[k] = this->seg.t[k];
            hi[k] = this->seg.b[k];
        } else {
            lo[k] = this->seg.b[k];
            hi[k] = this->seg.t[k];
        }

        lo[k] -= 0.01f;
        hi[k] += 0.01f;
    }

    unsigned char outcodes[CAaBspNodeDigest_MaxVertices];

    for (int i = 0; i < digest->nVertices; i++) {
        const C3Vector& v = digest->vertices[i];
        unsigned char oc = 0;

        if (lo.x > v.x)
            oc |= 0x20;
        if (hi.x < v.x)
            oc |= 0x10;
        if (lo.y > v.y)
            oc |= 0x08;
        if (hi.y < v.y)
            oc |= 0x04;
        if (lo.z > v.z)
            oc |= 0x02;
        if (hi.z < v.z)
            oc |= 0x01;

        outcodes[i] = oc;
    }

    for (int i = 0; i < digest->nFaces; i++) {
        uint32_t ignore = this->faceIgnoreFlags;

        if (digest->faceFlags[i] & ignore)
            continue;

        uint16_t face = digest->faceIndices[i];
        uint32_t flags = this->faces[face].flags;

        if (flags & ignore)
            continue;

        if (BspQuery::testFaceSub >= BSPQUERY_MAX_FACES)
            break;

        BspQuery::testFaces[BspQuery::testFaceSub] = face;
        BspQuery::testFaceSub++;

        this->faces[face].flags |= BSPQUERY_FACE_TESTED;

        unsigned char oc = outcodes[digest->faceVertexIndices[3 * i + 1]] & outcodes[digest->faceVertexIndices[3 * i + 2]] & outcodes[digest->faceVertexIndices[3 * i + 0]];

        if (oc & 0x3F)
            continue;

        float t = 0.0f;

        if (!NTempest::Intersect(&this->ray, digest->vertices, &digest->faceVertexIndices[3 * i], &t, nullptr, 0.002f))
            continue;

        bool checkSlot1;

        if (flags & BSPQUERY_LINK_BOTH) {
            if (t < 0.0f)
                continue;

            if (t <= this->bestT0) {
                this->bestT0 = t;
                this->bestFace0 = face;
            }
            checkSlot1 = true;
        } else if (flags & BSPQUERY_LINK_FIRST) {
            if (t < 0.0f)
                continue;

            if (t > this->bestT0)
                continue;

            this->bestFace0 = face;
            this->bestT0 = t;
            checkSlot1 = false;
        } else if (flags & BSPQUERY_LINK_SECOND) {
            if (t < 0.0f)
                continue;

            checkSlot1 = true;
        } else {
            continue;
        }

        if (checkSlot1) {
            if (t > this->bestT1)
                continue;

            this->bestFace1 = face;
            this->bestT1 = t;
        }
    }

    return true;
}
