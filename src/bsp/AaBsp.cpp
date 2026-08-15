#include "bsp/AaBsp.hpp"
#include "bsp/CAaBspDigestCache.hpp"
#include "bsp/CAaBspNode.hpp"
#include <bc/memory/Storm.hpp>

CAaBspDigestCache* g_BspDigestCache;

// INLINED
void CAaBsp::Init() {
    this->rootNode = 0;
    this->nodes = 0;
    this->nodeFaceIndices = 0;
    this->nNodes = 0;
    this->nNodeFaceIndices = 0;
    this->faceVertexIndices = 0;
    this->nFaceVertexIndices = 0;
    this->vertices = 0;
    this->nVertices = 0;
    this->nodeSize = 0;
    this->nodeNext = 0;
    this->nodeFaceIndicesSize = 0;
    this->nodeFaceIndicesNext = 0;
    this->buildFaceIndices = 0;
    this->buildFaceIndicesNext = 0;
    this->buildFaceIndicesSize = 0;
    this->treeDepth = 0;
    this->bFree = 1;
    this->avgNodeFaces = 0;
}

// OFFSET: 0x79B070
CAaBsp::CAaBsp() {
    this->m_unk64 = 0;
    Init();
}

// OFFSET: 0x79B2B0
CAaBsp::~CAaBsp() {
    Free();
}

// OFFSET: 0x79ADC0
void CAaBsp::Set(CAaBspNode* nodeList, uint32_t nNodes, uint16_t* faceIndices, uint32_t nFaceIndices, const CAaBox& box) {
    this->nodes = nodeList;
    this->rootNode = nodeList;
    this->nodeFaceIndices = faceIndices;
    this->nNodeFaceIndices = nFaceIndices;
    this->bFree = 0;
    this->nNodes = nNodes;
    this->aaBox = box;
}

// OFFSET: 0x79B2C0
void CAaBsp::Clear() {
    Free();
    Init();
}

// OFFSET: 0x79B0D0
void CAaBsp::Free() {
    if (g_BspDigestCache) {
        for (uint32_t i = 0; i < this->nNodes; i++) {
            if (this->nodes[i].flags & CAaBspNode_Leaf)
                g_BspDigestCache->RemoveEntry(&this->nodes[i]);
        }
    }

    if (this->bFree) {
        if (this->nodes)
            STORM_FREE(this->nodes);

        if (this->nodeFaceIndices)
            STORM_FREE(this->nodeFaceIndices);
    }

    if (this->buildFaceIndices)
        STORM_FREE(this->buildFaceIndices);
}
