#ifndef BSP_C_AABSP_QUERY_HPP
#define BSP_C_AABSP_QUERY_HPP

#include <cstdint>
#include "bsp/CAaBspNode.hpp"
#include "bsp/AaBsp.hpp"
#include "tempest/Box.hpp"

template <class F>
struct CAaBsp_Query
{
    const CAaBsp* aaBsp; // +0x00
    F* f;                // +0x04

    void GetFaceIndices(const CAaBspNode* node) {
        if (!this->f->GetFaceIndicesUsingCache(*this->aaBsp, node)) {
            const uint16_t* faceIndices =
                this->aaBsp->nodeFaceIndices + node->faceStart;

            for (uint32_t i = 0; i < node->nFaces; i++)
                (*this->f)(faceIndices[i]);
        }
    }
};

template <class F>
struct CAaBsp_Query_AaBox : public CAaBsp_Query<F> {
    void GetFaceIndices(uint32_t nodeIndex, const CAaBox& queryBox, const CAaBox& nodeBox) {
        const CAaBspNode* node = &this->aaBsp->nodes[nodeIndex];

        if (node->flags & CAaBspNode_Leaf) {
            this->CAaBsp_Query<F>::GetFaceIndices(node);
            return;
        }

        uint32_t axis = node->flags & CAaBspNode_AxisMask;

        if (nodeBox.b[axis] <= queryBox.t[axis] &&
            nodeBox.t[axis] >= queryBox.b[axis]) {
            CAaBox posBox = nodeBox;
            posBox.b[axis] = node->planeDist;

            CAaBox negBox = nodeBox;
            negBox.t[axis] = node->planeDist;

            if (node->planeDist < queryBox.b[axis]) {
                if (node->posChild != CAABSP_NO_CHILD)
                    GetFaceIndices(node->posChild, queryBox, posBox);
            } else if (node->planeDist > queryBox.t[axis]) {
                if (node->negChild != CAABSP_NO_CHILD)
                    GetFaceIndices(node->negChild, queryBox, negBox);
            } else {
                if (node->posChild != CAABSP_NO_CHILD) {
                    CAaBox subQuery = queryBox;
                    subQuery.b[axis] = node->planeDist;
                    GetFaceIndices(node->posChild, subQuery, posBox);
                }
                if (node->negChild != CAABSP_NO_CHILD) {
                    CAaBox subQuery = queryBox;
                    subQuery.t[axis] = node->planeDist;
                    GetFaceIndices(node->negChild, subQuery, negBox);
                }
            }
        }
    }
};

template <class F>
struct CAaBsp_Query_Segment : public CAaBsp_Query<F> {
    void GetFaceIndices(uint32_t nodeIndex, const C3Segment& seg, const CAaBox& nodeBox) {
        const CAaBspNode* node = this->aaBsp->nodes + nodeIndex;

        if (!node)
            return;

        if (node->flags & CAaBspNode_Leaf) {
            this->CAaBsp_Query<F>::GetFaceIndices(node);
            return;
        }

        uint32_t axis = node->flags & CAaBspNode_AxisMask;

        if ((seg.b[axis] - nodeBox.b[axis] >= -0.01f || seg.t[axis] - nodeBox.b[axis] >= -0.01f)
            && (nodeBox.t[axis] - seg.b[axis] >= 0.01f || nodeBox.t[axis] - seg.t[axis] >= 0.01f)) {
            CAaBox posBox = nodeBox;
            posBox.b[axis] = node->planeDist;

            CAaBox negBox = nodeBox;
            negBox.t[axis] = node->planeDist;

            float d0 = seg.b[axis] - node->planeDist;
            float d1 = seg.t[axis] - node->planeDist;

            if ((d0 >= -0.01f && d0 <= 0.01f) || (d1 >= -0.01f && d1 <= 0.01f)) {
                if (node->posChild != CAABSP_NO_CHILD)
                    GetFaceIndices(node->posChild, seg, posBox);
                if (node->negChild != CAABSP_NO_CHILD)
                    GetFaceIndices(node->negChild, seg, negBox);
            } else if (d0 > 0.01f && d1 > 0.01f) {
                if (node->posChild != CAABSP_NO_CHILD)
                    GetFaceIndices(node->posChild, seg, posBox);
            } else if (d0 < -0.01f && d1 < -0.01f) {
                if (node->negChild != CAABSP_NO_CHILD)
                    GetFaceIndices(node->negChild, seg, negBox);
            } else {
                C3Vector mid = seg.Lerp(d0 / (d0 - d1));

                if (d0 > 0.0f) {
                    if (node->posChild != CAABSP_NO_CHILD)
                        GetFaceIndices(node->posChild,
                                       C3Segment(seg.b, mid), posBox);
                    if (node->negChild != CAABSP_NO_CHILD)
                        GetFaceIndices(node->negChild,
                                       C3Segment(mid, seg.t), negBox);
                } else {
                    if (node->negChild != CAABSP_NO_CHILD)
                        GetFaceIndices(node->negChild,
                                       C3Segment(seg.b, mid), negBox);
                    if (node->posChild != CAABSP_NO_CHILD)
                        GetFaceIndices(node->posChild,
                                       C3Segment(mid, seg.t), posBox);
                }
            }
        }
    }
};

#endif
