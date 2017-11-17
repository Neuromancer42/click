#ifndef CLICKNET_OSPF_H
#define CLICKNET_OSPF_H

#include <click/cxxprotect.h>

CLICK_CXX_PROTECT

#if CLICK_LINUXMODULE
# include <net/checksum.h>
# include <linux/in.h>
#else
# include <sys/types.h>
# include <netinet/in.h>
#endif

/*
 * <clicknet/ospf.h> -- extremely simplified OSPF definitions
 *
 * Relevant RFC include:
 *   RFC 2328 OSPF Version 2
 * 
 * Note:
 *   Use a IP protocol code 89 to distinguish.
 *   Only 2 kinds of packets, hello and broadcast.
 * Hello packets informs existence to direct neigh-
 * bours, meanwhile broadcast packet broadcast all
 * known direct neighbours to all routers in the 
 * network. 
 *   Clients also sends hello packets, but do not 
 * process any OSPF packets.
 *   For simplification, no authentification or 
 * hierarchy optimizations. All metrics are simply
 * set as 1.
 */

#ifndef IPPROTO_OSPF
#define IPPROTO_OSPF 89
#endif

struct click_ospf {
  uint8_t ospf_ver;          /* packet version     */
  uint8_t ospf_type;         /* packet type, 0: hello, 1: broadcast */
  uint16_t ospf_len;         /* neighbours length  */
  uint32_t ospf_rt;          /* router's id        */
};

/* Some constants, set as unconfigurable, and consistent in whole network */
/* All time constants are in msec */

/* Time interval between two hello packets */
#ifndef OSPF_INTERVAL_HELLO
#define OSPF_INTERVAL_HELLO 3000
#endif

/* A neighbour router's record's time-to-live after last received hello */
#ifndef OSPF_INTERVAL_ROUTER_DOWN
#define OSPF_INTERVAL_ROUTER_DOWN 9000
#endif

/* Time interval between two broadcast packets */
#ifndef OSPF_INTERVAL_BROADCAST
#define OSPF_INTERVAL_BROADCAST 3000
#endif

/* A link state record's time-to-live after last received LS packet */
#ifndef OSPF_INTERVAL_LINK_DOWN
#define OSPF_INTERVAL_LINK_DOWN 9000
#endif

/* Time interval between two network status updates */
#ifndef OSPF_INTERVAL_UPDATE
#define OSPF_INTERVAL_UPDATE 6000
#endif

CLICK_CXX_UNPROTECT

#include <click/cxxunprotect.h>
#endif
