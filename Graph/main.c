/*
 * main.c
 *
 *  Created on: May 12, 2020
 *      Author: andres
 */

#include "graph.h"
#include "CommandParser/libcli.h"
#include "comm.h"

//Declare a reference tot he mehtod that is implemented in topologies.c
extern graph_t *build_first_topo();
extern graph_t *build_lineal_topo();


extern int sendPktOut(char *packet, unsigned int pktSize, interface_t *interface);
//VAriable made global to be accessible over the whole code
graph_t *topo = NULL;

int main(int argc, char **argv)
{
	nwInitCLI();
//	topo = build_first_topo();
	topo = build_lineal_topo();

	sleep(2);
//
//	node_t *snode = getNodeByNodeName(topo, "R0_re");
//	interface_t *oif = getNodeIfByName(snode, "eth0/0");
//
//	char msg[] = "This is a test message to be sent \0";
//	sendPktOut(msg, strlen(msg), oif);

	start_shell();
	return 0;
}
