/*
 * gddl.c
 *
 *  Created on: May 11, 2020
 *      Author: andres
 */

#include "gddl.h"
#include <stdlib.h>



void ddlInit(gddl_t *baseGddl)
{
	baseGddl->left = NULL;
	baseGddl->right = NULL;
}

/*
 *This function will add a new gddl_t after the indicated gddl
 */
void ddlAddNext(gddl_t *baseGddl, gddl_t *newGddl)
{
	//This means that the base gddl is the last one
	if(!baseGddl->right)
	{
		baseGddl->right = newGddl;
		newGddl->left = baseGddl;
		return;
	}
	//in the case this is not the first one, it will get the a temp, insert and the
	//leave everything as it was
	gddl_t *temp = baseGddl->right;
	baseGddl->right = newGddl;
	newGddl->left = baseGddl;
	newGddl->right = temp;
	temp->left = newGddl;
	return;
}

//Adds a node before the base gddl
void ddlAddBefore(gddl_t *baseGddl, gddl_t *newGddl)
{
	//In the case the base gddl is the first in the list
	if(!baseGddl->left)
	{
		baseGddl->left = newGddl;
		newGddl->right = baseGddl;
		newGddl->left = NULL;
		return;
	}
	gddl_t *temp = baseGddl->left;
	baseGddl->left = newGddl;
	newGddl->left = temp;
	newGddl->right = baseGddl;
	temp->left = newGddl;
}

//Add a gddl at the end of the gdll
void ddlAddLast(gddl_t *baseGddl, gddl_t *newGddl)
{
	gddl_t *gddlptr = NULL, *prevgddlptr = NULL;

	ITERATE_GDDL_BEGIN(baseGddl, gddlptr){
		prevgddlptr = gddlptr;
	}ITERATE_GDDL_END(baseGddl, gddlptr);

	if(prevgddlptr)
		ddlAddNext(prevgddlptr, newGddl);
	else
		ddlAddNext(baseGddl, newGddl);

}

//Remove a ggdl from the list
void ddlRemove(gddl_t *toRemove)
{
	if(!toRemove->left)
	{
		if(toRemove->right)
		{
			toRemove->right->left = NULL;
			toRemove->right = 0;
			return;
		}
		return;
	}
	if(!toRemove->right)
	{
		toRemove->left->right = NULL;
		toRemove->left = NULL;
		return;
	}
	toRemove->left->right = toRemove->right;
	toRemove->right->left = toRemove->left;
	toRemove->left = 0;
	toRemove->right = 0;
}

void ddlDeleteList(gddl_t *baseGddl)
{
	gddl_t *gddlptr = NULL;

	ITERATE_GDDL_BEGIN(baseGddl, gddlptr){
		ddlRemove(gddlptr);
	}ITERATE_GDDL_END(baseGddl, gddlptr);

}

unsigned int ddlGetListCount (gddl_t *baseGddl)
{
	unsigned int i=0;
	gddl_t *gddlptr = NULL;
	ITERATE_GDDL_BEGIN(baseGddl, gddlptr){
		i++;
	}ITERATE_GDDL_END(baseGddl, gddlptr);
	return i;
}

void ddlPriorityInsert(gddl_t *baseGddl,
					   gddl_t *gddl,
                       int (*comp_fn)(void *, void *),
                       int offset)
{

}



