#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct data{
	int value;
	char str[10];
} data;

int main() {
    data *d;
    hash_table htable;
    hash_table* table = &htable;
    hash_table_item *np = NULL;

    init_hash_table(table);

    printf("Testing the symbol table implementation:\n");
    np = hash_table_lookup(table, "foo");
    printf("Lookup: %lu (should be 0 / NULL)\n", (long)np);
    np = NULL;

    d = (data*) malloc(sizeof(data));
    d->value = 5;
    strcpy(d->str, "abc");
    np = hash_table_add(table, "foo", d);
    d = (data*) np->data;
    printf("Insertion: %d %s (should be 5 abc)\n", d->value, d->str);
    np = hash_table_lookup(table, "foo");
    d = (data*) np->data;
    printf("Lookup after insertion: %d %s (should be 5 abc)\n", d->value, d->str);
    np = NULL;
    d = (data*) malloc(sizeof(data));
    d->value = 6;
    strcpy(d->str, "def");
    np = hash_table_add(table, "foo", d);
    printf("Lookup should fail: np==NULL? %d\n", (np==NULL));
    return 0;
}
