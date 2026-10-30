#pragma once

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define M_E        2.71828182845904523536
#define M_LN2      0.69314718055994530942
#define M_LN10     2.30258509299404568402
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.78539816339744830962
#define M_SQRT2    1.41421356237309504880

#define HUGE_VAL  (__builtin_huge_val())
#define HUGE_VALF (__builtin_huge_valf())
#define INFINITY  (__builtin_inff())
#define NAN       (__builtin_nanf(""))

#define isnan(x)    __builtin_isnan(x)
#define isinf(x)    __builtin_isinf(x)
#define isfinite(x) __builtin_isfinite(x)
#define signbit(x)  __builtin_signbit(x)

static inline double fabs(double x) {
    return x < 0.0 ? -x : x;
}

static inline float fabsf(float x) {
    return x < 0.0f ? -x : x;
}

double ldexp(double value, int exponent);
double frexp(double value, int *exponent);
double sqrt(double x);
float sqrtf(float x);
double floor(double x);
float floorf(float x);
double ceil(double x);
float ceilf(float x);
double round(double x);
float roundf(float x);
double trunc(double x);
float truncf(float x);
long lround(double x);
double fmod(double x, double y);
float fmodf(float x, float y);
double modf(double x, double *ip);
double fmin(double a, double b);
double fmax(double a, double b);
float fminf(float a, float b);
float fmaxf(float a, float b);
double sin(double x);
float sinf(float x);
double cos(double x);
float cosf(float x);
double tan(double x);
float tanf(float x);
double atan(double x);
double atan2(double y, double x);
float atan2f(float y, float x);
double asin(double x);
double acos(double x);
float acosf(float x);
double exp(double x);
float expf(float x);
double log(double x);
float logf(float x);
double log2(double x);
double log10(double x);
double pow(double x, double y);
float powf(float x, float y);
double hypot(double x, double y);
