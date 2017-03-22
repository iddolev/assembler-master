#ifndef __DEFS_H
#define __DEFS_H


#define CODE_SECTION_SIZE 1000
#define DATA_SECTION_SIZE 1000
#define ENTRY_SECTION_SIZE 1000
#define EXTERN_SECTION_SIZE 1000

#define CODE_SECTION_OFFSET 100   /* The first code instruction starts at address 100 */

#define MAX_LINE_LENGTH 80
#define MAX_SYMBOL_SIZE 30
#define MAX_FILE_NAME_SIZE 50

typedef enum {NOT, NEW_LINE, SYMBOL, METHOD, OPERANT, REGISTER, ERROR, COMMENT} OP_TYPE;

void op_type_to_string(OP_TYPE op_type, char buffer[]);



#endif
