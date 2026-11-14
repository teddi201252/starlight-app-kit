#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stddef.h>
#include "peos/alloc.h"
#include "peos/wifi.h"
#include "peos/capability.h"
#include "peos/metrics_abi.h"
#include "peos/sysinfo_abi.h"
#include "peos/proc_abi.h"
#include "peos/path_scope_abi.h"
#include "peos/event_abi.h"
#include "peos/disks.h"
#include "peos/gpu.h"

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint16_t bpp;
    uint8_t rshift, gshift, bshift;
    uint8_t rsize, gsize, bsize;
} peos_fb_info_t;

typedef struct {
    uint64_t addr;
    uint64_t len;
} peos_fb_map_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} peos_fb_rect_t;

typedef struct {
    uint64_t addr;
    uint64_t len;
} peos_shm_map_t;

typedef struct {
    uint8_t ip[4];
    uint8_t netmask[4];
    uint8_t gateway[4];
    uint8_t dns[4];
    uint8_t mac[6];        // filled by netcfg_get (all zero when there is no NIC)
    uint8_t reserved[2];
} peos_netcfg_t;

typedef struct {
    char name[32];
    uint8_t type;
    uint8_t reserved[3];
    uint32_t sector_size;
    uint64_t sector_count;
} peos_install_disk_t;

// Where PeOS can be installed on a disk (release 0.17.0, phase 4): the whole
// disk, a free space of its GPT, or one of its partitions put in place.
enum {
    PEOS_INSTALL_PLACE_DISK = 0,        // the whole disk: everything on it goes
    PEOS_INSTALL_PLACE_FREE = 1,        // a free space: nothing else is touched
    PEOS_INSTALL_PLACE_PARTITION = 2,   // a partition, erased and replaced
};
enum {
    PEOS_INSTALL_WHY_OK = 0,
    PEOS_INSTALL_WHY_SMALL = 1,         // under 8 GiB, or the system does not fit
    PEOS_INSTALL_WHY_PROTECTED = 2,     // macOS (APFS, Recovery), an EFI partition, Windows's reserved/recovery
    PEOS_INSTALL_WHY_IN_USE = 3,        // mounted, or the system started from it
    PEOS_INSTALL_WHY_READ_ONLY = 4,     // the disk is not writable
};
typedef struct {
    char target[32];        // what peos_install_run takes: "nvme0n1", "nvme0n1p3", "nvme0n1@2048"
    uint8_t kind;           // PEOS_INSTALL_PLACE_*
    uint8_t part_type;      // a partition's PEOS_PART_* (peos/disks.h)
    uint8_t usable;         // 1 when the installer may use it
    uint8_t why;            // PEOS_INSTALL_WHY_* when not
    uint32_t sector_size;
    uint64_t first_lba;     // where it starts and how long it is, in the disk's sectors
    uint64_t lba_count;
} peos_install_place_t;

typedef struct {
    uint32_t status;
    uint32_t phase;
    uint32_t percent;
    int32_t result;
    uint64_t bytes_done;
    uint64_t bytes_total;
    char target[32];
    char message[64];
} peos_install_progress_t;

typedef struct {
    uint64_t size;
    uint64_t mtime_ns;
    uint64_t ctime_ns;
    uint8_t is_dir;
    uint8_t _pad[7];
} peos_stat_t;

typedef struct {
    const char *buf;
    size_t len;
    size_t pos;
} peos_dir_iter_t;

#ifndef PEOS_INPUT_EVENT_T_DEFINED
#define PEOS_INPUT_EVENT_T_DEFINED 1
typedef struct peos_input_event {
    uint16_t code;      // keycode interno
    uint8_t pressed;    // 1=down, 0=up
    uint8_t reserved;
    uint64_t time_ns;
} peos_input_event_t;
#endif

#ifndef PEOS_MOUSE_EVENT_T_DEFINED
#define PEOS_MOUSE_EVENT_T_DEFINED 1
typedef struct peos_mouse_event {
    int32_t dx;
    int32_t dy;
    uint8_t buttons;
    int8_t wheel_y;
    int8_t wheel_x;
    uint8_t reserved;
    uint64_t time_ns;
} peos_mouse_event_t;
#endif

static inline uint64_t align_up(uint64_t v, uint64_t a) { return (v + a - 1) & ~(a - 1); }

int atoi(const char *s);
// Minimal userland API built on top of PeOS syscalls.

uint64_t peos_write(const void *buf, size_t len);
uint64_t peos_write_fd(uint64_t fd, const void *buf, size_t len);
uint64_t peos_puts(const char *s);
uint64_t peos_read(void *buf, size_t len);
uint64_t peos_read_fd(uint64_t fd, void *buf, size_t len);
void peos_clear(void);
uint64_t peos_exec(const char *path, size_t len);
uint64_t peos_execv(const char *path, size_t len, const char *arg_str);
uint64_t peos_getpid(void);
/* Fills *out with kernel counters and latency histograms (needs PEOS_CAP_DEBUG). */
int64_t peos_metrics(peos_metrics_t *out);
// CPU time counters, volume, uptime, memory and disk, for any task (SYS_SYSINFO).
int64_t peos_sysinfo(peos_sysinfo_t *out);
// The processes running (SYS_PROC_LIST): count written to out, or -1.
int64_t peos_proc_list(peos_proc_info_t *out, size_t max);
// End a process (SYS_PROC_KILL, needs PEOS_CAP_MANAGE): 0 or -1.
int64_t peos_proc_kill(uint64_t pid);
// The kernel watchdog (SYS_WATCHDOG, needs PEOS_CAP_DEBUG): a CPU stuck for
// 5 s says where on the screen. Off by default. 1 on, 0 off, or -1.
#define PEOS_WATCHDOG_STATUS 0
#define PEOS_WATCHDOG_ON 1
#define PEOS_WATCHDOG_OFF 2
int64_t peos_watchdog(int op);
// An ACPI table of the firmware, by index (SYS_ACPI_TABLE, needs
// PEOS_CAP_DEBUG): the table's whole length (copies what fits in len), or -1
// past the last one. The tables of the XSDT, then the DSDT.
int64_t peos_acpi_table(uint32_t index, void *buf, size_t len);
/* The GPU (peos/gpu.h): descriptors, or -1; 0/-1 for info, map and bind. */
int64_t peos_gpu_open(void);
int64_t peos_gpu_info(int64_t gpu, peos_gpu_info_t *out);
int64_t peos_gpu_vm_create(int64_t gpu);
int64_t peos_gpu_bo_create(int64_t gpu, uint64_t size, uint32_t flags);
int64_t peos_gpu_bo_map(int64_t bo, peos_gpu_map_t *out);
int64_t peos_gpu_vm_bind(int64_t vm, const peos_gpu_bind_t *ops, uint32_t count);
int64_t peos_gpu_queue_create(int64_t vm, uint32_t engine);
int64_t peos_gpu_exec(int64_t queue, const peos_gpu_exec_t *exec);
// Cryptographic random bytes from the kernel (SYS_RANDOM, any task): len or -1.
int64_t peos_random(void *buf, size_t len);
/* Path scope: narrow what this task (and its children) may touch. Never widens. */
int64_t peos_path_scope_set(const peos_path_grant_t *grants, size_t count);
/* Path scope for the next peos_spawnv() child: (own scope narrowed by grants). One-shot. */
int64_t peos_path_set_spawn_scope(const peos_path_grant_t *grants, size_t count);
int64_t peos_path_scope_query(peos_path_scope_t *out);
/* Event model. Every descriptor has rights (PEOS_FD_*) that can only shrink. */
int64_t peos_fd_restrict(uint64_t fd, uint32_t keep_rights);
int64_t peos_fd_rights(uint64_t fd);
/* A second descriptor of the same object, with the same rights. */
int64_t peos_fd_dup(uint64_t fd);
/* Spawned children get only 0/1/2; list extra descriptors for the next spawnv here. */
int64_t peos_spawn_set_fds(const peos_spawn_fd_t *fds, size_t count);
/* Wait until one item is ready or timeout_ns passes (PEOS_WAIT_FOREVER, 0 = poll).
 * Returns the number of ready items (their revents are set), 0 on timeout, -1 on error. */
int64_t peos_wait(peos_wait_item_t *items, size_t count, uint64_t timeout_ns);
/* Timer descriptor: read() returns a uint64_t count of expirations. */
int64_t peos_timer_create(void);
int64_t peos_timer_set(uint64_t fd, uint64_t initial_ns, uint64_t interval_ns);
/* Process descriptor for a direct child: readable when it exits; read() returns
 * its int32_t exit code and reaps it. */
int64_t peos_proc_open(uint64_t pid);
/* Input endpoint: read() returns peos_input_record_t; wait on it with PEOS_WAIT_READ. */
int64_t peos_input_open(void);
/* Playback stream: write 48 kHz s16le stereo PCM. A write waits for room
 * (peos_fwrite_nb does not); wait with PEOS_WAIT_WRITE for space for a period. */
int64_t peos_audio_open(void);
/* The network card as a descriptor of whole Ethernet frames (needs "nic"):
 * read one frame per call into a PEOS_NIC_FRAME_MAX buffer, write one frame
 * per call. Exclusive: netd holds it. The network itself is peos/net.h. */
int64_t peos_nic_open(uint32_t index);
int64_t peos_nic_info(int64_t fd, peos_nic_info_t *out);
/* Service registry: register a name (needs "service") to get a listening
 * channel; connect to a name to get a channel to its service. */
int64_t peos_svc_register(const char *name);
int64_t peos_svc_connect(const char *name);
/* Channels (see event_abi.h). send/recv return 0 / byte count, PEOS_CHANNEL_AGAIN,
 * PEOS_CHANNEL_CLOSED or -1. */
int64_t peos_channel_create(int64_t out_pair[2]);
int64_t peos_channel_send(int64_t fd, const peos_channel_msg_t *msg, uint64_t flags);
int64_t peos_channel_recv(int64_t fd, peos_channel_msg_t *msg, uint64_t flags);
int64_t peos_thread_spawn(void (*entry)(void *), void *arg, void *stack_top);
uint64_t peos_waitpid(uint64_t pid, int *status);
uint64_t peos_waitpid_flags(uint64_t pid, int *status, uint64_t flags);
uint64_t peos_pipe(uint64_t out_pair[2]);
int64_t peos_spawnv(const char *path, size_t len, const char *arg_str,
                    int64_t stdin_fd, int64_t stdout_fd, int64_t stderr_fd);
uint64_t peos_fopen(const char *path, size_t len, uint64_t flags);
uint64_t peos_fread(uint64_t fd, void *buf, size_t len);
uint64_t peos_fwrite(uint64_t fd, const void *buf, size_t len);
uint64_t peos_fread_nb(uint64_t fd, void *buf, size_t len);
uint64_t peos_fwrite_nb(uint64_t fd, const void *buf, size_t len);
uint64_t peos_fseek(uint64_t fd, size_t pos);
uint64_t peos_fsize(uint64_t fd);
uint64_t peos_fclose(uint64_t fd);
uint64_t peos_chdir(const char *path, size_t len);
uint64_t peos_getcwd(char *buf, size_t len);
/* The names in a folder, "name\n" (a folder "name/\n"); out_len at most
 * PEOS_READDIR_MAX (a larger one gets nothing). Filled to out_len - 1 or
 * more: there may be more names than fit. */
#define PEOS_READDIR_MAX 65536u
uint64_t peos_readdir(const char *path, size_t path_len, char *out, size_t out_len);
int64_t peos_readdir_begin(const char *path, size_t path_len,
                           char *buf, size_t buf_len, peos_dir_iter_t *it);
int64_t peos_readdir_next(peos_dir_iter_t *it, char *name, size_t name_len);
uint64_t peos_stat(const char *path, size_t path_len, peos_stat_t *out, size_t out_len);
uint64_t peos_mkdir(const char *path, size_t len);
uint64_t peos_unlink(const char *path, size_t len);
uint64_t peos_rename(const char *oldpath, size_t old_len, const char *newpath, size_t new_len);
uint64_t peos_rmdir(const char *path, size_t len, uint64_t flags);
int64_t peos_exists(const char *path, size_t path_len);
int64_t peos_is_dir(const char *path, size_t path_len);
int64_t peos_is_file(const char *path, size_t path_len);
int64_t peos_mkdir_p(const char *path, size_t path_len);
int64_t peos_read_file(const char *path, size_t path_len, void *out, size_t out_cap, size_t *out_size);
int64_t peos_write_file(const char *path, size_t path_len, const void *data, size_t data_len, uint64_t open_flags);
int64_t peos_path_join(const char *base, const char *name, char *out, size_t out_len);
int64_t peos_path_basename(const char *path, char *out, size_t out_len);
int64_t peos_path_parent(const char *path, char *out, size_t out_len);
uint64_t peos_mount(const char *dev, size_t dev_len, const char *mount_path, size_t mount_len,
                    const char *fs_name, size_t fs_len);
uint64_t peos_umount(const char *mount_path, size_t mount_len);
uint64_t peos_mounts(char *out, size_t out_len);
void peos_sleep(uint64_t ticks);
__attribute__((noreturn)) void peos_exit(uint64_t code);
uint64_t peos_poweroff(void);
uint64_t peos_reboot(void);
uint64_t peos_time(void);
uint64_t peos_time_ns(void);
uint64_t peos_hwinfo(uint64_t *ram_total, uint64_t *ram_free,
                     uint64_t *disk_total, uint64_t *disk_free);
int64_t peos_hwinfo_report(char *buf, size_t len);
uint64_t peos_brk(uint64_t addr);
uint64_t peos_mmap(uint64_t addr, uint64_t len, uint64_t prot, uint64_t flags);
uint64_t peos_munmap(uint64_t addr, uint64_t len);
// Pages of an anonymous mapping (SYS_MRELEASE): PEOS_MRELEASE_OUT gives their
// memory back but keeps the range reserved (touching it then is a fault);
// PEOS_MRELEASE_IN maps zeroed pages there again. 0 or -1.
#define PEOS_MRELEASE_OUT 0u
#define PEOS_MRELEASE_IN 1u
int64_t peos_mrelease(uint64_t addr, uint64_t len, uint32_t op);
int64_t peos_shm_create(size_t len);
int64_t peos_shm_map(uint64_t id, peos_shm_map_t *out);
int64_t peos_shm_destroy(uint64_t id);
/* Shared memory as a descriptor: freed when the last descriptor closes and it
 * was never mapped. Mapping is read-only when the descriptor lacks PEOS_FD_WR. */
int64_t peos_shm_create_fd(size_t len);
int64_t peos_shm_map_fd(int64_t fd, peos_shm_map_t *out);
/* Let `pid` and its descendants map an id-based object (creator only). */
int64_t peos_shm_allow(uint64_t id, uint64_t pid);
uint64_t peos_fbinfo(peos_fb_info_t *out);
uint64_t peos_fbpresent(const void *buf, size_t len, uint64_t flags);
int64_t peos_fbpresent_damage(const void *frame, size_t frame_len,
                              const peos_fb_rect_t *rects, size_t rect_count,
                              uint64_t flags);
int64_t peos_fbmap(peos_fb_map_t *out, uint64_t flags);
int64_t peos_fb_set_mode(uint32_t width, uint32_t height);
int64_t peos_cursor_set_visible(uint64_t visible);
int64_t peos_cursor_move_abs(uint32_t x, uint32_t y);
int64_t peos_cursor_set_image_rgba(const void *rgba, uint32_t width, uint32_t height,
                                   uint32_t hot_x, uint32_t hot_y);
int64_t peos_bootlog_snapshot(void *buf, size_t len);
uint64_t peos_input_mode(uint64_t mode);
uint64_t peos_input_poll(peos_input_event_t *out, size_t max_events);
int64_t peos_input_mouse(peos_mouse_event_t *out);
int peos_net_parse_ipv4(const char *s, uint8_t out[4]);
int peos_net_dns_resolve_a(const char *host, uint8_t out_ip[4], uint32_t timeout_ticks);
int peos_net_resolve_ipv4(const char *host_or_ip, uint8_t out_ip[4], uint32_t timeout_ticks);
int64_t peos_audio_set_log_level(uint32_t level);
int64_t peos_install_list_disks(peos_install_disk_t *out, size_t max_entries);
int64_t peos_install_get_progress(peos_install_progress_t *out);
/* The places of one disk ("nvme0n1"): the whole disk first, then in disk
 * order its partitions and free spaces (1 GiB or more). Count, or -1. */
int64_t peos_install_list_places(const char *disk, size_t len, peos_install_place_t *out, size_t max_entries);
/* target: a disk (all of it), a partition (put in place: "nvme0n1p3") or a
 * free space ("nvme0n1@<first LBA>"), as peos_install_list_places gives. */
int64_t peos_install_run(const char *target, size_t len);

size_t peos_strlen(const char *s);
void *peos_memset(void *dst, int c, size_t n);
void *peos_memcpy(void *dst, const void *src, size_t n);

// Open flags (userland)
#define PEOS_O_READ  0x0u
#define PEOS_O_CREAT 0x1u
#define PEOS_O_TRUNC 0x2u
#define PEOS_O_WRITE 0x4u
#define PEOS_FD_NONBLOCK_BIT (1ull << 63)

#define PEOS_RMDIR_RECURSIVE 0x1u
#define PEOS_PATH_MAX 1024u

// mmap prot flags
#define PEOS_PROT_READ   (1u << 0)
#define PEOS_PROT_WRITE  (1u << 1)
#define PEOS_PROT_EXEC   (1u << 2)

// mmap map flags
#define PEOS_MAP_PRIVATE 0x02u
#define PEOS_MAP_FIXED   0x10u
#define PEOS_MAP_ANON    0x20u

// Input modes
#define PEOS_INPUT_MODE_COOKED 0u
#define PEOS_INPUT_MODE_RAW    1u

#define PEOS_WAIT_NOHANG 0x1u

#define PEOS_INSTALL_STATUS_IDLE 0u
#define PEOS_INSTALL_STATUS_BUSY 1u
#define PEOS_INSTALL_STATUS_DONE 2u
#define PEOS_INSTALL_STATUS_ERROR 3u

#define PEOS_INSTALL_PHASE_NONE 0u
#define PEOS_INSTALL_PHASE_PREPARE 1u
#define PEOS_INSTALL_PHASE_PARTITION 2u
#define PEOS_INSTALL_PHASE_WRITE_ESP 3u
#define PEOS_INSTALL_PHASE_WRITE_ROOT 4u
#define PEOS_INSTALL_PHASE_FINALIZE 5u

// fbpresent flags
#define PEOS_FBPRESENT_EXCLUSIVE (1u << 0)
#define PEOS_FBPRESENT_DAMAGE_MAX_RECTS 256u
// fbmap flags
#define PEOS_FBMAP_EXCLUSIVE     (1u << 0)

// Keycodes (PS/2 Set 1 style, extended keys use 0xE0xx).
#define PEOS_KEY_ESC    0x0001u
#define PEOS_KEY_Q      0x0010u
#define PEOS_KEY_W      0x0011u
#define PEOS_KEY_E      0x0012u
#define PEOS_KEY_R      0x0013u
#define PEOS_KEY_T      0x0014u
#define PEOS_KEY_Y      0x0015u
#define PEOS_KEY_U      0x0016u
#define PEOS_KEY_I      0x0017u
#define PEOS_KEY_O      0x0018u
#define PEOS_KEY_P      0x0019u
#define PEOS_KEY_A      0x001Eu
#define PEOS_KEY_S      0x001Fu
#define PEOS_KEY_D      0x0020u
#define PEOS_KEY_F      0x0021u
#define PEOS_KEY_G      0x0022u
#define PEOS_KEY_H      0x0023u
#define PEOS_KEY_J      0x0024u
#define PEOS_KEY_K      0x0025u
#define PEOS_KEY_L      0x0026u
#define PEOS_KEY_Z      0x002Cu
#define PEOS_KEY_X      0x002Du
#define PEOS_KEY_C      0x002Eu
#define PEOS_KEY_V      0x002Fu
#define PEOS_KEY_B      0x0030u
#define PEOS_KEY_N      0x0031u
#define PEOS_KEY_M      0x0032u
#define PEOS_KEY_SPACE  0x0039u
#define PEOS_KEY_CAPSLOCK 0x003Au
#define PEOS_KEY_BACKSPACE 0x000Eu
#define PEOS_KEY_TAB    0x000Fu
#define PEOS_KEY_ENTER  0x001Cu
#define PEOS_KEY_LCTRL  0x001Du
#define PEOS_KEY_LSHIFT 0x002Au
#define PEOS_KEY_RSHIFT 0x0036u
#define PEOS_KEY_LALT   0x0038u
#define PEOS_KEY_UP     0xE048u
#define PEOS_KEY_LEFT   0xE04Bu
#define PEOS_KEY_RIGHT  0xE04Du
#define PEOS_KEY_DOWN   0xE050u
#define PEOS_KEY_HOME   0xE047u
#define PEOS_KEY_END    0xE04Fu
#define PEOS_KEY_RCTRL  0xE01Du
#define PEOS_KEY_RALT   0xE038u
#define PEOS_KEY_LSUPER 0xE05Bu
#define PEOS_KEY_RSUPER 0xE05Cu

// Normalized extended keycodes (when E0 prefix is stripped).
#define PEOS_KEY_UP_NORM      0x0048u
#define PEOS_KEY_LEFT_NORM    0x004Bu
#define PEOS_KEY_RIGHT_NORM   0x004Du
#define PEOS_KEY_DOWN_NORM    0x0050u
#define PEOS_KEY_HOME_NORM    0x0047u
#define PEOS_KEY_END_NORM     0x004Fu
#define PEOS_KEY_PGUP_NORM    0x0049u
#define PEOS_KEY_PGDN_NORM    0x0051u
#define PEOS_KEY_RCTRL_NORM   0x001Du
#define PEOS_KEY_RALT_NORM    0x0038u
#define PEOS_KEY_LSUPER_NORM  0x005Bu
#define PEOS_KEY_RSUPER_NORM  0x005Cu
#ifdef __cplusplus
}
#endif
