
#include "employee.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>


//Pointer for an employee object
emp_t *emp = NULL;


void pirnt_emp_details_second(glthread_node_t *head)
{

	emp_t *data = NULL;
	if(!head) return;
	while(head)
	{
	 data =(emp_t *)((char *)head - offsetof(emp_t, glnode));
	 pirnt_emp_details(data);
	 head = head->right;
	}
	return;
}

void pirnt_emp_details(emp_t *employee)
{
	printf("Name = %s \n", employee->name);
	printf("ID = %u \n", employee->emp_id);
	printf("Salary = %u \n", employee->salary);
	printf("Designation = %s \n", employee->designation);
	return;
}

int main()
{
	//This creates the variable in the heap memory space
	emp = malloc(sizeof(emp));

	strncpy(emp->designation,"Developer", strlen("Developer"));
	emp->emp_id = 5;
	strncpy(emp->name, "Linval Gomez", strlen("Linval Gomez"));
	emp->salary = 30000;
	glthread_node_init((&emp->glnode));

	//pirnt_emp_details_second(&emp->glnode);

	emp_t * emp1 = malloc(sizeof(emp_t));
	strncpy(emp1->designation, "Seller", strlen("Seller"));
	emp1->emp_id = 5;
	strncpy(emp1->name,"Mike Bahia",strlen("Mike Bahia"));
	emp1->salary = 25000;
	glthread_node_init((&emp1->glnode));

	//Create a new double linked list of employees!
	glthread_t *empl_list = calloc(1, sizeof(glthread_t));
	init_glthread(empl_list, offsetof(emp_t, glnode));

	glthread_add(empl_list, &emp->glnode);
	glthread_add(empl_list, &emp->glnode);

	emp_t *ptr = NULL;
	ITERATE_GL_THREADS_BEGIN(empl_list, emp_t, ptr)
	{
		pirnt_emp_details(ptr);
	} ITERATE_GL_THREADS_ENDS;

	return 0;
}
