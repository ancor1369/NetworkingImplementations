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

//Static makes the members to be private

//Permanent set of memory available for use
static char recvBuffer[MAX_PACKET_BUFFER_SIZE];
static char sendBuffer[MAX_PACKET_BUFFER_SIZE];


int pkt_receive(node_t *node, interface_t *interface, char *pkt, unsigned int pktSize)
{
	/*
	 * Entry point into data link layer from physical layer
	 * Ingress journey of the packet starts from here in TCP/IP stack
	 */

	printf("msg received = %s, on node = %s, IIF = %s\n", pkt, node->node_name, interface->if_name);
	return 0;
}



static void _pkt_receive(node_t *receiving_node, char *pkt_with_aux_data, unsigned int pkt_size)
{
	char *recvIntfName = pkt_with_aux_data;
	interface_t *recvIntef = getNodeIfByName(receiving_node, receiving_node);
	if(!recvIntef){
		printf("Error : pkt received on unknown interface %s on node %s\n",recvIntef->if_name, receiving_node->node_name);
				return ;
	}

	pkt_receive(receiving_node, recvIntef, pkt_with_aux_data + IF_NAME_SIZE, pkt_size - IF_NAME_SIZE);

}

static int _send_pkt_out(int sockFD,char *pktData, unsigned int pktSize, unsigned int dstUdtPortNo)
{
	int rc;
	struct sockaddr_in destAddr;

	struct hostent *host = (struct hostent *)gethostbyname("127.0.0.1");
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = dstUdtPortNo;
	destAddr.sin_addr = *((struct in_addr *)host->h_addr);

	rc = sendto(sockFD, pktData, pktSize, 0, (struct sockaddr *)&destAddr, sizeof(struct sockaddr));

	return rc;
}

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



static void * networkStartPacketReceiverThread(void *arg)
{
	node_t *node;
	gddl_t *curr;

	fd_set activeSockFDSet,
			backupSockFDSet;

	int sockMaxFD = 0;
	int bytesRecevd = 0;

	graph_t *topo = (void *)arg;
	int addrLen = sizeof(struct sockaddr);

	FD_ZERO(&activeSockFDSet);
	FD_ZERO(&backupSockFDSet);

	struct sockaddr_in sender_addr;
	//it is necessary to copy the set of file descriptors
	//so that it is possible to listen to them
	ITERATE_GDDL_BEGIN(&topo->node_list, curr){

			node = graphGlueToNode(curr);
			if(!node->udpSocketFileDescriptor)
				continue;

			if(node->udpSocketFileDescriptor > sockMaxFD)
				sockMaxFD = node->udpSocketFileDescriptor;

			FD_SET(node->udpSocketFileDescriptor, &backupSockFDSet);

	}ITERATE_GDDL_END(&topo->node_list, curr);

	while(1)
	{
		memcpy(&activeSockFDSet, &backupSockFDSet, sizeof(fd_set));
		//this is a system call that gets data arriving over the sockets
		//select will remain blocked unless any of the file descriptors become activated, It unblocks only upon data arrival
		select(sockMaxFD + 1, &activeSockFDSet, NULL,NULL,NULL);
		//Once the code arrives here, it means that a new message has arrived on any of the file descriptors and it is necessary
		//to itereate over the file descriptors to find out what is the one that received the message. Here the message will be processed
		ITERATE_GDDL_BEGIN(&topo->node_list, curr)
		{
			node = graphGlueToNode(curr);
			//Funciton FD_ISSET reivews if the socket is activated
			if(FD_ISSET(node->udpSocketFileDescriptor, &activeSockFDSet)){
				memset(recvBuffer, 0, MAX_PACKET_BUFFER_SIZE);
				bytesRecevd = recvfrom(node->udpSocketFileDescriptor,(char *)recvBuffer,
						MAX_PACKET_BUFFER_SIZE, 0,(struct sockaddr *)&sender_addr, &addrLen);
				_pkt_receive(node, recvBuffer, bytesRecevd);
			}
		}ITERATE_GDDL_END(&topo->node_list, curr);
	}
}


void networkStartPktReceiverThread(graph_t *topo)
{
	pthread_attr_t attr;
	pthread_t receivedPacketThread;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&receivedPacketThread, &attr, networkStartPacketReceiverThread,(void *)topo);
}


int sendPktOut(char *packet, unsigned int pktSize, interface_t *interface)
{
	int rc = 0;

	node_t *sending_node = interface->att_node;
	node_t *nextNode = getNbrNode(interface);

	if(!nextNode)
		return -1;

	unsigned int dstUdtPortNo = nextNode->udpPortNumber;

	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(sock<0)
	{
		printf("Error: Sending socket Creation failed, errno = %d", errno);
		return -1;
	}

	interface_t *otherInterface = &interface->link->intf1 == interface ? &interface->link->intf2 : &interface->link->intf1;

	memset(sendBuffer, 0, MAX_PACKET_BUFFER_SIZE);
	//This is a representation of a char pointer to the initial
	//address of memory where the message is located
	char *pktWithAuxData = sendBuffer;

	//Cppy the name of the interface in the first section of the buffer
	strncpy(pktWithAuxData, otherInterface->if_name, IF_NAME_SIZE);
	pktWithAuxData[IF_NAME_SIZE] = '\0';

	//move the starting point of the buffer by IF_NAME_SIZE and copy the packet content there
	memcpy(pktWithAuxData + IF_NAME_SIZE, packet, pktSize);

	rc = _send_pkt_out(sock, pktWithAuxData, pktSize + IF_NAME_SIZE, dstUdtPortNo);

	close(sock);
	return rc;
}














