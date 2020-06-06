/*
 * Layer2.c
 *
 *  Created on: May 29, 2020
 *      Author: andres
 */

#include "Layer2.h"
#include "../gddl/gddl.h"
#include "../tcpconstants.h"

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


