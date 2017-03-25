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
	int num_digits = 0;

	if (s[i] == '#') 
	{
		i++;
		if (s[i] == '+' || s[i] == '-')  
		{
			i++;
		}
		while (isdigit(s[i]))  
		{
			num_digits++;
			i++;
		}
		if (num_digits == 0)
		{
			printf("Missing digits after '#' at line %d\n", line_num);
			return ILLEGAL_OPERAND;
		}
		if (s[i] == '\0')
		{
			return IMMEDIATE;
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
	char buff[3];      /* 2 characters for register name + terminating '\0' */
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
	ADR_METHOD addressing_method;
	char nextWord[MAX_SYMBOL_SIZE+1];
    
	while (!is_end_char(*linep))
	{
		if (position >= 2)
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
		strcpy(operands[position].text, nextWord);
		addressing_method = get_addressing_method(nextWord, line_num);
		if (addressing_method == ILLEGAL_OPERAND)
		{
			return 0;
		}
		operands[position].addressing_method = addressing_method;
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

int encode_command(int group, int opcode, ADR_METHOD src, ADR_METHOD dst)
{
	int encoding = 0;
	
	encoding = push_to_encoding(encoding, 3, 7);         /* constant 111 in binary */
	encoding = push_to_encoding(encoding, 2, group);
	encoding = push_to_encoding(encoding, 4, opcode);
	encoding = push_to_encoding(encoding, 2, src);
	encoding = push_to_encoding(encoding, 2, dst);
	encoding = push_to_encoding(encoding, 2, ABSOLUTE);         /* command has absolute coding type */
	return encoding;
}

int encode_registers(int register1, int register2)
{
	int encoding = 0;
	encoding = push_to_encoding(encoding, 6, register1);
	encoding = push_to_encoding(encoding, 6, register2);
	encoding = push_to_encoding(encoding, 2, ABSOLUTE);
	return encoding;
}

int encode_argument(CODING_TYPE coding_type, int value)
{
	int encoding = 0;
	int max, mask;
	int num_bits = VALUE_REPRESENTATION_NUMBER_OF_BITS;

	if (value >= 0)
	{
		/* Since we are using a complement-2 representation using 13 bits, 
		   and we reserve the most significant bit of the 13 bits to mark a negative number,
		   this means we can only use 12 bits to encode a positive number.
		   So if value is larger than what can be encoded in 12 bits, this is an error */
		max = 1 << (num_bits-1);
		if (value >= max)
		{
			printf("Cannot encode the value %d into only %d bits (complement-2)\n", value, num_bits);
			return -1;
		}
	}
	else  /* value < 0 */
	{
		/* If value is a negative number, its form is 111...111.
                   As with positive numbers above, there is a limit to the possible numbers we can accept: */
		max = 1 << (num_bits-1);
		if (-value > max)  /* The largest possible negative number in binary is represented as 100...000.
					Its absolute value, (-value) is == max.
					So if -value > max we have an error */
		{
			printf("Cannot encode the value %d into only %d bits (complement-2)\n", value, num_bits);
			return -1;
		}

		mask = (1 << num_bits) - 1;    /*  mask = 0000...01111111111111  (13 times 1 bit, rest is zero) */
		value = value & mask;   /* the complement-2 representation over 13 bits uses the 13 least significant bits of value */
	}
	encoding = push_to_encoding(encoding, num_bits, value);
	encoding = push_to_encoding(encoding, 2, coding_type);
	return encoding;
}


