#include "defs.h"

typedef enum {DATA, STRING, EXTERN, ENTRY} ADR_TYPE;

OP_TYPE is_label(char *s, int lineNum);
ADR_TYPE is_instruction(char * word, char * linep, int lineNum);
int getDataOp(char * linep, int lineNum);
int getStringOp(char * linep, int lineNum);

