#include "symbol_table.h"
#include <stdio.h>

int main() {
    int success;
    symbol_table_data *np = NULL;
    printf("Testing the symbol table implementation:\n");
    np = symbol_table_lookup("foo");
    printf("Lookup: %lu (should be 0 / NULL)\n", (long)np);
    np = NULL;
    success = symbol_table_add("foo", 100, 0, 0);
    printf("Insertion: %d (should be 1=success)\n", success);
    np = symbol_table_lookup("foo");
    printf("Insertion: %d %d %d (should be 100 0 0)\n", np->address, np->is_code, np->is_extern);
    np = NULL;
    success = symbol_table_add("foo", 200, 1, 1);
    printf("Replacement: %d (should be 0=already there)\n", success);
    np = symbol_table_lookup("foo");
    printf("Lookup after replacement: %d %d %d (should be 100 0 0)\n", np->address, np->is_code, np->is_extern);
    return 0;
}
