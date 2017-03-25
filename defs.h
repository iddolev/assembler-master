#ifndef __DEFS_H
#define __DEFS_H


#define CODE_SECTION_SIZE 1000
#define DATA_SECTION_SIZE 1000
#define ENTRY_SECTION_SIZE 1000
#define EXTERN_SECTION_SIZE 1000

#define CODE_SECTION_OFFSET 100   /* The first code instruction starts at address 100 */

#define MAX_NUM_OPERANDS 2 /* Maximum number of operands in command */
#define DATA_VALUE_NUMBER_OF_BITS  15    /* for data section */
#define IMMEDIATE_VALUE_NUMBER_OF_BITS  13  /* for immediate addressing operand */

#define MAX_LINE_LENGTH 80
#define MAX_SYMBOL_SIZE 30
#define MAX_FILE_NAME_SIZE 50

typedef enum {NOT, SYMBOL, ERROR} OP_TYPE;

typedef enum {NOT_INST, DATA, STRING, EXTERN, ENTRY} INSTRUCTION_TYPE;

/* for debugging */
char * op_type_to_string(OP_TYPE op_type);



#endif
