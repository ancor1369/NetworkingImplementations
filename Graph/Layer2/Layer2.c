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
#include "../comm.h"


typedef struct macTableEntry_{
	macAddr_t mac;//key
	char oifName[IF_NAME_SIZE];
	gddl_t macEntryGlue //For linked list insertion
}macTableEntry_t;




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


static void processArpReployMessage(node_t *node, interface_t *oif, ethernet_heather_t *ethernetHeader)
{
	printf("%s : ARP reply message received on interface %s of node %s\n",
			__FUNCTION__, oif->if_name, oif->att_node->node_name);

	arpTableUpdateFromArpReply(NODE_ARP_TABLE(node),(arp_header_t*)GET_ETHERNET_HDR_PAYLOAD(ethernetHeader),oif);
}

static void sendArpReplyMessage(ethernet_heather_t *ethernet_hearder, interface_t *iif)
{
	arp_header_t *arpHederIn = (arp_header_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernet_hearder));
	ethernet_heather_t *ethernetHeaderReply = (ethernet_heather_t *)calloc(1,MAX_PACKET_BUFFER_SIZE);

	memcpy(ethernetHeaderReply->dstMAC.mac, arpHederIn->srcMac.mac,sizeof(macAddr_t));
	memcpy(ethernetHeaderReply->srcMAC.mac, IF_MAC(iif), sizeof(macAddr_t));

	ethernetHeaderReply->type = ARP_MSG;
	arp_header_t *arpHeaderReply = (arp_header_t*)(GET_ETHERNET_HDR_PAYLOAD(ethernetHeaderReply));

	arpHeaderReply->hwType = 1;
	arpHeaderReply->protoType = 0x0800;
	arpHeaderReply->hwAddrLen = sizeof(macAddr_t);
	arpHeaderReply->proto_addr_len = 4;

	arpHeaderReply->opCode = ARP_REPLY;
	memcpy(arpHeaderReply->srcMac.mac, IF_MAC(iif), sizeof(macAddr_t));

	inet_pton(AF_INET, IF_IP(iif), &arpHeaderReply->srcIp);
	arpHeaderReply->srcIp = htonl(arpHeaderReply->srcIp);

	memcpy(arpHeaderReply->dstMac.mac, arpHederIn->srcMac.mac, sizeof(macAddr_t));
	arpHeaderReply->dstIp = arpHederIn->srcIp;

	ETH_FCS(ethernetHeaderReply, sizeof(arp_header_t)) = 0;

	unsigned int totalPksSize = ETH_HDR_SIZE_EXCL_PAYLOAD + sizeof(arp_header_t);

	char *shiftedPacketBuffer = pktBufferShiftRight((char *)ethernetHeaderReply, totalPksSize, MAX_PACKET_BUFFER_SIZE);

	sendPktOut(shiftedPacketBuffer, totalPksSize, iif);
	free(ethernetHeaderReply);
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
		printf("%s : ARP Broadcast request message dropped, Destination IP address %s did not match with interface IP: %s\n",node->node_name, ipAddr, IF_IP(iif));
	}
	sendArpReplyMessage(ethernetHeader, iif);
}

void layer2_frame_recv(node_t *node, interface_t *interface,char *pkt, unsigned int pkt_size)
{
    unsigned int vlan_id_to_tag = 0;

    ethernet_heather_t *ethernet_hdr = (ethernet_heather_t *)pkt;

    if(l2FrameReceivedQulifyOnInterface(interface, ethernet_hdr) == FALSE)
    {
        printf("L2 Frame Rejected on node %s\n", node->node_name);
        return;
    }

    printf("L2 Frame Accepted on node %s\n", node->node_name);

    switch(ethernet_hdr->type)
    {
    case ARP_MSG:
		{
			arp_header_t *arp_header = (arp_header_t *)(ethernet_hdr->payload);
			switch (arp_header->opCode) {
				case ARP_BROAD_REQ:
					processArpBroadcastRequest(node, interface, ethernet_hdr);
					break;
				case ARP_REPLY:
					processArpReployMessage(node, interface, ethernet_hdr);
					break;
				default:
					break;
			}
		}
		break;
    default:
    	//promotePktToLayer3(node,interface,pkt,pkt_size);
    	break;

    }
}

/*Interface config APIs for L2 mode configuration*/

void interfaceSetL2Mode(node_t *node, interface_t *interface, char *l2_mode_option)
{
    intfL2Mode_t intf_l2_mode;

    if(strncmp(l2_mode_option, "access", strlen("access")) == 0){
        intf_l2_mode = ACCESS;
    }
    else if(strncmp(l2_mode_option, "trunk", strlen("trunk")) ==0){
        intf_l2_mode = TRUNK;
    }
    else{
        assert(0);
    }

    /*Case 1 : if interface is working in L3 mode, i.e. IP address is configured.
     * then disable ip address, and set interface in L2 mode*/
    if(IS_INTF_L3_MODE(interface)){
        interface->infNwkProps.isIsIPAddrConfig = FALSE;
    	//interface->intf_nw_props.is_ipadd_config_backup = TRUE;

        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }

    /*Case 2 : if interface is working neither in L2 mode or L3 mode, then
     * apply L2 config*/
    if(IF_L2_MODE(interface) == L2_MODE_UNKNOWN){
        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }

    /*case 3 : if interface is operating in same mode, and user config same mode
     * again, then do nothing*/
    if(IF_L2_MODE(interface) == intf_l2_mode){
        return;
    }

    /*case 4 : if interface is operating in access mode, and user config trunk mode,
     * then overwrite*/
    if(IF_L2_MODE(interface) == ACCESS &&
            intf_l2_mode == TRUNK){
        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }

    /* case 5 : if interface is operating in trunk mode, and user config access mode,
     * then overwrite, remove all vlans from interface, user must enable vlan again
     * on interface*/
    if(IF_L2_MODE(interface) == TRUNK &&
           intf_l2_mode == ACCESS){

        IF_L2_MODE(interface) = intf_l2_mode;

        unsigned int i = 0;

//        for ( ; i < MAX_VLAN_MEMBERSHIP; i++){
//            interface->intf_nw_props.vlans[i] = 0;
//        }
    }
}


void nodeSetInfL2Mode(node_t *node, char *infName, intfL2Mode_t l2Mode)
{
	interface_t *interface = getNodeIfByName(node, infName);
	assert(interface);
	interfaceSetL2Mode(node, interface, intfL2ModeStr(l2Mode));
}

