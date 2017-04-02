#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "write_data.h"
#include "externs.h"
#include "utils.h"

/* functions for writing the code, data, entry, extern sections to files */

/* write a label and address to file (for .ent and .ext files) */
void write_file_line(FILE *f, char *label, int address)
{
        fprintf(f, "%s\t%02X\n", label, address);  
	/* for debug: */
	/* char buf[20];
	to_binary_string(address, 15, buf);
        fprintf(f, "%s\t%02X\t%s\n", label, address, buf); */
}

/* write a an address and value to file (for .ob file) */
void write_file_int(FILE *f, int address, int value)
{
        fprintf(f, "%02X\t%04X\n", address, value);

	/* for debug: */
	/* char buf[20];
	to_binary_string(value, 15, buf);
        fprintf(f, "%02X\t%04X\t%s\n", address, value, buf); */
}

/* write accumulated info to .ob file */
void write_object_file(char *file_prefix)
{
        FILE *f;
        int line_num;
        char full_name[MAX_FILE_NAME_SIZE];
        sprintf(full_name, "%s.ob", file_prefix);

	if (MAIN_DATA.IC == 0 && MAIN_DATA.DC == 0)
	{
		printf("Nothing to write to %s so not creating this file\n", full_name);
		remove(full_name);
		return;
	}

        if ((f = open_file(full_name, "w")))
        {
		printf("Writing to %s\n", full_name);
		/* first line: length of code section and of data section (saved from first pass) */
	        fprintf(f, "%X %X\n", MAIN_DATA.DATA_OFFSET, MAIN_DATA.DC);
                for (line_num=0; line_num<MAIN_DATA.IC; ++line_num)
                {
                        write_file_int(f, CODE_SECTION_OFFSET+line_num, MAIN_DATA.CODE_SECTION[line_num]);
                }
                for (line_num=0; line_num<MAIN_DATA.DC; ++line_num)
                {
                        write_file_int(f, CODE_SECTION_OFFSET+MAIN_DATA.IC+line_num, MAIN_DATA.DATA_SECTION[line_num]);
                }
	        fclose(f);
        }
}

/* write accumulated info to .ext and .ent files */
void write_e_file(char *file_prefix, char *file_extension, e_file_line* array[], int array_size)
{
        FILE *f;
        int line_num;
        e_file_line *line;
        char full_name[MAX_FILE_NAME_SIZE];
        sprintf(full_name, "%s.%s", file_prefix, file_extension);

	if (array_size == 0)
	{
		printf("Nothing to write to %s so not creating this file\n", full_name);
		remove(full_name);
		return;	
	}

        if ((f = open_file(full_name, "w")))
        {
		printf("Writing to %s\n", full_name);
                for (line_num = 0; line_num < array_size; ++line_num)
                {
                        line = array[line_num];
                        write_file_line(f, line->label, line->address);
                }
        }
        fclose(f);
}

/* write .ent file */
void write_entry_file(char *file_prefix)
{
        write_e_file(file_prefix, "ent", MAIN_DATA.ENTRY_SECTION, MAIN_DATA.YC);
}

/* write .ext file */
void write_extern_file(char *file_prefix)
{
        write_e_file(file_prefix, "ext", MAIN_DATA.EXTERN_SECTION, MAIN_DATA.XC);
}

/* write the output files */
void write_data_to_files(char *file_prefix)
{
        write_object_file(file_prefix);
        write_entry_file(file_prefix);
        write_extern_file(file_prefix);
}

/* delete old output files (if exist) */
void clear_output_files(char *file_prefix)
{
	char* suffixes[] = {"ob", "ent", "ext"};
	int i;
        char full_name[MAX_FILE_NAME_SIZE];
	
	for (i=0; i<3; i++)
	{
	        sprintf(full_name, "%s.%s", file_prefix, suffixes[i]);
		remove(full_name);
	}
}

