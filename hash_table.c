#include <string.h>
#include <stdlib.h>
#include "hash_table.h"
#include "utils.h"

/* hash: form hash value for a string s, directly copied from the book */
unsigned int hash_code(char *s) {
	unsigned int hashval;
	for (hashval = 0; *s != '\0'; s++) {
		hashval = *s + 31 * hashval;
	}
	return hashval % HASH_TABLE_SIZE;
}

void init_hash_table(hash_table *table)
{
	int i;
	for (i=0; i<HASH_TABLE_SIZE; ++i)
	{
		table->_data[i] = NULL;
	}
}

/* hash_table_lookup: Look up an item in the hash table for key s, returns NULL if not found */
hash_table_item *hash_table_lookup(hash_table *table, char *s) {
	hash_table_item *np;
	for (np = table->_data[hash_code(s)]; np != NULL; np = np->next) 
	{
		if (strcmp(s, np->name) == 0)
		return np;  /* found*/
	}
	return NULL; /* not found */
}

hash_table_item *hash_table_add(hash_table *table, char *name, void *data) 
{
	hash_table_item *np;
	unsigned int hashval;

	if ((np = hash_table_lookup(table, name)) == NULL) 
	{  /* not found */
		np = (hash_table_item *) malloc(sizeof(hash_table_item)); /* Allocate a new item */
		if (np == NULL || (np->name = strdup(name)) == NULL)
			return NULL; /* Allocation failed */
		/* Fill in values */
		np->data = data;
		hashval = hash_code(name);
		np->next = table->_data[hashval];
		table->_data[hashval] = np;
	} else {  /* already there */
		free(np->data);   /* This is a possibility, depending on what you want */
		np->data = data;  /* update data */
	}
	return np;
}
