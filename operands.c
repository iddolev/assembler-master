#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "operands.h"
#include "defs.h"
#include "opcodes.h"
#include "utils.h"

/* Check if a string is a valid label operand */
/* I uncommented this so I can actually test this code, but this should be removed later after I change is_label in instructions.c to fit the purpose */
int is_label_operand(char* s) {
    enum {BEGIN, IN, END} state = BEGIN;
    int len = 0;
    while (*s != '\0') {
        switch (state) {
            case BEGIN:
                if (*s != ' ' && *s != '\t') { /* ignore whitespace at start */
                    if (isalpha(*s)) { /* must start with a letter */
                        state = IN;
                        len++; /* we should also increase the length at the first character of the label */
                    } else {
                        return 0; /* starts with something that isn't a letter = invalid */
                    }
                }
            break;
            case IN:
                if (isalpha(*s) || isdigit(*s)) /* only valid characters for label */
                    len++;
                else if (*s == ' ' && *s == '\t') /* space at the end */
                    state = END;
            break;
            case END:
                if (*s != ' ' && *s != '\t') /* anything after the space? invalid */
                    return 0;
            break;
        }
        s++;
    }
    if (len > MAX_SYMBOL_SIZE)
        return 0; /* too long */
    return 1; /* if we got here, it's valid */
}

/* Check if string *s is a valid immediate. Return 1 if it is, or 0 if it's not */
int check_immediate(char * s)
{
    int i = 0;
    if (s[i] == '#') /*start of absolut addressing method*/
    {
        i++;
        if (s[i] == '+' || s[i] == '-' || isdigit(s[i])) /*something like '#-1'*/
        {
            i++;
            while(isdigit(s[i]))
            {
              i++;
            }
            if (s[i] == '\0') /*if it stoped because it reached the end it is legal*/
            {
              return 1;
            }
        }
    }
    /*otherwise it stoped because of an illegal character*/
    return 0;
}

/*checks for the addressing method of the operands*/
ADR_METHOD find_addressing_method(char *s)
{
    /* TODO: ignore space at the end */
    int len = strlen(s);
    if (len > 1)
    {
        if (check_immediate(s) == 1)
        {
            return IMMEDIATE;
        }
        if(is_register(s))/*if the string is something like 'r1' (because it stops collecting characters when it reaches ',')*/
            return D_REGISTER;
        if (len > 5)
        {
            if (s[0] == 'r' && s[2] == '[' && s[3] == 'r' && s[5] ==']' ) /*if the string is something like 'r1[r2]' (it is considered 1 operand therefore it hasn't stop collecting at '[')*/
            {
                if (c_atoi(s[1]) % 2 != 0) /*if the first register is an odd number */
                {
                    if (c_atoi(s[4]) % 2 == 0) /* and the second register is even */
                        return INDEX;
                }
            }
        }
    }
    if (is_label_operand(s))
        return DIRECT;
    /*otherwise it is illegal*/
    return ILLEGAL_OPERAND;
}

/* Check if an addressing method is valid for an operand, using the mode flags from opcode_item */
int valid_method_for_operand(int modeflags, ADR_METHOD method) {
    switch (method) {
        case IMMEDIATE:
            return modeflags & ADDRESSING_IMMEDIATE;
        break;
        case DIRECT:
            return modeflags & ADDRESSING_DIRECT;
        break;
        case D_REGISTER:
             return modeflags & ADDRESSING_REGISTER_DIRECT;
        break;
        case INDEX:
             return modeflags & ADDRESSING_REGISTER_INDEX;
        break;
        case ILLEGAL_OPERAND:
            return 0; /* It's unlikely that this will ever be reached, but it silences a compiler warning */
        break;
    }
    return 0; /* Just in case "method" was passed a value not in the ADR_METHOD enum */
}

/* Parse operands.
 * Returns 0 on failure, or the number of found operands on success.
 * Puts the parsed result in the parsed[] array passed as the first parameter. */
int parse_operands(parsed_operand parsed[], char *operands, int line_num) {
    char word[MAX_LINE_LENGTH];
    int operands_count = 0;
    while (*operands !='\n' && *operands != '\0') {
        operands = getNextToken(operands, word);
        if (*operands == ',' && strlen(word) == 0) {
            operands++;
            operands_count++;
            if (operands_count > 2) {
                fprintf(stderr, "Line %d: Unexpected comma, no operation has more than two operands\n", line_num);
                return 0;
            }
        } else if (*operands != ',' && *operands != '\n' && *operands != '\0') {
            fprintf(stderr, "Line %d: Unexpected token\n", line_num);
            return 0;
        } else {
            parsed_operand operand;
            operand.addressing_method = find_addressing_method(word);
            if (operand.addressing_method == ILLEGAL_OPERAND) {
                fprintf(stderr, "Line %d: invalid operand %s", line_num, word);
                return 0;
            }
            operand.text_value = strdup(word);
            /* TODO: populate operand.encoded */
            parsed[operands_count] = operand;
        }
    }
    operands_count++;
    return operands_count;
}

/* Validate addressing methods for operands */
int validate_operand_methods(opcode_item *opcode, parsed_operand operands[], int operand_count, int line_num) {
    if (operand_count == 1) {
        /* Only one operand, so it's dst */
        if (valid_method_for_operand(opcode->addressing_mode.dst, operands[0].addressing_method))
            return 1;
        else {
            fprintf(stderr, "Line %d: Invalid addressing method for operand %s", line_num, operands[0].text_value);
            return 0;
        }
    } else {
        /* Two operands, validate both src and dst */
        if (!valid_method_for_operand(opcode->addressing_mode.src, operands[0].addressing_method)) {
            fprintf(stderr, "Line %d: Invalid addressing method for operand %s", line_num, operands[0].text_value);
            return 0;
        }
        if (!valid_method_for_operand(opcode->addressing_mode.dst, operands[1].addressing_method)) {
            fprintf(stderr, "Line %d: Invalid addressing method for operand %s", line_num, operands[1].text_value);
            return 0;
        }
        return 1;
    }
}

/* Checks if a line is a legal operation line. Returns the length of the encoded line in processor words (either 1, 2 or 3) or 0 on error */
int check_operation(char *opcode_word, char* operands, int line_num)
{
    opcode_item* opcode_data;
    if ((opcode_data = opcode_lookup(opcode_word)) != NULL)
    {
        parsed_operand parsed[MAX_OPERANDS];
        int operands_count;
        if (strlen(operands) == 0) {
            /* No operands given */
            if (opcode_data->group == 0) { /* group 0: no operands */
                return 1; /* no operands, but it's okay. it'll use 1 word. */
            } else {
                fprintf(stderr, "Line %d: Expected %d operands for %s, got 0\n",
                        line_num, opcode_data->group, opcode_data->name);
                return 0; /* Not okay */
            }
        } else {
            if ((operands_count = parse_operands(parsed, operands, line_num))) {
                /* Parsed successfully */
                printf("Found %d operands\n", operands_count);
                if (operands_count != opcode_data->group) {
                    /* "group" is also the number of required operands */
                    fprintf(stderr, "Line %d: Expected %d operands for %s, got %d\n",
                            line_num, opcode_data->group, opcode_data->name, operands_count);
                    return 0; /* Not okay */
                } else {
                    /* Now figure out size in words: */
                    int words = 2; /* At least one word will be used if we got here, since there's always an operand */
                    if (!validate_operand_methods(opcode_data, parsed, operands_count, line_num)) {
                        return 0; /* addressing method validation fail */
                    }
                    if (operands_count > 1) {
                        switch (parsed[1].addressing_method) {
                            case IMMEDIATE:
                            case DIRECT:
                            case INDEX:
                                words++; /* These three always mean an extra word */
                            break;
                            case D_REGISTER:
                                if(parsed[0].addressing_method != D_REGISTER)
                                    words++; /* If both operands are registers, they share a word. If not, we need an extra one */
                            break;
                            case ILLEGAL_OPERAND:
                                /* Do nothing. This is here to silence a compiler warning about checking all enum options in the switch
                                 * and cannot be reached, since if it's illegal we'd return much earlier */
                            break;
                        }
                    }
                    return words;
                }
            } else {
                return 0; /* parsing error */
            }
        }
    }
    else
    {
        printf("Line %d: invalid operation '%s'\n", line_num, opcode_word);
        return 0;
    }
}

/*checks if the operand is a register*/
int is_register(char* s) {
    if(strlen(s) != 2)
    { /* register name must have 2 characters*/
      return 0;
    }
    else if(s[0] == 'r' && isdigit(s[1]))
    {
      if(c_atoi(s[1]) < 8)
        return 1;
    }
    return 0;
}
