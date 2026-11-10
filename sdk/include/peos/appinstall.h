/* Installing an app bundle from a tar stream (release 0.14.0, phase 4;
 * appget's, now shared with PeStore). The tar holds one <Name>.app folder
 * with its app.ini; it is unpacked beside the apps, in <apps>/.incoming,
 * and moved to <apps>/<Name>.app only once whole and, when asked, only if
 * its SHA-256 is the one expected: the dock never sees half an app, nor
 * one changed on the way. An installed bundle of the same name is replaced.
 *
 *   appinstall_t *ai = appinstall_begin("/apps");
 *   ...each piece of the download: appinstall_feed(ai, data, len)...
 *   appinstall_finish(ai, sha256_or_NULL)  -> 0, or -1 with ai->error
 *   appinstall_free(ai);
 */
#ifndef PEOS_APPINSTALL_H
#define PEOS_APPINSTALL_H

#include <stddef.h>
#include <stdint.h>

typedef struct appinstall appinstall_t;

struct appinstall {
    char apps[128];             /* where the apps are: /apps */
    char incoming[160];         /* <apps>/.incoming */
    char bundle[128];           /* the one top folder, <Name>.app */
    char error[128];            /* why it failed (English, for logs) */
    uint64_t bytes;             /* fed so far */
    uint32_t files;
    int replaced;               /* finish: a bundle of that name was there */
    void *priv;                 /* the tar reader and the hash */
};

/* NULL: no memory, or the incoming folder cannot be made. */
appinstall_t *appinstall_begin(const char *apps_dir);
/* 0 go on, 1 the archive ended, -1 broken (ai->error). */
int appinstall_feed(appinstall_t *ai, const uint8_t *data, size_t len);
/* The bundle into place. sha256: 32 bytes the whole stream must hash to,
 * or NULL. 0, or -1 with ai->error (nothing installed). */
int appinstall_finish(appinstall_t *ai, const uint8_t *sha256);
/* Frees; before appinstall_finish, gives up (the incoming folder goes). */
void appinstall_free(appinstall_t *ai);
/* Removes an installed bundle (<apps>/<Name>.app). 0, or -1. */
int appinstall_remove(const char *apps_dir, const char *bundle);

#endif
