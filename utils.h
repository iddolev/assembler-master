#include <stdio.h>

/* Open file filename with mode, and print error if failure. Returns pointer to file (or NULL if failed) */
FILE* open_file(char *filename, char *mode);
/* utils.h: header for utils.c, for utility functions */
char *strdup(const char* s);
/* Check if a string is made of only English alphabetic characters. Like isalpha() but for a string instead of a char. */
int isalpha_string(char* string);
/* Check if a string is made of only numeric charaters, like isdigit() but for a string instead of a char */
int is_number(char* string);
/* return 1 if char is a whitespace, otherwise 0 */
int is_whitespace(char c);
/* A char version of atoi(), because atoi() is for strings. Return 0 when the char is not a legal digit */
int c_atoi(char c);

/* Returns 1 iff c is '\0' or '\n'
   Useful for checking end of string in line and in tests */
int is_end_char(char c);

char * getNextToken(char * linep, char * buffer);
int verifyEndOfLine(char * linep);

/* Example: 
   current = 00000011
   to_add = 01
   num_bits = 2
   result = 00001101
*/
int push_to_encoding(int current, int to_add, int num_bits);
int encode_in_binary_complement2(int value, int num_bits, int line_num);

void to_binary_string(int value, int num_bits, char buffer[]);
void encoding_to_str(int encoding, int sizes[], int length, char output[]);


