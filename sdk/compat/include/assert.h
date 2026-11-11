/* libpeos-compat: C's assert for ports (the base's never checks). It checks
 * unless NDEBUG is defined where it is included, and may be included again
 * with NDEBUG changed, as C wants: no include guard. A failed one prints
 * where and what to stderr, then abort(). */
#undef assert
#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#ifdef __cplusplus
extern "C"
#endif
__attribute__((noreturn)) void __assert_fail(const char *expr, const char *file, unsigned int line,
                                             const char *func);
#define assert(x) ((x) ? (void)0 : __assert_fail(#x, __FILE__, __LINE__, __func__))
#endif

#if !defined(__cplusplus) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && \
    __STDC_VERSION__ < 202311L && !defined(static_assert)
#define static_assert _Static_assert
#endif
