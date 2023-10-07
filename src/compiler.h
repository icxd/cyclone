#ifndef _CYCLONE_COMPILER_H
#define _CYCLONE_COMPILER_H

#include "common.h"
#include "file.h"
#include "vector.h"
#include "vm.h"

typedef u8 CompilationTarget;
enum {
    TARGET_WIN_X86_64,
};

typedef struct Compiler {
    CompilationTarget target;
    vec(u32) file_ids;
    vec(File *) files;
    vec(string) file_names;
    usize ip;
} Compiler;

Compiler *compiler_new(CompilationTarget target);
void compiler_free(Compiler *compiler);
u32 compiler_new_file(Compiler *compiler, File *file);

string compiler_compile_x86_64(Compiler *compiler, VirtualMachine *vm);

#endif // _CYCLONE_COMPILER_H