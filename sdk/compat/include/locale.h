/* libpeos-compat: locales. PeOS's C library has one, "C" (libstdc++ is
 * built with the generic model): setlocale answers "C", localeconv C's. */
#pragma once
#include <stddef.h>
struct lconv {
    char *decimal_point, *thousands_sep, *grouping, *int_curr_symbol, *currency_symbol;
    char *mon_decimal_point, *mon_thousands_sep, *mon_grouping, *positive_sign, *negative_sign;
    char int_frac_digits, frac_digits, p_cs_precedes, p_sep_by_space, n_cs_precedes, n_sep_by_space;
    char p_sign_posn, n_sign_posn, int_p_cs_precedes, int_p_sep_by_space, int_n_cs_precedes;
    char int_n_sep_by_space, int_p_sign_posn, int_n_sign_posn;
};
#define LC_CTYPE 0
#define LC_NUMERIC 1
#define LC_TIME 2
#define LC_COLLATE 3
#define LC_MONETARY 4
#define LC_MESSAGES 5
#define LC_ALL 6
#ifdef __cplusplus
extern "C" {
#endif
char *setlocale(int category, const char *locale);
struct lconv *localeconv(void);
#ifdef __cplusplus
}
#endif
