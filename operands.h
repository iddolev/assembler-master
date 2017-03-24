#include "defs.h"

#define MAX_OPERANDS 2

typedef enum {IMMEDIATE, DIRECT, INDEX, REGISTER, ILLEGAL_OPERAND=-1, OTHER=-2} ADR_METHOD;

typedef struct {
	char text_value[MAX_SYMBOL_SIZE+1];
	ADR_METHOD addressing_method;
	unsigned int encoded;
} parsed_operand;


/*checks for the addressing method of the operands*/
ADR_METHOD find_addressing_method(char *s);

int is_register(char* s);
int check_operation(char *opcode_word, char* rest_of_line, int line_num);
int is_label_operand(char* s);

ADR_METHOD get_addressing_method(char *s, int line_num);
char * adr_method_to_string(ADR_METHOD adr_method);


