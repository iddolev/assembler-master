#include <stdlib.h>
#include <stdio.h>
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

	printf("MAIN_DATA.%s (%d):\n", name, size);
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

