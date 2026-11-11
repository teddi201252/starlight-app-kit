/* libpeos-compat: math.h for ports, the whole of C99 and XSI, all of it
 * musl's libm: exact float and double, true 80-bit long double. Unlike the
 * base's math.h it has no static inline functions (fabs, fabsf are real
 * ones here, as C++'s <cmath> wants); the constants and macros are the same. */
#pragma once

#define M_E        2.71828182845904523536
#define M_LOG2E    1.44269504088896340736
#define M_LOG10E   0.43429448190325182765
#define M_LN2      0.69314718055994530942
#define M_LN10     2.30258509299404568402
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.78539816339744830962
#define M_1_PI     0.31830988618379067154
#define M_2_PI     0.63661977236758134308
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2    1.41421356237309504880
#define M_SQRT1_2  0.70710678118654752440

#define HUGE_VAL  (__builtin_huge_val())
#define HUGE_VALF (__builtin_huge_valf())
#define INFINITY  (__builtin_inff())
#define NAN       (__builtin_nanf(""))

/* also in C++: <cmath> undefines them and puts its templates in their place */
#define isnan(x)    __builtin_isnan(x)
#define isinf(x)    __builtin_isinf(x)
#define isfinite(x) __builtin_isfinite(x)
#define signbit(x)  __builtin_signbit(x)

#ifdef __cplusplus
extern "C" {
#endif
double fabs(double x);
float fabsf(float x);
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
#ifdef __cplusplus
}
#endif

#define fpclassify(x) __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)
#define isnormal(x) __builtin_isnormal(x)
#define isgreater(x, y) __builtin_isgreater(x, y)
#define isgreaterequal(x, y) __builtin_isgreaterequal(x, y)
#define isless(x, y) __builtin_isless(x, y)
#define islessequal(x, y) __builtin_islessequal(x, y)
#define islessgreater(x, y) __builtin_islessgreater(x, y)
#define isunordered(x, y) __builtin_isunordered(x, y)
#define FP_NAN 0
#define FP_INFINITE 1
#define FP_ZERO 2
#define FP_SUBNORMAL 3
#define FP_NORMAL 4
#define HUGE_VALL (__builtin_huge_vall())
#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2
#define math_errhandling MATH_ERREXCEPT
#define FP_ILOGB0 (-2147483647 - 1)
#define FP_ILOGBNAN (-2147483647 - 1)
typedef float float_t;
typedef double double_t;

#ifdef __cplusplus
extern "C" {
#endif
#define PEOS_MATH3(name) float name##f(float); long double name##l(long double);
#define PEOS_MATH(name) double name(double); PEOS_MATH3(name)
/* the families, beside the forms declared above */
PEOS_MATH(acosh) PEOS_MATH(asinh) PEOS_MATH(atanh) PEOS_MATH(cbrt) PEOS_MATH(cosh) PEOS_MATH(sinh) PEOS_MATH(tanh)
PEOS_MATH(erf) PEOS_MATH(erfc) PEOS_MATH(exp2) PEOS_MATH(expm1) PEOS_MATH(lgamma) PEOS_MATH(tgamma)
PEOS_MATH(log1p) PEOS_MATH(logb) PEOS_MATH(nearbyint) PEOS_MATH(rint)
float asinf(float); long double asinl(long double);
long double acosl(long double); float atanf(float); long double atanl(long double);
long double atan2l(long double, long double); long double ceill(long double); long double cosl(long double);
long double expl(long double); long double fabsl(long double); long double floorl(long double);
long double fmodl(long double, long double); float frexpf(float, int *); long double frexpl(long double, int *);
float ldexpf(float, int); long double ldexpl(long double, int); long double logl(long double);
float log10f(float); long double log10l(long double); float log2f(float); long double log2l(long double);
float modff(float, float *); long double modfl(long double, long double *); long double powl(long double, long double);
long double sinl(long double); long double sqrtl(long double); long double tanl(long double);
long double roundl(long double); long double truncl(long double);
float hypotf(float, float); long double hypotl(long double, long double);
double copysign(double, double); float copysignf(float, float); long double copysignl(long double, long double);
double fdim(double, double); float fdimf(float, float); long double fdiml(long double, long double);
double fma(double, double, double); float fmaf(float, float, float); long double fmal(long double, long double, long double);
long double fmaxl(long double, long double); long double fminl(long double, long double);
int ilogb(double); int ilogbf(float); int ilogbl(long double);
long long llrint(double); long long llrintf(float); long long llrintl(long double);
long long llround(double); long long llroundf(float); long long llroundl(long double);
long lrint(double); long lrintf(float); long lrintl(long double);
long lroundf(float); long lroundl(long double);
double nan(const char *); float nanf(const char *); long double nanl(const char *);
double nextafter(double, double); float nextafterf(float, float); long double nextafterl(long double, long double);
double nexttoward(double, long double); float nexttowardf(float, long double); long double nexttowardl(long double, long double);
double remainder(double, double); float remainderf(float, float); long double remainderl(long double, long double);
double remquo(double, double, int *); float remquof(float, float, int *); long double remquol(long double, long double, int *);
double scalbln(double, long); float scalblnf(float, long); long double scalblnl(long double, long);
double scalbn(double, int); float scalbnf(float, int); long double scalbnl(long double, int);
void sincos(double, double *, double *); void sincosf(float, float *, float *); void sincosl(long double, long double *, long double *);
/* XSI and musl's extras, which libm itself uses */
double j0(double); double j1(double); double jn(int, double); double y0(double); double y1(double); double yn(int, double);
float j0f(float); float j1f(float); float jnf(int, float); float y0f(float); float y1f(float); float ynf(int, float);
double lgamma_r(double, int *); float lgammaf_r(float, int *); long double lgammal_r(long double, int *);
extern int signgam;
double drem(double, double); float dremf(float, float);
int finite(double); int finitef(float);
double scalb(double, double); float scalbf(float, float);
double significand(double); float significandf(float);
double exp10(double); float exp10f(float); long double exp10l(long double);
double pow10(double); float pow10f(float); long double pow10l(long double);
#undef PEOS_MATH
#undef PEOS_MATH3
#ifdef __cplusplus
}
#endif
