/*
 * net.h
 *
 *  Created on: May 13, 2020
 *      Author: andres
 *
 *  This class contains details about the interface information that
 *  is needed to implement the network details
 *
 */

#ifndef NET_NET_H_
#define NET_NET_H_

#include "../utils/utils.h"
#include <memory.h>


/* Device IDS */
#define L3_ROUTER   (1 << 0)
#define L2_SWITCH   (1 << 1)
#define HUB         (1 << 2)

#define NODE_ARP_TABLE(node_ptr)    (node_ptr->nodeNwkProp.arpTable)

typedef struct graph_ graph_t;
typedef struct interface_ interface_t;
typedef struct node_ node_t;

//This can represent even an IPv6 Ip address which is 16 bytes long
//It requires to be unsigned because char is composed of 8 bits each from 0 255.
typedef struct ipAddr{
	unsigned char ipAddr[16];
}ipAddr_t;

typedef struct macAddr{
	char mac[6];
}macAddr_t;

typedef struct arp_table_ arp_table_t;

typedef struct nodeNwkProp_{
	unsigned int flags;

	/*L3 properties*/
	bool_t isLbAddrConfig;

	ipAddr_t lbAddr; /*Loopback Address and node*/

	/*L2 properties*/
	arp_table_t *arpTable;

}nodeNwkProp_t;


typedef struct intfNwkProps_{
	//L2 propoerties
	macAddr_t macAddr;		//Mac addresses are supposed to be burned in NIC
	//L3 properties
	//Set to true if ip address is configured
	bool_t isIsIPAddrConfig;
	ipAddr_t ipAddr;
	char mask;
}intfNwkProps_t;

extern void initArpTable(arp_table_t **arptable);

static inline void initNodeNwkProp(nodeNwkProp_t *nodeNwkProp)
{
	nodeNwkProp->flags = 0;
	nodeNwkProp->isLbAddrConfig = FALSE;
	memset(nodeNwkProp->lbAddr.ipAddr,0,16);
	initArpTable(&(nodeNwkProp->arpTable));
}

static inline void initIntfNwkProp(intfNwkProps_t *intNwkProp)
{
	memset(intNwkProp->ipAddr.ipAddr,0,sizeof(intNwkProp->ipAddr.ipAddr));
	intNwkProp->isIsIPAddrConfig = FALSE;
	memset(intNwkProp->macAddr.mac,0,sizeof(intNwkProp->macAddr.mac));
	intNwkProp->mask = 0;;
}


void interfaceAssignMacAddress(interface_t *interface);

//Some macros to get fast information
#define IF_MAC(intfPtr) ((intfPtr)->infNwkProps.macAddr.mac)
#define IF_IP(intfPtr) ((intfPtr)->infNwkProps.ipAddr.ipAddr)

#define NODE_LO_ADDR(nodePtr) (nodePtr->nodeNwkProp.lbAddr.ipAddr)

#define IS_INTF_L3_MODE(intfPtr) (intfPtr->infNwkProps.isIsIPAddrConfig == TRUE)

//APIs to set network properties
bool_t nodeSetDeviceType(node_t *node, unsigned int F);
bool_t nodeSetLoopBack(node_t *node, char *ipAddr);
bool_t nodeSetIntfIpAddr(node_t * node, char *localIf, char *ipAddr, char mask);
bool_t nodeUnsetIntfIpAddr(node_t *node, char *localIf);


///Dumping functions to get information about the setting
void dumpNwkGraph(graph_t *graph);
void dumpNodeNwkProps(node_t *node);
void dumpIntfProps(interface_t *interface);


interface_t * nodeGetMatchingSubnetInterface(node_t *node, char *ipAddr);

unsigned int convertIpFromStrToInt(char *ipAddr);

void convertIpFromIntToStr(unsigned int ipAddr, char *ipAddrOut);

char * pktBufferShiftRight(char *pkt, unsigned int pktSize, unsigned int totalBufferSize);

#endif /* NET_NET_H_ */
