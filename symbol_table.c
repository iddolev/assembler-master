#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "hash_table.h"
#include "utils.h"

static hash_table symbol_table;

void symbol_table_init()
{
	init_hash_table(&symbol_table);
}

void symbol_table_clear()
{
	clear_hash_table(&symbol_table);
}

/* symbol_table_lookup: Look up data in the symbol table for key s, returns NULL if not found */
symbol_table_data *symbol_table_lookup(char *s) 
{
	hash_table_item *np = hash_table_lookup(&symbol_table, s);
	if (np)
	{
		return (symbol_table_data*) np->data;
	}
	return NULL; /* not found */
}

int symbol_table_add_and_verify(char *name, int address, char is_code, char is_extern, int line_num)
{
	int ret = symbol_table_add(name, address, is_code, is_extern);
	if (ret == 0)
	{
		printf("Symbol %s in line %d already appeared previously\n", name, line_num);
		return 0; 
	}
	if (ret == -1)
		return 0;  /* memory problem */
	return 1;
}

/* symbol_table_add: Add an item to the symbol table. 
  If the item already exists, return error 0;
  If memory problem, return error -1; */
int symbol_table_add(char *name, int address, char is_code, char is_extern) 
{
	symbol_table_data *data;

	if (symbol_table_lookup(name))
	{
		return 0;
	}
	data = (symbol_table_data*) malloc(sizeof(symbol_table_data));
	if (!data)
	{
		printf ("Memory problem in symbol table\n");
		return -1;
	}
        data->address = address;
        data->is_code = is_code;
        data->is_extern = is_extern;
	if (!hash_table_add(&symbol_table, name, data))
	{
		printf ("Memory problem in symbol table\n");
		free(data);
		return -1;
	}
	return 1;
}

/* for debugging */
void print_symbol_table()
{
	int i;
	symbol_table_data* data;

	for (i=0; i<HASH_TABLE_SIZE; ++i)
	{
		hash_table_item *np = symbol_table._data[i];
		if (np)
		{
			printf("cell [%d]:\n", i);
			while (np)
			{
				data = (symbol_table_data *) np->data;
				printf("%s: %d %d %d\n", np->name, data->address, data->is_code, data->is_extern);
				np = np->next;
			}
		}
	}
}


