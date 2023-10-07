#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"

Tokenizer *tokenizer_new(File *file) {
    Tokenizer *t = malloc(sizeof(Tokenizer));
    t->file = file;
    t->pos = 0;
    t->line = 1;
    t->column = 1;
    vec_init(&t->errors);
    return t;
}

void tokenizer_free(Tokenizer *tokenizer) {
    file_free(tokenizer->file);
    vec_free(&tokenizer->errors);
    free(tokenizer);
}

char tokenizer_advance(Tokenizer *tokenizer) {
    char c = *vec_get(&tokenizer->file->contents, tokenizer->pos);
    tokenizer->pos++;
    return c;
}

char tokenizer_peek(Tokenizer *tokenizer) {
    return *vec_get(&tokenizer->file->contents, tokenizer->pos);
}

char tokenizer_peek_next(Tokenizer *tokenizer) {
    return *vec_get(&tokenizer->file->contents, tokenizer->pos + 1);
}

static Location tokenizer_location(Tokenizer *tokenizer) {
    return LOC(tokenizer->file->id, tokenizer->line, tokenizer->column);
}

Token tokenizer_next_token(Tokenizer *tokenizer) {
    Token token = {0};
    token.kind = TOKEN_EOF;

    if (tokenizer->pos >= vec_len(&tokenizer->file->contents)) {
        return token;
    }

    char c = tokenizer_peek(tokenizer);
    token.loc = tokenizer_location(tokenizer);
    switch (c) {
    case '\r':
    case '\t':
    case ' ': {
        tokenizer_advance(tokenizer);
        tokenizer->column++;
        return tokenizer_next_token(tokenizer);
    } break;
    case '\n': {
        tokenizer_advance(tokenizer);
        tokenizer->column = 1;
        tokenizer->line++;
        return tokenizer_next_token(tokenizer);
    } break;
    case '+': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_PLUS;
        token.lexeme = SV("+");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '+') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_PLUS_PLUS;
            token.lexeme = SV("++");
            tokenizer->column++;
        } else if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_PLUS_EQ;
            token.lexeme = SV("+=");
            tokenizer->column++;
        }
    } break;
    case '-': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_MINUS;
        token.lexeme = SV("-");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '-') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_MINUS_MINUS;
            token.lexeme = SV("--");
            tokenizer->column++;
        } else if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_MINUS_EQ;
            token.lexeme = SV("-=");
            tokenizer->column++;
        }
    } break;
    case '*': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_STAR;
        token.lexeme = SV("*");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_STAR_EQ;
            token.lexeme = SV("*=");
            tokenizer->column++;
        }
    } break;
    case '/': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_SLASH;
        token.lexeme = SV("/");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_SLASH_EQ;
            token.lexeme = SV("/=");
            tokenizer->column++;
        } else if (tokenizer_peek(tokenizer) == '/') {
            tokenizer_advance(tokenizer);
            tokenizer->column++;
            for (;;) {
                if (tokenizer->pos >= vec_len(&tokenizer->file->contents)) {
                    break;
                }
                char c = tokenizer_peek(tokenizer);
                if (c == '\n') {
                    break;
                }
                tokenizer_advance(tokenizer);
                tokenizer->column++;
            }
            return tokenizer_next_token(tokenizer);
        }
    } break;
    case '%': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_PERCENT;
        token.lexeme = SV("%");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_PERCENT_EQ;
            token.lexeme = SV("%=");
            tokenizer->column++;
        }
    } break;
    case '=': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_EQ;
        token.lexeme = SV("=");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_EQ_EQ;
            token.lexeme = SV("==");
            tokenizer->column++;
        }
    } break;
    case '!': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_BANG;
        token.lexeme = SV("!");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_BANG_EQ;
            token.lexeme = SV("!=");
            tokenizer->column++;
        }
    } break;
    case '<': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_LT;
        token.lexeme = SV("<");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_LT_EQ;
            token.lexeme = SV("<=");
            tokenizer->column++;
        } else if (tokenizer_peek(tokenizer) == '<') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_LSHIFT;
            token.lexeme = SV("<<");
            tokenizer->column++;
            if (tokenizer_peek(tokenizer) == '=') {
                tokenizer_advance(tokenizer);
                token.kind = TOKEN_LSHIFT_EQ;
                token.lexeme = SV("<<=");
                tokenizer->column++;
            }
        }
    } break;
    case '>': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_GT;
        token.lexeme = SV(">");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_GT_EQ;
            token.lexeme = SV(">=");
            tokenizer->column++;
        } else if (tokenizer_peek(tokenizer) == '>') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_RSHIFT;
            token.lexeme = SV(">>");
            tokenizer->column++;
            if (tokenizer_peek(tokenizer) == '=') {
                tokenizer_advance(tokenizer);
                token.kind = TOKEN_RSHIFT_EQ;
                token.lexeme = SV(">>=");
                tokenizer->column++;
            }
        }
    } break;
    case '&': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_AMP;
        token.lexeme = SV("&");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '&') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_AMP_AMP;
            token.lexeme = SV("&&");
            tokenizer->column++;
        } else if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_AMP_EQ;
            token.lexeme = SV("&=");
            tokenizer->column++;
        }
    } break;
    case '|': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_PIPE;
        token.lexeme = SV("|");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '|') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_PIPE_PIPE;
            token.lexeme = SV("||");
            tokenizer->column++;
        } else if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_PIPE_EQ;
            token.lexeme = SV("|=");
            tokenizer->column++;
        }
    } break;
    case '^': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_CARET;
        token.lexeme = SV("^");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_CARET_EQ;
            token.lexeme = SV("^=");
            tokenizer->column++;
        }
    } break;
    case '~': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_TILDE;
        token.lexeme = SV("~");
        tokenizer->column++;
    } break;
    case '(': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_LPAREN;
        token.lexeme = SV("(");
        tokenizer->column++;
    } break;
    case ')': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_RPAREN;
        token.lexeme = SV(")");
        tokenizer->column++;
    } break;
    case '{': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_LBRACE;
        token.lexeme = SV("{");
        tokenizer->column++;
    } break;
    case '}': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_RBRACE;
        token.lexeme = SV("}");
        tokenizer->column++;
    } break;
    case '[': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_LBRACKET;
        token.lexeme = SV("[");
        tokenizer->column++;
    } break;
    case ']': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_RBRACKET;
        token.lexeme = SV("]");
        tokenizer->column++;
    } break;
    case ':': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_COLON;
        token.lexeme = SV(":");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '=') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_COLON_EQ;
            token.lexeme = SV(":=");
            tokenizer->column++;
        }
    } break;
    case ';': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_SEMICOLON;
        token.lexeme = SV(";");
        tokenizer->column++;
    } break;
    case ',': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_COMMA;
        token.lexeme = SV(",");
        tokenizer->column++;
    } break;
    case '.': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_DOT;
        token.lexeme = SV(".");
        tokenizer->column++;
        if (tokenizer_peek(tokenizer) == '.') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_DOT_DOT;
            token.lexeme = SV("..");
            tokenizer->column++;
        }
    } break;
    case '"': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_STRING_LIT;
        tokenizer->column++;
        char *start = vec_get(&tokenizer->file->contents, tokenizer->pos);
        usize len = 0;
        for (;;) {
            if (tokenizer->pos >= vec_len(&tokenizer->file->contents)) {
                token.kind = TOKEN_UNKNOWN;
                ERR_NO_VARARG(tokenizer, "unterminated string literal");
                return token;
            }
            char c = tokenizer_advance(tokenizer);
            if (c == '"') {
                tokenizer->column++;
                break;
            }
            if (c == '\n') {
                token.kind = TOKEN_UNKNOWN;
                ERR_NO_VARARG(tokenizer, "unterminated string literal");
                return token;
            }
            len++;
            tokenizer->column++;
        }
        token.lexeme = SV_LEN(start, len);
    } break;
    case '\'': {
        tokenizer_advance(tokenizer);
        token.kind = TOKEN_CHAR_LIT;
        tokenizer->column++;
        char *start = vec_get(&tokenizer->file->contents, tokenizer->pos);
        bool is_escaped = false;
        for (;;) {
            if (tokenizer->pos >= vec_len(&tokenizer->file->contents)) {
                token.kind = TOKEN_UNKNOWN;
                ERR_NO_VARARG(tokenizer, "unterminated character literal");
                return token;
            }
            char c = tokenizer_advance(tokenizer);
            if (c == '\'') {
                tokenizer->column++;
                break;
            }
            if (c == '\n') {
                token.kind = TOKEN_UNKNOWN;
                ERR_NO_VARARG(tokenizer, "unterminated character literal");
                return token;
            }

            if (c == '\\') {
                char code = tokenizer_advance(tokenizer);
                tokenizer->column++;

                switch (code) {
                case 'n':
                case 't':
                case 'r':
                case '\\':
                case '\'':
                case '"':
                case '0':
                    is_escaped = true;
                    break;
                default:
                    ERR(tokenizer, "unknown escape sequence '\\%c'", code);
                    break;
                }
            }
        }
        char *end = vec_get(&tokenizer->file->contents, tokenizer->pos - 1);
        if (end - start != 1 && !is_escaped) {
            token.kind = TOKEN_UNKNOWN;
            ERR_NO_VARARG(
                tokenizer,
                "character literal must contain exactly one character");
            return token;
        }
        token.lexeme = SV_LEN(start, is_escaped ? end - start : 1);
    } break;
    default: {
        if (isalpha(c) || c == '_') {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_IDENT;
            tokenizer->column++;
            char *start =
                vec_get(&tokenizer->file->contents, tokenizer->pos - 1);
            usize len = 1;
            for (;;) {
                if (tokenizer->pos >= vec_len(&tokenizer->file->contents)) {
                    break;
                }
                char c = tokenizer_peek(tokenizer);
                if (!isalnum(c) && c != '_') {
                    break;
                }
                tokenizer_advance(tokenizer);
                len++;
                tokenizer->column++;
            }
            token.lexeme = SV_LEN(start, len);
            if (sv_eq_cstr(token.lexeme, "fn"))
                token.kind = TOKEN_KW_FN;
            else if (sv_eq_cstr(token.lexeme, "struct"))
                token.kind = TOKEN_KW_STRUCT;
            else if (sv_eq_cstr(token.lexeme, "return"))
                token.kind = TOKEN_KW_RETURN;
            else if (sv_eq_cstr(token.lexeme, "this"))
                token.kind = TOKEN_KW_THIS;
            else if (sv_eq_cstr(token.lexeme, "if"))
                token.kind = TOKEN_KW_IF;
            else if (sv_eq_cstr(token.lexeme, "else"))
                token.kind = TOKEN_KW_ELSE;
            else if (sv_eq_cstr(token.lexeme, "let"))
                token.kind = TOKEN_KW_LET;
            else if (sv_eq_cstr(token.lexeme, "for"))
                token.kind = TOKEN_KW_FOR;
            else if (sv_eq_cstr(token.lexeme, "in"))
                token.kind = TOKEN_KW_IN;
            else if (sv_eq_cstr(token.lexeme, "true"))
                token.kind = TOKEN_KW_TRUE;
            else if (sv_eq_cstr(token.lexeme, "false"))
                token.kind = TOKEN_KW_FALSE;
            else if (sv_eq_cstr(token.lexeme, "try"))
                token.kind = TOKEN_KW_TRY;
            else if (sv_eq_cstr(token.lexeme, "throw"))
                token.kind = TOKEN_KW_THROW;
            else if (sv_eq_cstr(token.lexeme, "as"))
                token.kind = TOKEN_KW_AS;
        } else if (isdigit(c)) {
            tokenizer_advance(tokenizer);
            token.kind = TOKEN_INT_LIT;
            tokenizer->column++;
            char *start =
                vec_get(&tokenizer->file->contents, tokenizer->pos - 1);
            usize len = 1;
            for (;;) {
                if (tokenizer->pos >= vec_len(&tokenizer->file->contents)) {
                    break;
                }
                char c = tokenizer_peek(tokenizer);
                if (!isdigit(c)) {
                    break;
                }
                tokenizer_advance(tokenizer);
                len++;
                tokenizer->column++;
            }
            token.lexeme = SV_LEN(start, len);
            if (tokenizer_peek(tokenizer) == '.' &&
                isdigit(tokenizer_peek_next(tokenizer))) {
                tokenizer_advance(tokenizer);
                token.kind = TOKEN_FLOAT_LIT;
                token.lexeme = sv_concat(token.lexeme, SV("."));
                tokenizer->column++;
                char *start =
                    vec_get(&tokenizer->file->contents, tokenizer->pos - 1);
                usize len = 1;
                for (;;) {
                    if (tokenizer->pos >= vec_len(&tokenizer->file->contents)) {
                        break;
                    }
                    char c = tokenizer_peek(tokenizer);
                    if (!isdigit(c)) {
                        break;
                    }
                    tokenizer_advance(tokenizer);
                    len++;
                    tokenizer->column++;
                }
                token.lexeme = sv_concat(token.lexeme, SV_LEN(start, len));
            }
        } else {
            token.kind = TOKEN_UNKNOWN;
            ERR(tokenizer, "unexpected character '%c'", c);
        }
    } break;
    }

    return token;
}