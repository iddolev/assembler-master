#include <stdio.h>
#include "externs.h"
#include "first_pass.h"
#include "second_pass.h"
#include "write_data.h"
#include "symbol_table.h"
#include "opcodes.h"
#include "utils.h"
#include "write_data.h"


void run_on_file(char *file_name)
{
	FILE *f;
	int errors;
	char full_name[MAX_FILE_NAME_SIZE];
	int success;
	
	symbol_table_init();
	init_main_data();
	
	sprintf(full_name, "%s.as", file_name);

	printf("Processing file: %s\n", full_name);
	if ((f = open_file(full_name, "r")))
	{
		errors = first_pass(f);
		fclose(f);
		if (errors > 0)
		{
			printf("Encountered errors in first pass, so not doing second pass: %s\n", full_name);
		}
		else if ((f = open_file(full_name, "r")))
		{
			success = second_pass(f);
			fclose(f);
			if (success)
			{
				write_data_to_files(file_name);
			}
			else
			{
				printf("Error during second pass, so not writing output files: %s\n", full_name); 
			}
		}
	}
	
	reset_main_data();
	symbol_table_clear();  /* todo (add clear_hash_table(hash_table*) in hash_table.c, and use in symbol_table.c */ 
}

void run_on_files(char *file_names[], int num_files)
{
	int i;
	for (i = 0; i < num_files; ++i)
	{
		run_on_file(file_names[i]);
	}
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("No file names supplied\n");
		return 1;
	}
	
	if (!opcode_table_init())
	{
		printf("Opcode table initialization failed\n");
		return 1;
	}
	run_on_files(argv+1, argc-1);
	return 0;
}

