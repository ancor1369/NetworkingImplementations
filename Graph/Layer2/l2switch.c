/*
 * l2switch.c
 *
 *  Created on: Jun. 14, 2020
 *      Author: andres
 */

#include "Layer2.h"
#include "../gddl/gddl.h"
#include "../graph.h"
#include <stdio.h>
#include <stdlib.h>
#include "../comm.h"





typedef struct macTableEntry_{
	macAddr_t mac;//key
	char oifName[IF_NAME_SIZE];
	gddl_t macEntryGlue //For linked list insertion
}macTableEntry_t;
GLTHREAD_TO_STRUCT(mac_entry_glue_to_mac_entry, macTableEntry_t, macEntryGlue);

typedef struct mac_table_{
	gddl_t macEntries;
}mac_table_t;

#define IS_MAC_TABLE_ENTRY_EQUAL(mac_entry_1, mac_entry_2)   \
    (strncmp(mac_entry_1->mac.mac, mac_entry_2->mac.mac, sizeof(macAddr_t)) == 0 && \
            strncmp(mac_entry_1->oifName, mac_entry_2->oifName, IF_NAME_SIZE) == 0)


macTableEntry_t *macTableLookUp(mac_table_t *macTable, char *mac)
{
	 gddl_t *curr;
	 macTableEntry_t *mac_table_entry;

	 ITERATE_GDDL_BEGIN(&macTable->macEntries, curr)
	 {
		 mac_table_entry = mac_entry_glue_to_mac_entry(curr);
	     if(strncmp(mac_table_entry->mac.mac, mac, sizeof(macAddr_t)) == 0){
	            return mac_table_entry;
	        }
	    } ITERATE_GDDL_END(&macTable->macEntries, curr);
	    return NULL;
}

void deleteMacTableEntry(mac_table_t *macTable, char *mac)
{
	macTableEntry_t *mac_table_entry;
	    mac_table_entry = macTableLookUp(macTable, mac);
	    if(!mac_table_entry)
	        return;

	    ddlRemove(&mac_table_entry->macEntryGlue);
	    free(mac_table_entry);
}

void initMacTable(mac_table_t **macTable)
{
	 *macTable = calloc(1, sizeof(mac_table_t));
	 ddlInit(&((*macTable)->macEntries));
}


bool_t macTableEntryAdd(mac_table_t *macTable, macTableEntry_t *macTableEntry)
{
	macTableEntry_t *mac_table_entry_old = macTableLookUp(macTable,	macTableEntry->mac.mac);

	    if(mac_table_entry_old &&
	            IS_MAC_TABLE_ENTRY_EQUAL(mac_table_entry_old, macTableEntry)){

	        return FALSE;
	    }

	    if(mac_table_entry_old){
	    	deleteMacTableEntry(macTable, mac_table_entry_old->mac.mac);
	    }

	    ddlInit(&macTableEntry->macEntryGlue);
	    ddlAddNext(&macTable->macEntries, &macTableEntry->macEntryGlue);
	    return TRUE;
}

void dump_mac_table(mac_table_t *mac_table)
{
    gddl_t *curr;
    macTableEntry_t *mac_table_entry;

    ITERATE_GDDL_BEGIN(&mac_table->macEntries, curr)
    {

        mac_table_entry = mac_entry_glue_to_mac_entry(curr);
        printf("\tMAC : %u:%u:%u:%u:%u:%u   | Intf : %s\n",
            mac_table_entry->mac.mac[0],
            mac_table_entry->mac.mac[1],
            mac_table_entry->mac.mac[2],
            mac_table_entry->mac.mac[3],
            mac_table_entry->mac.mac[4],
            mac_table_entry->mac.mac[5],
            mac_table_entry->oifName);
    }ITERATE_GDDL_END(&mac_table->macEntries, curr);
}



static void l2_switch_perform_mac_learning(node_t *node, char *src_mac, char *if_name){

    bool_t rc;

    macTableEntry_t *mac_table_entry = calloc(1, sizeof(macTableEntry_t));
    memcpy(mac_table_entry->mac.mac, src_mac, sizeof(macAddr_t));
    strncpy(mac_table_entry->oifName, if_name, IF_NAME_SIZE);
    mac_table_entry->oifName[IF_NAME_SIZE - 1] = '\0';
    rc = macTableEntryAdd(NODE_MAC_TABLE(node), mac_table_entry);
    if(rc == FALSE){
        free(mac_table_entry);
    }
}




static void l2_switch_forward_frame(node_t *node, interface_t *recv_intf,
                        char *pkt, unsigned int pkt_size){

    /*If dst mac is broadcast mac, then flood the frame*/
    ethernet_heather_t *ethernet_hdr = (ethernet_heather_t *)pkt;
    if(IS_MAC_BROADCAST_ADDR(ethernet_hdr->dstMAC.mac)){
        sendPktFlood(node, recv_intf, pkt, pkt_size);
        return;
    }

    /*Check the mac table to forward the frame*/
    macTableEntry_t *mac_table_entry =
	macTableLookUp(NODE_MAC_TABLE(node), ethernet_hdr->dstMAC.mac);


    if(!mac_table_entry){
    	sendPktFlood(node, recv_intf, pkt, pkt_size);
        return;
    }

    char *oif_name = mac_table_entry->oifName;
    interface_t *oif = getNodeIfByName(node, oif_name);
    if(!oif){
        return;
    }
    sendPktOut(pkt, pkt_size, oif);
}


void l2SwitchRecvFrame(interface_t *interface,char *pkt, unsigned int pkt_size)
{
    node_t *node = interface->att_node;

    ethernet_heather_t *ethernet_hdr = (ethernet_heather_t *)pkt;

    char *dst_mac = (char *)ethernet_hdr->dstMAC.mac;
    char *src_mac = (char *)ethernet_hdr->srcMAC.mac;

    l2_switch_perform_mac_learning(node, src_mac, interface->if_name);
    l2_switch_forward_frame(node, interface, pkt, pkt_size);
}
