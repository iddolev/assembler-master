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

/* symbol_table_add: Add an item to the symbol table. 
  If the item already exists, its value will be overwritten. 
  returns 0 on failure */
int symbol_table_add(char *name, int address, char is_code, char is_extern) 
{
	symbol_table_data *data = (symbol_table_data*) malloc(sizeof(symbol_table_data));
	if (!data)
	{
		return 0;
	}
        data->address = address;
        data->is_code = is_code;
        data->is_extern = is_extern;
	if (!hash_table_add(&symbol_table, name, data))
	{
		free(data);
		return 0;
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

