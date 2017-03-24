#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "operands.h"
#include "defs.h"
#include "opcodes.h"
#include "utils.h"
#include "instructions.h"
#include "externs.h"


#define MAX_NUM_OPERANDS 2 /* Maximum number of operands in command */





/* Check if an addressing method is valid for an operand, using the mode flags from opcode_item */
int valid_method_for_operand(int modeflags, ADR_METHOD method) 
{
	switch (method) 
	{
		case IMMEDIATE:
			return modeflags & ADDRESSING_IMMEDIATE;
		case DIRECT:
			return modeflags & ADDRESSING_DIRECT;
		case REGISTER:
			return modeflags & ADDRESSING_REGISTER_DIRECT;
		case INDEX:
			return modeflags & ADDRESSING_REGISTER_INDEX;
		case ILLEGAL_OPERAND:
		default:
			return 0; /* It's unlikely that this will ever be reached, but it silences a compiler warning */
	}
	return 0; /* Just in case "method" was passed a value not in the ADR_METHOD enum */
}

/* we read the rest of the line (word by word) with linep and getNextToken 
and then save the operands in a strings array, then check if their legal */
int check_operands(opcode_item *opcode, char * linep, int lineNum)
{
    char operands[MAX_NUM_OPERANDS][MAX_SYMBOL_SIZE]; /* should not be more than 2 operands but check for overlap */
    int position = 0;
    ADR_METHOD srcAdr, dstAdr;
    char nextWord[MAX_SYMBOL_SIZE+1];
    
    if (opcode->group != 0)
    {
	while (*linep != '\n' && *linep != '\0')
	{
		linep = getNextToken(linep, nextWord);
		if (position > 2)
		{
		       printf("At line %d illegal number of operands\n", lineNum);
		       return 0; /*illegal number of operands*/
		}
		if (!linep)
		{
			printf("Illegal operand at line %d\n", lineNum);
			return 0;
		}
		if (strlen(nextWord) > 0)
		{ 
			strcpy(operands[position], nextWord);
			position++;
		}
		while (is_whitespace(*linep))
		{
			linep++; /*skip blank lines and stuff*/
		}
		if (*linep == '\n')
		{
			/* we successfully finished scanning the operands */
			break;
		}
		if (*linep != ',')
		{
			printf("Illegal operand at line %d \n", lineNum);
			return 0;   
		}
		else
		{
			linep++;
		}
	}
    }

    if (!verifyEndOfLine(linep))
    {
	printf("Illegal operand at line %d\n", lineNum);
	return 0;
    }

    if (position == opcode->group)
    {
        if (position > 0)
        {
            if (position == 1)
            {
                dstAdr =get_addressing_method(operands[0], lineNum);
                if (valid_method_for_operand(opcode->addressing_mode.dst, dstAdr))
                {
		    MAIN_DATA.IC += 2;
                    return 1;
                }
                else
                {
                    printf("Incompatible addressing method for operand at line %d\n", lineNum);
                    return 0;
                }
            }
            else if (position == 2)
            {
                srcAdr = get_addressing_method(operands[0], lineNum);
                dstAdr =get_addressing_method(operands[1], lineNum);
                if (valid_method_for_operand(opcode->addressing_mode.src, srcAdr) && valid_method_for_operand(opcode->addressing_mode.dst, dstAdr))
                {
		    if (srcAdr == REGISTER)
		    {
			if (dstAdr == REGISTER)
				MAIN_DATA.IC += 2;
				return 1;
		    }
		    MAIN_DATA.IC += 3;
                    return 1;
                }
                else
                {
                    printf("Incompatible addressing method for operand at line %d\n", lineNum);
                    return 0;
                }
            }
        }
	MAIN_DATA.IC += 1;
        return 1; /* no operands given for the given method as needed (i.e 'stop') */
    }
    else
    {
        printf("Unsuitable number of operands for method at line %d \n", lineNum);
        return 0;
    }
    
}

/* Checks if a line is a legal operation line. Returns the length of the encoded line in processor words (either 1, 2 or 3) or 0 on error */
int check_operation(char *opcode_word, char* rest_of_line, int line_num)
{
    opcode_item* opcode_data;
    if ((opcode_data = opcode_lookup(opcode_word)) != NULL)
    {
        int operands = check_operands(opcode_data, rest_of_line, line_num);/* to do: move outside this function */
        if (operands == 0)
            return 0;
        else
            return 1; /*TODO!!! Figure out how to use the data from check_operands to calculate how many processor words we need 1, 2 or 3 */
    } 
    else 
    {
        printf("Line %d: invalid operation '%s'\n", line_num, opcode_word);
        return 0;
    }
}


char * adr_method_to_string(ADR_METHOD adr_method)
{
	switch(adr_method)
	{
		case IMMEDIATE:
			return "IMMEDIATE";
		case DIRECT:
			return "DIRECT";
		case INDEX:
			return "INDEX";
		case REGISTER:
			return "REGISTER";
		case ILLEGAL_OPERAND:
			return "ILLEGAL_OPERAND";
		case OTHER:
			return "OTHER";
	};
	return "ILLEGAL ADR_METHOD!";
}

ADR_METHOD check_immediate(char * s, int line_num)
{
	int i = 0;
	if (s[i] == '#') 
	{
		i++;
		if (s[i] == '+' || s[i] == '-' || isdigit(s[i]))  
		{
			i++;
			while (isdigit(s[i]))  
			{
				i++;
			}
			if (s[i] == '\0')
			{
				return IMMEDIATE;
			}
		}
		printf("Illegal operand after '#' at line %d\n", line_num);
		return ILLEGAL_OPERAND;
	}
	return OTHER;
}

/* checks if the operand is a register name */
int is_register(char* s)
{
	if(!s || strlen(s) != 2) 
	{	/* register name must have 2 characters*/
		return 0;
	}
	else if(s[0] == 'r' && '0' <= s[1] && s[1] <= '7')
	{
		/* register name starts with r, then digit between 0 and 7 */
		return 1;
	}
	return 0;
}

ADR_METHOD check_index_method(char *s, int line_num)
{
	char buff[3];
	int len = strlen(s);

	if (len == 6 && s[2] == '[' && s[5] == ']')
	{
		buff[0] = s[0];
		buff[1] = s[1];
		buff[2] = '\0';
		if (!is_register(buff))
		{
			printf("Illegal register name \'%s\' at line %d\n", buff, line_num);
			return ILLEGAL_OPERAND;
		}
		buff[0] = s[3];
		buff[1] = s[4];
		buff[2] = '\0';
		if (!is_register(buff))
		{
			printf("Illegal register name \'%s\' at line %d\n", buff, line_num);
			return ILLEGAL_OPERAND;
		}

		if (c_atoi(s[1]) % 2 != 0) /*if the first register is an odd number */
		{
			if (c_atoi(s[4]) % 2 == 0) /* and the second register is even */
			{
			    return INDEX;
			}
		}
		printf("For index addressing method - left register must be odd and right register must be even - at line %d\n", line_num); 
		return ILLEGAL_OPERAND;
	}
	return OTHER;
}

/* checks for the addressing method of the operand */
ADR_METHOD get_addressing_method(char *s, int line_num) 
{
	ADR_METHOD adressing_method;	

	adressing_method = check_immediate(s, line_num);
	if (adressing_method == IMMEDIATE || adressing_method == ILLEGAL_OPERAND)
	{
		return adressing_method;
	}
	if (is_register(s))
	{
		return REGISTER;
	}
	adressing_method = check_index_method(s, line_num);
	if (adressing_method == INDEX || adressing_method == ILLEGAL_OPERAND)
	{
		return adressing_method;
	}
	if (is_label(s, line_num) == SYMBOL)
	{
		return DIRECT;
	}

	return ILLEGAL_OPERAND;
}

/* returns number of operands: 1, or 2
   and fills operands[]
   if error, returns 0 */
int collect_operands(parsed_operand operands[], char *linep, int line_num)
{
	int position = 0;
	char nextWord[MAX_SYMBOL_SIZE+1];
    
	while (!is_end_char(*linep))
	{
		if (position > 2)
		{
		       printf("At line %d illegal number of operands\n", line_num);
		       return 0; 
		}
		linep = getNextToken(linep, nextWord);
		if (!linep)
		{
			printf("Missing operand at line %d\n", line_num);
			return 0;
		}
		strcpy(operands[position].text_value, nextWord);
		position++;
		while (is_whitespace(*linep))
		{
			linep++; 
		}
		if (is_end_char(*linep))
		{
			/* we successfully finished scanning the operands */
			break;
		}
		if (*linep != ',')
		{
			printf("Missing comma at line %d \n", line_num);
			return 0;
		}
		linep++;
	}
	return position;
}

