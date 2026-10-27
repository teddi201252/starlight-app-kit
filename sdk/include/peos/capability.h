#pragma once

#include <stdint.h>

typedef uint64_t peos_cap_mask_t;

#define PEOS_CAP_FS_READ    (1ull << 0)
#define PEOS_CAP_FS_WRITE   (1ull << 1)
#define PEOS_CAP_PROCESS    (1ull << 2)
#define PEOS_CAP_DISPLAY    (1ull << 3)
#define PEOS_CAP_INPUT      (1ull << 4)
#define PEOS_CAP_AUDIO      (1ull << 5)
#define PEOS_CAP_NETWORK    (1ull << 6)
#define PEOS_CAP_MOUNT      (1ull << 7)
#define PEOS_CAP_POWER      (1ull << 8)
#define PEOS_CAP_DEBUG      (1ull << 9)
#define PEOS_CAP_SHM        (1ull << 10)
#define PEOS_CAP_INSTALL    (1ull << 11)
#define PEOS_CAP_MANAGE     (1ull << 12)
#define PEOS_CAP_NIC        (1ull << 13)   /* raw frames from the network card (netd only) */
#define PEOS_CAP_SERVICE    (1ull << 14)   /* register a name in the service registry */
#define PEOS_CAP_ALL        ((1ull << 15) - 1ull)

/* target_pid == 0 addresses the calling task. */
int64_t peos_cap_query(uint64_t target_pid, peos_cap_mask_t *out_caps);
int64_t peos_cap_grant(uint64_t target_pid, peos_cap_mask_t mask);
int64_t peos_cap_revoke(uint64_t target_pid, peos_cap_mask_t mask);

/* Limits the rights of the next peos_spawnv() child to (own rights & mask).
 * One-shot: it is consumed by that spawn, whether it succeeds or not. */
int64_t peos_cap_set_spawn_mask(peos_cap_mask_t mask);
