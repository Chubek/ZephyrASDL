#ifndef AST_H
#define AST_H

typedef enum NodeType NodeType;
typedef struct ASTNode ASTNode;


ASTNode *create_node(NodeType type, char *value);
ASTNode *add_child(ASTNode *parent, ASTNode *child);
void free_ast(ASTNode *node);
void print_ast(FILE *output, ASTNode *node);











#endif
