#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "opcodes.h"
#include "main_data.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "instructions.h"


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
	char buffer[256];

	for (i=0; i<num_labels; ++i)
	{
		printf("Calling is_label(\"%s\")\n", labels[i]);
		r = is_label(labels[i], i);
		op_type_to_string(r, buffer);
		printf("Result: %s\n", buffer);
	}
}

void test_getOp_str(char *s, int code)
{
	int ret;
	char *command = code == 1 ? "getDataOp" : "getStringOp";

	init_main_data();
	printf("Testing %s(\"%s\")\n", command, s); 
	if (code == 1)
		ret = getDataOp(s, 1);
	else
		ret = getStringOp(s, 1);
	printf("ret = %d\n", ret);
	if (ret)
		print_main_data();
	reset_main_data();
}

void test_getDataOp_str(char *s)
{
	test_getOp_str(s, 1);
}

void test_getStringOp_str(char *s)
{
	test_getOp_str(s, 2);
}

void test_getDataOp()
{
	test_getDataOp_str("5 ");
	test_getDataOp_str("  5, -3  ,7  ,   8,4  ,+6  ,  -88  ");
	test_getDataOp_str("5 3");
	test_getDataOp_str("5 , x");
	test_getDataOp_str("5 , ");
	test_getDataOp_str(" ");
	test_getDataOp_str("");
}

void test_getStringOp()
{
	test_getStringOp_str("\"abc\"");
	test_getStringOp_str("\"ab cd\"");
	test_getStringOp_str("\"abc\" ");
	test_getStringOp_str("\"abc");
	test_getStringOp_str("\"abc\n");
	test_getStringOp_str("x \"abc\"");
	test_getStringOp_str("\"abc\" x");
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


void nl()
{
	printf("\n");
}

int main(int argc, char* argv[])
{
	int code;

	if (argc == 1)
	{
		printf("Usage: %s <code>\n", argv[0]);
		printf("1: test getNextToken\n");
		printf("2: test is_label\n");
		printf("3: test getDataOp\n");
		printf("4: test getStringOp\n");
		printf("5: test first_pass_ee_command\n");
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
			test_getDataOp();
			break;
		case 4:
			test_getStringOp();
			break;
		case 5:
			test_first_pass_ee_command();
			break;
		default:
			printf("Illegal code\n");
	}
	return 0;
}

