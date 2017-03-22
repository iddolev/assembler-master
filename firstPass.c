#include <stdio.h>
#include "stack.h"
#include "utils.h"
#include "instructions.h"
#include "header.h"
#include "externs.h"


/* to do:
1. extern table
3. function like 'checkNumOp' that checks if the numbers of operands in the method are the right number,
 and also if it is the right adressing method for the method.
 it also should check the operands of an instruction command. (right num and right type).
4. function to add a symbol to the symbol table, check if it is not already there
 */

int first_pass(FILE *file)
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
		lineNum++;
		len = strlen(line);
		if (line[len-1] == EOF) /* so we don't have to deal with EOF all the time */
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
			continue;
		
		type = is_instruction(linep, nextWord, lineNum);
		if (type == DATA || type == STRING)
		{
			if (symbolFlag == 1)
			{
				symbol_table_add(label, DC, 0, 0);
			}
			if (type == DATA)
			{
				result = getDataOp(linep, lineNum); /*to do: should return a value for errorCount*/
				if (result == 0)
					errorCount++;
			}
			else
			{
				result = getStringOp(linep, lineNum); /*to do: should return a value for errorCoun*/
				if (result == 0)
					errorCount++;
			}
			continue;
		}

		if (type == EXTERN || type == ENTRY)
		{
			linep = getNextToken(linep, nextWord);
			type = is_label(nextWord, lineNum);
			if (type == SYMBOL)
			{
				if (type == EXTERN)
				{
					symbol_table_add(nextWord, 0, 0, 1);
				}
			}
			else
			{
				errorCount++;
			}
			/* now need to check if there isn't anything in the line afterward i.e '.extern LOOP asdfthgj' - it's not legal*/
			if (verifyEndOfLine(linep, lineNum) == 0)
			{
				errorCount++;
				printf("Illegal command at line %d \n", lineNum);
			}
			continue; 
		}

		if (symbolFlag == 1)
			symbol_table_add(label, IC, 1, 0);

		if (check_operation(nextWord, linep, lineNum) == 1)
			/* do lines 13-14 at pg.28*/
		else
			errorCount++;

		/*check if there isn't anything more in the line*/
		if (verifyEndOfLine(linep, lineNum) == 0)
			errorCount++;
			printf("Illegal command at line %d \n", lineNum);
	}
	return errorCount;
}
