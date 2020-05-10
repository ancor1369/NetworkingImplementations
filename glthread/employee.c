
#include "employee.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>


//Pointer for an employee object
emp_t *emp = NULL;


void pirnt_emp_details_second(glthread_node_t *node)
{

	emp_t *data = NULL;

	data =(emp_t *)((char *)node - offsetof(emp_t, glnode));
	pirnt_emp_details(data);
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

	pirnt_emp_details_second(&emp->glnode);

	return 0;
}
