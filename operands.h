/* header file for operands.c, which contains functions and data structures for parsing and validating operands */

#define MAX_OPERANDS 2
typedef enum {IMMEDIATE, DIRECT, INDEX, D_REGISTER, ILLEGAL_OPERAND=-1} ADR_METHOD;

typedef struct {
    char* text_value;
    ADR_METHOD addressing_method;
    unsigned int encoded;
} parsed_operand;


/*checks for the addressing method of the operands*/
ADR_METHOD find_addressing_method(char *s);

int is_register(char* s);
int check_operation(char *opcode_word, char* rest_of_line, int line_num);
int is_label_operand(char* s);
