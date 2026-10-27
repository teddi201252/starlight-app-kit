#pragma once

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static inline double fabs(double x) {
    return x < 0.0 ? -x : x;
}

static inline float fabsf(float x) {
    return x < 0.0f ? -x : x;
}

double ldexp(double value, int exponent);

