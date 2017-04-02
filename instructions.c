/* functions for parsing and validating lables and instructions.
 * Labels and instructions are here together because in assembly languages
 * a label is also a type of assembler instruction, and does not
 * appear in the compiled code */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "operands.h"
#include "utils.h"
#include "opcodes.h"
#include "externs.h"
#include "instructions.h"

/* a function to check if the given string is a label */
/* it can return SYMBOL == it is a label, or NOT == it is not a label but can be something else
 or ERROR == it is not a symbol and can not be anything else, it is a certain error */
OP_TYPE is_label(char *s, int line_num)
{
	int i=0;

	if (is_end_char(s[0]))
	{
		return NOT;
	}

	if (!isalpha(s[i])) /*if the first char in the symbol is not alpha then it is not a legal symbol*/
	{
		return NOT; /* can be something else, doesn't have to be an error*/
	}
	i++;
	
	while(s[i] != '\0' && i < MAX_SYMBOL_SIZE && (isalpha(s[i]) || isdigit(s[i])))
	{
		i++;
	}

	if (i == MAX_SYMBOL_SIZE)
	{
		fprintf(stderr, "At line %d symbol name is too long \n", line_num);
		return ERROR;
	} 

	if (s[i] == '\0') /* we stopped cuz we got to the end of the string and not because of an illegal char */
	{
		if (opcode_lookup(s) != NULL || is_register(s))  /*symbol name must not be a method or register name*/
		{
			return NOT;
		}
		return SYMBOL; /*then it is legal*/
	}

	fprintf(stderr, "At line %d illegal symbol name \n", line_num);
	return ERROR;
}

/* a function to check if the .data command operands are valid */
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
				/* if there are to many numbers for the .data command and there is no more space in the data section, return error */
				/* else add to data section and update MAIN_DATA.DC */
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
	fprintf(stderr, "Missing arguments at line %d \n", line_num);
	return 0;	
}

/* a function to check if the .string command operands are valid */
int process_string_instruction(char * linep, int line_num)
{
	int ascii_code;
	
	while (is_whitespace(*linep))
	{
		linep++; /*skip blank lines and stuff*/
	}

	if (*linep == '"') /* the string shouls start with a - " */
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
		if (*linep != '"') /* the string should end with a - " */
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

/* a function to identify which instruction is the given string */
INSTRUCTION_TYPE is_instruction(char * word)
{
	if (strcmp(word, ".data") == 0)
	{
		return DATA;
	}
	else if (strcmp(word, ".string") == 0)
	{
		return STRING;
	}
	if (strcmp(word, ".entry") == 0)
	{
		return ENTRY;
	}
	if (strcmp(word, ".extern") == 0)
	{
		return EXTERN;
	}

	return NOT_INST;
}

