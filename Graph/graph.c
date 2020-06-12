/*
 * graph.c
 *
 *  Created on: May 10, 2020
 *      Author: andres
 */

#include "graph.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


extern void initUdpSocket(node_t *node);

/* Implement the creation of a new graph, allocate memory to hold the data structure
 * and initilize the internal varialbes to be taken care of*/
graph_t *create_new_graph(char *topology_name)
{
	graph_t *graph = calloc(1, sizeof(graph_t));
	strncpy(graph->topology_name, topology_name, 32);
	graph->topology_name[32] = '\0';
	ddlInit(&graph->node_list);
	return graph;
}

/*
 * Creates and initializes all the information of a new node.
 *
 * It is necessary to assign a unique udp port number and
 * a file descriptor for the UDP socket created for this node
 */
node_t *create_graph_node(graph_t *graph, char *node_name)
{
	node_t *node = calloc(1,sizeof(node_t));
	strncpy(node->node_name,node_name, NODE_NAME_SIZE);
	node->node_name[NODE_NAME_SIZE - 1] = '\0';

	initUdpSocket(node);

	initNodeNwkProp(&node->nodeNwkProp);
	ddlInit(&node->graph_glue);
	ddlAddNext(&graph->node_list,&node->graph_glue);

	return node;
}

void insert_link_between_two_nodes(node_t *node1,node_t *node2,
									char *from_if_name, char *to_if_name,unsigned int cost)
{
	//Create a link in memory
	link_t *link = calloc(1,sizeof(link_t));
	//Set the names of the link
    strncpy(link->intf1.if_name, from_if_name, IF_NAME_SIZE);
    link->intf1.if_name[IF_NAME_SIZE -1 ] = '\0';
    strncpy(link->intf2.if_name, to_if_name, IF_NAME_SIZE);
    link->intf2.if_name[IF_NAME_SIZE - 1] = '\0';

    //set backpointers to link
    link->intf1.link = link;
    link->intf2.link = link;

    link->intf1.att_node = node1;
    link->intf2.att_node = node2;
    link->cost = cost;

    int emptyInitSlot;

    /*Plug interface ends into Node*/

    emptyInitSlot = get_node_intf_available_slot(node1);
    node1->intf[emptyInitSlot] = &link->intf1;

    emptyInitSlot = get_node_intf_available_slot(node2);
    node2->intf[emptyInitSlot] = &link->intf2;

    initIntfNwkProp(&link->intf1.infNwkProps);
    initIntfNwkProp(&link->intf2.infNwkProps);

    /*Assign random Generated MAC address to the interface*/
    interfaceAssignMacAddress(&link->intf1);
    interfaceAssignMacAddress(&link->intf2);

}


//Display information routines!
void dumpGraph(graph_t *graph)
{
	gddl_t *current;
	node_t *node;
	//pirnt the name of the graph
	printf("Topology Name = %s \n", graph->topology_name);

	ITERATE_GDDL_BEGIN(&graph->node_list, current){
		node = graphGlueToNode(current);
		dumpNode(node);
	}ITERATE_GDDL_END(&graph->node_list, current);



	return;
}
void dumpNode(node_t *node)
{
	interface_t *intf;
	//print the node name
	printf(node->node_name);
	for(int i=0;i<MAX_INTF_PER_NODE;i++)
	{
		intf = node->intf[i];
		if(!intf) break;
		dumpInterface(node->intf[i]);
	}
	return;
}
void dumpInterface(interface_t *interface)
{
   link_t *link=interface->link;
   node_t *nbrNode = getNbrNode(interface);
   //print the name of the interface
   printf("Local Node : %s, Interface Name = %s, Nbr Node %s, Cost = %u\n",
		   interface->att_node->node_name,interface->if_name, nbrNode->node_name, link->cost);
   return;
}

//return a pointer to the local interface of a node.
static inline interface_t *getNodeIntfByName(node_t * node, char *ifName)
{
	unsigned int i = 0;
	for(;i<MAX_INTF_PER_NODE;i++)
	{
		//Review that the interface exists
		if(!node->intf[i]) return NULL;
		//Verify the name of the i-th interface and return the one
		//that matches the interface name
		if(strncmp(node->intf[i]->if_name,ifName,IF_NAME_SIZE)==0)
		{
			return node->intf[i];
		}
	}
	// If the code reaches this pint, means that the
	// interface name does not exist in the current node
	return NULL;
}


//return a node pointer to the node when searched by name
//static
//inline
node_t * getNodeByNodeName(graph_t * topo, char * nodeName)
{
	node_t *temp = NULL;
	gddl_t *nodeTemp = NULL;
	gddl_t *baseNode = &topo->node_list;
	ITERATE_GDDL_BEGIN(baseNode,nodeTemp){
		temp = graphGlueToNode(nodeTemp);
		if(strncmp(temp->node_name, nodeName,IF_NAME_SIZE)==0)
		{
			return temp;
		}
	}ITERATE_GDDL_END(topo->node_list, tempNode->graph_glue);
	return NULL;
}



