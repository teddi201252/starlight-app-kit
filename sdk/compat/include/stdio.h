/* libpeos-compat: the C stdio a port expects, over the base's (its FILE). */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <peos-base/stdio.h>
#ifdef __cplusplus
}
#endif
#include <sys/types.h>

typedef long fpos_t;
#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2
#define L_tmpnam 32
#define TMP_MAX 10000
#define FOPEN_MAX 64
#define P_tmpdir "/tmp"

#ifdef __cplusplus
extern "C" {
#endif
int fgetpos(FILE *stream, fpos_t *pos);
int fsetpos(FILE *stream, const fpos_t *pos);
FILE *freopen(const char *path, const char *mode, FILE *stream);
void setbuf(FILE *stream, char *buf);
int setvbuf(FILE *stream, char *buf, int mode, size_t size);
FILE *tmpfile(void);
char *tmpnam(char *s);
int getchar(void);
int scanf(const char *fmt, ...);
int vscanf(const char *fmt, va_list ap);
int vfscanf(FILE *stream, const char *fmt, va_list ap);
int fseeko(FILE *stream, off_t offset, int whence);
off_t ftello(FILE *stream);
int dprintf(int fd, const char *fmt, ...);
int vdprintf(int fd, const char *fmt, va_list ap);
ssize_t getline(char **line, size_t *cap, FILE *stream);
ssize_t getdelim(char **line, size_t *cap, int delim, FILE *stream);
#ifdef __cplusplus
}
#endif
