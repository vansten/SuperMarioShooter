#ifndef UTILITY_H
#define UTILITY_H

#include <math.h>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define clamp(a, min_val, max_val) min(max(a, min_val), max_val)
#define PI 3.14159f
#define DEG2RAD PI / 180.0f

#endif