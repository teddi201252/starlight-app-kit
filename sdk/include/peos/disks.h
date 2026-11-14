/* The disks as PeOS sees them (release 0.16.0): every disk and partition,
 * what is on it, where it is mounted; and a descriptor that becomes readable
 * when that changes (a stick in or out, a mount, an unmount).
 * Shared by the kernel (kernel/abi/disk_abi.h) and userland. Both calls need
 * the "mount" capability. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define PEOS_DISKS_MAX 16u

enum {
    PEOS_DISK_OTHER = 0,
    PEOS_DISK_USB = 1,
    PEOS_DISK_SATA = 2,      // AHCI
    PEOS_DISK_ATA = 3,
    PEOS_DISK_NVME = 4,
};

enum {
    PEOS_DISK_PARTITION = 1u << 0,       // a partition of `parent`
    PEOS_DISK_REMOVABLE = 1u << 1,       // USB
    PEOS_DISK_ESP = 1u << 2,             // an EFI system partition (MBR 0xEF, GPT's ESP)
    PEOS_DISK_HAS_PARTITIONS = 1u << 3,  // a whole disk with partitions
    PEOS_DISK_SYSTEM = 1u << 4,          // the disk of the root, or one of its partitions
    PEOS_DISK_BOOT = 1u << 5,            // the EFI partition this system started from
};

/* A partition's type, from its GPT type GUID or MBR type byte (release
 * 0.17.0): what it is for, even when PeOS cannot read what is on it. */
enum {
    PEOS_PART_NONE = 0,          // not a partition, or a type PeOS does not know
    PEOS_PART_EFI = 1,           // EFI system partition
    PEOS_PART_APFS = 2,          // Apple APFS (macOS)
    PEOS_PART_HFS = 3,           // Apple HFS+
    PEOS_PART_APPLE_BOOT = 4,    // Apple boot (Recovery)
    PEOS_PART_MS_BASIC = 5,      // Microsoft basic data (NTFS, exFAT, FAT)
    PEOS_PART_MS_RESERVED = 6,   // Microsoft reserved
    PEOS_PART_MS_RECOVERY = 7,   // Windows recovery
    PEOS_PART_LINUX = 8,         // Linux filesystem (also PeOS's root)
    PEOS_PART_LINUX_SWAP = 9,    // Linux swap
};

typedef struct {
    char name[24];          // "usb0p1"
    char parent[24];        // the whole disk of a partition ("usb0"), "" otherwise
    char fs[8];             // what is on it: "exfat", "fat32", "xfs", "minix", "" unknown
    char label[48];         // the volume's name (UTF-8), "" if none
    char mount[96];         // where it is mounted, "" if not
    uint64_t sector_count;
    uint32_t sector_size;
    uint8_t type;           // PEOS_DISK_*
    uint8_t flags;          // PEOS_DISK_PARTITION | ...
    uint8_t part_type;      // a partition's type in its table: PEOS_PART_* (0.17.0)
    uint8_t reserved;
    uint64_t total_bytes;   // a mounted one, with PEOS_DISKS_SPACE: its filesystem's size
    uint64_t free_bytes;    // and what is free on it (0 and 0 otherwise)
} peos_disk_info_t;

#define PEOS_DISKS_SPACE 1u   // peos_disks_ex: also the space of the mounted ones (it may read a FAT)

/* Every disk and partition (up to max); the count, or -1. */
int64_t peos_disks(peos_disk_info_t *out, uint64_t max);
/* The same, with PEOS_DISKS_SPACE: total and free bytes of the mounted ones
 * (slower: a FAT32 counts its free clusters). */
int64_t peos_disks_ex(peos_disk_info_t *out, uint64_t max, uint64_t flags);
/* A descriptor readable (PEOS_WAIT_READ) when the disks change; reading it
 * gives 8 bytes, a counter of the changes, and makes it unreadable again
 * until the next one. -1 if not allowed. */
int64_t peos_disks_watch(void);

#ifdef __cplusplus
}
#endif
