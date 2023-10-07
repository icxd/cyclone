#ifndef _CYCLONE_TOKENIZER_H
#define _CYCLONE_TOKENIZER_H

#include "common.h"
#include "file.h"
#include "location.h"
#include "token.h"
#include "vector.h"

typedef struct {
    string message;
    Location loc;
} TokenizerError;
#define ERR(t, msg, ...)                                                       \
    do {                                                                       \
        string message = sv_from_fmt(msg, ##__VA_ARGS__);                      \
        TokenizerError err =                                                   \
            ((TokenizerError){message, LOC(t->file->id, t->line, t->column)}); \
        vec_push(&t->errors, err);                                             \
    } while (0)
#define ERR_NO_VARARG(t, msg)                                                  \
    do {                                                                       \
        string message = SV(msg);                                              \
        TokenizerError err =                                                   \
            ((TokenizerError){message, LOC(t->file->id, t->line, t->column)}); \
        vec_push(&t->errors, err);                                             \
    } while (0)

typedef struct {
    File *file;
    usize pos;
    u32 line, column;

    vec(TokenizerError) errors;
} Tokenizer;

Tokenizer *tokenizer_new(File *file);
void tokenizer_free(Tokenizer *tokenizer);

char tokenizer_advance(Tokenizer *tokenizer);
char tokenizer_peek(Tokenizer *tokenizer);
char tokenizer_peek_next(Tokenizer *tokenizer);
Token tokenizer_next_token(Tokenizer *tokenizer);

#endif // _CYCLONE_TOKENIZER_H