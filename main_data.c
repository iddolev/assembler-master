#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main_data.h"
#include "utils.h"


main_data MAIN_DATA;

/* a function to initialize the data */
void init_main_data()
{
	MAIN_DATA.IC = 0;
	MAIN_DATA.DC = 0;
	MAIN_DATA.YC = 0;
	MAIN_DATA.XC = 0;
	MAIN_DATA.DATA_OFFSET = 0;
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

/* a function to add data to the data section and update the DC */
int add_to_data_section(int value)
{
	int encoded_value;

	if (MAIN_DATA.DC >= DATA_SECTION_SIZE)
	{
		fprintf(stderr, "Error: Too many data items\n");
		return 0;
	}

	encoded_value = encode_in_binary_complement2(value, DATA_VALUE_NUMBER_OF_BITS);
	if (encoded_value == -1)
		return 0;   /* error flag */

	MAIN_DATA.DATA_SECTION[MAIN_DATA.DC++] = encoded_value;
	return 1;
}

/* a function to add code to the code section and update the IC */
int add_to_code_section(int value)
{
	if (MAIN_DATA.IC >= CODE_SECTION_SIZE)
	{
		fprintf(stderr, "Error: Too many code lines\n");
		return 0;
	}
	MAIN_DATA.CODE_SECTION[MAIN_DATA.IC++] = value;
	return 1;
}

/* a function to add the entry/extern lables to their section (so we can output them to the suitable files) */
int add_to_ee_section(char *section_name, e_file_line* array[], int pos, int max_pos, int address, char *label)
{
	e_file_line *line_struct;

	if (pos >= max_pos)
	{
		fprintf(stderr, "Error: Too many %s lines\n", section_name);
		return 0;
	}

	line_struct = (e_file_line*) malloc(sizeof(line_struct));
	if (!line_struct)
	{
		fprintf(stderr, "Memory allocation problem in %s table\n", section_name);
		return 0;
	}
	
	line_struct->address = address;
	strcpy(line_struct->label, label);
	array[pos] = line_struct;
	return 1;
}

/* a function to add the entries to the ENTRY_SECTION */
int add_to_entry_section(int address, char *label)
{
	int ret = add_to_ee_section("entry", MAIN_DATA.ENTRY_SECTION, MAIN_DATA.YC, ENTRY_SECTION_SIZE, address, label);
	if (ret)
		MAIN_DATA.YC++;
	return ret;
}

/* a function to add the externs to the EXTERN_SECTION */
int add_to_extern_section(int address, char *label)
{
	int ret = add_to_ee_section("extern", MAIN_DATA.EXTERN_SECTION, MAIN_DATA.XC, EXTERN_SECTION_SIZE, address, label);
	if (ret)
		MAIN_DATA.XC++;
	return ret;
}

/* for debugging */
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

/* for debugging */
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
	printf("IC = %d, DC = %d, YC = %d, XC = %d, DATA_OFFSET = %d\n", 
		MAIN_DATA.IC, MAIN_DATA.DC, MAIN_DATA.YC, MAIN_DATA.XC, MAIN_DATA.DATA_OFFSET);
	print_int_array("CODE_SECTION", MAIN_DATA.CODE_SECTION, MAIN_DATA.IC);
	print_int_array("DATA_SECTION", MAIN_DATA.DATA_SECTION, MAIN_DATA.DC);
	print_e_lines_array("ENTRY_SECTION", MAIN_DATA.ENTRY_SECTION, MAIN_DATA.YC);
	print_e_lines_array("EXTERN_SECTION", MAIN_DATA.EXTERN_SECTION, MAIN_DATA.XC);
}

