/*
 * main.c
 *
 *  Created on: May 12, 2020
 *      Author: andres
 */

#include "graph.h"
#include "CommandParser/libcli.h"
#include "communication/comm.h"

//Declare a reference tot he mehtod that is implemented in topologies.c
extern graph_t *build_first_topo();
//VAriable made global to be accessible over the whole code
graph_t *topo = NULL;

int main(int argc, char **argv)
{
	nwInitCLI();
	topo = build_first_topo();

	sleep(2);

	node_t *snode = getNodeByNodeName(topo, "R0_re");
	interface_t *oif = getNodeIfByName(snode, "eth0/0");

	char msg[] = "This is a test message to be sent \0";
	sendPktOut(msg, strlen(msg), oif);

	start_shell();
	return 0;
}
