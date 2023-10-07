#ifndef _CYCLONE_VM_H
#define _CYCLONE_VM_H

#include "common.h"
#include "instrucation.h"
#include "vector.h"

#define VM_ASSERT(vm, cond, msg, ...)                                          \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(stderr, "%s:%u: ERROR: " msg "\n", __FILE__, __LINE__,     \
                    ##__VA_ARGS__);                                            \
            vm->state = VM_STATE_ERROR;                                        \
            return;                                                            \
        }                                                                      \
    } while (0)
#define VM_ASSERT_NOVARARG(vm, cond, msg)                                      \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(stderr, "%s:%u: ERROR: " msg "\n", __FILE__, __LINE__);    \
            vm->state = VM_STATE_ERROR;                                        \
            return;                                                            \
        }                                                                      \
    } while (0)

typedef u8 VirtualMachineState;
enum {
    VM_STATE_RUNNING,
    VM_STATE_HALT,
    VM_STATE_ERROR,
};

typedef struct {
    string name;
    u8 label_id;
    usize ip;
} Label;

typedef struct {
    string name;
    vec(Instruction *) instructions;
} Segment;

typedef struct {
    VirtualMachineState state;

    // TODO: segments as a replacement for labels?
    // vec(Segment *) segments;
    vec(Instruction *) instructions;
    Value registers[REG_COUNT];
    vec(Label) labels;

    vec(Value) stack;
    usize sp, bp;

    usize ip;
} VirtualMachine;

Label label_new(VirtualMachine *vm, string name, usize ip);

VirtualMachine *vm_new(void);
void vm_free(VirtualMachine *vm);

Value vm_resolve_value(VirtualMachine *vm, Value value);
void vm_execute(VirtualMachine *vm);

void vm_emit(VirtualMachine *vm, Instruction *instr);
void vm_emit_label(VirtualMachine *vm, string label_name);
Label *vm_find_label_by_ip(VirtualMachine *vm, usize ip);
Label *vm_find_label_by_name(VirtualMachine *vm, string name);
void vm_reset_register(VirtualMachine *vm, Register reg);

void vm_dump(VirtualMachine *vm);

#endif // _CYCLONE_VM_H