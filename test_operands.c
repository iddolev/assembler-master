#include <stdio.h>
#include "opcodes.h"
#include "operands.h" /* gret -rn "word to search" */
#include "externs.h"

void test_check_operation()
{
	char * opcode = "mov";
	char * rest_of_line1 = " r1,r2 \n";
	int line_num = 0, result;
	result = check_operation(opcode, rest_of_line1, line_num);

	printf(" Test for: %s %s - result = %d \n", opcode, rest_of_line1, result);
	printf(" and IC = %d \n", MAIN_DATA.IC);  
}

int main()
{
	opcode_table_init();

	test_check_operation();
	return 0;
}

