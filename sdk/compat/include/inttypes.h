/* libpeos-compat: inttypes.h, the base's formats and the functions. */
#pragma once
#include <peos-base/inttypes.h>
#include <stddef.h>
#define PRIi8 "i"
#define PRIi16 "i"
#define PRIo32 "o"
#define PRIo64 "lo"
#define PRIdMAX "ld"
#define PRIuMAX "lu"
#define PRIxMAX "lx"
#define PRIiMAX "li"
#define PRIoMAX "lo"
#define PRIXMAX "lX"
#define PRIiPTR "li"
#define PRIoPTR "lo"
#define PRIXPTR "lX"
#define SCNdMAX "ld"
#define SCNiMAX "li"
#define SCNuMAX "lu"
#define SCNxMAX "lx"
#define SCNd8 "hhd"
#define SCNd16 "hd"
#define SCNi64 "li"
#define SCNdPTR "ld"
#define SCNiPTR "li"
#define SCNuPTR "lu"
#define SCNxPTR "lx"
#define SCNoPTR "lo"
typedef struct { intmax_t quot, rem; } imaxdiv_t;
#ifdef __cplusplus
extern "C" {
#endif
intmax_t imaxabs(intmax_t v);
imaxdiv_t imaxdiv(intmax_t a, intmax_t b);
intmax_t strtoimax(const char *s, char **endp, int base);
uintmax_t strtoumax(const char *s, char **endp, int base);
#ifdef __cplusplus
}
#endif
