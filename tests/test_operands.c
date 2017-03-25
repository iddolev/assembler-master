#include <stdio.h>
#include "opcodes.h"
#include "operands.h" /* gret -rn "word to search" */
#include "externs.h"
#include "utils.h"

#define LINE_SIZE 80

void test_check_operation(char * word)
{
	char opcode[10];
	char * rest_of_line1;
	int line_num = 0, result;

	rest_of_line1 = getNextToken(word, opcode);
	result = check_operation(opcode, rest_of_line1, line_num);

	printf(" Test for: %s %s - result = %d \n", opcode, rest_of_line1, result);
	printf(" and IC = %d \n", MAIN_DATA.IC);  
}

int main()
{
	char line[LINE_SIZE];

	opcode_table_init();

	printf("Please enter a command: \n");
	fgets(line, LINE_SIZE, stdin);
	test_check_operation(line);
	return 0;
}

