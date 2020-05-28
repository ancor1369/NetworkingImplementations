/*
 * comm.h
 *
 *  Created on: May 27, 2020
 *      Author: andres
 */

#ifndef COMMUNICATION_COMM_H_
#define COMMUNICATION_COMM_H_

#define MAX_PACKET_BUFFER_SIZE 2048

//This is a public API function so we declare it in this file
int sendPktOut(char *packet, unsigned int pktSize, interface_t *interface);


#endif /* COMMUNICATION_COMM_H_ */
