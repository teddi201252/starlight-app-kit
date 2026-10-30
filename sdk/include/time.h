#pragma once

#include <stddef.h>

typedef long time_t;
typedef long clock_t;
#define CLOCKS_PER_SEC 1000000l

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

time_t time(time_t *out);
struct tm *localtime(const time_t *value);
struct tm *localtime_r(const time_t *value, struct tm *out);
struct tm *gmtime(const time_t *value);
struct tm *gmtime_r(const time_t *value, struct tm *out);
time_t mktime(struct tm *tm);
time_t timegm(struct tm *tm);
double difftime(time_t a, time_t b);
char *asctime(const struct tm *tm);
char *asctime_r(const struct tm *tm, char *buf);
char *ctime(const time_t *value);
char *ctime_r(const time_t *value, char *buf);
size_t strftime(char *s, size_t max, const char *fmt, const struct tm *tm);
clock_t clock(void);
