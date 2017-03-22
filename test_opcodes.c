/* You can delete this file later, it's a test for opcodes.c + an example on how it works */
#include "opcodes.h"
#include <stdio.h>


void print_mode(int flags)
{
        if (flags & ADDRESSING_IMMEDIATE)
          printf("Immediate (0), ");
        if (flags & ADDRESSING_DIRECT)
          printf("Direct (1), ");
        if (flags & ADDRESSING_REGISTER_INDEX)
          printf("Register index (2), ");
        if (flags & ADDRESSING_REGISTER_DIRECT)
          printf("Register direct (3), ");
}

void print_valid_addressing_modes(opcode_item* opcode) {
    printf("Allowed addressing modes:\n");
    printf("\tSource: ");
    if (opcode->addressing_mode.src == 0) {
        printf("No source operand.");
    } else {
        print_mode(opcode->addressing_mode.src);
    }
    printf("\n\tDestination: ");
    if (opcode->addressing_mode.dst == 0) {
        printf("No destination operand.");
    } else {
        print_mode(opcode->addressing_mode.dst);
    }
    printf("\n");
}


int main() {
    char lookup_name[10];
    opcode_item *result;
    int opcodes = opcode_table_init();
    printf("\nLoaded %d opcodes\n", opcodes);
    if (opcodes == 0) {
        fprintf(stderr, "Allocation failure!");
        return 1;
    }
    printf("Enter an operation name to look up: ");
    scanf("%s", lookup_name);
    result = opcode_lookup(lookup_name);
    if (result == NULL) {
        printf("\nNot found!\n");
    } else {
        printf("\nFound!\nopcode: %d\ngroup: %d\n",
               result->opcode,
               result->group);
        print_valid_addressing_modes(result);
    }
    return 0;
}
