/*
 * Layer2.h
 *
 *  Created on: May 29, 2020
 *      Author: andres
 */

#ifndef LAYER2_LAYER2_H_
#define LAYER2_LAYER2_H_

#include "../net/net.h"
#include "../gddl/gddl.h"

#define ETH_HEADER_SIZE_EXCL_PAYLOAD (ethernetHeader_ptr) \
	(sizeof(ethernet_heather_t) - sizeof(((ethernet_heather_t *)0)->payload))


#define ETH_FCS(eth_hdr_ptr, payload_size) \
	(*(unsigned int *)(((char *)(((ethernet_heather_t *)ethernet_heather_t)->payload) + payload_size)))


#pragma pack (push, 1)
typedef struct ethernet_header_{

	macAddr_t dstMAC;
	macAddr_t srcMAC;
	short type;
	char payload[248]; //This is allowed to as big as 1500
	unsigned int FCS;
}ethernet_heather_t;
#pragma pack(pop)

static inline ethernet_heather_t * Alloc_eth_Header_with_Payload(char *pkt, unsigned pkt_size);


#endif /* LAYER2_LAYER2_H_ */
