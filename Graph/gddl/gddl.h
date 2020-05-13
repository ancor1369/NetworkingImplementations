/*
 * gddl.h
 *
 *  Created on: May 11, 2020
 *      Author: andres
 */

#ifndef GDDL_GDDL_H_
#define GDDL_GDDL_H_


typedef struct _gddl{
	struct _gddl *left;
	struct _gddl *right;
}gddl_t;


//This function will add a new gddl_t after the indicated gddl
void ddlAddNext(gddl_t *baseGddl, gddl_t *newGddl);

//Adds a node before the base gddl
void ddlAddBefore(gddl_t *baseGddl, gddl_t *newGddl);

//Remove a ggdl from the list
void ddlRemove(gddl_t *toRemove);

//Add a gddl at the end of the gdll
void ddlAddLast(gddl_t *baseGddl, gddl_t *newGddl);

//Delete the whole list
void ddlDeleteList(gddl_t *baseGddl);

//Count the number of elements contained in the gddl
unsigned int ddlGetListCount (gddl_t *baseGddl);

//Inserts a value
void ddlPriorityInsert(gddl_t *baseGddl,
					   gddl_t *gddl,
                       int (*comp_fn)(void *, void *),
                       int offset);


#define IS_GLTHREAD_LIST_EMPTY(glthreadptr)        									 \
    ((glthreadptr)->right == 0 && (glthreadptr)->left == 0)

#define GLTHREAD_TO_STRUCT(fn_name, structure_name, field_name)                        \
    static inline structure_name * fn_name(gddl_t *glthreadptr){                   \
        return (structure_name *)((char *)(glthreadptr) - (char *)&(((structure_name *)0)->field_name)); \
    }

/* delete safe loop*/
/*Normal continue and break can be used with this loop macro*/

#define BASE(glthreadptr)   ((glthreadptr)->right)

//This macro will go over all the gddl list and will return a pointer
//every time it is sorrounded

#define ITERATE_GDDL_BEGIN(glthreadptrstart, glthreadptr)                                       \
{                                                                                               \
    gddl_t *_glthread_ptr = NULL;                                                               \
    glthreadptr = BASE(glthreadptrstart);                                                       \
    for(; glthreadptr!= NULL; glthreadptr = _glthread_ptr){                                     \
        _glthread_ptr = (glthreadptr)->right;

#define ITERATE_GDDL_END(glthreadptrstart, glthreadptr)                                        \
}}

#define GLTHREAD_GET_USER_DATA_FROM_OFFSET(glthreadptr, offset)  \
    (void *)((char *)(glthreadptr) - offset)

#endif /* GDDL_GDDL_H_ */
