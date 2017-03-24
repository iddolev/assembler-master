#include <stdio.h>
#include "defs.h"


int first_pass(FILE *f);

int first_pass_data_command(ADR_TYPE type, char *linep, int line_num);
int first_pass_ee_command(ADR_TYPE type, char *linep, int line_num);

int getDataOp(char * linep, int lineNum);
int getStringOp(char * linep, int lineNum);

