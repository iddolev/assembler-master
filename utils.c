/* utils.c: utility functions */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "defs.h"

/* strdup: Duplicate a string. Identical to the POSIX library function with the same name */
char *strdup(const char* s) 
{
	char *new = malloc(strlen(s) + 1);
	if (new == NULL)
		return NULL;
	strcpy(new, s);
	return new;
}

/* Open file filename with mode, and print error if failure. Returns pointer to file (or NULL if failed) */
FILE* open_file(char *filename, char *mode)
{
	FILE *f = fopen(filename, mode);
	if (!f)
	{
		printf("ERROR: Could not open file: %s\n", filename);
	}
	return f;
}

/* Check if a string is made of only English alphabetic characters. Like isalpha() but for a string instead of a char. */
int isalpha_string(char* string) 
{
	int i;
	for (i=0; i<strlen(string); i++) 
	{
		if (!isalpha(string[i]))
		return 0;
	}
	return 1;
}

/* a simplyfied function. there is no need to check for whitespaces, because 'getNextToken' does that */
int is_number(char* str)
{
	int i = 1; /* we check index 0 in the second "if" below */
	int len = strlen(str);
	
	if (len > 0)
	{
		if (str[0] == '-' || str[0] == '+' || isdigit(str[0]))
		{
			while (i < len)
			{
				if (!isdigit(str[i]))
					return 0;
				i++;
			}
			return 1;
		}
		return 0;
	}
	return 0;
}

/* return 1 if char is a whitespace, otherwise 0 */
int is_whitespace(char c)
{
	return c == ' ' || c == '\t';
}

/* A char version of atoi(), because atoi() is for strings. Return 0 when the char is not a legal digit */
int c_atoi(char c) 
{
	if (!isdigit(c))
		return 0; /* Return 0 if it's not a digit */
	return c - '\0';  
}

/* Check if char c is a token separator */
int is_separator(char c) 
{
	return is_end_char(c) || c == ':'  || c == ',';
}

/* Read next word from linep into buffer.
   Return pointer to next position in linep.  */
char * getNextToken(char * linep, char * buffer)
{
	int j = 0;

	if(linep == NULL || is_end_char(*linep))
		return NULL;

	while (is_whitespace(*linep))
	{
		linep++; /*skip blank lines and stuff*/
	}

	while(!is_separator(*linep) && !is_whitespace(*linep) && j < MAX_SYMBOL_SIZE)
	{
		buffer[j] = *linep;    
		linep++;
		j++;
	}
	buffer[j]='\0';
	
	if (j == 0)
		return NULL;   /* We could not read a valid symbol */
	
	return linep; /* return the position that we reached (and did not use yet) */
}

int is_end_char(char c)
{
	return c == '\0' || c == '\n';
}

int verifyEndOfLine(char * linep)
{
	while (!is_end_char(*linep))
	{
		if (!is_whitespace(*linep))
		{
			return 0;
		}
		linep++;
	}
	return 1;
}

/* Example: 
   current = 00000011
   num_bits = 2
   to_add = 01
   result = 00001101
*/
int push_to_encoding(int current, int num_bits, int to_add)
{
	int max, mask;
	int result = current;
	result <<= num_bits;
	max = 1 << num_bits;
	if (to_add >= max)
	{
		printf("ERROR: push_to_encoding: Argument %d is to add is too large in %d bits\n", to_add, num_bits);
		mask = max-1;
		to_add = to_add & mask;    /* keep only the lower num_bits bits of to_add */ 
	}
	result += to_add;
	return result;
}

/* for debugging */
void to_binary_string(int value, int num_bits, char buffer[])
{
	int i, bit_mask, bit_value;
	for (i=num_bits-1; i>=0; --i)
	{
		bit_mask = 1 << i;
		bit_value = value & bit_mask;
		buffer[num_bits-1-i] = bit_value ? '1' : '0'; 
	}
	buffer[num_bits] = '\0';
	/* printf("to_binary_string(%d, %d) = %s\n", value, num_bits, buffer); */
}

/* for debugging */
void encoding_to_str(int encoding, int sizes[], int length, char output[])
{
	int accum[10];
	int i, mask, size;
	char buf[10];
	int result_pos=0;

	for (i=length-1; i>=0; --i)
	{
		size = sizes[i];
		mask = (1 << size) - 1;
		accum[i] = encoding & mask;
		encoding >>= size;
	}
	for (i=0; i<length; ++i)
	{
		to_binary_string(accum[i], sizes[i], buf);
		strncpy(output+result_pos, buf, sizes[i]);
		result_pos += sizes[i];
		output[result_pos++] = '-';
	}
	output[result_pos-1] = '\0';   /* replace the last '-' with '\0' */
}

