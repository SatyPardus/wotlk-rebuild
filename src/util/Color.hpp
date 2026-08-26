#ifndef UTIL_COLOR_HPP
#define UTIL_COLOR_HPP

class C3Vector;

void RGBtoHSV(const C3Vector* rgb, C3Vector* hsv);

void HSVtoRGB(const C3Vector* hsv, C3Vector* rgb);

#endif
