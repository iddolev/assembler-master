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

