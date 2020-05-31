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
#include <math.h>
#include <stdio.h>

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

unsigned int ip_addr_p_to_n(char *ipAddres)
{
	int locator = 0;
	char *local[5];
	local[locator] = ipAddres;
	locator++;

	unsigned int result = 0;

	//'\0'

	//Run this for a maximum of 16 positions in the char array
	for(char * i = ipAddres ;i<(ipAddres + 16);i++)
	{
		//Find the end position of the first integer to capture
		local[locator] = strpbrk(i,".");

		if(locator == 4) break;
		//update the position of the pointer to reflect the starting
		//point of the next string
		i = local[locator];
		locator++;
	}
	//Now that I know where each point is located, I will extract the integers
	//required to calculate the final result int

	char *temp = NULL;
	temp = (char *)malloc(16);
	int exponent = 24; //Used to calculate the output of the number required
	int temporal;
	int offset;
	char *value = NULL;
	int number =0;
	int passedOffset = 0;
	//Extract each number and convert it
	for(int i = 0; i<4;i++)
	{
		offset = (local[i+1]-local[i]);
		//Make sure that the points are not taken into account
		if(i==0)
		 strncpy(temp, local[i], offset);
		else if(i>0 && i<=2)
		 strncpy(temp, (local[i] + 1), (offset-1));
		//Copy the last section of the char *
		else if(i ==3)
		  strcpy(temp, (local[i]+1));
		temporal = pow(2,exponent);
		number = atoi(temp);
		result += ( number * temporal);
		exponent -=8;
		strcpy(temp,"0");
		passedOffset += offset;
	}
	return result;
}







