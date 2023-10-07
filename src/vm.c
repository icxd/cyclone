#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

Label label_new(VirtualMachine *vm, string name, usize ip) {
    Label label;
    label.label_id = vec_len(&vm->labels);
    label.name = name;
    label.ip = ip;
    return label;
}

VirtualMachine *vm_new(void) {
    VirtualMachine *vm = malloc(sizeof(VirtualMachine));
    vm->state = VM_STATE_RUNNING;
    vec_init(&vm->instructions);
    for (int i = 0; i < REG_COUNT; i++)
        vm->registers[i] = NONE_VALUE;
    vec_init(&vm->labels);
    vec_init(&vm->stack);
    vm->sp = 0;
    vm->bp = 0;
    vm->ip = 0;
    return vm;
}

void vm_free(VirtualMachine *vm) {
    for (usize i = 0; i < vec_len(&vm->instructions); i++)
        instruction_free(*vec_get(&vm->instructions, i));
    vec_free(&vm->instructions);
    free(vm);
}

Value vm_resolve_value(VirtualMachine *vm, Value value) {
    switch (value.type) {
    case VALUE_NONE: {
        return NONE_VALUE;
    }
    case VALUE_REGISTER: {
        return vm->registers[value.data.reg];
    }
    case VALUE_IMMEDIATE: {
        return value;
    }
    case VALUE_LABEL: {
        Label *label = vm_find_label_by_name(vm, value.data.label);
        if (label) {
            return IMM_VALUE(label->ip);
        }
    }
    }
    return NONE_VALUE;
}

static void vm_dump_value(Value value) {
    switch (value.type) {
    case VALUE_NONE: {
        printf("none");
    } break;
    case VALUE_REGISTER: {
        printf("r%d", value.data.reg);
    } break;
    case VALUE_IMMEDIATE: {
        printf("%f", value.data.imm);
    } break;
    case VALUE_LABEL: {
        printf(SV_FMT, SV_ARG(value.data.label));
    } break;
    }
}

void vm_execute(VirtualMachine *vm) {
    for (;;) {
        if (vm->ip >= vec_len(&vm->instructions))
            return;
        if (vm->state != VM_STATE_RUNNING)
            return;
        Instruction *instr = *vec_get(&vm->instructions, vm->ip);
        switch (instr->opcode) {
        case OP_NOP: {
#ifdef VM_DEBUG
            printf("NOP\n");
#endif
            vm->ip++;
        } break;
        case OP_MOV: {
            Value operand1 = instr->operand1;
            Value operand2 = instr->operand2;
            VM_ASSERT(vm, operand1.type == VALUE_REGISTER,
                      "Expected register, got %s", value_to_string(operand1));
            VM_ASSERT(vm,
                      operand2.type == VALUE_REGISTER ||
                          operand2.type == VALUE_IMMEDIATE,
                      "Expected register or immediate, got %s",
                      value_to_string(operand2));

            operand2 = vm_resolve_value(vm, operand2);
            VM_ASSERT(vm, operand2.type != VALUE_NONE, "Invalid value, got %s",
                      value_to_string(operand2));

            vm->registers[operand1.data.reg] = operand2;
#ifdef VM_DEBUG
            printf("MOV ");
            vm_dump_value(operand1);
            printf(", ");
            vm_dump_value(operand2);
            printf("\n");
#endif
            vm->ip++;
        } break;
        case OP_PUSH: {
            Value operand1 = instr->operand1;
            VM_ASSERT(vm,
                      operand1.type == VALUE_REGISTER ||
                          operand1.type == VALUE_IMMEDIATE,
                      "Expected register or immediate, got %s",
                      value_to_string(operand1));

            operand1 = vm_resolve_value(vm, operand1);
            VM_ASSERT(vm, operand1.type != VALUE_NONE, "Invalid value, got %s",
                      value_to_string(operand1));

            vec_push(&vm->stack, operand1);
            vm->sp++;
#ifdef VM_DEBUG
            printf("PUSH ");
            vm_dump_value(operand1);
            printf("\n");
#endif
            vm->ip++;
        } break;
        case OP_POP: {
            Value operand1 = instr->operand1;
            VM_ASSERT(vm, operand1.type == VALUE_REGISTER,
                      "Expected register, got %s", value_to_string(operand1));
            Register reg = operand1.data.reg;
            VM_ASSERT_NOVARARG(vm, vm->sp > 0, "Stack underflow");
            vm->registers[reg] = vec_pop(&vm->stack);
            vm->sp--;
#ifdef VM_DEBUG
            printf("POP ");
            vm_dump_value(operand1);
            printf("\n");
#endif
            vm->ip++;
        } break;
        case OP_ADD: {
            Value operand1 = instr->operand1;
            Value operand2 = instr->operand2;
            VM_ASSERT(vm, operand1.type == VALUE_REGISTER,
                      "Expected register, got %s", value_to_string(operand1));
            VM_ASSERT(vm,
                      operand2.type == VALUE_REGISTER ||
                          operand2.type == VALUE_IMMEDIATE,
                      "Expected register or immediate, got %s",
                      value_to_string(operand2));

            operand2 = vm_resolve_value(vm, operand2);
            VM_ASSERT(vm, operand2.type != VALUE_NONE, "Invalid value, got %s",
                      value_to_string(operand2));

            Register reg = operand1.data.reg;
            vm->registers[reg].data.imm += operand2.data.imm;
#ifdef VM_DEBUG
            printf("ADD ");
            vm_dump_value(operand1);
            printf(", ");
            vm_dump_value(operand2);
            printf("\n");
#endif
            vm->ip++;
        } break;
        case OP_SUB: {
            Value operand1 = instr->operand1;
            Value operand2 = instr->operand2;
            VM_ASSERT(vm, operand1.type == VALUE_REGISTER,
                      "Expected register, got %s", value_to_string(operand1));
            VM_ASSERT(vm,
                      operand2.type == VALUE_REGISTER ||
                          operand2.type == VALUE_IMMEDIATE,
                      "Expected register or immediate, got %s",
                      value_to_string(operand2));

            operand2 = vm_resolve_value(vm, operand2);
            VM_ASSERT(vm, operand2.type != VALUE_NONE, "Invalid value, got %s",
                      value_to_string(operand2));

            Register reg = operand1.data.reg;
            vm->registers[reg].data.imm -= operand2.data.imm;
#ifdef VM_DEBUG
            printf("SUB ");
            vm_dump_value(operand1);
            printf(", ");
            vm_dump_value(operand2);
            printf("\n");
#endif
            vm->ip++;
        } break;
        case OP_MUL: {
            Value operand1 = instr->operand1;
            Value operand2 = instr->operand2;
            VM_ASSERT(vm, operand1.type == VALUE_REGISTER,
                      "Expected register, got %s", value_to_string(operand1));
            VM_ASSERT(vm,
                      operand2.type == VALUE_REGISTER ||
                          operand2.type == VALUE_IMMEDIATE,
                      "Expected register or immediate, got %s",
                      value_to_string(operand2));

            operand2 = vm_resolve_value(vm, operand2);
            VM_ASSERT(vm, operand2.type != VALUE_NONE, "Invalid value, got %s",
                      value_to_string(operand2));

            Register reg = operand1.data.reg;
            vm->registers[reg].data.imm *= operand2.data.imm;
#ifdef VM_DEBUG
            printf("MUL ");
            vm_dump_value(operand1);
            printf(", ");
            vm_dump_value(operand2);
            printf("\n");
#endif
            vm->ip++;
        } break;
        case OP_DIV: {
            Value operand1 = instr->operand1;
            Value operand2 = instr->operand2;
            VM_ASSERT(vm, operand1.type == VALUE_REGISTER,
                      "Expected register, got %s", value_to_string(operand1));
            VM_ASSERT(vm,
                      operand2.type == VALUE_REGISTER ||
                          operand2.type == VALUE_IMMEDIATE,
                      "Expected register or immediate, got %s",
                      value_to_string(operand2));

            operand2 = vm_resolve_value(vm, operand2);
            VM_ASSERT(vm, operand2.type != VALUE_NONE, "Invalid value, got %s",
                      value_to_string(operand2));

            Register reg = operand1.data.reg;
            vm->registers[reg].data.imm /= operand2.data.imm;
#ifdef VM_DEBUG
            printf("DIV ");
            vm_dump_value(operand1);
            printf(", ");
            vm_dump_value(operand2);
            printf("\n");
#endif
            vm->ip++;
        } break;
        case OP_MOD: {
            Value operand1 = instr->operand1;
            Value operand2 = instr->operand2;
            VM_ASSERT(vm, operand1.type == VALUE_REGISTER,
                      "Expected register, got %s", value_to_string(operand1));
            VM_ASSERT(vm,
                      operand2.type == VALUE_REGISTER ||
                          operand2.type == VALUE_IMMEDIATE,
                      "Expected register or immediate, got %s",
                      value_to_string(operand2));

            operand2 = vm_resolve_value(vm, operand2);
            VM_ASSERT(vm, operand2.type != VALUE_NONE, "Invalid value, got %s",
                      value_to_string(operand2));

            Register reg = operand1.data.reg;
            double v = fmod(vm->registers[reg].data.imm, operand2.data.imm);
            vm->registers[reg].data.imm = v;
#ifdef VM_DEBUG
            printf("MOD ");
            vm_dump_value(operand1);
            printf(", ");
            vm_dump_value(operand2);
            printf("\n");
#endif
            vm->ip++;
        } break;
        case OP_JMP: {
            Value operand1 = instr->operand1;
            VM_ASSERT(vm, operand1.type == VALUE_LABEL,
                      "Expected label, got %s", value_to_string(operand1));
            string label_name = AS_LABEL(operand1);
            Label *label = vm_find_label_by_name(vm, label_name);
            VM_ASSERT(vm, label != NULL, "Label not found: " SV_FMT,
                      SV_ARG(label_name));
#ifdef VM_DEBUG
            printf("JMP ");
            vm_dump_value(operand1);
            printf("\n");
#endif
            vm->ip = label->ip;
        } break;
        case OP_HLT: {
#ifdef VM_DEBUG
            printf("HLT\n");
#endif
            vm->state = VM_STATE_HALT;
            return;
        }
        default: {
            UNIMPLEMENTED_MSG("opcode %u", instr->opcode);
        } break;
        }
    }
}

void vm_emit(VirtualMachine *vm, Instruction *instr) {
    vec_push(&vm->instructions, instr);
}

void vm_emit_label(VirtualMachine *vm, string label_name) {
    Label *label = malloc(sizeof(Label));
    label->name = label_name;
    label->label_id = vec_len(&vm->labels);
    label->ip = vm->ip;
    vec_push(&vm->labels, *label);
}

Label *vm_find_label_by_ip(VirtualMachine *vm, usize ip) {
    for (usize i = 0; i < vec_len(&vm->labels); i++) {
        Label *label = vec_get(&vm->labels, i);
        if (label->ip == ip)
            return label;
    }
    return NULL;
}

Label *vm_find_label_by_name(VirtualMachine *vm, string name) {
    for (usize i = 0; i < vec_len(&vm->labels); i++) {
        Label *label = vec_get(&vm->labels, i);
        if (sv_eq(label->name, name))
            return label;
    }
    return NULL;
}

void vm_reset_register(VirtualMachine *vm, Register reg) {
    vm->registers[reg] = NONE_VALUE;
}

void vm_dump(VirtualMachine *vm) {
    printf("Instructions:\n");
    for (usize i = 0; i < vec_len(&vm->instructions); i++) {
        Instruction *instr = *vec_get(&vm->instructions, i);
        printf("  %llu: ", i);
        switch (instr->opcode) {
        case OP_NOP: {
            printf("nop");
        } break;
        case OP_MOV: {
            printf("mov ");
            vm_dump_value(instr->operand1);
            printf(", ");
            vm_dump_value(instr->operand2);
        } break;
        case OP_PUSH: {
            printf("push ");
            vm_dump_value(instr->operand1);
        } break;
        case OP_POP: {
            printf("pop ");
            vm_dump_value(instr->operand1);
        } break;
        case OP_ADD: {
            printf("add ");
            vm_dump_value(instr->operand1);
            printf(", ");
            vm_dump_value(instr->operand2);
        } break;
        case OP_SUB: {
            printf("sub ");
            vm_dump_value(instr->operand1);
            printf(", ");
            vm_dump_value(instr->operand2);
        } break;
        case OP_MUL: {
            printf("mul ");
            vm_dump_value(instr->operand1);
            printf(", ");
            vm_dump_value(instr->operand2);
        } break;
        case OP_DIV: {
            printf("div ");
            vm_dump_value(instr->operand1);
            printf(", ");
            vm_dump_value(instr->operand2);
        } break;
        case OP_MOD: {
            printf("mod ");
            vm_dump_value(instr->operand1);
            printf(", ");
            vm_dump_value(instr->operand2);
        } break;
        case OP_JMP: {
            printf("jmp ");
            vm_dump_value(instr->operand1);
        } break;
        case OP_HLT: {
            printf("hlt");
        } break;
        default: {
            UNIMPLEMENTED_MSG("opcode %u", instr->opcode);
        } break;
        }
        printf("\n");
    }
    printf("Labels:\n");
    for (usize i = 0; i < vec_len(&vm->labels); i++) {
        Label *label = vec_get(&vm->labels, i);
        printf("  %llu: " SV_FMT "\n", i, SV_ARG(label->name));
    }
    printf("Registers:\n");
    for (usize i = 0; i < REG_COUNT; i++) {
        if (vm->registers[i].type == VALUE_NONE)
            continue;
        printf("  %lld: ", i);
        vm_dump_value(vm->registers[i]);
        printf("\n");
    }
    printf("Stack:\n");
    for (usize i = 0; i < vec_len(&vm->stack); i++) {
        printf("  %llu: ", i);
        vm_dump_value(*vec_get(&vm->stack, i));
        printf("\n");
    }
    printf("IP: %llu\n", vm->ip);
    printf("State: ");
    switch (vm->state) {
    case VM_STATE_RUNNING: {
        printf("running");
    } break;
    case VM_STATE_HALT: {
        printf("halt");
    } break;
    case VM_STATE_ERROR: {
        printf("error");
    } break;
    }
    printf("\n");
}