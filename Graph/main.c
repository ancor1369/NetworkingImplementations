/*
 * main.c
 *
 *  Created on: May 12, 2020
 *      Author: andres
 */

#include "graph.h"
#include "CommandParser/libcli.h"

//Declare a reference tot he mehtod that is implemented in topologies.c
extern graph_t *build_first_topo();
//VAriable made global to be accessible over the whole code
graph_t *topo = NULL;

int main(int argc, char **argv)
{
	nwInitCLI();
	topo = build_first_topo();
	start_shell();
	return 0;
}
