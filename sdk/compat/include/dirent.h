/* libpeos-compat: reading a folder, over peos_readdir (std::filesystem). */
#pragma once
#include <sys/types.h>
#define DT_UNKNOWN 0
#define DT_DIR 4
#define DT_REG 8
#define DT_LNK 10
/* PeOS has no such files; the values are there for code that names them */
#define DT_FIFO 1
#define DT_CHR 2
#define DT_BLK 6
#define DT_SOCK 12
#define DT_WHT 14
struct dirent {
    ino_t d_ino;
    unsigned char d_type;
    char d_name[256];
};
typedef struct peos_dir DIR;
#ifdef __cplusplus
extern "C" {
#endif
DIR *opendir(const char *path);
struct dirent *readdir(DIR *dir);
int closedir(DIR *dir);
void rewinddir(DIR *dir);
int dirfd(DIR *dir);
#ifdef __cplusplus
}
#endif
