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
    /* 37: udp_socket, retired in milestone 4 (the network is netd's) */
    /* 38: udp_bind, retired in milestone 4 (the network is netd's) */
    /* 39: udp_sendto, retired in milestone 4 (the network is netd's) */
    /* 40: udp_recvfrom, retired in milestone 4 (the network is netd's) */
    /* 41: udp_close, retired in milestone 4 (the network is netd's) */
    /* 42: netcfg_get, retired in milestone 4 (the network is netd's) */
    /* 43: netcfg_set, retired in milestone 4 (the network is netd's) */
    /* 44: tcp_socket, retired in milestone 4 (the network is netd's) */
    /* 45: tcp_connect, retired in milestone 4 (the network is netd's) */
    /* 46: tcp_send, retired in milestone 4 (the network is netd's) */
    /* 47: tcp_recv, retired in milestone 4 (the network is netd's) */
    /* 48: tcp_close, retired in milestone 4 (the network is netd's) */
    /* 49: icmp_echo, retired in milestone 4 (the network is netd's) */
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
    PEOS_SYS_thread_spawn = 69, // rdi=entry, rsi=arg, rdx=stack_top
    PEOS_SYS_hwinfo_report = 70, // rdi=buf_ptr (0=only size), rsi=buf_len
    PEOS_SYS_audio_set_log_level = 71, // rdi=level (0=error..3=debug)
    PEOS_SYS_shm_create = 72, // rdi=len -> shm id or -1
    PEOS_SYS_shm_map = 73, // rdi=id, rsi=out_ptr, rdx=out_len -> 0/-1
    PEOS_SYS_shm_destroy = 74, // rdi=id -> 0/-1
    PEOS_SYS_install_list_disks = 75, // rdi=out_ptr (0=only count), rsi=max_entries -> count or -1
    PEOS_SYS_install_get_progress = 76, // rdi=out_ptr, rsi=out_len -> 0/-1
    PEOS_SYS_install_run = 77, // rdi=target_ptr, rsi=target_len -> 0/-1
    PEOS_SYS_cap_query = 78, // rdi=target_pid (0=self) -> capability mask or -1
    PEOS_SYS_cap_grant = 79, // rdi=target_pid, rsi=mask -> 0/-1
    PEOS_SYS_cap_revoke = 80, // rdi=target_pid, rsi=mask -> 0/-1
    PEOS_SYS_fbpresent_damage = 81, // rdi=frame, rsi=len, rdx=rects, rcx=count, r8=flags
    PEOS_SYS_cap_set_spawn_mask = 82, // rdi=mask -> 0/-1; ceiling for the next spawnv child (one-shot)
    PEOS_SYS_metrics = 83, // rdi=out_ptr, rsi=len (>= sizeof(peos_metrics_t)) -> bytes written or -1
    PEOS_SYS_path_scope_set = 84, // rdi=grants_ptr, rsi=count -> 0/-1; narrows this task's path scope
    PEOS_SYS_path_set_spawn_scope = 85, // rdi=grants_ptr, rsi=count -> 0/-1; path scope for the next spawnv child (one-shot)
    PEOS_SYS_path_scope_query = 86, // rdi=out_ptr (peos_path_scope_t) -> 0/-1
    PEOS_SYS_fd_restrict = 87,     // rdi=fd, rsi=rights to keep -> 0/-1
    PEOS_SYS_fd_rights = 88,       // rdi=fd -> rights or -1
    PEOS_SYS_spawn_set_fds = 89,   // rdi=peos_spawn_fd_t[], rsi=count -> 0/-1; for the next spawnv (one-shot)
    PEOS_SYS_wait = 90,            // rdi=peos_wait_item_t[], rsi=count, rdx=timeout_ns -> ready count, 0 on timeout, -1
    PEOS_SYS_timer_create = 91,    // -> fd or -1
    PEOS_SYS_timer_set = 92,       // rdi=fd, rsi=initial_ns (0 = disarm), rdx=interval_ns (0 = one-shot) -> 0/-1
    PEOS_SYS_proc_open = 93,       // rdi=pid of a direct child -> fd or -1
    PEOS_SYS_channel_create = 94,  // rdi=int64_t[2] out -> 0/-1
    PEOS_SYS_channel_send = 95,    // rdi=fd, rsi=peos_channel_msg_t*, rdx=flags -> 0/AGAIN/CLOSED/-1
    PEOS_SYS_channel_recv = 96,    // rdi=fd, rsi=peos_channel_msg_t*, rdx=flags -> len/AGAIN/CLOSED/-1
    PEOS_SYS_shm_create_fd = 97,   // rdi=len -> fd or -1
    PEOS_SYS_shm_map_fd = 98,      // rdi=fd, rsi=out_ptr, rdx=out_len -> 0/-1 (read-only without WR)
    PEOS_SYS_shm_allow = 99,       // rdi=id, rsi=pid -> 0/-1; creator only
    PEOS_SYS_input_open = 100,     // -> fd of peos_input_record_t, or -1
    PEOS_SYS_audio_open = 101,     // -> fd for PCM playback (write, wait WRITE), or -1
    /* 102: net_socket, retired in milestone 4 (the network is netd's) */
    /* 103: net_bind, retired in milestone 4 (the network is netd's) */
    /* 104: net_connect, retired in milestone 4 (the network is netd's) */
    PEOS_SYS_nic_open = 105,       // rdi=index -> fd of whole Ethernet frames (exclusive), or -1
    PEOS_SYS_nic_info = 106,       // rdi=fd, rsi=peos_nic_info_t*, rdx=len -> 0/-1
    PEOS_SYS_svc_register = 107,   // rdi=name, rsi=len -> listening channel fd, or -1
    PEOS_SYS_svc_connect = 108,    // rdi=name, rsi=len -> channel fd to the service, -1, or PEOS_CHANNEL_AGAIN
    PEOS_SYS_sysinfo = 109,       // rdi=peos_sysinfo_t*, rsi=len -> bytes written or -1 (no capability)
    PEOS_SYS_proc_list = 110,     // rdi=peos_proc_info_t[], rsi=max -> count or -1 (no capability)
    PEOS_SYS_proc_kill = 111,     // rdi=pid -> 0/-1 (PEOS_CAP_MANAGE)
    PEOS_SYS_random = 112,        // rdi=buf, rsi=len (<= 65536) -> len or -1 (no capability)
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
    int8_t wheel_y;
    int8_t wheel_x;
    uint8_t reserved;
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
