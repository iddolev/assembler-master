#include <stdio.h>
#include "externs.h"
#include "first_pass.h"
#include "second_pass.h"
#include "write_data.h"
#include "symbol_table.h"
#include "opcodes.h"
#include "utils.h"
#include "write_data.h"

/* the main program */

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
		errors = first_pass(f); /* calling to file pass with a flag for the first pass */
		fclose(f);
		if (errors > 0)
		{
			printf("Encountered errors in first pass, so not doing second pass: %s\n", full_name);
		}
		else if ((f = open_file(full_name, "r")))
		{
			success = second_pass(f); /* calling to file pass with a flag for the second pass */
			fclose(f);
			clear_output_files(file_name);
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
	symbol_table_clear();  
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

