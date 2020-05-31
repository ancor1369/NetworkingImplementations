/*
 * utils.h
 *
 *  Created on: May 14, 2020
 *      Author: andres
 */

#ifndef UTILS_H_
#define UTILS_H_

//#define IS_MAC_BROADCAST_ADDR(mac) \
//	int i;							\
//   for(i=0;i<6;i++)     			\
//   if(mac[i]!=0xFF){ \
//	   return FALSE; } \
//	   }  if(i==5) return true;\

#define IS_MAC_BROADCAST_ADDR(mac) \
	(mac[0]==0xFF && mac[1] == 0xFF && mac[2]==0xFF && \
	mac[3] == 0xFF  && mac[4]==0xFF && mac[5] == 0xFF )




typedef enum{
	FALSE,
	TRUE
}bool_t;

#define IS_BIT_SET(n,pos) 	((n & (1 <<(pos))) != 0)
#define TOGGLE_BIT(n, pos)	(n = n ^ (1 << (pos)))
#define COMPLIMENT(num) (num = num ^ 0xFFFFFFFF)
#define UNISET_BIT(n,pos) (n = n & ((1 << pos))^0xFFFFFFFF)
#define SET_BIT(n, pos) (n= n | 1 << pos)

/*
 * prefix: pointer to an string representing an IP address
 * mask is any value 0-32
 * strPrefix: pointer to a 16 byte char array, the output fills the array when finishing
 */
void applyMask(char *prefix, char mask, char *strPrefix);

void layer2FillWithBroadcast(char *macArray);

unsigned int ip_addr_p_to_n(char *ipAddres);

void ip_addr_n_to_p(unsigned int ip_addr, char *ip_add_str);


#endif /* UTILS_H_ */
