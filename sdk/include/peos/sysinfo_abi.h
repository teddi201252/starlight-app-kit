#pragma once

#include <stdint.h>

/* SYS_SYSINFO: what a desktop shows about the system, readable by any task
 * (no capability): the CPU time counters, the output volume, the time since
 * boot, memory and disk space. The full metrics (SYS_METRICS) stay behind the
 * debug capability. A caller built before a field was added passes a smaller
 * size and gets the fields it knows. */

typedef struct {
    uint32_t size;              /* sizeof(peos_sysinfo_t) as the kernel built it */
    uint32_t cpu_count;
    uint64_t tsc_hz;
    uint64_t cpu_busy_cycles;   /* summed over CPUs since boot: two samples give the load */
    uint64_t cpu_idle_cycles;
    uint32_t volume;            /* output volume 0..100 (audio_set_volume) */
    uint32_t reserved;
    uint64_t uptime_ns;         /* since the kernel started its clock */
    uint64_t ram_total;         /* bytes */
    uint64_t ram_free;
    uint64_t disk_total;        /* bytes, the root file system */
    uint64_t disk_free;
} peos_sysinfo_t;

/* The size of the first version (CPU time and volume only). */
#define PEOS_SYSINFO_V1_SIZE 40u
