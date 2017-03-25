/* You can delete this file later, it's a test for operands.c + an example on how it works */
#include "operands.h"
#include "opcodes.h"
#include "defs.h"
#include "utils.h"
#include <stdio.h>

int main() {
    char buffer[MAX_LINE_LENGTH+1];
    char buffer2[MAX_LINE_LENGTH+1];
    char *buffer3;
    char c;
    int i;
    int words;
    int opcodes = opcode_table_init();
    printf("\nLoaded %d opcodes\n", opcodes);
    if (opcodes == 0) {
        fprintf(stderr, "Allocation failure!\n");
        return 1;
    }
    printf("Enter a command line: ");
    while ((c=getc(stdin)) != '\n') {
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\n';
    buffer[i++] = '\0';
    printf("\nRead:%s\n", buffer);
    buffer3 = getNextToken(buffer, buffer2);
    words = check_operation(buffer2, buffer3, 0);
    if (words > 0)
        printf("Result: %d proccessor words will be used by this command\n", words);
    else
        printf("An error occured.\n");
    return 0;
}
