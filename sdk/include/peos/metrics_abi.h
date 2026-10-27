#pragma once

#include <stdint.h>

/* Kernel metrics ABI shared by the kernel (via kernel/abi/metrics_abi.h) and
 * userland. Only stdint is needed so host tests can include it too.
 *
 * Latencies are recorded in TSC cycles in log-linear histograms (four buckets
 * per power of two, ~12% resolution). Convert with tsc_hz; it is 0 when the
 * kernel could not determine the TSC frequency. */

#define PEOS_METRICS_ABI_VERSION 5u
#define PEOS_METRICS_SYSCALL_SLOTS 128u
#define PEOS_METRICS_HIST_BUCKETS 256u
#define PEOS_METRICS_TASKS 16u
/* 64-bit values reach bucket 251 at most; 252..255 are never filled. */
#define PEOS_METRICS_HIST_USED 252u

typedef enum {
    PEOS_MH_SYSCALL = 0,   /* syscall entry to exit (only if never descheduled) */
    PEOS_MH_SCHED,         /* scheduler pass: pick next task and switch */
    PEOS_MH_IRQ_TIMER,     /* LAPIC timer interrupt, including the scheduler pass */
    PEOS_MH_IRQ_INPUT,     /* xHCI interrupt handler (USB input) */
    PEOS_MH_IRQ_AUDIO,     /* HDA interrupt handler */
    PEOS_MH_IRQ_NET,       /* network interrupt handler */
    PEOS_MH_INPUT_E2E,     /* input event stamped by the driver -> read by a task (NANOSECONDS).
                            * Only meaningful for drivers that stamp in their interrupt handler;
                            * today keyboard/mouse are read inside input_poll, so this is ~0 */
    PEOS_MH_TIMER_LATE,    /* a timeout's deadline -> the scheduler noticed it (TSC cycles) */
    PEOS_MH_COUNT
} peos_metrics_hist_t;

/* Histograms hold TSC cycles, except the ones this says are nanoseconds. */
static inline int peos_metrics_hist_is_ns(unsigned h) {
    return h == (unsigned)PEOS_MH_INPUT_E2E;
}

typedef struct {
    uint32_t pid;
    uint32_t parent;
    uint64_t cycles;        /* TSC cycles this task has spent running (sampled at scheduler entry) */
} peos_metrics_task_t;

typedef struct {
    uint32_t abi_version;
    uint32_t size;          /* sizeof(peos_metrics_t) as the kernel built it */
    uint32_t cpu_count;
    uint32_t reserved;
    uint64_t tsc_hz;
    uint64_t tsc_now;       /* TSC at snapshot time */
    peos_metrics_task_t tasks[PEOS_METRICS_TASKS];   /* busiest tasks since boot, most first; pid 0 = unused */

    uint64_t syscalls;              /* dispatched syscalls, including denied */
    uint64_t syscalls_denied;       /* rejected by capability policy */
    uint64_t syscalls_unmeasured;   /* task was descheduled: latency not recorded */
    uint64_t context_switches;
    uint64_t exceptions;            /* CPU exceptions (vectors 0..31) */
    uint64_t page_faults;
    uint64_t page_faults_stack_grow;/* user stack growth handled transparently */
    uint64_t irq_timer;
    uint64_t irq_input;
    uint64_t irq_audio;
    uint64_t irq_net;
    uint64_t irq_yield;
    uint64_t input_polls;           /* SYS_INPUT_POLL calls in raw mode */
    uint64_t input_polls_empty;     /* ... that returned no event */
    uint64_t input_events;          /* input events delivered to tasks (keys and mouse reports) */
    uint64_t cpu_busy_cycles;       /* CPU time running a task other than idle, summed over CPUs */
    uint64_t cpu_idle_cycles;       /* CPU time in the idle task, summed over CPUs */
    uint64_t task_blocks;           /* a task blocked on a wait queue or a timeout */
    uint64_t task_wakeups;          /* a blocked task was made runnable by sched_wake */
    uint64_t wake_ipis;             /* ... and another CPU had to be interrupted for it */
    uint64_t net_rx_frames;         /* frames taken from the NIC into the receive queue */
    uint64_t net_rx_dropped;        /* ... dropped: queue full or frame too long */
    uint64_t net_tx_frames;         /* frames handed to the NIC */
    uint64_t net_tx_ring_full;      /* sends refused because the NIC ring was full */
    uint64_t net_tx_dropped;        /* sends refused for another reason (too long, NIC down) */

    uint64_t syscall_by_id[PEOS_METRICS_SYSCALL_SLOTS];
    uint64_t hist[PEOS_MH_COUNT][PEOS_METRICS_HIST_BUCKETS];
} peos_metrics_t;

/* Values 0..7 get their own bucket, then four buckets per power of two. */
static inline uint32_t peos_metrics_bucket(uint64_t v) {
    if (v < 8u) return (uint32_t)v;
    uint32_t msb = 63u - (uint32_t)__builtin_clzll(v);
    uint32_t sub = (uint32_t)(v >> (msb - 2u)) & 3u;
    return 4u * (msb - 1u) + sub;
}

/* Smallest value that lands in bucket b. */
static inline uint64_t peos_metrics_bucket_floor(uint32_t b) {
    if (b < 8u) return b;
    if (b >= PEOS_METRICS_HIST_USED) return ~0ull;
    uint32_t msb = b / 4u + 1u;
    uint64_t sub = b % 4u;
    return (4u + sub) << (msb - 2u);
}

/* Largest value that lands in bucket b. */
static inline uint64_t peos_metrics_bucket_ceil(uint32_t b) {
    if (b + 1u >= PEOS_METRICS_HIST_USED) return ~0ull;
    return peos_metrics_bucket_floor(b + 1u) - 1u;
}

static inline uint64_t peos_metrics_hist_total(const uint64_t *hist) {
    uint64_t total = 0;
    for (uint32_t i = 0; i < PEOS_METRICS_HIST_BUCKETS; i++) total += hist[i];
    return total;
}

/* Conservative percentile: upper bound of the bucket holding the sample of
 * rank ceil(total * permille / 1000). Returns 0 for an empty histogram. */
static inline uint64_t peos_metrics_percentile(const uint64_t *hist, uint32_t permille) {
    uint64_t total = peos_metrics_hist_total(hist);
    if (total == 0u) return 0u;
    if (permille > 1000u) permille = 1000u;
    uint64_t rank = (total * permille + 999u) / 1000u;
    if (rank == 0u) rank = 1u;
    uint64_t seen = 0;
    for (uint32_t i = 0; i < PEOS_METRICS_HIST_BUCKETS; i++) {
        seen += hist[i];
        if (seen >= rank) return peos_metrics_bucket_ceil(i);
    }
    return peos_metrics_bucket_ceil(PEOS_METRICS_HIST_BUCKETS - 1u);
}

/* cycles -> nanoseconds without 128-bit math; 0 if the TSC rate is unknown. */
static inline uint64_t peos_metrics_cycles_to_ns(uint64_t cycles, uint64_t tsc_hz) {
    if (tsc_hz == 0u) return 0u;
    return (cycles / tsc_hz) * 1000000000ull + ((cycles % tsc_hz) * 1000000000ull) / tsc_hz;
}

/* b = a - b per element (a taken later); for windowed reports. */
static inline void peos_metrics_diff(peos_metrics_t *later_minus_earlier,
                                     const peos_metrics_t *earlier) {
    uint64_t *x = (uint64_t *)(void *)&later_minus_earlier->syscalls;
    const uint64_t *y = (const uint64_t *)(const void *)&earlier->syscalls;
    /* counters, syscall_by_id and hist are one contiguous run of uint64_t */
    uint64_t n = ((uint64_t)((const char *)(later_minus_earlier + 1) -
                             (const char *)&later_minus_earlier->syscalls)) / sizeof(uint64_t);
    for (uint64_t i = 0; i < n; i++) x[i] -= y[i];
}
