#ifndef AST_H
#define AST_H

typedef enum NodeType NodeType;
typedef struct ASTNode ASTNode;


ASTNode *createNode(NodeType type, char *value);
ASTNode *addChild(ASTNode *parent, ASTNode *child);












#endif
