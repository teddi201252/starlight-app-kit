/* The GPU (the Intel GPU plan, phases 3 and 4b): native calls on the xe model.
 *
 * The GPU, its address spaces and work queues are descriptors, closing one
 * frees it; the end of a piece of work is a descriptor too, waited for with
 * peos_wait. A buffer is a number in the GPU object it was made in (not a
 * descriptor: a game makes thousands); closing that GPU object frees them
 * all. All need "gpu" (PEOS_CAP_GPU).
 *
 *   gpu = peos_gpu_open()                      a GPU object of its own (the first GPU)
 *   peos_gpu_info(gpu, &info)                  what it is
 *   vm = peos_gpu_vm_create(gpu)               an address space of the GPU, for gpu's buffers
 *   bo = peos_gpu_bo_create(gpu, size, 0)      a buffer (RAM pages): its number, 0 if none
 *   peos_gpu_bo_map(gpu, bo, &map)             ...in this program's memory
 *   peos_gpu_bo_close(gpu, bo)                 the number is free again
 *   peos_gpu_vm_bind(vm, ops, n)               buffers at addresses the
 *                                              program chooses (or gone)
 *   q = peos_gpu_queue_create(vm, ENGINE)      a work queue on that space
 *   done = peos_gpu_exec(q, &exec)             a batch run; `done` is ready
 *                                              (PEOS_WAIT_READ) when it ended
 *   peos_fread(done, &status, 4)               PEOS_GPU_DONE_OK or _ERROR
 *
 * The virtio-gpu 3D (virgl, docs/virgl-plan.md; info.kind VIRTIO) has no
 * address spaces, queues or batches (those answer -1 there). The GPU object
 * is a virgl context; a buffer is a 3D resource of the host, its pages the
 * resource's memory in PeOS:
 *   bo = peos_gpu_res_create(gpu, &res)        a resource: its number, 0 if none
 *   peos_gpu_transfer(gpu, &t)                 its pages to the host or back
 *   peos_gpu_capset(gpu, id, version, buf, n)  what the host's renderer offers
 *   (peos_gpu_bo_map and peos_gpu_bo_close as for any buffer)
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
 * (MAP; bo: the number of a buffer of the GPU object the space was made
 * from), or nothing at gpu_va..+size (UNMAP: bo ignored). All page
 * aligned; gpu_va below 2^va_bits. vm_bind returns when the GPU sees it;
 * an UNMAP first waits for the work running on that space. */
#define PEOS_GPU_BIND_MAP 1u
#define PEOS_GPU_BIND_UNMAP 2u
#define PEOS_GPU_BIND_MAX 64u
typedef struct {
    uint32_t op;
    uint32_t bo;
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

/* A virgl 3D resource (peos_gpu_res_create): the fields of the virtio-gpu
 * RESOURCE_CREATE_3D (target, format, bind are virgl's own values, as
 * Mesa's virgl_hw.h has them), and `size`, the bytes of its memory in
 * PeOS (what transfers copy from and to). */
typedef struct {
    uint32_t target, format, bind;
    uint32_t width, height, depth, array_size;
    uint32_t last_level, nr_samples, flags;
    uint64_t size;
} peos_gpu_res_t;

/* A copy between a resource's pages and the host's copy of it: the box
 * (x, y, z, w, h, d) of mip `level`, at `offset` in the pages, rows of
 * `stride` bytes, layers of `layer_stride` (0: worked out by the host). */
#define PEOS_GPU_TRANSFER_TO_HOST 1u
#define PEOS_GPU_TRANSFER_FROM_HOST 2u
typedef struct {
    uint32_t bo;
    uint32_t dir;           /* PEOS_GPU_TRANSFER_* */
    uint32_t level;
    uint32_t stride;
    uint32_t layer_stride;
    uint32_t x, y, z, w, h, d;
    uint64_t offset;
} peos_gpu_transfer_t;

/* The capsets a virtio-gpu host offers (peos_gpu_capset's id). */
#define PEOS_GPU_CAPSET_VIRGL 1u
#define PEOS_GPU_CAPSET_VIRGL2 2u
