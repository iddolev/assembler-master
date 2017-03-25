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

int get_register_number(char *s)
{
	return s[1] - '0';
}

int get_value_from_immediate(char *s)
{
	return atoi(s+1);   /* skip initial s[0] == '#' */
}

int second_pass_ee_command(INSTRUCTION_TYPE type, char *line)
{
	char next_word[MAX_SYMBOL_SIZE];
	symbol_table_data *s_data;
	
	getNextToken(line, next_word);
	s_data = symbol_table_lookup(next_word);
	if (!s_data)
	{
		printf("ERROR on second pass: symbol '%s' missing from symbol table\n", next_word);
		return 0;
	}
	if (type == ENTRY)
	{
		add_to_entry_section(s_data->address, next_word);
	}
	else  /* type == EXTERN */
	{
		add_to_extern_section(s_data->address, next_word);
	}
	return 1;
}

/* is_destination == 1 means: destination, 0 means: source */
int process_argument(parsed_operand *operand, int is_destination)
{
	symbol_table_data* symbol_data;
	int value;
	int encoding = 0;
	int register_number1, register_number2;

	switch (operand->addressing_method)
	{
		case IMMEDIATE:
			value = get_value_from_immediate(operand->text);
			encoding = encode_argument(ABSOLUTE, value);  
			break;
		case DIRECT:
			symbol_data = symbol_table_lookup(operand->text);
			if (symbol_data->is_extern)
			{
				encoding = encode_argument(EXTERNAL, NOT_USED);  
			}
			else if (symbol_data->is_code)
			{
				encoding = encode_argument(RELOCATABLE, symbol_data->address); 
				/* to do: fix address */
			}
			else /* symbol is of .data or .string line */
			{
				encoding = encode_argument(RELOCATABLE, symbol_data->address); 
				/* to do: fix address */
			}
			break;
		case INDEX:
			register_number1 = get_register_number(operand->text);
			register_number2 = get_register_number(operand->text+3);    /* skip initial register + '[' */
			encoding = encode_registers(register_number1, register_number2);  
			break;  
		case REGISTER:
			register_number1 = get_register_number(operand->text);
			if (is_destination)
				encoding = encode_registers(register_number1, NOT_USED);  
			else
				encoding = encode_registers(NOT_USED, register_number1);   
			break;
		default:
			printf("UNEXPECTED ERROR\n");
			break;
	}
	return encoding;
}

/* Assuming this is called only in second pass, only on legal input,
   so skipping all checks */
int second_pass_process_operands(char *opcode_word, char *linep)
{
	parsed_operand operands[MAX_NUM_OPERANDS];
	ADR_METHOD srcAdr, dstAdr;
	int num_op;
	int encoding;
	int register_number1, register_number2;

	opcode_item* opcode_data = opcode_lookup(opcode_word);
	if (opcode_data->group == 0)
	{
		encoding = encode_command(opcode_data->group, opcode_data->opcode, ADR_METHOD_DONT_CARE, ADR_METHOD_DONT_CARE);
		add_to_code_section(encoding);
		return 1;
	}
	num_op = collect_operands(operands, linep, 0);
	if (num_op == 1)
	{
                dstAdr = operands[0].addressing_method;
		encoding = encode_command(opcode_data->group, opcode_data->opcode, ADR_METHOD_DONT_CARE, dstAdr);
		add_to_code_section(encoding);

		encoding = process_argument(&(operands[0]), 1);    /* 1 = is destination */
		add_to_code_section(encoding);
		return 1;
	}
	if (num_op == 2)
	{
                srcAdr = operands[0].addressing_method;
                dstAdr = operands[1].addressing_method;
		encoding =  encode_command(opcode_data->group, opcode_data->opcode, srcAdr, dstAdr);
		add_to_code_section(encoding);

		if (srcAdr == REGISTER && dstAdr == REGISTER)
		{
			register_number1 = get_register_number(operands[0].text);
			register_number2 = get_register_number(operands[1].text);
			encoding = encode_registers(register_number1, register_number2);  
			add_to_code_section(encoding);
		}
		else
		{
			encoding = process_argument(&(operands[0]), 0);    /* 0 = is source */
			add_to_code_section(encoding);
			encoding = process_argument(&(operands[1]), 1);    /* 1 = is destination */
			add_to_code_section(encoding);
		}
		return 1;
	}

	return 0;   /* should not be reached */
}


