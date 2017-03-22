#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "operands.h"
#include "defs.h"
#include "opcodes.h"
#include "utils.h"

#define MAX_NUM_OPERANDS 2 /* Maximum number of operands in command */

/* Check if a string is a valid label operand */
/* TODO: maybe replace with "get_label_operand which will return the label name */
/* ALSO TODO: error reporting? */
/* int is_label_operand(char* s) {
    enum {BEGIN, IN, END} state = BEGIN;
    int len = 0;
    while (*s != '\0') {
        switch (state) {
            case BEGIN:
                if (*s != ' ' && *s != '\t') { /* ignore whitespace at start 
                    if (isalpha(*s)) { /* must start with a letter 
                        state = IN;
                        len++; /* we should also increase the length at the first character of the label 
                    } else {
                        return 0; /* starts with something that isn't a letter = invalid 
                    }
                }
            break;
            case IN:
                if (isalpha(*s) || isdigit(*s)) /* only valid characters for label 
                    len++;
                else if (*s == ' ' && *s == '\t') /* space at the end 
                    state = END;
            break;
            case END:
                if (*s != ' ' && *s != '\t') /* anything after the space? invalid 
                    return 0;
            break;
        }
        s++;
    }
    if (len > MAX_SYMBOL)
        return 0; /* too long 
    return 1; /* if wee got here, it's valid 
} */

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
ADR_METHOD find_addressing_method(char *s)
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
    if (is_label_operand(s))
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

/* Check validity of operands for the given opcode.
 * opcode is an opcode_item returned from opcode_lookup
 * operands is the line, starting *after* the operation name
 * line_num is the line number, for error reporting.
 *
 * Returns the number of operands on success, or -1 on error.
 * */
/* int check_operands(opcode_item *opcode, char* operands, int line_num) {
    int found_operands = 0;
    char buffer[MAX_OPERANDS][MAX_LINE]; /* An operand will never be longer than the maximum length of a line.
    ADR_METHOD adr_method[MAX_OPERANDS];
    int j = 0;
    enum  {BEGIN, IN_OPERAND} current_state = BEGIN;
    while (*operands != '\0') {
        switch (current_state) {
            case BEGIN:
                if (*operands != ' ' && *operands != '\t') { /* ignore whitespace 
                    current_state = IN_OPERAND;
                    buffer[found_operands][j] = *operands;
                    j++;
                }
            break;
            case IN_OPERAND:
                if (opcode->group == 0) { /* group is also the number of operands 
                    printf("Line %d: too many operands for %s, expected no operands\n", line_num, opcode->name);
                    return -1;
                }
                if (*operands != ',') { /* read until ',' 
                    buffer[found_operands][j] = *operands; /* fill the buffer 
                    j++;
                }
            break;
        }
        if (current_state == IN_OPERAND && *operands==',') {
            if (found_operands == 1) {
                printf("Line %d: unexpected comma, no command has 3 operands\n", line_num);
            }
            buffer[found_operands][++j] = '\0'; /* Make sure the string is terminated 
            adr_method[found_operands] = find_addressing_method(buffer[found_operands]);
            if (adr_method[found_operands] == ILLEGAL_OPERAND) {
                printf("Line %d: Illegal operand %s", line_num, buffer[found_operands]);
                return -1;
            }
            found_operands++;
            j = 0; /* go back to the start of the buffer for the next operand 
            current_state = BEGIN; /* reset state 
        }
        operands++;
    }
    if (current_state == IN_OPERAND) {
        adr_method[found_operands] = find_addressing_method(buffer[found_operands]);
        if (adr_method[found_operands] == ILLEGAL_OPERAND) {
            printf("Line %d: Illegal operand %s\n", line_num, buffer[found_operands]);
            return -1;
        }
        found_operands++;
    }
    if (found_operands != opcode->group) {
        printf("Line %d: incorrect number of operands for %s, expected %d, got %d\n", line_num, opcode->name, opcode->group, found_operands);
        return -1;
    }
    if (found_operands == 1) {
        /* when there's only one operand, it's always dst 
        if (!valid_method_for_operand(opcode->addressing_mode.dst, adr_method[0])) {
            printf("Line %d: Unsupported destination operand addressing mode in operand '%s'\n", line_num, buffer[0]);
            return -1;
        }
    } else if (found_operands > 1) {
        /* Two operands - first is source, second is destination 
        if (!valid_method_for_operand(opcode->addressing_mode.src, adr_method[0])) {
            printf("Line %d: Unsupported source operand addressing mode in operand '%s'\n", line_num, buffer[0]);
            return -1;
        }
        if (!valid_method_for_operand(opcode->addressing_mode.dst, adr_method[1])) {
            printf("Line %d: Unsupported destination operand addressing mode in operand '%s'\n", line_num, buffer[1]);
            return -1;
        }
    }
    return found_operands;
} */

 /* I tried to make the method less complicated */
/* we read the rest of the line (word by word) with linep and getNextToken 
and then save the operands in a strings array, then check if their legal */
int check_operands(opcode_item *opcode, char * linep, int lineNum)
{
    char operands[MAX_NUM_OPERANDS][MAX_SYMBOL_SIZE]; /* should not be more than 2 operands but check for overlap */
    int position = 0;
    ADR_METHOD srcAdr, dstAdr;
    char nextWord[MAX_SYMBOL_SIZE+1];
    
    while (*linep != '\n')
    {
	linep = getNextToken(linep, nextWord);
	if (position > 2)
	{
               printf("At line %d illegal number of operands", lineNum);
               return 0; /*illegal number of operands*/
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
        
    if (position == opcode->group)
    {
        if (position > 0)
        {
            if (position == 1)
            {
                dstAdr =find_addressing_method(operands[0]);
                if (valid_method_for_operand(opcode->addressing_mode.dst, dstAdr))
                {
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
                    return 1;
                }
                else
                {
                    printf("Incompatible addressing method for operand at line %d", lineNum);
                    return 0;
                }
            }
        }
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
   /* enum {BEGIN, IN} state = BEGIN;
    char buffer[MAX_OPNAME + 1];
    opcode_item* opcode;
    int len = 0;
    int found = 0;
    while (*s != '\0' && !found) {
        switch (state) {
            case BEGIN:
                if (*s != ' ' && *s != '\t') {
                    state = IN;
                    buffer[len] = *s;
                    len++;
                }
            break;
            case IN:
                if (len > MAX_OPNAME) {
                    printf("Line %d: invalid operation\n", line_num);
                    return 0;
                }
                if (*s != ' ' && *s != '\t') { /* read until space 
                    buffer[len] = *s;
                    len++;
                } else {
                    found = 1;
                }
        }
        s++;
    }
    buffer[++len] = '\0'; /* Make sure the string is terminated */
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
