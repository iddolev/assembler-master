#include <stdio.h>
#include "defs.h"


int first_pass(FILE *f);

int first_pass_data_command(INSTRUCTION_TYPE type, char *linep, int line_num);
int first_pass_ee_command(INSTRUCTION_TYPE type, char *linep, int line_num);
int first_pass_check_operation(char * opcode_word, char * linep, int line_num);

int process_data_instruction(char * linep, int line_num);
int process_string_instruction(char * linep, int line_num);

