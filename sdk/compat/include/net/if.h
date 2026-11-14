/* libpeos-compat: net/if.h, the interface flags getifaddrs gives. */
#pragma once

#define IFNAMSIZ 16
#define IF_NAMESIZE IFNAMSIZ

#define IFF_UP 0x1
#define IFF_BROADCAST 0x2
#define IFF_LOOPBACK 0x8
#define IFF_RUNNING 0x40
#define IFF_MULTICAST 0x1000

#ifdef __cplusplus
extern "C" {
#endif
unsigned int if_nametoindex(const char *name);
char *if_indextoname(unsigned int index, char *name);
#ifdef __cplusplus
}
#endif
