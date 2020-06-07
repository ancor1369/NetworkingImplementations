/*
 * main.c
 *
 *  Created on: May 12, 2020
 *      Author: andres
 */

#include "graph.h"
#include "CommandParser/libcli.h"
#include "comm.h"

#include "utils/utils.h"

//Declare a reference tot he mehtod that is implemented in topologies.c
extern graph_t *build_first_topo();
extern graph_t *build_lineal_topo();


extern int sendPktOut(char *packet, unsigned int pktSize, interface_t *interface);
//VAriable made global to be accessible over the whole code
graph_t *topo = NULL;

int main(int argc, char **argv)
{
	nwInitCLI();
	topo = build_first_topo();
//	topo = build_lineal_topo();

	start_shell();

//	char *ip_add_str = NULL;
//	ip_add_str = (char*)malloc(16);
//	char *IP = "192.168.0.15\0";
//	unsigned int result = 0;
//	result = ip_addr_p_to_n(IP);
//	printf("This is the result: %u \n", result);
//	ip_addr_n_to_p(result, ip_add_str);
//	printf("This is the ip again. %s \n", ip_add_str);


	return 0;
}
