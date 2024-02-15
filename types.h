#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Rule Rule;
typedef struct Type Type;
typedef struct Sum Sum;
typedef struct Product Product;
typedef struct Constructor Constructor;
typedef struct Field Field;

typedef enum FieldKind FieldKind;
typedef enum TypeKind TypeKind;

struct Rule {
  char *id;
  Type *type;
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
  Constructor **cons;
  size_t num_cons;
  Field **attrs;
  size_t num_attrs;
};

struct Product {
  Field **fields;
  size_t num_fields;
};

struct Constructor {
  char *id;
  Field **fields;
  size_t num_fields;
};

struct Field {
  enum FieldKind {
    SEQUENCE,
    OPTIONAL,
    NORMAL,
  } kind;
  char *type_id;
  char *id;
};

void add_field(char *type_id, int opt, char *id);
void add_constructor(char *con_id, Field **fields, int num_fields);
void add_sum_type(Constructor **constructors, int num_constructors,
                  Field **attributes, int num_attributes);
void add_product_type(Field **fields, int num_fields);
char *dup_str(char *s, int n);
void *dup_mem(void *m, int n, int nmemb);
void dump_heaps(void);

#endif
