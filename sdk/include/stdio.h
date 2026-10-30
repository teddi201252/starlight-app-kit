#pragma once
#include <stdarg.h>
#include <stddef.h>

typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define EOF (-1)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fd, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int feof(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);

int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int fprintf(FILE *stream, const char *fmt, ...);
int vfprintf(FILE *stream, const char *fmt, va_list ap);
int snprintf(char *buf, size_t len, const char *fmt, ...);
int vsnprintf(char *buf, size_t len, const char *fmt, va_list ap);
int sscanf(const char *str, const char *fmt, ...);

int putchar(int c);
int puts(const char *s);
int fputs(const char *s, FILE *stream);
int remove(const char *path);
int rename(const char *oldpath, const char *newpath);
int mkdir(const char *path, int mode);
int fflush(FILE *stream);
int fileno(FILE *stream);
int ferror(FILE *stream);
void clearerr(FILE *stream);
void rewind(FILE *stream);
int ungetc(int c, FILE *stream);
int getc(FILE *stream);
int putc(int c, FILE *stream);
int vprintf(const char *fmt, va_list ap);
int vsprintf(char *buf, const char *fmt, va_list ap);
int vsscanf(const char *str, const char *fmt, va_list ap);
int fscanf(FILE *stream, const char *fmt, ...);
void perror(const char *s);
#define BUFSIZ 4096
#define FILENAME_MAX 256
