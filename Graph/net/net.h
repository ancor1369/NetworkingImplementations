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

#include "utils.h"
#include <memory.h>

typedef struct graph_ graph_t;
typedef struct interface_ interface_t;
typedef struct node_ node_t;

typedef struct ipAddr{
	char ipAddr[16];
}ipAddr_t;

typedef struct macAddr{
	char mac[48];
}macAddr_t;

typedef struct nodeNwkProp_{
	unsigned int flags;

	/*L3 properties*/
	bool_t isLbAddrConfig;
	/*Loopback Address and node*/
	ipAddr_t lbAddr;
}nodeNwkProp_t;

static inline void initNodeNwkProp(nodeNwkProp_t *nodeNwkProp)
{
	//TODO:
}



typedef struct intfNwkProps_{

	//L2 propoerties
	macAddr_t macAddr;		//Mac addresses are supposed to be burned in NIC

	//L3 properties
	//Set to true if ip address is configured
	bool_t isIsIPAddrConfig;

	ipAddr_t ipAddr;
	char mask;

}intfNwkProps_t;

static inline void initIntfNwkProp(intfNwkProps_t *intNwkProp)
{
	//TODO:
}
void interfaceAssignMacAddress(interface_t *interface);

//Some macros to get fast information
#define IF_MAC(intfPtr) ((intfPtr)->intfNwkProps.macAddr.mac)
#define IF_IP(intfPtr) ((infPtr)->intfNwkProps.ipAddr.ipAddr)

#define NODE_LO_ADDR(node_ptr) (node_ptr->nodeNwkProp.lbAddr.ipAddr)

//APIs to set network properties
bool_t nodeSetDeviceType(node_t *node, unsigned int F);
bool_t nodeSetLoopBack(node_t *node, char *ipAddr);
bool_t nodeSetIntfIpAddr(node_t * node, char *localIf, char *ipAddr, char mask);
bool_t nodeUnsetIntfIpAddr(node_t *node, char *localIf);


///Dumpoing ufnctions to get information about the setting
void dumpNwkGraph(graph_t *graph);
void dumpNodeNwkProps(node_t *node);
void dumpIntfProps(interface_t *interface);

#endif /* NET_NET_H_ */
