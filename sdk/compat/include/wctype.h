/* libpeos-compat: wctype.h, ASCII only. */
#pragma once
#include <wchar.h>
typedef unsigned long wctype_t;
typedef const int *wctrans_t;
#ifdef __cplusplus
extern "C" {
#endif
int iswalpha(wint_t c);
int iswdigit(wint_t c);
int iswspace(wint_t c);
int iswupper(wint_t c);
int iswlower(wint_t c);
int iswalnum(wint_t c);
int iswpunct(wint_t c);
int iswprint(wint_t c);
int iswcntrl(wint_t c);
int iswgraph(wint_t c);
int iswxdigit(wint_t c);
int iswblank(wint_t c);
wint_t towupper(wint_t c);
wint_t towlower(wint_t c);
wctype_t wctype(const char *name);
int iswctype(wint_t c, wctype_t t);
#ifdef __cplusplus
}
/* libstdc++ is built without wchar_t: <cwctype> puts none of these in std::
 * (Rift Ascend names std::towlower), so they come from here */
namespace std {
using ::iswalnum;
using ::iswalpha;
using ::iswblank;
using ::iswcntrl;
using ::iswctype;
using ::iswdigit;
using ::iswgraph;
using ::iswlower;
using ::iswprint;
using ::iswpunct;
using ::iswspace;
using ::iswupper;
using ::iswxdigit;
using ::towlower;
using ::towupper;
using ::wctype;
using ::wctype_t;
}
#endif
