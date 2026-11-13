/* The GPU (the Intel GPU plan, phase 3): native calls on the xe model.
 *
 * Everything is a descriptor, closing it frees it; the end of a piece of
 * work is a descriptor too, waited for with peos_wait. All need "gpu"
 * (PEOS_CAP_GPU).
 *
 *   gpu = peos_gpu_open()                      the GPU (the first one)
 *   peos_gpu_info(gpu, &info)                  what it is
 *   vm = peos_gpu_vm_create(gpu)               an address space of the GPU
 *   bo = peos_gpu_bo_create(gpu, size, 0)      a buffer (RAM pages)
 *   peos_gpu_bo_map(bo, &map)                  ...in this program's memory
 *   peos_gpu_vm_bind(vm, ops, n)               buffers at addresses the
 *                                              program chooses (or gone)
 *   q = peos_gpu_queue_create(vm, ENGINE)      a work queue on that space
 *   done = peos_gpu_exec(q, &exec)             a batch run; `done` is ready
 *                                              (PEOS_WAIT_READ) when it ended
 *   peos_fread(done, &status, 4)               PEOS_GPU_DONE_OK or _ERROR
 *
 * Shared by the kernel (kernel/abi/gpu_abi.h) and userland. */
#pragma once

#include <stdint.h>

#define PEOS_GPU_KIND_INTEL 1u
#define PEOS_GPU_KIND_VIRTIO 2u

typedef struct {
    uint32_t kind;          /* PEOS_GPU_KIND_* */
    uint16_t vendor;        /* PCI ids */
    uint16_t device;
    uint8_t revision;
    uint8_t gen;            /* Intel: the graphics generation (9 for Gen9) */
    uint8_t va_bits;        /* the address space's width (48) */
    uint8_t engines;        /* PEOS_GPU_ENGINE_* bits it has */
    uint64_t gtt_bytes;     /* the global GTT */
    uint64_t mem_bytes;     /* the memory buffers come from (system RAM) */
    /* Intel: the EUs the GPU really has (its fuses), what Mesa's
     * intel_device_info needs. Bit s of slice_mask: slice s; bit ss of
     * subslice_mask[s]: subslice ss of it; bit e of eu_mask[s][ss]: EU e. */
    uint8_t slice_mask;
    uint8_t subslice_mask[3];
    uint8_t eu_mask[3][4];
    uint32_t reserved[4];
} peos_gpu_info_t;

#define PEOS_GPU_ENGINE_RENDER 0u      /* engine numbers; the bit is 1 << n */

/* A buffer in this program's memory (peos_gpu_bo_map). */
typedef struct {
    uint64_t addr;
    uint64_t len;
} peos_gpu_map_t;

/* One change to an address space: bo_offset..+size of a buffer at gpu_va
 * (MAP), or nothing at gpu_va..+size (UNMAP: bo_fd ignored). All page
 * aligned; gpu_va below 2^va_bits. vm_bind returns when the GPU sees it;
 * an UNMAP first waits for the work running on that space. */
#define PEOS_GPU_BIND_MAP 1u
#define PEOS_GPU_BIND_UNMAP 2u
#define PEOS_GPU_BIND_MAX 64u
typedef struct {
    uint32_t op;
    int32_t bo_fd;
    uint64_t bo_offset;
    uint64_t gpu_va;
    uint64_t size;
} peos_gpu_bind_t;

/* GPU addresses: 48 bits, or "canonical" (bit 47 repeated above, as Mesa
 * keeps them). */
/* A batch to run: its address in the queue's space. The work starts after
 * the `done` descriptors in `waits` (n_waits of them, int32 each). */
#define PEOS_GPU_EXEC_MAX_WAITS 16u
typedef struct {
    uint64_t batch_va;
    uint64_t waits;         /* const int32_t *, or 0 */
    uint32_t n_waits;
    uint32_t flags;         /* 0 */
} peos_gpu_exec_t;

/* What a `done` descriptor reads (4 bytes, once it is ready). */
#define PEOS_GPU_DONE_OK 0u
#define PEOS_GPU_DONE_ERROR 1u   /* the work hung: the engine was reset, the work dropped */
