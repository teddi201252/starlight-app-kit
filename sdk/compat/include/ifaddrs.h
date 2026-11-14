/* libpeos-compat: getifaddrs, from netd's configuration: "lo" (127.0.0.1)
 * and, once the network has an address, "eth0" with it and its netmask.
 * IPv4 only. */
#pragma once
#include <sys/socket.h>

struct ifaddrs {
    struct ifaddrs *ifa_next;
    char *ifa_name;
    unsigned int ifa_flags;
    struct sockaddr *ifa_addr;
    struct sockaddr *ifa_netmask;
    union {
        struct sockaddr *ifu_broadaddr;
        struct sockaddr *ifu_dstaddr;
    } ifa_ifu;
    void *ifa_data;
};
#define ifa_broadaddr ifa_ifu.ifu_broadaddr
#define ifa_dstaddr ifa_ifu.ifu_dstaddr

#ifdef __cplusplus
extern "C" {
#endif
int getifaddrs(struct ifaddrs **ifap);
void freeifaddrs(struct ifaddrs *ifa);
#ifdef __cplusplus
}
#endif
