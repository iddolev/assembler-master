#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "opcodes.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "instructions.h"
#include "operands.h"
#include "externs.h"


void test_getNextToken()
{
	char buffer[100];
	char *str = "  This is   a te2t 234:   ";
	char *p = str;
	int i;
	printf("Testing getNextToken on: \"%s\"\n", str);
	i = 0;
	while (p && i++ < 10)
	{
		p = getNextToken(p, buffer);
		if (p)
		{
			printf("Token: \"%s\"\n", buffer);
		}
	}
	if (p)
	{
		printf("ERROR: still remaining in p: \"%s\"\n", p);
	}
}

void test_is_label()
{
	char* labels[] = {"abc", "ab4a", "4abc", "ab-e", "abc ", "axaxaxaxaxaxaxaxaxaxaxaxaxaxaxaxaxax", "r1", "mov", "#-1"};
	int num_labels = 9;
	int i;
	OP_TYPE r;

	for (i=0; i<num_labels; ++i)
	{
		printf("Calling is_label(\"%s\")\n", labels[i]);
		r = is_label(labels[i], i);
		printf("Result: %s\n", op_type_to_string(r));
	}
}

void test_get_addressing_method_str(char *s)
{
	ADR_METHOD ret;
	printf("Testing get_addressing_method(\"%s\", 10)\n", s);
	ret = get_addressing_method(s, 10);
	printf("ret = %s\n", adr_method_to_string(ret));
}

void test_get_addressing_method()
{
	test_get_addressing_method_str("#1");
	test_get_addressing_method_str("#354");
	test_get_addressing_method_str("#-65");
	test_get_addressing_method_str("#5a");
	test_get_addressing_method_str("#a");
	test_get_addressing_method_str("#");
	test_get_addressing_method_str("#-");
	test_get_addressing_method_str("r1");
	test_get_addressing_method_str("r9");
	test_get_addressing_method_str("r110");
	test_get_addressing_method_str("r1[r2]");
	test_get_addressing_method_str("r1[r3]");
	test_get_addressing_method_str("r2[r3]");
	test_get_addressing_method_str("loop");
	test_get_addressing_method_str("lo-op");
	test_get_addressing_method_str("loop-");
}

void test_getOp_str(char *s, int code)
{
	int ret;
	char *command = code == 1 ? "process_data_instruction" : "process_string_instruction";

	init_main_data();
	printf("Testing %s(\"%s\")\n", command, s); 
	if (code == 1)
		ret = process_data_instruction(s, 1);
	else
		ret = process_string_instruction(s, 1);
	printf("ret = %d\n", ret);
	if (ret)
		print_main_data();
	reset_main_data();
}

void test_process_data_instruction_str(char *s)
{
	test_getOp_str(s, 1);
}

void test_process_string_instruction_str(char *s)
{
	test_getOp_str(s, 2);
}

void test_process_data_instruction()
{
	test_process_data_instruction_str("5 ");
	test_process_data_instruction_str("  5, -3  ,7  ,   8,4  ,+6  ,  -88  ");
	test_process_data_instruction_str("5 3");
	test_process_data_instruction_str("5 , x");
	test_process_data_instruction_str("5 , ");
	test_process_data_instruction_str(" ");
	test_process_data_instruction_str("");
}

void test_process_string_instruction()
{
	test_process_string_instruction_str("\"abc\"");
	test_process_string_instruction_str("\"ab cd\"");
	test_process_string_instruction_str("\"abc\" ");
	test_process_string_instruction_str("\"abc");
	test_process_string_instruction_str("\"abc\n");
	test_process_string_instruction_str("x \"abc\"");
	test_process_string_instruction_str("\"abc\" x");
}

void test_first_pass_ee_command()
{
	int ret;

	init_main_data();
	printf("Testing test_first_pass_ee_command(EXTERN, \"LOOP\", 5)\n"); 
	ret = first_pass_ee_command(EXTERN, "LOOP", 5);	
	printf("ret = %d\n", ret);
	printf("Testing test_first_pass_ee_command(EXTERN, \"BOBO \", 6)\n"); 
	ret = first_pass_ee_command(EXTERN, "BOBO  ", 6);	
	printf("ret = %d\n", ret);
	printf("Testing test_first_pass_ee_command(EXTERN, \" KKK\", 7)\n"); 
	ret = first_pass_ee_command(EXTERN, " KKK", 7);	
	printf("ret = %d\n", ret);
	printf("Testing test_first_pass_ee_command(EXTERN, \"  \", 8)\n"); 
	ret = first_pass_ee_command(EXTERN, "  ", 8);	
	printf("Testing test_first_pass_ee_command(EXTERN, \"\", 9)\n"); 
	ret = first_pass_ee_command(EXTERN, "", 9);	
	printf("ret = %d\n", ret);
	printf("Testing test_first_pass_ee_command(EXTERN, \"ABC DEF\", 10)\n"); 
	ret = first_pass_ee_command(EXTERN, "ABC DEF", 10);	
	printf("ret = %d\n", ret);

	print_symbol_table();
	reset_main_data();
}

void test_first_pass_check_operands1(char *opcode, char *linep)
{
	int ret;

	printf("Testing first_pass_check_operands(\"%s\", \"%s\", 10)\n", opcode, linep);
	ret = first_pass_check_operands(opcode, linep, 10);
	printf("ret = %d   ", ret);
	printf("IC = %d\n", MAIN_DATA.IC);
}

void test_first_pass_check_operands()
{
	test_first_pass_check_operands1("stop", "");
	test_first_pass_check_operands1("stop", " loop");
	test_first_pass_check_operands1("prn", " #1");
	test_first_pass_check_operands1("prn", " xxx");
	test_first_pass_check_operands1("prn", " r1");
	test_first_pass_check_operands1("prn", " r1[r2]");
	test_first_pass_check_operands1("prn", " r1 r2");
	test_first_pass_check_operands1("prn", " r1, r2");
	test_first_pass_check_operands1("clr", " #1");
	test_first_pass_check_operands1("clr", " xxx");
	test_first_pass_check_operands1("clr", " r1");
	test_first_pass_check_operands1("clr", " r1[r2]");
	test_first_pass_check_operands1("clr", "");
	test_first_pass_check_operands1("mov", " x, y");
	test_first_pass_check_operands1("mov", " #1, #2");
	test_first_pass_check_operands1("mov", " r1, r2");
	test_first_pass_check_operands1("mov", " r1[r2], r3[r4]");
	test_first_pass_check_operands1("mov", " r1, r2 r3");
	test_first_pass_check_operands1("mov", " r1, r2, r3");
	test_first_pass_check_operands1("mov", " r1");
	test_first_pass_check_operands1("mov", " ");
}

void nl()
{
	printf("\n");
}

void print_menu(char *program)
{
	printf("Usage: %s <code>\n", program);
	printf("1: test getNextToken\n");
	printf("2: test is_label\n");
	printf("3: test get_addressing_method\n");
	printf("4: test process_data_instruction\n");
	printf("5: test process_string_instruction\n");
	printf("6: test first_pass_ee_command\n");
	printf("7: test first_pass_check_operands\n");
}

int main(int argc, char* argv[])
{
	int code;

	if (argc == 1)
	{
		print_menu(argv[0]);
		return 0;
	}

	opcode_table_init();

	code = atoi(argv[1]);

	switch (code)
	{
		case 1:
			test_getNextToken();
			break;
		case 2:
			test_is_label();
			break;
		case 3:
			test_get_addressing_method();
			break;
		case 4:
			test_process_data_instruction();
			break;
		case 5:
			test_process_string_instruction();
			break;
		case 6:
			test_first_pass_ee_command();
			break;
		case 7:
			test_first_pass_check_operands();
			break;
		default:
			print_menu(argv[0]);
	}
	return 0;
}

