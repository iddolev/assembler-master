#include "defs.h"

typedef struct e_file_line {
	int address;
	char label[MAX_SYMBOL_SIZE];
} e_file_line;

typedef struct main_data {
	int IC;  /* Instruction counter - for CODE_SECTION */
	int DC;  /* Data counter - for DATA_SECTION */
	int YC;  /* counter for ENTRY_SECTION */
	int XC;  /* counter for EXTERN_SECTION */
	int CODE_SECTION[CODE_SECTION_SIZE];
	int DATA_SECTION[DATA_SECTION_SIZE];
	e_file_line* ENTRY_SECTION[ENTRY_SECTION_SIZE];
	e_file_line* EXTERN_SECTION[EXTERN_SECTION_SIZE];
} main_data;


/* Must be called at beginning of program */
void init_main_data();

/* free the accumulated e_file_line structs */
void reset_main_data();

/* for debug */
void print_main_data();

void add_to_code_section(int value);


