#include <stdlib.h>

#include "compiler.h"
#include "instrucation.h"

Compiler *compiler_new(CompilationTarget target) {
    Compiler *compiler = malloc(sizeof(*compiler));
    compiler->target = target;
    vec_init(&compiler->file_ids);
    vec_init(&compiler->files);
    vec_init(&compiler->file_names);
    compiler->ip = 0;
    return compiler;
}

void compiler_free(Compiler *compiler) {
    vec_free(&compiler->file_ids);
    vec_free(&compiler->files);
    vec_free(&compiler->file_names);
    free(compiler);
}

u32 compiler_new_file(Compiler *compiler, File *file) {
    u32 id = compiler->files.size;
    vec_push(&compiler->file_ids, id);
    vec_push(&compiler->files, file);
    vec_push(&compiler->file_names, file->path);
    return id;
}

static string compiler_value_to_string(Value value) {
    switch (value.type) {
    case VALUE_NONE:
        return SV("none");
    case VALUE_REGISTER: {
        switch (value.data.reg) {
        case REG_RAX:
            return SV("rax");
        case REG_RBX:
            return SV("rbx");
        case REG_RCX:
            return SV("rcx");
        case REG_RDX:
            return SV("rdx");
        case REG_RSI:
            return SV("rsi");
        case REG_RDI:
            return SV("rdi");
        case REG_RBP:
            return SV("rbp");
        case REG_RSP:
            return SV("rsp");
        case REG_R8:
            return SV("r8");
        case REG_R9:
            return SV("r9");
        case REG_R10:
            return SV("r10");
        case REG_R11:
            return SV("r11");
        case REG_R12:
            return SV("r12");
        case REG_R13:
            return SV("r13");
        case REG_R14:
            return SV("r14");
        case REG_R15:
            return SV("r15");
        }
    } break;
    case VALUE_IMMEDIATE: {
        return sv_from_fmt("0x%x", value.data.imm);
    } break;
    case VALUE_LABEL:
        return value.data.label;
    }
    return SV("<unknown>");
}

string compiler_compile_x86_64(Compiler *compiler, VirtualMachine *vm) {
    string result = SV("");
    for (;;) {
        Label *label = vm_find_label_by_ip(vm, compiler->ip);
        if (label != NULL) {
            string label_str = sv_from_fmt(SV_FMT ":\n", SV_ARG(label->name));
            result = sv_concat(result, label_str);
        }

        Instruction *instr = *vec_get(&vm->instructions, compiler->ip);
        switch (instr->opcode) {
        case OP_NOP: {
            compiler->ip++;
        } break;
        case OP_MOV: {
            Value operand1 = instr->operand1;
            Value operand2 = instr->operand2;
            string operand1_str = compiler_value_to_string(operand1);
            string operand2_str = compiler_value_to_string(operand2);
            string instr_str = SV("    mov ");
            instr_str = sv_concat(instr_str, operand1_str);
            instr_str = sv_concat(instr_str, SV(", "));
            instr_str = sv_concat(instr_str, operand2_str);
            instr_str = sv_concat(instr_str, SV("\n"));
            result = sv_concat(result, instr_str);
            compiler->ip++;
        } break;
        case OP_JMP: {
            Value operand1 = instr->operand1;
            string operand1_str = compiler_value_to_string(operand1);
            string instr_str = SV("    jmp ");
            instr_str = sv_concat(instr_str, operand1_str);
            instr_str = sv_concat(instr_str, SV("\n"));
            result = sv_concat(result, instr_str);
            compiler->ip++;
        } break;
        case OP_HLT: {
            compiler->ip++;
            result = sv_concat(result, SV("    hlt\n"));
            return result;
        } break;
        }
    }
    return result;
}