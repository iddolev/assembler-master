/* NOTE: This is an experiment to try to make the pass on a file generic and shared between the first pass and second pass.
   Don't use this file yet. */
   
#include <stdio.h>
#include "stack.h"
#include "utils.h"
#include "instructions.h"
#include "header.h"
#include "externs.h"


void first_pass_data_command(OP_TYPE type, char *linep, int lineNum, int symbolFlag, char *label)
{
	int result;
	
	if (symbolFlag == 1)
	{
		symbol_table_add(label, DC, 0, 0);
	}
	if (type == DATA)
	{
		result = getDataOp(linep, lineNum); /*to do: should return a value for errorCount*/
		if (result == 0)
			return 0;
	}
	else
	{
		result = getStringOp(linep, lineNum); /*to do: should return a value for errorCoun*/
		if (result == 0)
			return 0;
	}
	return 1;
}

void first_pass_ee_command(OP_TYPE type, char *linep, int lineNum)
{
	OP_TYPE type;
	int lab;
	char nextWord[MAX_LINE+2]; 
	
	linep = getNextToken(linep, nextWord);
	lab = is_label(nextWord, lineNum);
	if (lab == SYMBOL)
	{
		if (type == EXTERN)
		{
			symbol_table_add(nextWord, 0, 0, 1);
		}
	}
	else
	{
		return 0;
	}
	/* now need to check if there isn't anything in the line afterward i.e '.extern LOOP asdfthgj' - it's not legal*/
	if (!verifyEndOfLine(linep, lineNum))
	{
		printf("Illegal extra at line %d: %s \n", lineNum, linep);
		return 0;
	}
	return 1; 
}

int second_pass_ee_command(OP_TYPE type, char *line)
{
	symbol_table_data s_data;
	e_file_line *line_struct;
	
	line_struct = (line_struct*) malloc(sizeof(line_struct));
	if (!line_struct)
	{
		printf("Memory allocation problem\n");
		return 0;
	}
	
	linep = getNextToken(linep, nextWord);
	if (type == ENTRY)
	{
		symbol_table_data s_data = symbol_table_lookup(nextWord);
		line_struct->address = s_data->address;
		strcpy(line_struct->label, nextWord);
		MAIN_DATA.ENTRY_SECTION[MAIN_DATA.YC] = line_struct;
		MAIN_DATA.YC++;
	}
	else
	{
		line_struct->address = MAIN_DATA.IC;
		strcpy(line_struct->label, nextWord);
		MAIN_DATA.EXTERN_SECTION[MAIN_DATA.XC] = line_struct;
		MAIN_DATA.XC++;
	}
	return 1;
}


/* pass_number should be 1 or 2 */
int file_pass(FILE *file, int pass_number)
{
	int i = 0, lineNum = -1, symbolFlag = 0;
	char * linep, op;
	char nextWord[MAX_LINE+2]; /*+2 for the '\n' and '\0' char*/
	char label[MAX_LABEL_SIZE+1];
	int errorCount, len, result;
	OP_TYPE type;
	char line[MAX_LINE+2]; /*+2 for the '\n' and '\0' char*/

	while (linep = fgets(line, MAX_LINE+1, file))
	{
		lineNum++;     /* we started with lineNum == -1 */
		len = strlen(line);
		if (len > 0 && line[len-1] == EOF) /* so we don't have to deal with EOF all the time */
		{
			line[len-1] = '\n';
		}

		if (linep[0] == '\0' || linep[0] == '\n' || linep[0] ==';')
		{
			continue; /*need to ignore blank lines and comment lines*/
		}

		linep = getNextToken(linep, nextWord);
		
		if (!linep)
			continue;
		
		type = is_label(nextWord, lineNum);
		if(type == SYMBOL && *linep == ':') /*maybe need to check if isSymbol == error*/
		{
			/* to do: maybe allow whitespace between nextWord and ':' */
			strcpy(label, nextWord); /* we save the symbol name for further use */
			symbolFlag = 1;
			/*collect next operator*/
			linep++;
			linep = getNextToken(linep, nextWord);
		}
		else if (type == ERROR || (type == SYMBOL && linep != ':'))
		{
			errorCount++;
			/*collect next operator*/
			linep = getNextToken(linep, nextWord);
		}

		if (!linep)
		{
			/* to do - print error? */
			continue;
		}
		
		type = is_instruction(linep, nextWord, lineNum);
		if (type == DATA || type == STRING)
		{
			if (pass_number == 1)
				if (!first_pass_data_command(linep, lineNum, symbolFlag, label))
					errorCount++;
			continue;
		}

		if (type == EXTERN || type == ENTRY)
		{
			if (pass_numer == 1)
				if (!first_pass_ee_command(type, linep, lineNum))
					errorCount++;
			else
			{
				if (!second_pass_ee_command(type, linep))
				{
					return 0;    /* FATAL ERROR */
				}
			}
			continue;
		}

		if (symbolFlag == 1)
		{
			if (pass_numer == 1)
				symbol_table_add(label, IC, 1, 0);
		}

		if (check_operation(nextWord, linep, lineNum) == 1)
			/* do lines 13-14 at pg.28*/
		else if (type == ERROR)
			errorCount++;

		/*check if there isn't anything more in the line*/
		if (verifyEndOfLine(linep, lineNum) == 0)
		{
			errorCount++;
			printf("Illegal command at line %d \n", lineNum);
		}
	}
	return errorCount;
}
