#ifndef _CYCLONE_AST_H
#define _CYCLONE_AST_H

#include "common.h"
#include "location.h"
#include "token.h"
#include "vector.h"

typedef u8 AstNodeType;
enum {
#define AST_NODE(id, name, data) AST_##id,
#include "ast_nodes.def"
#undef AST_NODE

    AST_COUNT,
};

static inline const char *ast_node_type_to_string(AstNodeType type) {
    const char *strings[AST_COUNT] = {
#define AST_NODE(id, name, data) #name,
#include "ast_nodes.def"
#undef AST_NODE
    };
    return strings[type];
}

typedef struct AstNode AstNode;
struct AstNode {
    AstNodeType type;
    Location loc;
    union {
        rawptr dummy;
#define AST_NODE(id, name, data) struct data name;
#include "ast_nodes.def"
#undef AST_NODE
    } data;
};

typedef struct {
    vec(AstNode *) nodes;
} Ast;

Ast ast_new(void);
void ast_free(Ast *ast);
void ast_add_node(Ast *ast, AstNode *node);

AstNode *ast_node_new(AstNodeType type, Location loc);
void ast_node_free(AstNode *node);

void ast_print(Ast *ast, usize indent);

#endif // _CYCLONE_AST_H