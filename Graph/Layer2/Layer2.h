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
#include "../tcpconstants.h"

#define ARP_BROAD_REQ   1

#define ETH_HDR_SIZE_EXCL_PAYLOAD   \
    (sizeof(ethernet_heather_t) - sizeof(((ethernet_heather_t *)0)->payload))


#define ETH_HEADER_SIZE_EXCL_PAYLOAD (ethernetHeader_ptr) \
	(sizeof(ethernet_heather_t) - sizeof(((ethernet_heather_t *)0)->payload))


#define ETH_FCS(eth_hdr_ptr, payload_size)  \
    (*(unsigned int *)(((char *)(((ethernet_heather_t *)eth_hdr_ptr)->payload) + payload_size)))

#define VLAN_ETH_FCS(vlan_eth_hdr_ptr, payload_size)  \
    (*(unsigned int *)(((char *)(((vlan_ethernet_header_t *)vlan_eth_hdr_ptr)->payload) + payload_size)))

#define VLAN_ETH_HDR_SIZE_EXCL_PAYLOAD  \
   (sizeof(vlan_ethernet_header_t) - sizeof(((vlan_ethernet_header_t *)0)->payload))

#define GET_COMMON_ETH_FCS(eth_hdr_ptr, payload_size)   \
        (is_pkt_vlan_tagged(eth_hdr_ptr) ? VLAN_ETH_FCS(eth_hdr_ptr, payload_size) : \
            ETH_FCS(eth_hdr_ptr, payload_size))

/*ARP Table APIs*/
typedef struct arp_table_{
    gddl_t arp_entries;
} arp_table_t;

typedef struct arp_entry_{
	ipAddr_t ipAddr;
	macAddr_t mac_addr;
	char oif_name[IF_NAME_SIZE];
	gddl_t arp_glue;
}arp_entry_t;


#pragma pack (push, 1)
typedef struct vlan_8021q_header_{

	unsigned short tpid; //0x8100
	short tci_pcp : 3; //Not used in the course
	short tci_dei : 1; //Not used
	short tci_vid : 12; //tagged Vlan ID

}vlan_8021q_header_t;

typedef struct vlan_ethernet_header_{

	macAddr_t dstMAC;
	macAddr_t srcMAC;
	vlan_8021q_header_t vlan_8021q_header;
	unsigned short type;
	char payload[248]; //This is allowed to as big as 1500
	unsigned int FCS;
}vlan_ethernet_header_t;


typedef struct arp_header_{
	short hwType;
	short protoType;
	char hwAddrLen;
	char proto_addr_len;    /*4 for IPV4*/
	short opCode;
	macAddr_t srcMac;
	unsigned int srcIp;
	macAddr_t dstMac;
	unsigned int dstIp;
}arp_header_t;


typedef struct ethernet_header_{

	macAddr_t dstMAC;
	macAddr_t srcMAC;
	unsigned short type;
	char payload[248]; //This is allowed to as big as 1500
	unsigned int FCS;
}ethernet_heather_t;
#pragma pack(pop)


static inline unsigned int GET_802_1Q_VLAN_ID(vlan_8021q_header_t *vlan_8021q_hdr){

    return (unsigned int)vlan_8021q_hdr->tci_vid;
}



static inline vlan_8021q_header_t* is_pkt_vlan_tagged(ethernet_heather_t *ethernet_hdr){

    /*Check the 13th and 14th byte of the ethernet hdr,
     *      * if is value is 0x8100 then it is vlan tagged*/

    vlan_8021q_header_t *vlan_8021q_hdr =
        (vlan_8021q_header_t *)((char *)ethernet_hdr + (sizeof(macAddr_t) * 2));

    if(vlan_8021q_hdr->tpid == VLAN_8021Q_PROTO)
        return vlan_8021q_hdr;

    return NULL;
}

static inline char * GET_ETHERNET_HDR_PAYLOAD(ethernet_heather_t *ethernet_hdr){

   if(is_pkt_vlan_tagged(ethernet_hdr)){
        return ((vlan_ethernet_header_t *)(ethernet_hdr))->payload;
   }
   else
       return ethernet_hdr->payload;
}

static inline void SET_COMMON_ETH_FCS(ethernet_heather_t *ethernet_hdr,
                   unsigned int payload_size,
                   unsigned int new_fcs){

    if(is_pkt_vlan_tagged(ethernet_hdr)){
        VLAN_ETH_FCS(ethernet_hdr, payload_size) = new_fcs;
    }
    else{
        ETH_FCS(ethernet_hdr, payload_size) = new_fcs;
    }
}

//APIS to support the operation of ARP protocol
void initArpTable(arp_table_t **arp_table);

arp_entry_t * arpTableLookup(arp_table_t *arpTable, char *ipAddr);

void clearArpTable(arp_table_t *arpTable);

void deleteArpTableEntry(arp_table_t *arpTable, char *ipAddr);

bool_t arpTableEntryAdd(arp_table_t *arpTable, arp_entry_t *arpEntry);

void dumpArpTable(arp_table_t *arpTable);

void arpTableUpdateFromArpReply(arp_table_t *arpTable, arp_header_t *arpHeader, interface_t *itf);


void sendArpBroadcastRequest(node_t *node, interface_t *oif, char *ipAddr);
GLTHREAD_TO_STRUCT(arp_glue_to_arp_entry, arp_entry_t, arp_glue);

static inline ethernet_heather_t * Alloc_eth_Header_with_Payload(char *pkt, unsigned pkt_size);

/*
 * This function decides if an incoming packet shall
 * be processed or not by the routing device. This enforces
 * L3 processing rules
*/
static inline bool_t l2FrameReceivedQulifyOnInterface(interface_t *interface, ethernet_heather_t *ethernetHEader)
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


static inline unsigned int GET_ETH_HDR_SIZE_EXCL_PAYLOAD(ethernet_heather_t *ethernet_hdr){

    if(is_pkt_vlan_tagged(ethernet_hdr)){
        return VLAN_ETH_HDR_SIZE_EXCL_PAYLOAD;
    }
    else{
        return ETH_HDR_SIZE_EXCL_PAYLOAD;
    }
}

static inline ethernet_heather_t * ALLOC_ETH_HDR_WITH_PAYLOAD(char *pkt, unsigned int pkt_size){

    char *temp = calloc(1, pkt_size);
    memcpy(temp, pkt, pkt_size);

    ethernet_heather_t *eth_hdr = (ethernet_heather_t *)(pkt - ETH_HDR_SIZE_EXCL_PAYLOAD);
    memset((char *)eth_hdr, 0, ETH_HDR_SIZE_EXCL_PAYLOAD);
    memcpy(eth_hdr->payload, temp, pkt_size);
    SET_COMMON_ETH_FCS(eth_hdr, pkt_size, 0);
    free(temp);
    return eth_hdr;
}


void nodeSetInfL2Mode(node_t *node, char *infName, intfL2Mode_t l2Mode);





#endif /* LAYER2_LAYER2_H_ */
