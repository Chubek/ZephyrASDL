#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
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
typedef struct TypeId TypeId;
typedef struct Symtable Symtable;
typedef struct Stub Stub;

typedef enum FieldKind FieldKind;
typedef enum TypeKind TypeKind;
typedef enum TypeIdKind TypeIdKind;

struct Rule {
  char *id;
  Type *type;
  Rule *next;
};

struct TypeId {
  enum TypeIdKind {
    TYPENAME_STRING,
    TYPENAME_INT8,
    TYPENAME_UINT8,
    TYPENAME_INT16,
    TYPENAME_UINT16,
    TYPENAME_INT32,
    TYPENAME_UINT32,
    TYPENAME_INT64,
    TYPENAME_UINT64,
    TYPENAME_FLOAT32,
    TYPENAME_FLOAT64,
    TYPENAME_FLOAT80,
    TYPENAME_CHAR,
    TYPENAME_UCHAR,
    TYPENAME_SIZE,
    TYPENAME_USIZE,
    TYPENAME_BOOL,
    TYPENAME_IDENTIFIER,
    TYPENAME_BYTEARRAY,
    TYPENAME_ID,
  } kind;
  char *value;
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
  bool is_enum;
};

struct Field {
  enum FieldKind {
    FIELD_SEQUENCE,
    FIELD_OPTIONAL,
    FIELD_NORMAL,
  } kind;
  TypeId *type_id;
  char *id;
  char *cache[4];
  Field *next;
};

struct Translator {
  FILE *locators;
  FILE *prelude;
  FILE *tydefs;
  FILE *tyspecs;
  FILE *decls;
  FILE *defs;
  FILE *appendage;
  Rule *rules;
  char *outpath;
  char *sympath;
};

struct Symtable {
  char *key;
  void *value;
  Symtable *next;
};

struct Stub {
  char *id;
  char *constructor_id;
  Stub *next;
};

TypeId *create_typeid(TypeIdKind kind, char *value);
Field *add_field(TypeId *type_id, int opt, char *id);
Constructor *add_constructor(char *con_id, Field *fields, bool is_enum);
Rule *add_sum_type(Constructor *constructors, Field *attributes);
Rule *add_product_type(Field *fields);

Symtable *symtable_init(void);
void symtable_insert(const char *key, const void *value);
void *symtable_retrieve(const char *key);
bool symtable_exists(const char *key);

char *gc_strndup(const char *str, size_t n);

void translate_rule_chain(Rule *rules);

void assign_prefixes(char *fn);
void assign_suffixes(char *def, char *fn, char *arg, char *kind);

void init_translator(char *, char *);
void finalize_translator(void);
void dump_translator(void);

void init_absyn(void);
void finalize_absyn(void);
void dump_absyn(void);

void emit_prelude(char *s);
void emit_appendage(char *s);

#endif
