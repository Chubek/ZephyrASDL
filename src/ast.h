#ifndef AST_H
#define AST_H

typedef enum NodeType NodeType;
typedef struct ASTNode ASTNode;

enum NodeType {
    NODE_NONE,
    NODE_ATTRIBUTES,
    NODE_CON_ID,
    NODE_IDENT,
    NODE_TYPE_ID_OPT,
    NODE_TYPE_ID_KLEENE,
    NODE_TYPE_ID_ORD,
    NODE_LHS_IDENT,
};

ASTNode *create_node(NodeType type, char *value);
ASTNode *add_child(ASTNode *parent, ASTNode *child);
void free_ast(ASTNode *node);
void print_ast(FILE *output, ASTNode *node, int level);











#endif
