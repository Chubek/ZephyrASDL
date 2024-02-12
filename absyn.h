#ifndef ABSYN_H
#define ABSYN_H

#include <stdlib.h> 

typedef enum AbsynType AbsynType;
typedef struct AbsynNode AbsynNode;
typedef struct AbsynNodeList AbsynNodeList;

enum AbsynType {
  IDENT,
  TYPE_ID,
  CONS_ID,
  CONSTRUCTOR,
  ATTRIBUTE,
  DEFN,
  LHS_IDENT,
  MARKER,
  TUPLE 
};

struct AbsynNode {
   AbsynType type;
   const char *value;
   bool is_leaf;
   AbsynNode *next;
};

static inline AbsynNode *create_absyn_node(void) {
    AbsynNode *node = (AbsynNode*)calloc(1, sizeof(AbsynNode)); 
    node->value = NULL;
    node->next = NULL;
    return node;
}

static inline AbsynNode *create_ident(const char *id, char marker) {
   AbsynNode *node = create_absyn_node();
   node->value = id;
   node->type = IDENT;
   node->is_leaf = true;
   node->next = create_absyn_node();
   node->next->value = &marker;
   node->next->type = MARKER;
   node->next->is_leaf = true;
   return node;
}

static inline AbsynNode *create_field(const char *type_id, AbsynNode *ident) {
   AbsynNode *node = create_absyn_node();
   node->value = type_id;
   node->type = TYPE_ID;
   node->next = ident; 
   return node;
}

static inline AbsynNode *create_constructor(const char *cons_id, AbsynNode *fields) {
   AbsynNode *node = create_absyn_node();
   node->value = cons_id; 
   node->type = CONS_ID;
   node->next = fields; 
   return node;
}

static inline AbsynNode *create_attr(AbsynNode *fields) {
    AbsynNode *node = create_absyn_node();
    node->type = ATTRIBUTE;
    node->next = fields;
    return node;
}

static inline AbsynNode *create_defn(const char *type_id, AbsynNode *rhs) {
   AbsynNode *node = create_absyn_node();
   node->type = LHS_IDENT;
   node->value = type_id;
   node->next = rhs;
   return node;
}

static inline AbsynNode *create_tuple(AbsynNode *node1, AbsynNode *node2) {
   AbsynNode *node = create_absyn_node();
   node->type = TUPLE;
   node->value = (AbsynNode**)calloc(2, sizeof(AbsynNode*)); 
   node->value[0] = node1; 
   node->value[1] = node2;
   return node;
}

static inline AbsynNode *append_subtree(AbsynNode *node, AbsynNode *subtree) {
   AbsynNode *temp = node;
   while (temp->next != NULL) {
       temp = temp->next;
   }
   temp->next = subtree;
   return temp->next; 
}

#endif

