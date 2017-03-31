#include "defs.h"

OP_TYPE is_label(char *s, int line_num);
INSTRUCTION_TYPE is_instruction(char * word);
int process_data_instruction(char * linep, int line_num);
int process_string_instruction(char * linep, int line_num);

