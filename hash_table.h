/* implementation of a hash table adapted from pages 144-145 of The C Programming Language book */

#define HASH_TABLE_SIZE 101

typedef struct hash_table_item { /* table entry */
	struct hash_table_item * next;
	char * name;
	void * data;
} hash_table_item;

typedef struct hash_table {
	hash_table_item *_data[HASH_TABLE_SIZE];
} hash_table;

void init_hash_table(hash_table *table);
void clear_hash_table(hash_table *table);
hash_table_item* hash_table_lookup(hash_table* table, char *s);
hash_table_item* hash_table_add(hash_table* table, char *name, void *data);


/* Example usage:
hash_table my_table;
hash_table *table = &my_table;
init_hash_table(table);

some_data_type *p = malloc(sizeof(some_data_type));
p->field = 123;
hash_table_add(table, "some key", p);

hash_table_item *np = hash_table_lookup(table, "some key");
  Since np->data is (void*), we need to cast is to the required type:
p = (some_data_type*) np->data;
*/
