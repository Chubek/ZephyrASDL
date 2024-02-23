#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

typedef struct Rule Rule;
typedef struct Type Type;
typedef struct Sum Sum;
typedef struct Product Product;
typedef struct Constructor Constructor;
typedef struct Field Field;
typedef struct GCNode GCNode;
typedef struct Heap Heap;

typedef enum FieldKind FieldKind;
typedef enum TypeKind TypeKind;

struct Rule {
  char *id;
  Type *type;
  Rule *next;
};

struct Type {
  enum TypeKind {
    TYPE_SUM,
    TYPE_PRODUCT,
  } kind;
  union {
    Product *product;
    Sum *sum;
  };
};

struct Sum {
  Constructor *constructors;
  Field *attributes;
};

struct Product {
  Field *fields;
};

struct Constructor {
  char *id;
  Field *fields;
};

struct Field {
  enum FieldKind {
    FIELD_SEQUENCE,
    FIELD_OPTIONAL,
    FIELD_NORMAL,
  } kind;
  char *type_id;
  char *id;
  char *cache;
  Field *next;
};

struct Translator {
   FILE *prelude;
   FILE *decls;
   FILE *defs;
   FILE *appendage;
   Rule *rules;
   char *outpath;
};

struct GCNode {
  void *data;
  GCNode *next;
  int marked;
};

struct Heap {
  GCNode *head;
};

void add_field(char *type_id, int opt, char *id);
void add_constructor(char *con_id, Field *fields);
void add_sum_type(Constructor *constructors, Field *attributes);
void add_product_type(Field *fields);

GCNode *new_gc_node(void *data);
Heap *create_heap();
void *heap_alloc(Heap *heap, size_t size);
void *heap_realloc(Heap *heap, void *data, size_t new_size);
void mark(GCNode *node);
void sweep(Heap *heap);
void garbage_collect(Heap *heap);
void dump_heap(Heap *heap);

char *gc_strndup(const char *str, size_t n);

void translate_rule_chain(Rule *rules);


void init_translator(void);
void finalize_translator(void);
void dump_translator(void);

void init_absyn(void);
void finalize_absyn(void);
void dump_absyn(void);

#endif
