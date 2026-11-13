/* libpeos-compat: unistd.h for ports, over the base's. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <peos-base/unistd.h>
#ifdef __cplusplus
}
#endif
#include <sys/types.h>

#define _POSIX_VERSION 200809L
#define _POSIX_THREADS 200809L
#define _POSIX_TIMEOUTS 200809L
#define _POSIX_MONOTONIC_CLOCK 200809L
#define _POSIX_TIMERS 200809L
#define _POSIX_SEMAPHORES -1
#define _POSIX_READER_WRITER_LOCKS 200809L
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define F_OK 0
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define _SC_NPROCESSORS_CONF 83
#define _SC_NPROCESSORS_ONLN 84
#define _SC_PAGESIZE 30
#define _SC_PAGE_SIZE _SC_PAGESIZE
#define _SC_CLK_TCK 2
#define _SC_PHYS_PAGES 85
#define _SC_AVPHYS_PAGES 86
#define _PC_PATH_MAX 4

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((noreturn)) void _exit(int status);
pid_t getpid(void);
uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
long sysconf(int name);
long pathconf(const char *path, int name);
int access(const char *path, int mode);
int usleep(unsigned int usec);
unsigned int sleep(unsigned int sec);
ssize_t readlink(const char *path, char *buf, size_t size);
int symlink(const char *target, const char *linkpath);
int link(const char *oldpath, const char *newpath);
int rmdir(const char *path);
int truncate(const char *path, off_t length);
int ftruncate(int fd, off_t length);
int fsync(int fd);
int dup(int fd);
int dup2(int fd, int fd2);
int pipe(int fds[2]);
int gethostname(char *name, size_t len);
int getpagesize(void);
/* PeOS starts programs with spawn: no fork, no exec (ENOSYS). */
pid_t fork(void);
pid_t getppid(void);
int execv(const char *path, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);
int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, ...);
#ifdef __cplusplus
}
#endif
