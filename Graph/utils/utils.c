/*
 * utils.c
 *
 *  Created on: May 17, 2020
 *      Author: andres
 */

#include "utils.h"
#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

/*
 * prefix: pointer to an string representing an IP address
 * mask is any value 0-32
 * strPrefix: pointer to a 16 byte char array, the output fills the array when finishing
 */

void applyMask(char *prefix, char mask, char *strPrefix)
{
	uint32_t binary_prefix = 0;
	uint32_t subnet_mask = 0xFFFFFFFF;

	//Default case when the mask takes the whole representation span
	if(mask == 32)
	{
		strncpy(strPrefix, prefix, 16);
		strPrefix[15] = '\0';
		return;
	}

	/*Use arpa libraries to convert data*/
	inet_pton(AF_INET, prefix, &binary_prefix);
	binary_prefix = htonl(binary_prefix);

	//The mask is calculated by making some bitwise operations, in this case
	//It will rotate the number to the left to leave insert zeroes on the right

	subnet_mask = subnet_mask << (32 - mask);

	//Compute the bitwise and operation between the two members of the function
	//This operation simply takes the mask and eliminates the non network part
	binary_prefix = binary_prefix & subnet_mask;

	//Convert the network obtained to a viewable way again
	binary_prefix = htonl(binary_prefix);
	inet_ntop(AF_INET, &binary_prefix, strPrefix, 16);
	strPrefix[15] = '\0';

}


void layer2FillWithBroadcast(char *macArray)
{
//	char *broadcast = "FF:FF:FF:FF:FF:FF";
//	strncpy(macArray,broadcast, sizeof(broadcast));
	int i;
	for(i = 0;i<6;i++)
	{
		macArray[i] = 0XFF;
	}
}



