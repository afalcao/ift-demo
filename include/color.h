#ifndef _COLOR_H_
#define _COLOR_H_

#include "common.h"

/// RGB to Lab using sRGB->XYZ matrix and white point
void Lab2RGB(float* l, float* a, float* b);
void RGB2Lab(float* r, float* g, float * b);

#endif
