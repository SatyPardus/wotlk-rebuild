#ifndef WORLD_CWORLDMATH_HPP
#define WORLD_CWORLDMATH_HPP

#include <cstdint>
#include "tempest/Matrix.hpp"
#include "tempest/Box.hpp"

class CWorldMath {
    public:
    static void TransformAABox(C44Matrix& m, CAaBox& src, CAaBox& dst);
};

#endif
