/*
 * net.c
 *
 *  Created on: May 13, 2020
 *      Author: andres
 */

#include "net.h"

void interfaceAssignMacAddress(interface_t *interface)
{

}

//APIs to set network properties
bool_t nodeSetDeviceType(node_t *node, unsigned int F)
{

}

bool_t nodeSetLoopBack(node_t *node, char *ipAddr)
{

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
