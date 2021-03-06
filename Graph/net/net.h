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
#define NODE_MAC_TABLE(node_ptr)    (node_ptr->nodeNwkProp.mac_table)
#define IS_INTF_L3_MODE(intf_ptr)   (intf_ptr->nodeNwkProp.isLbAddrConfig == TRUE)


typedef struct graph_ graph_t;
typedef struct interface_ interface_t;
typedef struct node_ node_t;
typedef struct mac_table_ mac_table_t;

typedef enum{

    ACCESS,
    TRUNK,
    L2_MODE_UNKNOWN
} intfL2Mode_t;


static inline char * intfL2ModeStr(intfL2Mode_t intf_l2_mode)
{
    switch(intf_l2_mode){
        case ACCESS:
            return "access";
        case TRUNK:
            return "trunk";
        default:
            return "L2_MODE_UNKNWON";
    }
}

//This can represent even an IPv6 Ip address which is 16 bytes long
//It requires to be unsigned because char is composed of 8 bits each from 0 255.
#pragma pack (push,1)
typedef struct ipAddr{
	unsigned char ipAddr[16];
}ipAddr_t;

typedef struct macAddr{
	unsigned char mac[6];
}macAddr_t;
#pragma pack(pop)

typedef struct arp_table_ arp_table_t;

typedef struct nodeNwkProp_{
	unsigned int flags;

	/*L3 properties*/
	bool_t isLbAddrConfig;

	ipAddr_t lbAddr; /*Loopback Address and node*/

	/*L2 properties*/
	arp_table_t *arpTable;
	mac_table_t *mac_table;

}nodeNwkProp_t;


typedef struct intfNwkProps_{
	//L2 propoerties
	macAddr_t macAddr;		//Mac addresses are supposed to be burned in NIC
	//L3 properties
	//Set to true if ip address is configured
	bool_t isIsIPAddrConfig;
	ipAddr_t ipAddr;
	char mask;
	intfL2Mode_t infL2Mode;  //If IP-Address  is configured on this interface, this property should be configured as UNKNOWN
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
#define IF_L2_MODE(intf_ptr)    (intf_ptr->infNwkProps.infL2Mode)
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

interface_t * node_get_matching_subnet_interface(node_t *node, char *ip_addr);




#endif /* NET_NET_H_ */
