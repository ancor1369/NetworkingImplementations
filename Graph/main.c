/*
 * main.c
 *
 *  Created on: May 12, 2020
 *      Author: andres
 */

#include "graph.h"


//Declare a reference tot he mehtod that is implemented in topologies.c
extern graph_t *build_first_topo();

int main(int argc, char **argv)
{
	graph_t *topo = build_first_topo();
	dumpGraph(topo);
	return 0;
}
