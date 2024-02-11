#include <stdio.h>
#include <stdlib.h>

typedef enum NodeType {
    NODE_TYPE_ID,
    NODE_CON_ID,
    NODE_ID,
    NODE_OPT,
    NODE_CLOSURE,
    NODE_NONE,
    NODE_CONSTRUCTOR,
    NODE_FIELD,
    NODE_SUM_TYPE,
    NODE_PRODUCT_TYPE,
    NODE_DEFINITION,
    NODE_PROGRAM
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


void printAST(ASTNode *node, int depth) {
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) {
        printf("    ");
    }

    switch (node->type) {
        case NODE_TYPE_ID:
            printf("Type ID: %s\n", node->value);
            break;
        case NODE_CONSTRUCTOR:
            printf("Constructor: %s\n", node->value);
            break;
        case NODE_FIELD:
            printf("Field: %s\n", node->value);
            break;
        case NODE_SUM_TYPE:
            printf("Sum Type\n");
            break;
        case NODE_PRODUCT_TYPE:
            printf("Product Type\n");
            break;
        case NODE_DEFINITION:
            printf("Definition\n");
            break;
        case NODE_PROGRAM:
            printf("Program\n");
            break;
        default:
            printf("Unknown Node\n");
            break;
    }

    ASTNode *child = node->children;
    while (child != NULL) {
        printAST(child, depth + 1);
        child = child->next;
    }
}


