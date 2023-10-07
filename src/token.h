#ifndef _CYCLONE_TOKEN_H
#define _CYCLONE_TOKEN_H

#include "common.h"
#include "location.h"
#include "vector.h"

typedef u8 TokenKind;
enum {
#define TOKEN(id, name) TOKEN_##id,
#include "tokens.def"
#undef TOKEN
};

static inline const char *token_kind_to_string(TokenKind type) {
    static const char *strings[] = {
#define TOKEN(id, name) name,
#include "tokens.def"
#undef TOKEN
    };
    return strings[type];
}

typedef struct {
    TokenKind kind;
    string lexeme;
    Location loc;
} Token;

#endif // _CYCLONE_TOKEN_H