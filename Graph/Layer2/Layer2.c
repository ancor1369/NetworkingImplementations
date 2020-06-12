/*
 * Layer2.c
 *
 *  Created on: May 29, 2020
 *      Author: andres
 */

#include "Layer2.h"
#include "../gddl/gddl.h"
#include "../tcpconstants.h"
#include <sys/socket.h>

static inline ethernet_heather_t * Alloc_eth_Header_with_Payload(char *pkt, unsigned pkt_size)
{
	ethernet_heather_t *ethernetHeader;

	//Move the data from the buffer to the new ethernet frame
	memcpy(ethernetHeader->payload, pkt, pkt_size);
	return ethernetHeader;
}

void initArpTable(arp_table_t **arptable)
{
	*arptable = calloc(1,sizeof(arp_table_t));
	ddlInit(&((*arptable)->arp_entries));
}


arp_entry_t * arpTableLookup(arp_table_t *arpTable, char *ipAddr)
{
	gddl_t *curr;
	arp_entry_t *arpEntry;
	ITERATE_GDDL_BEGIN(&arpTable->arp_entries, curr){
		arpEntry = arp_glue_to_arp_entry(curr);
		if(strncmp(arpEntry->ipAddr.ipAddr, ipAddr,16)==0)
		{
			return arpEntry;
		}
	}ITERATE_GDDL_END(&arpTable->arp_entries, curr)
	return NULL;
}

void deleteArpTableEntry(arp_table_t *arpTable, char *ipAddr)
{
	gddl_t *curr;
		arp_entry_t *arpEntry;
		ITERATE_GDDL_BEGIN(&arpTable->arp_entries, curr){
			arpEntry = arp_glue_to_arp_entry(curr);
			if(strncmp(arpEntry->ipAddr.ipAddr, ipAddr,16)==0)
			{
				//Delete the found informtion from the table
				ddlRemove(&arpEntry->arp_glue);
				free(arpEntry);
			}
		}ITERATE_GDDL_END(&arpTable->arp_entries, curr)
}

bool_t arpTableEntryAdd(arp_table_t *arpTable, arp_entry_t *arpEntry)
{
	arp_entry_t *arpOldEntry = arpTableLookup(arpTable, arpEntry->ipAddr.ipAddr);

	if(arpOldEntry && memcmp(arpOldEntry, arpEntry, sizeof(arp_entry_t))==0)
		return FALSE;
	ddlInit(&arpEntry->arp_glue);
	ddlAddNext(&arpTable->arp_entries, &arpEntry->arp_glue);

	return TRUE;
}

void arpTableUpdateFromArpReply(arp_table_t *arpTable, arp_header_t *arpHeader, interface_t *itf)
{
	unsigned int src_ip = 0;
	assert(arpHeader->opCode == ARP_REPLY);
	//Allocating space in the heap to keep the new object
	arp_entry_t *arpEntry = calloc(1, sizeof(arp_entry_t));
	src_ip = htonl(arpHeader->srcIp);
	inet_ntop(AF_INET, &src_ip, &arpEntry->ipAddr.ipAddr, 16);
	arpEntry->ipAddr.ipAddr[15]='\0';
	memcpy(arpEntry->oif_name, itf->if_name, IF_NAME_SIZE);

	bool_t rc = arpTableEntryAdd(arpTable, arpEntry);
	if(rc==FALSE)
	{
		free(arpEntry);
	}
}


void clearArpTable(arp_table_t *arpTable)
{

}

void sendArpBroadcastRequest(node_t *node, interface_t *oif, char *ipAddr)
{
	 /*Take memory which can accomodate Ethernet hdr + ARP hdr*/
    unsigned int payload_size = sizeof(arp_header_t);
	ethernet_heather_t *ethernet_hdr = (ethernet_heather_t *)calloc(1, ETH_HDR_SIZE_EXCL_PAYLOAD + payload_size);

	    if(!oif){

	        oif = nodeGetMatchingSubnetInterface(node, ipAddr);
	        if(!oif){
	            printf("Error : %s : No eligible subnet for ARP resolution for Ip-address : %s",
	                    node->node_name, ipAddr);
	            return;
	        }
	    }
	    /*STEP 1 : Prepare ethernet hdr*/
	    layer2FillWithBroadcast(ethernet_hdr->dstMAC.mac);
	    //layer2_fill_with_broadcast_mac(ethernet_hdr->dstMAC.mac);//->dst_mac.mac);
	    memcpy(ethernet_hdr->srcMAC.mac, IF_MAC(oif), sizeof(macAddr_t));
	    ethernet_hdr->type = ARP_MSG;

	    /*Step 2 : Prepare ARP Broadcast Request Msg out of oif*/
	    arp_header_t *arp_hdr = (arp_header_t *)(ethernet_hdr->payload);
	    arp_hdr->hwType = 1;
	    arp_hdr->protoType = 0x0800;
	    arp_hdr->hwAddrLen = sizeof(macAddr_t);
	    arp_hdr ->proto_addr_len = 4;

	    arp_hdr->opCode = ARP_BROAD_REQ;

	    memcpy(arp_hdr->srcMac.mac, IF_MAC(oif), sizeof(macAddr_t));

	    inet_pton(AF_INET, IF_IP(oif), &arp_hdr->srcIp);
	    arp_hdr->srcIp = htonl(arp_hdr->srcIp);

	    memset(arp_hdr->dstMac.mac, 0,  sizeof(macAddr_t));

	    inet_pton(AF_INET, ipAddr, &arp_hdr->dstIp);
	    arp_hdr->dstIp = htonl(arp_hdr->dstIp);

	    //ethernet_hdr->FCS = 0; /*Not used*/
	    ETH_FCS(ethernet_hdr, sizeof(arp_header_t)) = 0;

	    /*STEP 3 : Now dispatch the ARP Broadcast Request Packet out of interface*/
	    sendPktOut((char *)ethernet_hdr, sizeof(ethernet_hdr) + sizeof(arp_header_t),oif);

	    free(ethernet_hdr);
}

void dumpArpTable(arp_table_t *arpTable)
{
	gddl_t *curr;
	arp_entry_t *arp_entry;

	ITERATE_GDDL_BEGIN(&arpTable->arp_entries, curr){

		arp_entry = arp_glue_to_arp_entry(curr);
		printf("IP : %s, MAC: %u:%u%u:%u%u:%u, OIF = %s\n",
				arp_entry->ipAddr.ipAddr,
				arp_entry->mac_addr.mac[0],
				arp_entry->mac_addr.mac[1],
				arp_entry->mac_addr.mac[2],
				arp_entry->mac_addr.mac[3],
				arp_entry->mac_addr.mac[4],
				arp_entry->mac_addr.mac[5]
		);

	}ITERATE_GDDL_END(&arpTable->arp_entries, curr)
}


//void sendArpBroadcastRequest(node_t *node, interface_t *oif, char *ipAddr)
//{
//
//}
//
//static void processArpReployMessage(ethernet_heather_t *ethernetHeaderIn, interface_t *oif){
//
//}
//
//static void processArpBroadcastRequest(node_t *node, interface_t *iif, ethernet_heather_t *ethernetHeader)
//{
//
//}
//

static void sendArpReplyMessage(ethernet_heather_t *ethernet_hearder, interface_t *iif)
{

}

static void processArpBroadcastRequest(node_t *node, interface_t *iif, ethernet_heather_t *ethernetHeader)
{
	printf("%s : ARP Broadcast msg received on interface %s of node %s \n",
			__FUNCTION__, iif->att_node->node_name);

	char ipAddr[16];
	arp_header_t *arpHeader = (arp_header_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernetHeader));

	unsigned int arpDestIp = htonl(arpHeader->dstIp);
	inet_ntop(AF_INET, &arpDestIp, ipAddr,16);

	ipAddr[15] = '\0';

	if(strncmp(IF_IP(iif), ipAddr,16))
	{
		printf("%s : ARP Broadcast req meg dropped, Dst IP address %s dod not mtch with interface ip: %s\n",node->node_name, ipAddr, IF_IP(iif));
	}
	sendArpReplyMessage(ethernetHeader, iif);
}

