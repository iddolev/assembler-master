typedef struct symbol_table_data { /* table entry */
	int address;
	char is_code;     /* boolean for whether the symbol is in the code section (1) or in the data section (0) */
	char is_extern;      /* boolean for whether the symbol is extern */
	char was_used;       /* after writing an .entry information to .ent file, this is set to 1, 
				so we won't write the information twice in case the same .entry statement is encountered */
} symbol_table_data;

/* initializes the symbol table */
void symbol_table_init();
/* clears the symbol table */
void symbol_table_clear();
/* returns the symbol data for s, if exists in the table, else NULL */
symbol_table_data* symbol_table_lookup(char *s);
/* returns 1 if successful, else 0 (already exist), or -1 (if memory problem) */
int symbol_table_add(char *name, int address, char is_code, char is_extern);
/* Check if symbol already in the table, and if so, issue error. Otherwise, call symbol_table_add */
int symbol_table_add_and_verify(char *name, int address, char is_code, char is_extern, int line_num);

/* clear symbol table */
void symbol_table_clear();  

/* for debugging */
void print_symbol_table();



