/* functions for parsing and validating lables and instructions.
 * Labels and instructions are clumped together because in assembly languages
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


/*checks if line starts with a valid label*/
/*TODO FIXME:
 * is_label doesn't actually work. A label is any alphanumeric string that starts with a letter and is shorter than 30 characters, and ends with ":".
 * So to properly check that you need to iterate over characters to build a buffer, and stop when you get to 30 chars, ":", or a non-alphanumeric (eg. whitespace) character.
 * Then you need to check if it ends with ":".
 *
 * This shouldn't return an error *unless* it ends with ":", so it makes more sense to turn it to a function that returns a string with the name of the label,
 * and it can be empty if there's an error or NULL if there's no error and no label. We'd need the label name later in first_pass to put in the symbol table.
 *
 * - Elad
 * */

OP_TYPE is_label(char *s, int lineNum)
{
	int i=0;

	if (!isalpha(s[i])) /*if the first char in the symbol is not alpha then it is not a legal symbol*/
	{
		/*printf("at line %d symbol name must start with a letter", lineNum);*/
		return NOT; /* can be something else, doesn't have to be an error*/
	}
	i++;
	
	while(s[i] != '\0' && i < MAX_SYMBOL_SIZE && (isalpha(s[i]) || isdigit(s[i])))
	{
		i++;
	}

	if (i < MAX_SYMBOL_SIZE && s[i] == '\0') /* we stopped cuz we got to the end of the string and not because of an illegal char */
	{
		if (i <= 5 && (opcode_lookup(s) != NULL || is_register(s)))/*symbol name must not be a method or register name*/
		{
			printf("At line %d symbol name cannot be method or register name \n", lineNum);
			return ERROR;
		}
		return SYMBOL; /*then it is legal*/
	}
	else if (s[i] != '\0')
	{
		printf("At line %d illegal symbol name \n", lineNum);
		return ERROR;
	}

	if (i > MAX_SYMBOL_SIZE)
	{
		printf("At line %d symbol name is too long \n", lineNum);
		return ERROR;
	}

	return NOT;
}

/* TODO: this shouldn't use "checkop" (renamed: check_operands), but rather different functions that will check for valid strings / arrays / whatever */
ADR_TYPE is_instruction(char * word)
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

	return NOT;
}
