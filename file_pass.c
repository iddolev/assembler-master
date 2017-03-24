/* NOTE: This is an experiment to try to make the pass on a file generic and shared between the first pass and second pass.
   Don't use this file yet. */
   
#include <stdio.h>
#include <string.h>
#include "file_pass.h"
#include "utils.h"
#include "externs.h"
#include "first_pass.h"
#include "second_pass.h"
#include "utils.h"
#include "operands.h"
#include "symbol_table.h"
#include "instructions.h"


/* pass_number should be 1 or 2 */
int file_pass(FILE *file, int pass_number)
{
	int line_num = -1, symbol_flag = 0;
	char * linep;
	char next_word[MAX_SYMBOL_SIZE+1]; /* +1 for terminating '\0' */
	char label[MAX_SYMBOL_SIZE+1];
	int error_count, len;
	OP_TYPE op_type;
	ADR_TYPE adr_type;
	char line[MAX_LINE_LENGTH+2]; /*+2 for the '\n' and '\0' char*/

	while ((linep = fgets(line, MAX_LINE_LENGTH+10, file)))
	{
		line_num++;     /* we started with line_num == -1 */
		len = strlen(line);
		if (len > MAX_LINE_LENGTH)
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
		if (op_type == SYMBOL) /*maybe need to check if isSymbol == error*/
		{
			if (*linep == ':')
			{
				/* to do: maybe allow whitespace between next_word and ':' */
				strcpy(label, next_word); /* we save the symbol name for further use */
				symbol_flag = 1;
				/*collect next operator*/
				linep++;   /* skip ':' */
				linep = getNextToken(linep, next_word);
				if (!linep)
				{
					printf("No content after label in line %d\n", line_num);
					error_count++;
					continue;
				}
			}
			else
			{
				printf("Missing ':' after label in line %d\n", line_num);
				error_count++;
				continue;
			}
		}
		else if (op_type == ERROR)
		{
			/* error message printed inside is_label */
			error_count++;
			continue;
		}

		adr_type = is_instruction(next_word);
		if (adr_type == DATA || adr_type == STRING)
		{
			if (pass_number == 1)
			{
				if (symbol_flag)
				{
					symbol_table_add(label, MAIN_DATA.DC, 0, 0);
				}
				if (!first_pass_data_command(adr_type, linep, line_num))
					error_count++;
			}
			continue;
		}
		else if (adr_type == EXTERN || adr_type == ENTRY)
		{
			if (symbol_flag)
			{
				printf("Warning: Ignoring label before extern/entry instruction at line %d\n", line_num);
				/* not counting as error */
			}
			if (pass_number == 1)
			{
				if (!first_pass_ee_command(adr_type, linep, line_num))
					error_count++;
			}
			else
			{
				if (!second_pass_ee_command(adr_type, linep))
				{
					return 0;    /* FATAL MEMORY ALLOCATION ERROR */
				}
			}
			continue;
		}

		if (symbol_flag)
		{
			if (pass_number == 1)
				symbol_table_add(label, MAIN_DATA.IC, 1, 0);
		}

		if (check_operation(next_word, linep, line_num))
		{
			/* do lines 13-14 at pg.28*/
		}
		else
		{
			error_count++;
		}

		/* check if there isn't anything more in the line */
		if (!verifyEndOfLine(linep))
		{
			error_count++;
			printf("Illegal command at line %d \n", line_num);
		}
	}
	return error_count;
}

