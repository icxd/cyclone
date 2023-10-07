#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "utf8.h"

Ast ast_new(void) {
    Ast ast;
    vec_init(&ast.nodes);
    return ast;
}

void ast_free(Ast *ast) {
    AstNode *node;
    vec_foreach(&ast->nodes, node) { ast_node_free(node); }
    vec_free(&ast->nodes);
}

void ast_add_node(Ast *ast, AstNode *node) { vec_push(&ast->nodes, node); }

AstNode *ast_node_new(AstNodeType type, Location loc) {
    AstNode *node = malloc(sizeof(*node));
    node->type = type;
    node->loc = loc;
    return node;
}

void ast_node_free(AstNode *node) { UNIMPLEMENTED_MSG("%s", "ast_node_free"); }

static void ast_node_print(AstNode *node, Ast *ast, usize indent) {
    for (usize i = 0; i < indent; i++) {
        printf("  ");
    }

    printf("%s%s %s0x%p %s<%u:%u:%u>%s\n", color(COLOR_RED, ATTR_NONE),
           ast_node_type_to_string(node->type), color(COLOR_YELLOW, ATTR_NONE),
           (rawptr)node, color(COLOR_MAGENTA, ATTR_NONE), node->loc.file_id,
           node->loc.line, node->loc.column, color_reset());

    switch (node->type) {
    case AST_BINARY_EXPR: {
        ast_node_print(node->data.BinaryExpr.left, ast, indent + 1);
        ast_node_print(node->data.BinaryExpr.right, ast, indent + 1);
    }
    }
}

void ast_print(Ast *ast, usize indent) {
    AstNode *node;
    vec_foreach(&ast->nodes, node) { ast_node_print(node, ast, indent); }
}