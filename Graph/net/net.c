/*
 * net.c
 *
 *  Created on: May 13, 2020
 *      Author: andres
 */

#include "net.h"
#include <memory.h>
#include "../utils/utils.h"
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
//	if(IS_BIT_SET(node->nodeNwkProp.flags, HUB))
//		assert(0); //This means that the device is a HUB, so it does not have any IP address
//
//	if(!IS_BIT_SET(node->nodeNwkProp.flags,L3_ROUTER))
//		assert(0); // This means that the L3 Routing capacity is not enabled yet

	node->nodeNwkProp.isLbAddrConfig = TRUE;
    //strncpy(node->nodeNwkProp.lbAddr.ipAddr, ipAddr, 16);
	strncpy(NODE_LO_ADDR(node), ipAddr, 16);
	NODE_LO_ADDR(node)[16] = '\0';
	return TRUE;
}

bool_t nodeSetIntfIpAddr(node_t * node, char *localIf, char *ipAddr, char mask)
{
	interface_t *intrface = getNodeIfByName(node, localIf);
	strncpy(IF_IP(intrface),ipAddr, 16);
	IF_IP(intrface)[16] = '\0';
	intrface->infNwkProps.mask = mask;
	intrface->infNwkProps.isIsIPAddrConfig = TRUE;
	return TRUE;
}

bool_t nodeUnsetIntfIpAddr(node_t *node, char *localIf)
{
	interface_t *interface = getNodeIfByName(node, localIf);
	memset(IF_IP(interface),0, 16);
	interface->infNwkProps.isIsIPAddrConfig = FALSE;
	memset(interface->infNwkProps.mask, 0,48);
	return TRUE;
}

void dumpNodeNwkProps(node_t *node)
{
	printf("\nNode Name = %s\n", node->node_name);
	printf("\t node flags: %u", node->nodeNwkProp.flags);
	if(node->nodeNwkProp.isLbAddrConfig){
		printf("\t lo addr: %s/32\n", NODE_LO_ADDR(node));
	}
}

void dumpIntfProps(interface_t *interface)
{
	dumpInterface(interface);
	if(interface->infNwkProps.isIsIPAddrConfig){
		printf("\t IP Addr = %s\n", IF_IP(interface), interface->infNwkProps.mask);
	}
	else{
		printf("\t IP Addr = %s/%u", "Nil", 0);
	}
	printf("\t MAC : %u:%u:%u:%u:%u:%u:\n",
			IF_MAC(interface)[0], IF_MAC(interface)[1],
			IF_MAC(interface)[2], IF_MAC(interface)[3],
			IF_MAC(interface)[4], IF_MAC(interface)[5]);
}

///Dumping functions to get information about the setting
void dumpNwkGraph(graph_t *graph)
{
	node_t *node;
	gddl_t *curr;
	interface_t *interface;
	unsigned int i;

	printf("Topology Name = %s\n", graph->topology_name);


	ITERATE_GDDL_BEGIN(&graph->node_list, curr){
		node = graphGlueToNode(curr);
		dumpNodeNwkProps(node);
		for(i = 0; i < MAX_INTF_PER_NODE; i++){
			interface = node->intf[i];
			if(!interface) break;
			dumpIntfProps(interface);

			char result[16];
			char *intfAddr = NULL;
			intfAddr = IF_IP(interface);
			applyMask(intfAddr, interface->infNwkProps.mask,result);
			printf("result %s\n", result);
		}

	}ITERATE_GDDL_END(&graph->node_list, curr);

}

