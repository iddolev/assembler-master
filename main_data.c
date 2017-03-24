#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main_data.h"


main_data MAIN_DATA;

void init_main_data()
{
	MAIN_DATA.IC = 0;
	MAIN_DATA.DC = 0;
	MAIN_DATA.YC = 0;
	MAIN_DATA.XC = 0;
}

/* free the accumulated e_file_line structs */
void reset_main_data()
{
	int i;
	for (i=0; i<MAIN_DATA.YC; ++i)
	{
		free(MAIN_DATA.ENTRY_SECTION[i]); 
	}
	for (i=0; i<MAIN_DATA.XC; ++i)
	{
		free(MAIN_DATA.EXTERN_SECTION[i]); 
	}
	init_main_data();
}

int add_to_data_section(int value)
{
	if (MAIN_DATA.DC >= DATA_SECTION_SIZE)
	{
		printf("Error: Too many data items\n");
		return 0;
	}
	MAIN_DATA.DATA_SECTION[MAIN_DATA.DC++] = value;
	return 1;
}

int add_to_code_section(int value)
{
	if (MAIN_DATA.IC >= CODE_SECTION_SIZE)
	{
		printf("Error: Too many code lines\n");
		return 0;
	}
	MAIN_DATA.CODE_SECTION[MAIN_DATA.IC++] = value;
	return 1;
}

int add_to_entry_section(int address, char *label)
{
	e_file_line *line_struct;

	if (MAIN_DATA.YC >= ENTRY_SECTION_SIZE)
	{
		printf("Error: Too many entry lines\n");
		return 0;
	}

	line_struct = (e_file_line*) malloc(sizeof(line_struct));
	if (!line_struct)
	{
		printf("Memory allocation problem\n");
		return 0;
	}
	
	line_struct->address = address;
	strcpy(line_struct->label, label);
	MAIN_DATA.ENTRY_SECTION[MAIN_DATA.YC++] = line_struct;
	return 1;
}

int add_to_extern_section(int address, char *label)
{
	e_file_line *line_struct;

	if (MAIN_DATA.XC >= EXTERN_SECTION_SIZE)
	{
		printf("Error: Too many extern lines\n");
		return 0;
	}

	line_struct = (e_file_line*) malloc(sizeof(line_struct));
	if (!line_struct)
	{
		printf("Memory allocation problem\n");
		return 0;
	}
	
	line_struct->address = address;
	strcpy(line_struct->label, label);
	MAIN_DATA.EXTERN_SECTION[MAIN_DATA.XC++] = line_struct;
	return 1;
}

void print_int_array(char *name, int array[], int size)
{
	int i;

	printf("MAIN_DATA.%s (%d):", name, size);
	if (size == 0)
		printf(" EMPTY");
	printf("\n");
	for (i=0; i<size; ++i)
	{
		printf("%s[%d] = %d\n", name, i, array[i]);
	}
}

void print_e_lines_array(char *name, e_file_line* array[], int size)
{
	int i;

	printf("MAIN_DATA.%s (%d):", name, size);
	if (size == 0)
		printf(" EMPTY");
	printf("\n");
	for (i=0; i<size; ++i)
	{
		printf("%s[%d] = %s %d\n", name, i, array[i]->label, array[i]->address);
	}
}

void print_main_data()   /* for debugging */
{

	print_int_array("CODE_SECTION", MAIN_DATA.CODE_SECTION, MAIN_DATA.IC);
	print_int_array("DATA_SECTION", MAIN_DATA.DATA_SECTION, MAIN_DATA.DC);
	print_e_lines_array("ENTRY_SECTION", MAIN_DATA.ENTRY_SECTION, MAIN_DATA.YC);
	print_e_lines_array("EXTERN_SECTION", MAIN_DATA.EXTERN_SECTION, MAIN_DATA.XC);
}

