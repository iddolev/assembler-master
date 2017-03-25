#include "defs.h"

#define MAX_OPERANDS 2    /* the maximal number of operands for an opcode */
#define VALUE_REPRESENTATION_NUMBER_OF_BITS 13    /* how many bits we are using to encode a value (2-complement) */


typedef enum {IMMEDIATE=0, DIRECT=1, INDEX=2, REGISTER=3, ILLEGAL_OPERAND=-1, OTHER=-2} ADR_METHOD;
#define ADR_METHOD_DONT_CARE 0
#define NOT_USED 0

typedef enum {ABSOLUTE=0, EXTERNAL=1, RELOCATABLE=2} CODING_TYPE;

typedef struct {
	char text[MAX_SYMBOL_SIZE+1];
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

int collect_operands(parsed_operand operands[], int expected_num_operands, char *linep, int line_num);
int valid_method_for_operand(int modeflags, ADR_METHOD method);

int encode_command(int group, int opcode, ADR_METHOD src, ADR_METHOD dst);
int encode_registers(int register1, int register2);
int encode_argument(CODING_TYPE coding_type, int value);


