#include "util/Color.hpp"
#include <tempest/Vector.hpp>
#include <cstdint>

// OFFSET: 0x984F60
void RGBtoHSV(const C3Vector* rgb, C3Vector* hsv) {
    const int32_t major = rgb->MajorAxis();
    const int32_t minor = rgb->MinorAxis();

    const float maxComponent = (&rgb->x)[major];
    hsv->z = maxComponent;

    float saturation;

    if (maxComponent == 0.0f) {
        saturation = 0.0f;
    } else {
        saturation = (maxComponent - (&rgb->x)[minor]) / maxComponent;
    }

    hsv->y = saturation;

    if (saturation == 0.0f) {
        hsv->x = -1.0f;
        return;
    }

    const float delta = maxComponent - (&rgb->x)[minor];

    if (major == 0) {
        hsv->x = (rgb->y - rgb->z) / delta;
    } else if (major == 1) {
        hsv->x = (rgb->z - rgb->x) / delta + 2.0f;
    } else if (major == 2) {
        hsv->x = (rgb->x - rgb->y) / delta + 4.0f;
    }

    hsv->x = hsv->x * 60.0f;

    if (hsv->x < 0.0f) {
        hsv->x = hsv->x + 360.0f;
    }
}

// OFFSET: 0x985030
void HSVtoRGB(const C3Vector* hsv, C3Vector* rgb) {
    if (hsv->y == 0.0f) {
        rgb->x = hsv->z;
        rgb->y = hsv->z;
        rgb->z = hsv->z;
        return;
    }

    float hue = hsv->x;

    if (hue >= 360.0f) {
        hue = hue - 360.0f;
    }

    hue = hue * 0.016666668f;

    int32_t sector = (int32_t)hue;

    if (sector > 5) {
        sector = 5;
    }

    const float fraction = hue - (float)sector;

    float saturation;

    if (hsv->y >= 1.0f) {
        saturation = 1.0f;
    } else {
        saturation = hsv->y;
    }

    const float p = (1.0f - saturation) * hsv->z;
    const float q = (1.0f - saturation * fraction) * hsv->z;
    const float t = (1.0f - saturation * (1.0f - fraction)) * hsv->z;

    switch (sector) {
    case 0:
        rgb->x = hsv->z;
        rgb->y = t;
        rgb->z = p;
        break;
    case 1:
        rgb->x = q;
        rgb->y = hsv->z;
        rgb->z = p;
        break;
    case 2:
        rgb->x = p;
        rgb->y = hsv->z;
        rgb->z = t;
        break;
    case 3:
        rgb->x = p;
        rgb->y = q;
        rgb->z = hsv->z;
        break;
    case 4:
        rgb->x = t;
        rgb->y = p;
        rgb->z = hsv->z;
        break;
    case 5:
        rgb->x = hsv->z;
        rgb->y = p;
        rgb->z = q;
        break;
    default:
        return;
    }
}
