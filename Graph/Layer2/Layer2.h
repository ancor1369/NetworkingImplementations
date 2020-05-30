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
#include "../graph.h"

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

/*
 * This function decides if an incoming packet shall
 * be processed or not by the routing device. This enforces
 * L3 processing rules
*/
static inline bool_t
l2FrameReceivedQulifyOnInterface(interface_t *interface, ethernet_heather_t *ethernetHEader)
{
	if(!IS_INTF_L3_MODE(interface))
		return FALSE;

	//Return true if the packet needs to be processed
	if(memcmp(IF_MAC(interface), ethernetHEader->dstMAC.mac, sizeof(macAddr_t)) == 0)
	{
		return TRUE;
	}

	if(IS_MAC_BROADCAST_ADDR(ethernetHEader->dstMAC.mac))
	{
		return TRUE;
	}

	return FALSE;
}


#endif /* LAYER2_LAYER2_H_ */
