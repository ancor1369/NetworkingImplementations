/*
 * net.c
 *
 *  Created on: May 13, 2020
 *      Author: andres
 */

#include "net.h"
#include <memory.h>
#include "../utils.h"
#include <stdio.h>
#include "../graph.h"


void interfaceAssignMacAddress(interface_t *interface)
{
    //Resets the content of the variable
    memset(IF_MAC(interface), 0,48);
    strcpy(IF_MAC(interface), interface->att_node->node_name);
    strcat(IF_MAC(interface), interface->if_name);
}

//APIs to set network properties
bool_t nodeSetDeviceType(node_t *node, unsigned int F)
{
	SET_BIT(node->nodeNwkProp.flags,F);
	return TRUE;
}

bool_t nodeSetLoopBack(node_t *node, char *ipAddr)
{
	assert(ipAddr);
	if(IS_BIT_SET(node->nodeNwkProp.flags, HUB))
		assert(0); //This means that the device is a HUB, so it does not have any IP address

	if(!IS_BIT_SET(node->nodeNwkProp.flags,L3_ROUTER))
		assert(0); // This means that the L3 Routing capacity is not enabled yet

	node->nodeNwkProp.isLbAddrConfig = TRUE;
    //strncpy(node->nodeNwkProp.lbAddr.ipAddr, ipAddr, 16);
	strncpy(NODE_LO_ADDR(node), ipAddr, 16);
	NODE_LO_ADDR(node)[16] = '\0';
	return TRUE;
}

bool_t nodeSetIntfIpAddr(node_t * node, char *localIf, char *ipAddr, char mask)
{

}

bool_t nodeUnsetIntfIpAddr(node_t *node, char *localIf)
{

}


///Dumpoing ufnctions to get information about the setting
void dumpNwkGraph(graph_t *graph)
{

}
void dumpNodeNwkProps(node_t *node)
{

}
void dumpIntfProps(interface_t *interface)
{

}
