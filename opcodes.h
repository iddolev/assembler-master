/* Note: These are flags for the allowed mode, not the actual encoding of the mode */
#define ADDRESSING_NONE 0
#define ADDRESSING_IMMEDIATE 0x01
#define ADDRESSING_DIRECT 0x02
#define ADDRESSING_REGISTER_INDEX 0x04
#define ADDRESSING_REGISTER_DIRECT 0x08
#define ADDRESSING_ALL ADDRESSING_IMMEDIATE | ADDRESSING_DIRECT | ADDRESSING_REGISTER_INDEX | ADDRESSING_REGISTER_DIRECT
/* ADDRESSING_DATA = memory and registers, but not immediate*/
#define ADDRESSING_DATA ADDRESSING_DIRECT | ADDRESSING_REGISTER_INDEX | ADDRESSING_REGISTER_DIRECT

typedef struct opcode_item { /* table entry */
    char* name;
    unsigned int opcode: 4; /* 4 bits for the opcode, since there are only 16 */
    unsigned int group: 2; /* 2 bits for the group, since there are only 3 */
    struct {
        unsigned int src: 4; /* flags for allowed addressing mode of the source register */
        unsigned int dst: 4; /* flags for allowed addressing mode of the destination register */
    } addressing_mode;
} opcode_item;

opcode_item* opcode_lookup(char *s); /* Find an operation in the opcodes table. Returns NULL when not found */
int opcode_table_init(); /* Initialize the opcode table. Must be called before lookup */
