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
	int ret; 
	MAIN_DATA.DATA_OFFSET = MAIN_DATA.IC;   /* data offset = IC of end of first pass */
	MAIN_DATA.IC = 0; /* restart the IC for the second pass */
	ret = file_pass(f, 2);
	if (ret)
		ret = verify_extern_used();
	return ret;
}

/* Address is from first pass (starting from 0). Return the offset address for the final code section */
int code_address(int address)
{
	return CODE_SECTION_OFFSET + address;
}

/* Address is from first pass (starting from 0). Return the offset address for the final data section */
int data_address(int address)
{
	return CODE_SECTION_OFFSET + MAIN_DATA.DATA_OFFSET + address;
}

int get_register_number(char *s)
{
	return s[1] - '0';
}

int get_value_from_immediate(char *s)
{
	return atoi(s+1);   /* skip initial s[0] == '#' */
}

int second_pass_ee_command(INSTRUCTION_TYPE type, char *line, int line_num)
{
	char next_word[MAX_SYMBOL_SIZE];
	symbol_table_data *s_data;
	int address;
	
	getNextToken(line, next_word);
	s_data = symbol_table_lookup(next_word);
	if (!s_data)
	{
		printf("ERROR on second pass: symbol '%s' missing from symbol table at line %d\n", next_word, line_num);
		return 0;
	}
	if (type == ENTRY)
	{
		if (s_data->was_used)
		{
			printf("Warning: duplicate .entry %s statement in line %d\n", next_word, line_num);
			return 1;   /* don't write it again in the .ent file */
		}
		if (s_data->is_extern)
		{
			printf("Error in line %d: .entry for symbol %s is also declared as extern\n", line_num, next_word);
			return 0;
		}
		address = s_data->address;
		address = s_data->is_code ? code_address(address) : data_address(address);
		add_to_entry_section(address, next_word);
		s_data->was_used = 1;   /* so that we won't write it again in the .ent file if duplicate .entry statement */
	}
	/* if type == EXTERN -- nothing to do in second pass */
	return 1;
}

/* is_destination == 1 means: destination, 0 means: source */
int process_argument(parsed_operand *operand, int is_destination, int line_num)
{
	symbol_table_data* symbol_data;
	int value;
	int encoding = 0;
	int register_number1, register_number2, address;

	switch (operand->addressing_method)
	{
		case IMMEDIATE:
			value = get_value_from_immediate(operand->text);
			encoding = encode_argument(ABSOLUTE, value);  
			break;
		case DIRECT:
			symbol_data = symbol_table_lookup(operand->text);
			if (!symbol_data)
			{
				printf("ERROR on second pass: symbol '%s' missing from symbol table at line %d\n", operand->text, line_num);
				return -1;
			}
			if (symbol_data->is_extern)
			{
				encoding = encode_argument(EXTERNAL, NOT_USED);
				/* In addition, we need to add the address IC to the extern file with this extern symbol */
				address = code_address(MAIN_DATA.IC);
				add_to_extern_section(address, operand->text);
				symbol_data->was_used = 1;
			}
			else if (symbol_data->is_code)
			{
				encoding = encode_argument(RELOCATABLE, code_address(symbol_data->address)); 
			}
			else /* symbol is of .data or .string line */
			{
				encoding = encode_argument(RELOCATABLE, data_address(symbol_data->address)); 
			}
			break;
		case INDEX:
			register_number2 = get_register_number(operand->text);
			register_number1 = get_register_number(operand->text+3);    /* skip initial register + '[' */
			encoding = encode_registers(register_number1, register_number2);  
			break;  
		case REGISTER:
			register_number1 = get_register_number(operand->text);
			if (is_destination)
				encoding = encode_registers(NOT_USED, register_number1);  
			else
				encoding = encode_registers(register_number1, NOT_USED);   
			break;
		default:
			printf("UNEXPECTED ERROR\n");
			break;
	}
	return encoding;
}

/* Assuming this is called only in second pass, only on legal input,
   so skipping all checks */
int second_pass_process_operands(char *opcode_word, char *linep, int line_num)
{
	parsed_operand operands[MAX_NUM_OPERANDS];
	ADR_METHOD srcAdr, dstAdr;
	int encoding;
	int register_number1, register_number2;

	opcode_item* opcode_data = opcode_lookup(opcode_word);
	if (opcode_data->group == 0)
	{
		encoding = encode_command(opcode_data->group, opcode_data->opcode, ADR_METHOD_DONT_CARE, ADR_METHOD_DONT_CARE);
		add_to_code_section(encoding);
		return 1;
	}
	if (!collect_operands(operands, opcode_data->group, linep, 0))
	{
		printf("Unexpected error in collect_operands\n");
		return 0;
	}
	if (opcode_data->group == 1)
	{
                dstAdr = operands[0].addressing_method;
		encoding = encode_command(opcode_data->group, opcode_data->opcode, ADR_METHOD_DONT_CARE, dstAdr);
		add_to_code_section(encoding);

		encoding = process_argument(&(operands[0]), 1, line_num);    /* 1 = is destination */
		if (encoding == -1)    /* error */
			return 0;   
		add_to_code_section(encoding);
		return 1;
	}
	else if (opcode_data->group == 2)
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
			if (encoding == -1)    /* error */
				return 0;   
			add_to_code_section(encoding);
		}
		else
		{
			encoding = process_argument(&(operands[0]), 0, line_num);    /* 0 = is source */
			if (encoding == -1)    /* error */
				return 0;   
			add_to_code_section(encoding);
			encoding = process_argument(&(operands[1]), 1, line_num);    /* 1 = is destination */
			if (encoding == -1)    /* error */
				return 0;   
			add_to_code_section(encoding);
		}
		return 1;
	}

	return 0;   /* should not be reached */
}


