#include <stdio.h>
#include "defs.h"


int second_pass(FILE *f);

int second_pass_ee_command(INSTRUCTION_TYPE type, char *line, int line_num);
int second_pass_process_operands(char *next_word, char *linep, int line_num);

