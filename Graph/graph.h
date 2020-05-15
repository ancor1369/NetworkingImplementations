/*
 * graph.h
 *
 *  Created on: May 10, 2020
 *      Author: andres
 */

#ifndef GRAPH_H_
#define GRAPH_H_
#include <assert.h>
#include "gddl/gddl.h"
#include "net/net.h"


#define IF_NAME_SIZE 16
#define NODE_NAME_SIZE 16
#define MAX_INTF_PER_NODE 10

//Macros to make fast operations over the data structure interface
#define IF_MAC(intfPtr) ((intfPtr)->infNwkProps.macAddr.mac)
#define IF_IP(intfPtr) ((intfPtr)->infNwkProps.ipAddr.ipAddr)

//forward declaration
typedef struct node_ node_t;
typedef struct link_ link_t;
//Review appendix D

typedef struct interface_ {

	char if_name[IF_NAME_SIZE];
	struct node_ *att_node;
	struct link_ *link;
	//Assigning network properties to the interface type
	intfNwkProps_t infNwkProps;

}interface_t;

struct link_{
	interface_t intf1;
	interface_t intf2;
	unsigned int cost;
};

struct node_{
	char node_name[NODE_NAME_SIZE];
	interface_t *intf[MAX_INTF_PER_NODE];
	gddl_t graph_glue;
	//Assigning node network properties to this type
	nodeNwkProp_t nodeNwkProp;
};

GLTHREAD_TO_STRUCT(graphGlueToNode, node_t, graph_glue);

typedef struct graph_{
	char topology_name[32];
	//This is an implementation of an special linked list
	gddl_t node_list;
}graph_t;

//Function to create a new graph!
graph_t *create_new_graph(char *topology_name);

node_t *create_graph_node(graph_t *graph, char *node_name);

void insert_link_between_two_nodes(node_t *node1,
									node_t *node2,
									char *from_if_name,
									char *to_if_name,
									unsigned int cost);


static inline node_t* getNbrNode(interface_t *interface)
{
	assert(interface->att_node);
	assert(interface->link);

	link_t *link = interface->link;
	if(&link->intf1 == interface)
		return link->intf2.att_node;
	else
		link->intf1.att_node;

}

static inline int get_node_intf_available_slot(node_t *node){

    int i ;
    for( i = 0 ; i < MAX_INTF_PER_NODE; i++){
        if(node->intf[i])
            continue;
        return i;
    }
    return -1;
}

//return a pointer to the local interface of a node.
static inline interface_t *getNodeByName(node_t * node, char *ifName);


//retur a node pointer by name
static inline node_t * getNodeByNodeName(graph_t * topo, char * nodeName);


void dumpGraph(graph_t *graph);
void dumpNode(node_t *node);
void dumpInterface(interface_t *interface);


#endif /* GRAPH_H_ */
