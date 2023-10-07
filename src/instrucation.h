#ifndef _CYCLONE_INSTRUCTION_H
#define _CYCLONE_INSTRUCTION_H

#include "common.h"
#include "vector.h"

typedef u8 Opcode;
enum {
    // clang-format off
    OP_NOP = 0x00,
    OP_MOV,
    OP_PUSH, OP_POP,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_JMP, OP_JE, OP_JNE, OP_JG, OP_JGE, OP_JL, OP_JLE,
    OP_CMP,
    OP_CALL, OP_RET,
    OP_HLT = 0xFF,
    // clang-format on
};

#define NOP() instruction_new(OP_NOP, NONE_VALUE, NONE_VALUE)
#define MOV(a, b) instruction_new(OP_MOV, a, b)
#define PUSH(a) instruction_new(OP_PUSH, a, NONE_VALUE)
#define POP(a) instruction_new(OP_POP, a, NONE_VALUE)
#define ADD(a, b) instruction_new(OP_ADD, a, b)
#define SUB(a, b) instruction_new(OP_SUB, a, b)
#define MUL(a, b) instruction_new(OP_MUL, a, b)
#define DIV(a, b) instruction_new(OP_DIV, a, b)
#define MOD(a, b) instruction_new(OP_MOD, a, b)
#define JMP(a) instruction_new(OP_JMP, a, NONE_VALUE)
#define JE(a) instruction_new(OP_JE, a, NONE_VALUE)
#define JNE(a) instruction_new(OP_JNE, a, NONE_VALUE)
#define JG(a) instruction_new(OP_JG, a, NONE_VALUE)
#define JGE(a) instruction_new(OP_JGE, a, NONE_VALUE)
#define JL(a) instruction_new(OP_JL, a, NONE_VALUE)
#define JLE(a) instruction_new(OP_JLE, a, NONE_VALUE)
#define CMP(a, b) instruction_new(OP_CMP, a, b)
#define CALL(a) instruction_new(OP_CALL, a, NONE_VALUE)
#define RET() instruction_new(OP_RET, NONE_VALUE, NONE_VALUE)
#define HLT() instruction_new(OP_HLT, NONE_VALUE, NONE_VALUE)

typedef u8 Register;
enum {
    // clang-format off
    REG_RAX, REG_RBX, REG_RCX, REG_RDX,
    REG_RSI, REG_RDI, REG_RBP, REG_RSP,
    REG_R8,  REG_R9,  REG_R10, REG_R11,
    REG_R12, REG_R13, REG_R14, REG_R15,
    // clang-format on

    REG_COUNT,
};

typedef struct {
    enum { VALUE_NONE, VALUE_REGISTER, VALUE_IMMEDIATE, VALUE_LABEL } type;
    union {
        Register reg;
        double imm;
        string label;
    } data;
} Value;

static inline char *value_to_string(Value value) {
    switch (value.type) {
    case VALUE_NONE:
        return "none";
    case VALUE_REGISTER:
        return "register";
    case VALUE_IMMEDIATE:
        return "immediate";
    case VALUE_LABEL:
        return "label";
    }
    return "unknown";
}

#define NONE_VALUE ((Value){.type = VALUE_NONE})
#define REG_VALUE(x) ((Value){.type = VALUE_REGISTER, .data.reg = x})
#define IMM_VALUE(x) ((Value){.type = VALUE_IMMEDIATE, .data.imm = x})
#define LABEL_VALUE(x) ((Value){.type = VALUE_LABEL, .data.label = SV(x)})

#define AS_REGISTER(x) ((x).data.reg)
#define AS_IMMEDIATE(x) ((x).data.imm)
#define AS_LABEL(x) ((x).data.label)

typedef struct {
    Opcode opcode;
    Value operand1, operand2;
} Instruction;

Instruction *instruction_new(Opcode opcode, Value operand1, Value operand2);
void instruction_free(Instruction *instruction);

#endif // _CYCLONE_INSTRUCTION_H