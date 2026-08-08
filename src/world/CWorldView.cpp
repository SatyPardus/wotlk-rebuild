#include "world/CWorldView.hpp"

float CWorldView::s_fadeSize[] = {
    1.0f,
    4.0f,
    15.0f,
    100.0f,
    100000.0f
};

float CWorldView::s_fadeDistMax[] = {
    900.0f,
    10000.0f,
    40000.0f,
    562500.0f,
    1562500.0f
};

uint8_t CWorldView::GetFadeLevelForDistance(float dist) {
    if (dist < 0.0f)
        return 0;
    float distSquare = dist * dist;
    if (s_fadeDistMax[0] > distSquare)
        return 0;
    if (s_fadeDistMax[1] > distSquare)
        return 1;
    if (s_fadeDistMax[2] > distSquare)
        return 2;
    if (distSquare >= s_fadeDistMax[3])
        return 4;
    return 3;
}
