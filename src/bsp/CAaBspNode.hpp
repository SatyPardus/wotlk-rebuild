#ifndef BSP_C_AABSP_NODE_HPP
#define BSP_C_AABSP_NODE_HPP

#include <cstdint>

enum {
    CAaBspNode_AxisMask = 0x0003,
    CAaBspNode_Leaf = 0x0004
};

struct CAaBspNode {
    uint16_t flags;     // +0x00  low 2 bits = split axis, bit 2 = leaf
    uint16_t negChild;  // +0x02  0xFFFF = none
    uint16_t posChild;  // +0x04  0xFFFF = none
    uint16_t nFaces;    // +0x06  leaf only
    uint32_t faceStart; // +0x08  leaf only, index into nodeFaceIndices
    float planeDist;    // +0x0C  internal only
};

#endif
