/*
 * comm.h
 *
 *  Created on: May 27, 2020
 *      Author: andres
 */

#ifndef COMMUNICATION_COMM_H_
#define COMMUNICATION_COMM_H_

#define MAX_PACKET_BUFFER_SIZE 2048

//Any function declared here is a public function that will be available for other
//sections of the program to use

int sendPktOut(char *packet, unsigned int pktSize, interface_t *interface);

int pkt_receive(node_t *node, interface_t *interface, char *pkt, unsigned int pktSize);


#endif /* COMMUNICATION_COMM_H_ */
