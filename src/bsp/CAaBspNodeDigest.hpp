#ifndef BSP_C_AABSP_NODE_DIGEST_HPP
#define BSP_C_AABSP_NODE_DIGEST_HPP

#include <cstdint>
#include "bsp/CAaBspNode.hpp"
#include "tempest/vector/C3Vector.hpp"

enum {
    CAaBspNodeDigest_MaxFaces = 0x12C,   // 300
    CAaBspNodeDigest_MaxVertices = 0x1C2 // 450
};

enum {
    CAaBspNodeDigest_Ok = 0,
    CAaBspNodeDigest_TooManyFaces = 1,
    CAaBspNodeDigest_TooManyVerts = 2
};

struct CAaBspNodeDigest {
    const CAaBspNode* node;                                    // +0x0000
    uint8_t status;                                            // +0x0004
                                                               // +0x0005 pad
    uint16_t nVertices;                                        // +0x0006
    C3Vector vertices[CAaBspNodeDigest_MaxVertices];           // +0x0008
    uint16_t vertexIndices[CAaBspNodeDigest_MaxVertices];      // +0x1520
    uint16_t nFaces;                                           // +0x18A4
    uint16_t faceVertexIndices[CAaBspNodeDigest_MaxFaces * 3]; // +0x18A6
    uint16_t faceFlags[CAaBspNodeDigest_MaxFaces];             // +0x1FAE
    uint16_t faceIndices[CAaBspNodeDigest_MaxFaces];           // +0x2206
};   

#endif
