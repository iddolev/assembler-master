#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "utils.h"
#include "symbol_table.h"
#include "externs.h"
#include "file_pass.h"
#include "opcodes.h"
#include "operands.h"


int second_pass(FILE *f)
{
	return file_pass(f, 2);
}

int second_pass_ee_command(INSTRUCTION_TYPE type, char *line)
{
	char next_word[MAX_SYMBOL_SIZE];
	symbol_table_data *s_data;
	e_file_line *line_struct;
	
	line_struct = (e_file_line*) malloc(sizeof(line_struct));
	if (!line_struct)
	{
		printf("Memory allocation problem\n");
		return 0;
	}
	
	getNextToken(line, next_word);
	if (type == ENTRY)
	{
		s_data = symbol_table_lookup(next_word);
		line_struct->address = s_data->address;
		strcpy(line_struct->label, next_word);
		MAIN_DATA.ENTRY_SECTION[MAIN_DATA.YC] = line_struct;
		MAIN_DATA.YC++;
	}
	else  /* type == EXTERN */
	{
		line_struct->address = MAIN_DATA.IC;
		strcpy(line_struct->label, next_word);
		MAIN_DATA.EXTERN_SECTION[MAIN_DATA.XC] = line_struct;
		MAIN_DATA.XC++;
	}
	return 1;
}

/* Assuming this is called only in second pass, only on legal input,
   so skipping all checks */
int second_pass_process_operands(char *opcode_word, char *linep)
{
	parsed_operand operands[MAX_NUM_OPERANDS];
	ADR_METHOD srcAdr, dstAdr;
	int num_op;

	opcode_item* opcode_data = opcode_lookup(opcode_word);
	if (opcode_data->group == 0)
	{
		add_to_code_section(opcode_data->opcode);
		return 1;
	}
	num_op = collect_operands(operands, linep, 0);
	if (num_op == 1)
	{
                dstAdr = operands[0].addressing_method;
		add_to_code_section(opcode_data->opcode);
		add_to_code_section(dstAdr);
		return 1;
	}
	if (num_op == 2)
	{
                srcAdr = operands[0].addressing_method;
                dstAdr = operands[1].addressing_method;
		add_to_code_section(opcode_data->opcode);
		if (srcAdr == REGISTER)
		{
			if (dstAdr == REGISTER)
			{
				add_to_code_section(srcAdr + dstAdr);
				return 1;
			}
		}
		add_to_code_section(srcAdr);
		add_to_code_section(dstAdr);
		return 1;
	}

	return 0;   /* should not be reached */
}


