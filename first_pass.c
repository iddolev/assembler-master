#include <stdio.h>
#include <stdlib.h>
#include "first_pass.h"
#include "symbol_table.h"
#include "utils.h"
#include "externs.h"
#include "file_pass.h"
#include "instructions.h"


int first_pass(FILE *f)
{
	return file_pass(f, 1);
}


int getDataOp(char * linep, int lineNum)
{
	char nextWord[MAX_SYMBOL_SIZE];
	
	while (*linep != '\n')
		{
			linep = getNextToken(linep, nextWord);
			if (!linep)
			{
				printf("Missing oprands for '.data' at line %d \n", lineNum);
				return 0;
			}
			if (is_number(nextWord))
			{
				MAIN_DATA.DATA_SECTION[MAIN_DATA.DC] = atoi(nextWord);
				MAIN_DATA.DC++;
				if (MAIN_DATA.DC >= DATA_SECTION_SIZE)
				{
					printf("At line %d exceeded maximal number of data elements \n", lineNum);
					return 0;
				}
			}
			else
			{
				printf("At line %d illegal operand \"%s\" for '.data' \n", lineNum, nextWord);
				return 0;
			}
			/* now check legal nextWord, and update MAIN_DATA.DC */
			while (is_whitespace(*linep))
			{
				linep++; /*skip blank lines and stuff*/
			}
			if (*linep == '\n' || *linep == '\0')
			{
				/* we successfully finished scanning the data section so move to next line */
				return 1; 
			}
			if (*linep != ',')
			{
				printf("Missing comma at line %d \n", lineNum);
				return 0;   /* continue to next line: no point in continuing to make sense of the corrupt data section*/
			}
			else
			{
				linep++;
			}
		}
	return 0;	
}

int getStringOp(char * linep, int lineNum)
{
	int asciiChar;
	
	while (is_whitespace(*linep))
	{
		linep++; /*skip blank lines and stuff*/
	}

	if (*linep == '"')
	{
		linep++;    /* skip " character */

		while(*linep != '\0' && *linep != '\n' && *linep != '"') /* scan it without the " " characters */
		{
			asciiChar = *linep;
			MAIN_DATA.DATA_SECTION[MAIN_DATA.DC] = asciiChar;
			MAIN_DATA.DC++;
			if (MAIN_DATA.DC >= DATA_SECTION_SIZE)
			{
				printf("At line %d exceeded maximal number of data elements \n", lineNum);
				return 0;
			}
			linep++;
		}
		if (*linep != '"')
		{
			printf("Missing end quote character at .string line %d\n", lineNum);
			return 0;
		}
		linep++;    /* skip " character */
	}
	else
	{
		printf("Missing begin quote character at .string line %d\n", lineNum);
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
		printf("Illegal characters after end quote at .string line %d\n", lineNum);
		return 0;
	}
	return 0;
}

int first_pass_data_command(INSTRUCTION_TYPE type, char *linep, int line_num)
{
	int result;
	
	if (type == DATA)
	{
		result = getDataOp(linep, line_num); /*to do: should return a value for error_count*/
		if (result == 0)
			return 0;
	}
	else  /* type == STRING  */
	{
		result = getStringOp(linep, line_num); /*to do: should return a value for errorCoun*/
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
		printf("Missing operand in line %d\n", line_num);
		return 0;
	}

	lab = is_label(next_word, line_num);
	if (lab != SYMBOL)
	{
		printf("Illegal operand in line %d\n", line_num);
		return 0;
	}

	/* now need to check if there isn't anything in the line afterward i.e '.extern LOOP asdfthgj' - it's not legal*/
	if (!verifyEndOfLine(linep))
	{
		printf("Illegal extra at line %d: %s\n", line_num, linep);
		return 0;
	}

	if (type == EXTERN)
	{
		symbol_table_add(next_word, 0, 0, 1);
	}
	/* if type == ENTRY, no need to do anything in first pass */

	return 1; 
}

