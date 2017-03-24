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

int checkImmediate(char * s)
{
    int i = 0;
    if (s[i] == '#') /*start of absolut addressing method*/
    {
        i++;
        if (s[i] == '+' || s[i] == '-' || isdigit(s[i])) /*something like '#-1'*/
        {
            i++;
            while(isdigit(s[i]))  
            {
              i++;
            }
            if (s[i] == '\0') /*if it stoped because it reached the end it is legal*/
            {
              return 1;
            }
        }
    }
    /*otherwise it stoped because of an illegal character*/
    return 0;
}

/*checks for the addressing method of the operands*/
ADR_METHOD find_addressing_method(char *s)/* to do: add lineNum*/
{
    /* TODO: ignore space at the end */
    int len = strlen(s);
    if (len > 1)
    {
        if (checkImmediate(s) == 1)
        {
            return IMMEDIATE;
        }
        if(is_register(s))/*if the string is something like 'r1' (because it stops collecting characters when it reaches ',')*/
            return D_REGISTER;
        if (len > 5)
        {
            if (s[0] == 'r' && s[2] == '[' && s[3] == 'r' && s[5] ==']' ) /*if the string is something like 'r1[r2]' (it is considered 1 operand therefore it hasn't stop collecting at '[')*/
            {
                if (c_atoi(s[1]) % 2 != 0) /*if the first register is an odd number */
                {
                    if (c_atoi(s[4]) % 2 == 0) /* and the second register is even */
                        return INDEX;
                }
            }
        }
    }
    if (is_label(s, 0))
        return DIRECT;
    /*otherwise it is illegal*/
    return ILLEGAL_OPERAND;
}

/* Check if an addressing method is valid for an operand, using the mode flags from opcode_item */
int valid_method_for_operand(int modeflags, ADR_METHOD method) {
    switch (method) {
        case IMMEDIATE:
            return modeflags & ADDRESSING_IMMEDIATE;
        break;
        case DIRECT:
            return modeflags & ADDRESSING_DIRECT;
        break;
        case D_REGISTER:
             return modeflags & ADDRESSING_REGISTER_DIRECT;
        break;
        case INDEX:
             return modeflags & ADDRESSING_REGISTER_INDEX;
        break;
        case ILLEGAL_OPERAND:
            return 0; /* It's unlikely that this will ever be reached, but it silences a compiler warning */
        break;
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
    printf("linep = \"%s\"\n", linep);
    while (*linep != '\n' && *linep != '\0')
    {
	printf("IN loop\n");
	linep = getNextToken(linep, nextWord);
	if (position > 2)
	{
               printf("At line %d illegal number of operands", lineNum);
               return 0; /*illegal number of operands*/
	}
	if (!linep)
	{
		break;
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
		/* printf error missing comma */
		return 0;   
	}
	else
	{
		linep++;
	}
    }
	printf("position is : %d\n", position);
    if (position == opcode->group)
    {
        if (position > 0)
        {
            if (position == 1)
            {
                dstAdr =find_addressing_method(operands[0]);
                if (valid_method_for_operand(opcode->addressing_mode.dst, dstAdr))
                {
		    MAIN_DATA.IC += 2;
                    return 1;
                }
                else
                {
                    printf("Incompatible addressing method for operand at line %d", lineNum);
                    return 0;
                }
            }
            else if (position == 2)
            {
                srcAdr = find_addressing_method(operands[0]);
                dstAdr =find_addressing_method(operands[1]);
                if (valid_method_for_operand(opcode->addressing_mode.src, srcAdr) && valid_method_for_operand(opcode->addressing_mode.dst, dstAdr))
                {
		    if (srcAdr == D_REGISTER)
		    {
			if (dstAdr == D_REGISTER)
				MAIN_DATA.IC += 2;
				return 1;
		    }
		    MAIN_DATA.IC += 3;
                    return 1;
                }
                else
                {
                    printf("Incompatible addressing method for operand at line %d", lineNum);
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
        int operands = check_operands(opcode_data, rest_of_line, line_num);
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

/*checks if the operand is a register*/
int is_register(char* s)
{
    if(strlen(s) != 2) 
    { /* register name must have 2 characters*/
      return 0;
    }
    else if(s[0] == 'r' && isdigit(s[1]))
    {
      if(c_atoi(s[1]) < 8)
        return 1;
    }
    return 0;
}
