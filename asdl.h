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
  size_t num_cons;
  size_t num_attrs;
};

struct Product {
  Field *fields;
  size_t num_fields;
};

struct Constructor {
  char *id;
  Field *fields;
  size_t num_fields;
};

struct Field {
  enum FieldKind {
    FIELD_SEQUENCE,
    FIELD_OPTIONAL,
    FIELD_NORMAL,
  } kind;
  char *type_id;
  char *id;
  Field *next;
};

struct GCNode {
  void *data;
  GCNode *next;
  int marked;
};

struct Heap {
  GCNode *head;
};

void add_field(Field **prev, char *type_id, int opt, char *id);
void add_constructor(Constructor **prev, char *con_id, Field *fields,
                     int num_fields);
void add_sum_type(Rule **prev, Constructor *constructors, int num_constructors,
                  Field *attributes, int num_attributes);
void add_product_type(Rule **prev, Field *fields, int num_fields);

GCNode *new_gc_node(void *data);
Heap *create_heap();
void *heap_alloc(Heap *heap, size_t size);
void *heap_realloc(Heap *heap, void *data, size_t new_size);
void mark(GCNode *node);
void sweep(Heap *heap);
void garbage_collect(Heap *heap);
void free_heap(Heap *heap);

char *gc_strndup(const char *str, size_t n);

#endif
