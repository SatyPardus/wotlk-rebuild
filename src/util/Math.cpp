#include "util/Math.hpp"
#include "tempest/Math.hpp"

float CalculateFacingTo(C3Vector* a1, C3Vector* a2) {
    auto v2 = a2->x - a1->x;
    auto v3 = a2->y - a1->y;
    if (fabs(v2) >= 0.00000023841858) {
        if (fabs(v3) >= 0.00000023841858) {
            return atan2(v3, v2);
        } else if (a1->x <= a2->x) {
            return 0.0;
        } else {
            return 3.1415927;
        }
    } else if (v3 >= 0.0) {
        return 0.5 * 3.1415927;
    } else {
        return 1.5 * 3.1415927;
    }
}
