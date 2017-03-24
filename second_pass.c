#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "utils.h"
#include "symbol_table.h"
#include "externs.h"


int second_pass_ee_command(ADR_TYPE type, char *line)
{
	char next_word[MAX_SYMBOL_SIZE];
	symbol_table_data *s_data;
	e_file_line *line_struct;
	
	line_struct = (e_file_line*) malloc(sizeof(line_struct));
	if (!line_struct)
	{
		printf("Memory allocation problem\n");
		return 0;
	}
	
	getNextToken(line, next_word);
	if (type == ENTRY)
	{
		s_data = symbol_table_lookup(next_word);
		line_struct->address = s_data->address;
		strcpy(line_struct->label, next_word);
		MAIN_DATA.ENTRY_SECTION[MAIN_DATA.YC] = line_struct;
		MAIN_DATA.YC++;
	}
	else
	{
		line_struct->address = MAIN_DATA.IC;
		strcpy(line_struct->label, next_word);
		MAIN_DATA.EXTERN_SECTION[MAIN_DATA.XC] = line_struct;
		MAIN_DATA.XC++;
	}
	return 1;
}

