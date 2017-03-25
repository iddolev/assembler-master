#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"
#include "hash_table.h"
#include "utils.h"

static hash_table opcode_table;

/* opcode_lookup: Look up an operation in the opcode table with the name "s", returns NULL if not found */
opcode_item *opcode_lookup(char *s)
{
	hash_table_item *np = hash_table_lookup(&opcode_table, s);
	if (np) 
	{
		opcode_item* item = (opcode_item*) np->data;
		item->name = np->name; /* Do this in the lookup to avoid having to keep more than 1 copy of the name in memory */
		return item;
	}
	return NULL; /* not found */
}

/* opcode_add: Add an item to the opcode table. 
  If the item already exists, fail and return 0. */
int opcode_add(char *name,
               unsigned char opcode,
               unsigned char group,
               unsigned char addressing_mode_src,
               unsigned char addressing_mode_dst) 
{
	opcode_item *data;
	if (opcode_lookup(name))
	{
		printf("Error: symbol '%s' is already in opcode table\n", name);
		return 0;
	}
	data = (opcode_item*) malloc(sizeof(opcode_item));
	if (!data) 
	{
		return 0;
	}
	data->opcode = opcode;
	data->group = group;
	data->addressing_mode.src = addressing_mode_src;
	data->addressing_mode.dst = addressing_mode_dst;
	if (!hash_table_add(&opcode_table, name, data))
	{
		free(data);
		return 0;
	}
	return 1;
}

/* opcode_table_init: Add all the opcodes to the opcode table. Returns 0 on failure, or number of opcodes on success */
int opcode_table_init() 
{
	int allocated = 0; /* Counter for how many opcodes were allocated, used for error handling*/
	init_hash_table(&opcode_table);

	/* add all the opcodes */
	/*                      name, code, group, src addressing, dst addressing */
	allocated += opcode_add("mov",  0,  2, ADDRESSING_ALL,  ADDRESSING_DATA);
	allocated += opcode_add("cmp",  1,  2, ADDRESSING_ALL,  ADDRESSING_ALL);
	allocated += opcode_add("add",  2,  2, ADDRESSING_ALL,  ADDRESSING_DATA);
	allocated += opcode_add("sub",  3,  2, ADDRESSING_ALL,  ADDRESSING_DATA);
	allocated += opcode_add("not",  4,  1, ADDRESSING_NONE, ADDRESSING_DATA);
	allocated += opcode_add("clr",  5,  1, ADDRESSING_NONE, ADDRESSING_DATA);
	allocated += opcode_add("lea",  6,  2, ADDRESSING_DIRECT | ADDRESSING_REGISTER_INDEX, ADDRESSING_DATA);
	allocated += opcode_add("inc",  7,  1, ADDRESSING_NONE, ADDRESSING_DATA);
	allocated += opcode_add("dec",  8,  1, ADDRESSING_NONE, ADDRESSING_DATA);
	allocated += opcode_add("jmp",  9,  1, ADDRESSING_NONE, ADDRESSING_DATA);
	allocated += opcode_add("bne",  10, 1, ADDRESSING_NONE, ADDRESSING_DATA);
	allocated += opcode_add("red",  11, 1, ADDRESSING_NONE, ADDRESSING_DATA);
	allocated += opcode_add("prn",  12, 1, ADDRESSING_NONE, ADDRESSING_ALL);
	allocated += opcode_add("jsr",  13, 1, ADDRESSING_NONE, ADDRESSING_DATA);
	allocated += opcode_add("rts",  14, 0, ADDRESSING_NONE, ADDRESSING_NONE);
	allocated += opcode_add("stop", 15, 0, ADDRESSING_NONE, ADDRESSING_NONE);
	if (allocated != 16)
		allocated = 0;
	return allocated;
}

