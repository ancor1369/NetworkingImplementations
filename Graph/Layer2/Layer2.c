/*
 * Layer2.c
 *
 *  Created on: May 29, 2020
 *      Author: andres
 */


static inline ethernet_heather_t * Alloc_eth_Header_with_Payload(char *pkt, unsigned pkt_size)
{
	ethernet_heather_t *ethernetHeader;

	//Move the data from the buffer to the new ethernet frame
	memcpy(ethernetHeader->payload, pkt, pkt_size);
	return ethernetHeader;
}
