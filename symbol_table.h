typedef struct symbol_table_data { /* table entry */
    int address;
    char is_code;     /* boolean for whether the symbol is in the code section (1) or in the data section (0) */
    char is_extern;      /* boolean for whether the symbol is extern */
} symbol_table_data;

/* initializes the symbol table */
void symbol_table_init();
/* returns the symbol data for s, if exists in the table, else NULL */
symbol_table_data* symbol_table_lookup(char *s);
/* returns 1 if successful, else 0 */
int symbol_table_add(char *name, int address, char is_code, char is_extern);

/* clear symbol table */
void symbol_table_clear();  

/* for debugging */
void print_symbol_table();



