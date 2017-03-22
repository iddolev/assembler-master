#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "write_data.h"
#include "externs.h"
#include "utils.h"

void write_file_line(FILE *f, char *label, int address)
{
        fprintf(f, "%s\t%02X\n", label, address);
}

void write_file_int(FILE *f, int address, int value)
{
        fprintf(f, "%02X\t%04X\n", address, value);
        /* to do - need to improve this function to handle negative value,
because we should use only 15 bits */
}

void write_object_file(char *file_prefix)
{
        FILE *f;
        int line_num;
        char full_name[MAX_FILE_NAME_SIZE];
        sprintf(full_name, "%s.ob", file_prefix);

        if ((f = open_file(full_name, "w")))
        {
                for (line_num=0; line_num<MAIN_DATA.IC; ++line_num)
                {
                        write_file_int(f, CODE_SECTION_OFFSET+line_num, MAIN_DATA.CODE_SECTION[line_num]);
                }
                for (line_num=0; line_num<MAIN_DATA.DC; ++line_num)
                {
                        write_file_int(f, CODE_SECTION_OFFSET+MAIN_DATA.IC+line_num, MAIN_DATA.DATA_SECTION[line_num]);
                }
        }
        fclose(f);
}

void write_e_file(char *file_prefix, char *file_extension,
e_file_line* array[], int array_size)
{
        FILE *f;
        int line_num;
        e_file_line *line;
        char full_name[MAX_FILE_NAME_SIZE];
        sprintf(full_name, "%s.%s", file_prefix, file_extension);

        if ((f = open_file(full_name, "w")))
        {
                for (line_num = 0; line_num < array_size; ++line_num)
                {
                        line = array[line_num];
                        write_file_line(f, line->label, line->address);
                }
        }
        fclose(f);
}

void write_entry_file(char *file_prefix)
{
        write_e_file(file_prefix, "ent", MAIN_DATA.ENTRY_SECTION, MAIN_DATA.YC);
}

void write_extern_file(char *file_prefix)
{
        write_e_file(file_prefix, "ext", MAIN_DATA.EXTERN_SECTION, MAIN_DATA.XC);
}

void write_data_to_files(char *file_prefix)
{
        write_object_file(file_prefix);
        write_entry_file(file_prefix);
        write_extern_file(file_prefix);
}
