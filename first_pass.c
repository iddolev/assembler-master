#include <stdio.h>
#include <stdlib.h>
#include "first_pass.h"
#include "symbol_table.h"
#include "utils.h"
#include "externs.h"
#include "file_pass.h"
#include "instructions.h"
#include "opcodes.h"
#include "operands.h"

/* functions for the first pass */

int first_pass(FILE *f)
{
	return file_pass(f, 1);
}


int process_data_instruction(char * linep, int line_num)
{
	char nextWord[MAX_SYMBOL_SIZE];
	
	while (*linep != '\n')
		{
			linep = getNextToken(linep, nextWord);
			if (!linep)
			{
				fprintf(stderr, "Missing oprands for '.data' at line %d \n", line_num);
				return 0;
			}
			if (is_number(nextWord))
			{
				if (!add_to_data_section(atoi(nextWord)))
				{
					fprintf(stderr, "At line %d exceeded maximal number of data elements \n", line_num);
					return 0;
				}
			}
			else
			{
				fprintf(stderr, "At line %d illegal operand \"%s\" for '.data' \n", line_num, nextWord);
				return 0;
			}
			/* now check legal nextWord, and update MAIN_DATA.DC */
			while (is_whitespace(*linep))
			{
				linep++; /*skip blank lines and stuff*/
			}
			if (is_end_char(*linep))
			{
				/* we successfully finished scanning the data section so move to next line */
				return 1; 
			}
			if (*linep != ',')
			{
				fprintf(stderr, "Missing comma at line %d \n", line_num);
				return 0;   /* continue to next line: no point in continuing to make sense of the corrupt data section*/
			}
			else
			{
				linep++;
			}
		}
	return 0;	
}

int process_string_instruction(char * linep, int line_num)
{
	int ascii_code;
	
	while (is_whitespace(*linep))
	{
		linep++; /*skip blank lines and stuff*/
	}

	if (*linep == '"')
	{
		linep++;    /* skip " character */

		while(!is_end_char(*linep) && *linep != '"') /* scan it without the " " characters */
		{
			ascii_code = *linep;
			if (!add_to_data_section(ascii_code))
			{
				fprintf(stderr, "At line %d exceeded maximal number of data elements \n", line_num);
				return 0;
			}
			linep++;
		}
		if (!add_to_data_section(0))   /* null terminating the string */
		{
			fprintf(stderr, "At line %d exceeded maximal number of data elements \n", line_num);
			return 0;
		}
		if (*linep != '"')
		{
			fprintf(stderr, "Missing end quote character at .string line %d\n", line_num);
			return 0;
		}
		linep++;    /* skip " character */
	}
	else
	{
		fprintf(stderr, "Missing begin quote character at .string line %d\n", line_num);
		return 0;
	}
	
	while (is_whitespace(*linep))
	{
		linep++; /*skip blank lines and stuff*/
	}
	
	if (*linep == '\n' || *linep == '\0')
	{
		/* we successfully finished scanning the data section so move to next line */
		return 1; 
	}
	else
	{
		fprintf(stderr, "Illegal characters after end quote at .string line %d\n", line_num);
		return 0;
	}
	return 0;
}

int first_pass_data_command(INSTRUCTION_TYPE type, char *linep, int line_num)
{
	int result;
	
	if (type == DATA)
	{
		result = process_data_instruction(linep, line_num); 
		if (result == 0)
			return 0;
	}
	else  /* type == STRING  */
	{
		result = process_string_instruction(linep, line_num); 
		if (result == 0)
			return 0;
	}
	return 1;
}

int first_pass_ee_command(INSTRUCTION_TYPE type, char *linep, int line_num)
{
	int lab;
	char next_word[MAX_LINE_LENGTH+2]; 
	
	linep = getNextToken(linep, next_word);
	if (!linep)
	{
		fprintf(stderr, "Missing operand in line %d\n", line_num);
		return 0;
	}

	lab = is_label(next_word, line_num);
	if (lab != SYMBOL)
	{
		fprintf(stderr, "Illegal operand in line %d\n", line_num);
		return 0;
	}

	/* now need to check if there isn't anything in the line afterward i.e '.extern LOOP asdfthgj' - it's not legal*/
	if (!verifyEndOfLine(linep))
	{
		fprintf(stderr, "Illegal extra at line %d: %s\n", line_num, linep);
		return 0;
	}

	if (type == EXTERN)
	{
		if (!symbol_table_add_and_verify(next_word, 0, 0, 1, line_num))
			return 0; 
	}
	/* if type == ENTRY, no need to do anything in first pass */

	return 1; 
}

int first_pass_check_operation(char * opcode_word, char * linep, int line_num)
{
	parsed_operand operands[MAX_NUM_OPERANDS];
	ADR_METHOD srcAdr, dstAdr;
	opcode_item* opcode_data;

	if (*linep == ':')
	{
		fprintf(stderr, "At line %d: Illegal label name: %s\n", line_num, opcode_word);
		return 0;
	}

	opcode_data = opcode_lookup(opcode_word); /* check if legal method name */
	if (!opcode_data)
	{
		fprintf(stderr, "Line %d: invalid opcode '%s'\n", line_num, opcode_word);
		return 0;
	}
	if (opcode_data->group == 0) /* if the method shouldn't get any operands */
	{
		if (!verifyEndOfLine(linep)) /* there should be nothing after the method name */
		{
			fprintf(stderr, "Line %d: No operands should appear after opcode '%s'\n", line_num, opcode_word);
			return 0;
		}
		MAIN_DATA.IC += 1;
		return 1;
	}
	if (!collect_operands(operands, opcode_data->group, linep, line_num))
	{
		return 0;
	}
	if (opcode_data->group == 1)
	{
                dstAdr = operands[0].addressing_method;
                if (valid_method_for_operand(opcode_data->addressing_mode.dst, dstAdr))
                {
			MAIN_DATA.IC += 2;
			return 1;
                }
		fprintf(stderr, "Incompatible addressing method for operand at line %d\n", line_num);
		return 0;
	}
	else if (opcode_data->group == 2)
	{
                srcAdr = operands[0].addressing_method;
                dstAdr = operands[1].addressing_method;
                if (valid_method_for_operand(opcode_data->addressing_mode.src, srcAdr) 
			&& valid_method_for_operand(opcode_data->addressing_mode.dst, dstAdr))
                {
			if (srcAdr == REGISTER)
			{
				if (dstAdr == REGISTER)
				{
					MAIN_DATA.IC += 2;
					return 1;
				}
			}
			MAIN_DATA.IC += 3;
			return 1;
                }
		fprintf(stderr, "Incompatible addressing method for operand at line %d\n", line_num);
		return 0;
	}

	return 0;   /* should not be reached */
}


