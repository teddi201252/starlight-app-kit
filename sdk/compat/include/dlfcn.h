/* libpeos-compat: dlfcn. PeOS programs are static: nothing is loaded at
 * run time (dlopen NULL, dlerror says so). */
#pragma once

#define RTLD_LAZY 0x1
#define RTLD_NOW 0x2
#define RTLD_LOCAL 0x0
#define RTLD_GLOBAL 0x100
#define RTLD_DEFAULT ((void *)0)
#define RTLD_NEXT ((void *)-1)
#define RTLD_NOLOAD 0x4
#define RTLD_NODELETE 0x1000

typedef struct {
    const char *dli_fname;
    void *dli_fbase;
    const char *dli_sname;
    void *dli_saddr;
} Dl_info;

#ifdef __cplusplus
extern "C" {
#endif
void *dlopen(const char *path, int flags);
void *dlsym(void *handle, const char *name);
int dlclose(void *handle);
char *dlerror(void);
int dladdr(const void *addr, Dl_info *info);
#ifdef __cplusplus
}
#endif
