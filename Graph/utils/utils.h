/*
 * utils.h
 *
 *  Created on: May 14, 2020
 *      Author: andres
 */

#ifndef UTILS_H_
#define UTILS_H_

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

#endif /* UTILS_H_ */
