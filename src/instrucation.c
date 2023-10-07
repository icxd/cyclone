#include <stdlib.h>

#include "instrucation.h"

Instruction *instruction_new(Opcode opcode, Value operand1, Value operand2) {
    Instruction *instruction = malloc(sizeof(Instruction));
    instruction->opcode = opcode;
    instruction->operand1 = operand1;
    instruction->operand2 = operand2;
    return instruction;
}

void instruction_free(Instruction *instruction) { free(instruction); }