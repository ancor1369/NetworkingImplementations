/*
 * comm.c
 *
 *  Created on: May 27, 2020
 *      Author: andres
 */

#include "../graph.h"
#include "comm.h"
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>

static unsigned int udpPortNumber = 40000;

static unsigned int getNextUdpPortNumber()
{
	return udpPortNumber++;
}


void initUdpSocket(node_t *node)
{
	node->udpPortNumber = getNextUdpPortNumber();
	int udpSockFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	struct sockaddr_in nodeAddr;
	nodeAddr.sin_family = AF_INET;
	nodeAddr.sin_port = node->udpPortNumber;
	nodeAddr.sin_addr.s_addr = INADDR_ANY;

	if(bind(udpSockFileDescriptor,(struct sockaddr *)&nodeAddr, sizeof(struct sockaddr))==-1)
	{
		printf("Error: socket bind failed for Node %s/n", node->node_name);
		return;
	}
	node->udpSocketFileDescriptor = udpSockFileDescriptor;
}

