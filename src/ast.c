#include <stdio.h>
#include <stdlib.h>

typedef enum NodeType {
    NODE_NONE,
    NODE_SUM_TYPE,
    NODE_PRODUCT_TYPE,
    NODE_ATTRIBUTES,
    NODE_CON_ID,
    NODE_TYPE_ID_OPT,
    NODE_TYPE_ID_KLEENE,
    NODE_TYPE_ID_ORD,
    NODE_RHS_TYPE,
    NODE_LHS_IDENT,
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char *value;
    struct ASTNode *children;
    struct ASTNode *next; 
} ASTNode;

ASTNode *createNode(NodeType type, char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->value = value;
    node->children = NULL;
    node->next = NULL;
    return node;
}

ASTNode *addChild(ASTNode *parent, ASTNode *child) {
    if (parent->children == NULL) {
        parent->children = child;
    } else {
        ASTNode *temp = parent->children;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = child;
    }
    return parent;
}




