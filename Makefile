all: utils.o symbol_table.o stack.o operands.o instructions.o
# TODO: add main and such, and then change "all" to be "main"
.PHONY : all

utils.o: utils.c utils.h
	gcc -ansi -Wall -Werror -pedantic -c utils.c -o utils.o

symbol_table.o: utils.o symbol_table.h symbol_table.c
	gcc -ansi -Wall -Werror -pedantic -c symbol_table.c -o symbol_table.o

opcodes.o: utils.o opcodes.h symbol_table.h
	gcc -ansi -Wall -Werror -pedantic -c opcodes.c -o opcodes.o

stack.o: stack.c defs.h
	gcc -ansi -Wall -Werror -pedantic -c stack.c -o stack.o

operands.o: utils.o opcodes.o operands.c defs.h
	gcc -ansi -Wall -Werror -pedantic -c operands.c -o operands.o

instructions.o: utils.o opcodes.o operands.o defs.h instructions.h instructions.c
	gcc -ansi -Wall -Werror -pedantic -c instructions.c -o instructions.o

# test_symbol_table: utils.o symbol_table.o test_symbol_table.c
#  	gcc -ansi -Wall -Werror -pedantic -c test_symbol_table.c -o test_symbol_table.o
#  	gcc -ansi -Wall -Werror -pedantic -g utils.o symbol_table.o test_symbol_table.o -o test_symbol_table

