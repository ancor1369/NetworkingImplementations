/*
 * topologies.c
 *
 *  Created on: May 10, 2020
 *      Author: andres
 */


#include "graph.h"

extern void networkStartPktReceiverThread(graph_t *topo);

graph_t * build_first_topo()
{


#if 0

                          +----------+
                      0/4 |          |0/0
         +----------------+   R0_re  +---------------------------+
         |     40.1.1.1/24| 122.1.1.0|20.1.1.1/24                |
         |                +----------+                           |
         |                                                       |
         |                                                       |
         |                                                       |
         |40.1.1.2/24                                            |20.1.1.2/24
         |0/5                                                    |0/1
     +---+---+                                              +----+-----+
     |       |0/3                                        0/2|          |
     | R2_re +----------------------------------------------+    R1_re |
     |       |30.1.1.2/24                        30.1.1.1/24|          |
     +-------+                                              +----------+

#endif

	graph_t *topo = create_new_graph("This is the very first topology");

	node_t *R0_re = create_graph_node(topo, "R0_re");
	node_t *R1_re = create_graph_node(topo, "R1_re");
	node_t *R2_re = create_graph_node(topo, "R2_re");

	insert_link_between_two_nodes(R0_re, R1_re, "eth0/0","eth0/1",1);
	insert_link_between_two_nodes(R1_re, R2_re, "eth0/2","eth0/3",1);
	insert_link_between_two_nodes(R0_re, R2_re, "eth0/4","eth0/5",1);

	nodeSetLoopBack(R0_re, "122.1.1.0");
	nodeSetIntfIpAddr(R0_re, "eth0/4", "40.1.1.1",24);
	nodeSetIntfIpAddr(R0_re, "eth0/0", "20.1.1.1",24);

	nodeSetLoopBack(R1_re, "122.1.1.1");
	nodeSetIntfIpAddr(R1_re, "eth0/1", "20.1.1.2",24);
	nodeSetIntfIpAddr(R1_re, "eth0/2", "30.1.1.1",24);

	nodeSetLoopBack(R2_re, "122.1.1.2");
	nodeSetIntfIpAddr(R2_re, "eth0/3", "30.1.1.2",24);
	nodeSetIntfIpAddr(R2_re, "eth0/5", "40.1.1.2",24);


	networkStartPktReceiverThread(topo);

	return topo;

}

graph_t *build_lineal_topo()
{
	graph_t *topo = create_new_graph("Lineal Topo");

	node_t *H1 = create_graph_node(topo,"H1");
	node_t *H2 = create_graph_node(topo,"H2");
	node_t *H3 = create_graph_node(topo, "H3");

	insert_link_between_two_nodes(H1, H2, "eth0/1", "eth0/2",1);
	insert_link_between_two_nodes(H2,H3, "eth0/3", "eth0/4",1);

	nodeSetLoopBack(H1, "127.1.1.1");
	nodeSetLoopBack(H2,"127.1.1.2");
	nodeSetLoopBack(H3,"127.1.1.3");


	nodeSetIntfIpAddr(H1,"eth0/1","10.1.1.1",24);
	nodeSetIntfIpAddr(H2,"eth0/2","10.1.1.2",24);
	nodeSetIntfIpAddr(H2,"eth0/3","20.1.1.2",24);
	nodeSetIntfIpAddr(H3,"eth0/4","20.1.1.1",24);

	networkStartPktReceiverThread(topo);

	return topo;
}

