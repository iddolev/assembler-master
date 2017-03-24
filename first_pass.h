#include "defs.h"
#include "instructions.h"

int first_pass_data_command(ADR_TYPE type, char *linep, int line_num, int symbol_flag, char *label);
int first_pass_ee_command(ADR_TYPE type, char *linep, int line_num);

int getDataOp(char * linep, int lineNum);
int getStringOp(char * linep, int lineNum);

