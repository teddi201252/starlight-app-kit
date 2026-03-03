#pragma once
#include <stdint.h>
#include <stddef.h>
#include "peos/alloc.h"
#include "peos/wifi.h"

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
    uint8_t ip[4];
    uint8_t netmask[4];
    uint8_t gateway[4];
    uint8_t dns[4];
} peos_netcfg_t;

typedef struct {
    uint64_t size;
    uint64_t mtime_ns;
    uint64_t ctime_ns;
    uint8_t is_dir;
    uint8_t _pad[7];
} peos_stat_t;

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
uint64_t peos_waitpid(uint64_t pid, int *status);
uint64_t peos_waitpid_flags(uint64_t pid, int *status, uint64_t flags);
uint64_t peos_pipe(uint64_t out_pair[2]);
int64_t peos_spawnv(const char *path, size_t len, const char *arg_str,
                    int64_t stdin_fd, int64_t stdout_fd, int64_t stderr_fd);
uint64_t peos_fopen(const char *path, size_t len, uint64_t flags);
uint64_t peos_fread(uint64_t fd, void *buf, size_t len);
uint64_t peos_fwrite(uint64_t fd, const void *buf, size_t len);
uint64_t peos_fseek(uint64_t fd, size_t pos);
uint64_t peos_fsize(uint64_t fd);
uint64_t peos_fclose(uint64_t fd);
uint64_t peos_chdir(const char *path, size_t len);
uint64_t peos_getcwd(char *buf, size_t len);
uint64_t peos_readdir(const char *path, size_t path_len, char *out, size_t out_len);
uint64_t peos_stat(const char *path, size_t path_len, peos_stat_t *out, size_t out_len);
uint64_t peos_mkdir(const char *path, size_t len);
uint64_t peos_unlink(const char *path, size_t len);
uint64_t peos_rename(const char *oldpath, size_t old_len, const char *newpath, size_t new_len);
uint64_t peos_rmdir(const char *path, size_t len, uint64_t flags);
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
uint64_t peos_fbinfo(peos_fb_info_t *out);
uint64_t peos_fbpresent(const void *buf, size_t len, uint64_t flags);
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
int64_t peos_udp_socket(void);
int64_t peos_udp_bind(int64_t sock, uint16_t local_port);
int64_t peos_udp_sendto(int64_t sock, const void *buf, size_t len,
                        const uint8_t dst_ip[4], uint16_t dst_port);
int64_t peos_udp_recvfrom(int64_t sock, void *buf, size_t len,
                          uint8_t out_src_ip[4], uint16_t *out_src_port);
int64_t peos_udp_close(int64_t sock);
int64_t peos_netcfg_get(peos_netcfg_t *cfg);
int64_t peos_netcfg_set(const peos_netcfg_t *cfg);
int peos_net_parse_ipv4(const char *s, uint8_t out[4]);
int peos_net_dns_resolve_a(const char *host, uint8_t out_ip[4], uint32_t timeout_ticks);
int peos_net_resolve_ipv4(const char *host_or_ip, uint8_t out_ip[4], uint32_t timeout_ticks);
int64_t peos_tcp_socket(void);
int64_t peos_tcp_connect(int64_t sock, const uint8_t dst_ip[4], uint16_t dst_port);
int64_t peos_tcp_send(int64_t sock, const void *buf, size_t len);
int64_t peos_tcp_recv(int64_t sock, void *buf, size_t len);
int64_t peos_tcp_close(int64_t sock);
int64_t peos_icmp_echo(const uint8_t dst_ip[4], uint16_t id, uint16_t seq);

size_t peos_strlen(const char *s);
void *peos_memset(void *dst, int c, size_t n);
void *peos_memcpy(void *dst, const void *src, size_t n);

// Open flags (userland)
#define PEOS_O_CREAT 0x1u
#define PEOS_O_TRUNC 0x2u
#define PEOS_O_WRITE 0x4u

#define PEOS_RMDIR_RECURSIVE 0x1u

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

// fbpresent flags
#define PEOS_FBPRESENT_EXCLUSIVE (1u << 0)
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
#define PEOS_KEY_ENTER  0x001Cu
#define PEOS_KEY_LCTRL  0x001Du
#define PEOS_KEY_LSHIFT 0x002Au
#define PEOS_KEY_RSHIFT 0x0036u
#define PEOS_KEY_UP     0xE048u
#define PEOS_KEY_LEFT   0xE04Bu
#define PEOS_KEY_RIGHT  0xE04Du
#define PEOS_KEY_DOWN   0xE050u
