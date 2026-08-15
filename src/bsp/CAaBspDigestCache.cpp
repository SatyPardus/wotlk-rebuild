#include "bsp/CAaBspDigestCache.hpp"
#include "bsp/AaBsp.hpp"

int32_t CAaBspDigestCache::s_digestReplaceIndex;

// OFFSET: 0x79B160
CAaBspDigestCache::CAaBspDigestCache() {
    memset(this->m_nodes, 0, sizeof(this->m_nodes));
    memset(this->m_digests, 0, sizeof(this->m_digests));
}

// OFFSET: 0x79B1C0
void CAaBspDigestCache::Reset() {
    memset(this->m_nodes, 0, sizeof(this->m_nodes));
    memset(this->m_digests, 0, sizeof(this->m_digests));
}

// OFFSET: 0x79AE10
void CAaBspDigestCache::RemoveEntry(const CAaBspNode* node) {
    uint32_t set = (((uintptr_t)node >> 5) ^ node->nFaces) & 0x7F;

    for (int way = 0; way < CAaBspDigestCache_Ways; way++) {
        if (this->m_nodes[set * CAaBspDigestCache_Ways + way] == node) {
            int entry = way + set * CAaBspDigestCache_Ways;

            this->m_nodes[entry] = 0;
            memset(&this->m_digests[entry], 0, sizeof(CAaBspNodeDigest));
            return;
        }
    }
}

// OFFSET: 0x79B1F0
CAaBspNodeDigest* CAaBspDigestCache::GetDigest(const CAaBsp& aaBsp, const CAaBspNode* node, const SMOPoly* polyList, const C3Vector* vertexList, const uint16_t* indices) {
    uint32_t set = (((uintptr_t)node >> 5) ^ node->nFaces) & 0x7F;

    int found = -1;
    int empty = -1;

    for (int way = 0; way < CAaBspDigestCache_Ways; way++) {
        const CAaBspNode* tag =
            this->m_nodes[way + set * CAaBspDigestCache_Ways];

        if (tag == node) {
            found = way;
            break;
        }

        if (!tag) {
            empty = way;
            break;
        }
    }

    CAaBspNodeDigest* digest;

    if (found >= 0) {
        digest = &this->m_digests[found + set * CAaBspDigestCache_Ways];
    } else {
        if (empty < 0)
            empty = (++s_digestReplaceIndex) & 7;

        int entry = empty + set * CAaBspDigestCache_Ways;

        this->m_nodes[entry] = node;
        digest = &this->m_digests[entry];

        GenerateDigest(digest, aaBsp, node, polyList, vertexList, indices);
    }

    if (digest->status)
        return 0;

    return digest;
}

// OFFSET: 0x79AE80
void CAaBspDigestCache::GenerateDigest(CAaBspNodeDigest* digest, const CAaBsp& aaBsp, const CAaBspNode* node, const SMOPoly* polyList, const C3Vector* vertexList, const uint16_t* indices) {
    uint16_t slotVertex[1024];
    uint16_t slotLocal[1024];

    memset(digest, 0, sizeof(CAaBspNodeDigest));

    digest->node = node;
    digest->status = CAaBspNodeDigest_Ok;
    digest->nVertices = 0;
    digest->nFaces = 0;

    memset(slotVertex, 0xFF, sizeof(slotVertex));

    if (node->nFaces > CAaBspNodeDigest_MaxFaces) {
        digest->status = CAaBspNodeDigest_TooManyFaces;
        return;
    }

    const uint16_t* nodeFaces = aaBsp.nodeFaceIndices + node->faceStart;

    for (uint32_t i = 0; i < node->nFaces; i++) {
        uint16_t face = nodeFaces[i];

        digest->faceIndices[i] = face;

        const uint16_t* faceVerts = indices + 3 * face;

        for (int k = 0; k < 3; k++) {
            uint16_t vertex = faceVerts[k];
            uint32_t slot = vertex & 0x3FF;
            int hit = -1;

            do {
                if (slotVertex[slot] == vertex) {
                    hit = slot;
                } else if (slotVertex[slot] == 0xFFFF) {
                    if (digest->nVertices >= CAaBspNodeDigest_MaxVertices) {
                        digest->status = CAaBspNodeDigest_TooManyVerts;
                        return;
                    }

                    digest->vertices[digest->nVertices] = vertexList[vertex];
                    digest->vertexIndices[digest->nVertices] = vertex;

                    slotLocal[slot] = digest->nVertices;
                    slotVertex[slot] = vertex;

                    digest->nVertices++;
                    hit = slot;
                } else {
                    slot = (slot + 1) & 0x3FF;
                }
            } while (hit < 0);

            digest->faceVertexIndices[3 * digest->nFaces + k] = slotLocal[hit];
        }

        digest->faceFlags[digest->nFaces] = polyList[face].flags & 0x7F;
        digest->nFaces++;
    }
}
