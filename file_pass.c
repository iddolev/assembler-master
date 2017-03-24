/* NOTE: This is an experiment to try to make the pass on a file generic and shared between the first pass and second pass.
   Don't use this file yet. */
   
#include <stdio.h>
#include "stack.h"
#include "utils.h"
#include "instructions.h"
#include "header.h"
#include "externs.h"
#include "first_pass.h"


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
	
	linep = getNextToken(linep, next_word);
	if (type == ENTRY)
	{
		symbol_table_data s_data = symbol_table_lookup(next_word);
		line_struct->address = s_data->address;
		strcpy(line_struct->label, next_word);
		MAIN_DATA.ENTRY_SECTION[MAIN_DATA.YC] = line_struct;
		MAIN_DATA.YC++;
	}
	else
	{
		line_struct->address = MAIN_DATA.IC;
		strcpy(line_struct->label, next_word);
		MAIN_DATA.EXTERN_SECTION[MAIN_DATA.XC] = line_struct;
		MAIN_DATA.XC++;
	}
	return 1;
}


/* pass_number should be 1 or 2 */
int file_pass(FILE *file, int pass_number)
{
	int i = 0, line_num = -1, symbol_flag = 0;
	char * linep, op;
	char next_word[MAX_LINE+2]; /*+2 for the '\n' and '\0' char*/
	char label[MAX_LABEL_SIZE+1];
	int error_count, len, result;
	OP_TYPE op_type;
	ADR_TYPE adr_type;
	char line[MAX_LINE+2]; /*+2 for the '\n' and '\0' char*/

	while (linep = fgets(line, MAX_LINE+10, file))
	{
		line_num++;     /* we started with line_num == -1 */
		len = strlen(line);
		if (len > MAX_LINE)
		{
			printf("Line number %d is too long \n", line_num);
			error_count++;
			continue;
		}
		if (len > 0 && line[len-1] == EOF) /* so we don't have to deal with EOF all the time */
		{
			line[len-1] = '\n';
		}

		if (linep[0] == '\0' || linep[0] == '\n' || linep[0] ==';')
		{
			continue; /*need to ignore blank lines and comment lines*/
		}

		linep = getNextToken(linep, next_word);
		
		if (!linep)
			continue;
		
		op_type = is_label(next_word, line_num);
		if(op_type == SYMBOL && *linep == ':') /*maybe need to check if isSymbol == error*/
		{
			/* to do: maybe allow whitespace between next_word and ':' */
			strcpy(label, next_word); /* we save the symbol name for further use */
			symbol_flag = 1;
			/*collect next operator*/
			linep++;
			linep = getNextToken(linep, next_word);
		}
		else if (op_type == ERROR || (op_type == SYMBOL && linep != ':'))
		{
			error_count++;
			/*collect next operator*/
			linep = getNextToken(linep, next_word);
		}

		if (!linep)
		{
			printf("No content after label in line %d\n", line_num);
			continue;
		}
		
		adr_type = is_instruction(next_word);
		if (adr_type == DATA || adr_type == STRING)
		{
			if (pass_number == 1)
				if (!first_pass_data_command(linep, line_num, symbol_flag, label))
					error_count++;
			continue;
		}

		if (adr_type == EXTERN || adr_type == ENTRY)
		{
			if (pass_numer == 1)
				if (!first_pass_ee_command(adr_type, linep, line_num))
					error_count++;
			else
			{
				if (!second_pass_ee_command(adr_type, linep))
				{
					return 0;    /* FATAL ERROR */
				}
			}
			continue;
		}

		if (symbol_flag == 1)
		{
			if (pass_numer == 1)
				symbol_table_add(label, IC, 1, 0);
		}

		if (check_operation(next_word, linep, line_num) == 1)
			/* do lines 13-14 at pg.28*/
		else if (type == ERROR)
			error_count++;

		/*check if there isn't anything more in the line*/
		if (verifyEndOfLine(linep, line_num) == 0)
		{
			error_count++;
			printf("Illegal command at line %d \n", line_num);
		}
	}
	return error_count;
}
