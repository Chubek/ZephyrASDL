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
typedef struct Translator Translator;

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
  Constructor *next;
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

Field *add_field(char *type_id, int opt, char *id);
Constructor *add_constructor(char *con_id, Field *fields);
Rule *add_sum_type(Constructor *constructors, Field *attributes);
Rule *add_product_type(Field *fields);

char *gc_strndup(const char *str, size_t n);

void translate_rule_chain(Rule *rules);

void assign_suffixes(char *def, char *fn, char *arg, char *kind);

void init_translator(char *);
void finalize_translator(void);
void dump_translator(void);

void init_absyn(void);
void finalize_absyn(void);
void dump_absyn(void);

void emit_prelude(char c);

#endif
