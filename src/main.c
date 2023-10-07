#include <stdio.h>

#include "ast.h"
#include "common.h"
#include "compiler.h"
#include "file.h"
#include "instrucation.h"
#include "location.h"
#include "token.h"
#include "tokenizer.h"
#include "vector.h"
#include "vm.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define CMD(x)                                                                 \
    do {                                                                       \
        printf("[CMD] " x "\n");                                               \
        system(x);                                                             \
    } while (0)

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <compile/interpret> <file>\n", argv[0]);
        return 1;
    }

    bool compile = false;
    if (sv_eq_cstr(SV(argv[1]), "compile")) {
        compile = true;
    } else if (sv_eq_cstr(SV(argv[1]), "interpret")) {
        compile = false;
    } else {
        fprintf(stderr, "usage: %s <compile/interpret> <file>\n", argv[0]);
        return 1;
    }

    string filename = SV(argv[2]);

    Compiler *compiler = compiler_new(TARGET_WIN_X86_64);
    File *file = file_new(filename);
    file->id = compiler_new_file(compiler, file);

    // for (usize i = 0; i < vec_len(&file->contents); i++) {
    //     char c = *vec_get(&file->contents, i);
    //     printf("%c", c);
    // }
    // printf("\n");

    Tokenizer *tokenizer = tokenizer_new(file);
    Token tok = tokenizer_next_token(tokenizer);
    for (;;) {
        if (tok.kind == TOKEN_EOF || tok.kind == TOKEN_UNKNOWN)
            break;
        printf("[" LOC_FMT "] " SV_FMT "\n", LOC_ARG(compiler, tok.loc),
               SV_ARG(tok.lexeme));
        tok = tokenizer_next_token(tokenizer);
    }

    for (usize i = 0; i < vec_len(&tokenizer->errors); i++) {
        TokenizerError *err = vec_get(&tokenizer->errors, i);
        fprintf(stderr, "%s" LOC_FMT " %serror: %s" SV_FMT ".%s\n",
                color(COLOR_WHITE, ATTR_BOLD), LOC_ARG(compiler, err->loc),
                color(COLOR_RED, ATTR_BOLD), color(COLOR_WHITE, ATTR_BOLD),
                SV_ARG(err->message), color_reset());
    }

    VirtualMachine *vm = vm_new();

    if (compile) {
        string out = compiler_compile_x86_64(compiler, vm);

        FILE *fp = fopen("out.asm", "w");
        fprintf(fp, SV_FMT, SV_ARG(out));
        fclose(fp);

        CMD("nasm -f win64 out.asm -o out.obj");
        CMD("ld out.obj -o out.exe");
        CMD("out.exe");
    } else {
        vm_execute(vm);
        // vm_dump(vm);
    }

    vm_free(vm);
    tokenizer_free(tokenizer);
    file_free(file);
    compiler_free(compiler);
    return 0;
}