#pragma once
#include <stdint.h>
#include <stddef.h>

// PeOS syscall numbers (int 0x80 ABI).
enum {
    PEOS_SYS_write = 1, // rdi=fd, rsi=buf, rdx=len
    PEOS_SYS_sleep = 2, // rdi=ticks
    PEOS_SYS_exit  = 3, // rdi=code
    PEOS_SYS_read  = 4, // rdi=fd, rsi=buf, rdx=len
    PEOS_SYS_exec  = 5, // rdi=ptr, rsi=len
    PEOS_SYS_clear = 6, // no args
    PEOS_SYS_getpid = 7, // rax=pid
    PEOS_SYS_waitpid = 8, // rdi=pid, rsi=status_ptr, rdx=flags (PEOS_WAIT_NOHANG)
    PEOS_SYS_fopen = 9,  // rdi=ptr, rsi=len, rdx=flags
    PEOS_SYS_fread = 10, // rdi=fd, rsi=buf, rdx=len
    PEOS_SYS_fseek = 11, // rdi=fd, rsi=pos
    PEOS_SYS_fsize = 12, // rdi=fd
    PEOS_SYS_fclose = 13, // rdi=fd
    PEOS_SYS_execv = 14, // rdi=path_ptr, rsi=path_len, rdx=arg_str
    PEOS_SYS_chdir = 15, // rdi=path_ptr, rsi=path_len
    PEOS_SYS_getcwd = 16, // rdi=out_ptr, rsi=out_len
    PEOS_SYS_readdir = 17, // rdi=path_ptr, rsi=path_len, rdx=out_ptr, rcx=out_len
    PEOS_SYS_fwrite = 18, // rdi=fd, rsi=buf, rdx=len
    PEOS_SYS_mkdir = 19, // rdi=path_ptr, rsi=path_len
    PEOS_SYS_unlink = 20, // rdi=path_ptr, rsi=path_len
    PEOS_SYS_rmdir = 21, // rdi=path_ptr, rsi=path_len, rdx=flags
    PEOS_SYS_poweroff = 22, // no args
    PEOS_SYS_reboot = 23, // no args
    PEOS_SYS_time = 24, // rax=unix seconds
    PEOS_SYS_time_ns = 25, // rax=unix nanoseconds
    PEOS_SYS_hwinfo = 26, // rdi=ram_total_ptr, rsi=ram_free_ptr, rdx=disk_total_ptr, rcx=disk_free_ptr
    PEOS_SYS_brk = 27, // rdi=addr
    PEOS_SYS_mmap = 28, // rdi=addr, rsi=len, rdx=prot, rcx=flags, r8=fd, r9=off
    PEOS_SYS_munmap = 29, // rdi=addr, rsi=len
    PEOS_SYS_fbinfo = 30, // rdi=out_ptr, rsi=out_len
    PEOS_SYS_fbpresent = 31, // rdi=buf_ptr, rsi=buf_len, rdx=flags
    PEOS_SYS_input_poll = 32, // rdi=out_ptr, rsi=max_events
    PEOS_SYS_input_mode = 33, // rdi=mode (0=cooked, 1=raw)
    PEOS_SYS_audio_write = 34, // rdi=buf, rsi=len -> bytes written or -1
    PEOS_SYS_audio_stream_write = 35, // rdi=buf, rsi=len -> bytes written or -1
    PEOS_SYS_audio_stream_avail = 36, // rax=available bytes for writing
    PEOS_SYS_udp_socket = 37, // rax=sockfd
    PEOS_SYS_udp_bind = 38, // rdi=sockfd, rsi=port
    PEOS_SYS_udp_sendto = 39, // rdi=sockfd, rsi=buf, rdx=len, rcx=ip, r8=port
    PEOS_SYS_udp_recvfrom = 40, // rdi=sockfd, rsi=buf, rdx=len, rcx=ip, r8=port
    PEOS_SYS_udp_close = 41, // rdi=sockfd
    PEOS_SYS_netcfg_get = 42, // rdi=cfg_ptr, rsi=cfg_len
    PEOS_SYS_netcfg_set = 43, // rdi=cfg_ptr, rsi=cfg_len
    PEOS_SYS_tcp_socket = 44, // rax=sockfd
    PEOS_SYS_tcp_connect = 45, // rdi=sockfd, rcx=dst_ip_ptr, r8=dst_port
    PEOS_SYS_tcp_send = 46, // rdi=sockfd, rsi=buf, rdx=len
    PEOS_SYS_tcp_recv = 47, // rdi=sockfd, rsi=buf, rdx=len
    PEOS_SYS_tcp_close = 48, // rdi=sockfd
    PEOS_SYS_icmp_echo = 49, // rdi=dst_ip_ptr, rsi=id, rdx=seq
    PEOS_SYS_wifi_scan = 50, // rdi=scan_req_ptr, rsi=scan_req_len
    PEOS_SYS_wifi_connect = 51, // rdi=conn_ptr, rsi=conn_len
    PEOS_SYS_wifi_status = 52, // rdi=status_ptr, rsi=status_len
    PEOS_SYS_wifi_disconnect = 53, // no args
    PEOS_SYS_stat = 54, // rdi=path_ptr, rsi=path_len, rdx=out_ptr, rcx=out_len
    PEOS_SYS_input_mouse = 55, // rdi=out_ptr, rsi=out_len
    PEOS_SYS_rename = 56, // rdi=old_ptr, rsi=old_len, rdx=new_ptr, rcx=new_len
    PEOS_SYS_fbmap = 57, // rdi=out_ptr, rsi=out_len, rdx=flags
    PEOS_SYS_cursor_set_visible = 58, // rdi=visible
    PEOS_SYS_cursor_move_abs = 59, // rdi=x, rsi=y
    PEOS_SYS_cursor_set_image_rgba = 60, // rdi=rgba_ptr, rsi=len, rdx=width, rcx=height, r8=hot_x, r9=hot_y
    PEOS_SYS_fb_set_mode = 61, // rdi=width, rsi=height (0,0=host/default)
    PEOS_SYS_bootlog_snapshot = 62, // rdi=buf_ptr (0=only size), rsi=buf_len
    PEOS_SYS_audio_set_volume = 63, // rdi=volume (0..100)
    PEOS_SYS_pipe = 64, // rdi=out_ptr_to_u64_pair[2] -> 0/-1
    PEOS_SYS_spawnv = 65, // rdi=path_ptr, rsi=path_len, rdx=arg_str, rcx=stdin_fd, r8=stdout_fd, r9=stderr_fd
    PEOS_SYS_mount = 66, // rdi=dev_ptr, rsi=dev_len, rdx=mount_ptr, rcx=mount_len, r8=fs_ptr, r9=fs_len
    PEOS_SYS_umount = 67, // rdi=mount_ptr, rsi=mount_len
    PEOS_SYS_mounts = 68, // rdi=out_ptr, rsi=out_len
};

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
    uint8_t reserved[3];
    uint64_t time_ns;
} peos_mouse_event_t;
#endif

static inline uint64_t peos_syscall0(uint64_t n) {
    uint64_t ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(n) : "memory");
    return ret;
}

static inline uint64_t peos_syscall1(uint64_t n, uint64_t a0) {
    uint64_t ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(n), "D"(a0) : "memory");
    return ret;
}

static inline uint64_t peos_syscall2(uint64_t n, uint64_t a0, uint64_t a1) {
    uint64_t ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(n), "D"(a0), "S"(a1) : "memory");
    return ret;
}

static inline uint64_t peos_syscall3(uint64_t n, uint64_t a0, uint64_t a1, uint64_t a2) {
    uint64_t ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(n), "D"(a0), "S"(a1), "d"(a2) : "memory");
    return ret;
}

static inline uint64_t peos_syscall4(uint64_t n, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3) {
    uint64_t ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(n), "D"(a0), "S"(a1), "d"(a2), "c"(a3) : "memory");
    return ret;
}

static inline uint64_t peos_syscall6(uint64_t n, uint64_t a0, uint64_t a1, uint64_t a2,
                                     uint64_t a3, uint64_t a4, uint64_t a5) {
    register uint64_t r8 __asm__("r8") = a4;
    register uint64_t r9 __asm__("r9") = a5;
    uint64_t ret;
    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(n), "D"(a0), "S"(a1), "d"(a2), "c"(a3), "r"(r8), "r"(r9)
                     : "memory");
    return ret;
}
