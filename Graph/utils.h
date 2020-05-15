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

#endif /* UTILS_H_ */
