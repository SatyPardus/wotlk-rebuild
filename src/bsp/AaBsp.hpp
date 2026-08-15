#ifndef COMMON_AABSP_HPP
#define COMMON_AABSP_HPP

#include "tempest/Vector.hpp"
#include "tempest/Box.hpp"
#include "bsp/CAaBspNode.hpp"

#define CAABSP_NO_CHILD 0xFFFF

class CAaBspDigestCache;

extern CAaBspDigestCache* g_BspDigestCache;

class CAaBsp {
    public:
    CAaBsp();
    ~CAaBsp();
    void Set(CAaBspNode* nodeList, uint32_t nNodes, uint16_t* faceIndices, uint32_t nFaceIndices, const CAaBox& box);
    void Clear();

    CAaBspNode* rootNode;      // +0x00
    CAaBspNode* nodes;         // +0x04
    uint16_t* nodeFaceIndices; // +0x08
    uint32_t nNodes;           // +0x0C
    uint32_t nNodeFaceIndices; // +0x10
    uint16_t* faceVertexIndices;   // +0x14
    uint32_t nFaceVertexIndices;   // +0x18
    C3Vector* vertices;            // +0x1C
    uint32_t nVertices;            // +0x20
    uint32_t nodeSize;             // +0x24
    uint32_t nodeNext;             // +0x28
    uint32_t nodeFaceIndicesSize;  // +0x2C
    uint32_t nodeFaceIndicesNext;  // +0x30
    uint16_t* buildFaceIndices;    // +0x34
    uint32_t buildFaceIndicesSize; // +0x38
    uint32_t buildFaceIndicesNext; // +0x3C

    uint32_t treeDepth;    // +0x40
    uint32_t avgNodeFaces; // +0x44
    int bFree;             // +0x48
    CAaBox aaBox;          // +0x4C
    int m_unk64; // +0x64

    private:
    void Init();
    void Free();
};

#endif
