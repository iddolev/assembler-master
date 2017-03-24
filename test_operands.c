#include <stdio.h>
#include "opcodes.h"
#include "operands.h" /* gret -rn "word to search" */
#include "externs.h"
#include "utils.h"

void test_check_operation(char * word)
{
	char opcode[10];
	char * rest_of_line1 = getNextToken(word, opcode);
	int line_num = 0, result;
	printf("opcode is: \"%s\" and rest of line: \"%s\"\n", opcode, rest_of_line1);
	result = check_operation(opcode, rest_of_line1, line_num);

	printf(" Test for: %s %s - result = %d \n", opcode, rest_of_line1, result);
	printf(" and IC = %d \n", MAIN_DATA.IC);  
}

int main()
{
	char word[20];

	opcode_table_init();

	printf("Please enter a command: \n");
	scanf("%s", word);
	test_check_operation(word);
	return 0;
}

