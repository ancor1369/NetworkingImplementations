/*
 * glthread.c
 *
 *  Created on: May 10, 2020
 *      Author: andres
 */


#include "glthread.h"
#include <stdlib.h>
#include <stdio.h>


//Making this funciton static makes it non visible outside of the file

//Private function to add a new node right after the current node
static void _glthread_add_next(glthread_node_t *curr_node,glthread_node_t *new_node)
{
	if(!curr_node->right)
	{
		curr_node->right = new_node;
		new_node->left = curr_node;
		return;
	}
	glthread_node_t *temp = curr_node->right;
	curr_node->right = new_node;
	new_node->left = curr_node;
	new_node->right = temp;
	temp->left = new_node;
}

//Function to insert a new GLNode into a glthread at the first position
//in a glthread,
void glthread_add(glthread_t *lst, glthread_node_t *glnode)
{
	glnode->left = NULL;
	glnode->right = NULL;
	if(!lst->head){
		lst->head = glnode;
		return;
	}
	glthread_node_t *head = lst->head;
	_glthread_add_next(glnode, head);
	lst->head = glnode;
}

//Private function to remove a variable from the
static void _remove_glthread(glthread_node_t *glnode){
	if(!glnode->left)
	{
		if(glnode->right)
		{
			glnode->right->left = NULL;
			glnode->right = 0;
			return;
		}
		return;
	}
	if(!glnode->right)
	{
		glnode->left->right = NULL;
		glnode->left = NULL;
		return;
	}
	glnode->left->right = glnode->right;
	glnode->right->left = glnode->left;

	glnode->left = 0;
	glnode->right = 0;
}


//This function is visible and it is used as an API
void glthread_remove(glthread_t *lst, glthread_node_t *glnode)
{
	glthread_node_t *head = lst->head;

	if(head == glnode)
	{
		lst->head = head->right;
	}
	_remove_glthread(glnode);
}

//Start the glthread
void init_glthread(glthread_t *glthread, unsigned int offset)
{
	glthread->head = NULL;
	glthread->offset = offset;
}

