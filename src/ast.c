#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

typedef struct ASTNode {
    NodeType type;
    char *value;
    struct ASTNode *children;
    struct ASTNode *next; 
} ASTNode;

ASTNode *create_node(NodeType type, char *value) {
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

ASTNode *add_child(ASTNode *parent, ASTNode *child) {
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



void free_ast(ASTNode *node) {
    if (node) {
        free(node->value);
        ASTNode *child = node->children;
        while (child) {
            ASTNode *next = child->next;
            free_ast(child);
            child = next;
        }
        free(node);
    }
}


void print_ast(FILE *output, ASTNode *root, int level) {
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++)
       fprintf(output, "  ");

    switch (root->type) {
        case NODE_NONE:
            fprintf(output, "Type: NODE_NONE");
            break;
        case NODE_ATTRIBUTES:
            fprintf(output, "Type: NODE_ATTRIBUTES");
            break;
        case NODE_CON_ID:
           fprintf(output, "Type: NODE_CON_ID");
            break;
        case NODE_TYPE_ID_OPT:
           fprintf(output, "Type: NODE_TYPE_ID_OPT");
            break;
        case NODE_TYPE_ID_KLEENE:
           fprintf(output, "Type: NODE_TYPE_ID_KLEENE");
            break;
        case NODE_TYPE_ID_ORD:
           fprintf(output, "Type: NODE_TYPE_ID_ORD");
            break;
        case NODE_LHS_IDENT:
           fprintf(output, "Type: NODE_LHS_IDENT");
            break;
        default:
           fprintf(output, "Type: UNKNOWN");
            break;
    }

    if (root->value != NULL)
       fprintf(output, ", Value: %s", root->value);
    
    fprintf(output, "\n");

    print_ast(output, root->children, level + 1);
    print_ast(output, root->next, level);
}

