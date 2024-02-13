#ifndef ABSYN_H
#define ABSYN_H

#include <stdlib.h>
#include <stdint.h>

#alloc field_heap , field_alloc , field_realloc , field_dump
#alloc con_heap , con_alloc , con_realloc , con_dump
#alloc type_heap , type_alloc , type_realloc , type_dump
#alloc rule_heap , rule_alloc , rule_realloc , rule_dump
#alloc attrs_heap , attrs_alloc , attrs_realloc , attrs_dump

#hashfunc tree_hash_func

typedef struct Rule Rule;
typedef struct Type Type;
typedef struct Sum Sum;
typedef struct Product Product;
typedef struct Constructor Constructor;
typedef struct Field Field;

struct Rule {
    char *id;
    Type **types;
    size_t num_types;
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
    Filed **attrs;
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
	SEQUENCE, OPTIONAL, NORMAL,
   } kind;
   char *type_name;
   char *id;
};



Rule *rule = NULL, **rules = NULL;
Type *type = NULL, **types = NULL;
Sum  *sumtype = NULL, **sumtypes = NULL;
Product *prodtype = NULL, **prodtypes = NULL;
Constructor *con = NULL, **cons = NULL;
Field *field = NULL, **fields = NULL, **attrs = NULL;

char modifier = 0, *lowercase_id = NULL, *pascalcase_id = NULL, *cstyle_id = NULL, *cons_id = NULL, *type_id = NULL, *identifier = NULL;

enum FieldKind MOD_LUT['~'] = {
  [0 .. '~'] = NORMAL,
  ['*'] = SEQUENCE,
  ['?'] = OPTIONAL,
};

size_t num_fields = 0, num_cons = 0, num_types = 0, num_rules = 0, num_attrs = 0;

#include "parse.peg.h"

extern void walk_rules(Rules**, size_t);

void parse_and_translate(const char *infile, const char *outfile) {
    if (infile != NULL)
	   stdin = freopen(infile, "r", stdin);
    if (outfile != NULL)
	    stdout = freopen(outfile, "w" stdout);

    while (yyparse());

    walk_rules(rules, num_rules);
	
    field_dump();
    con_dump();
    type_dump();
    rule_dump();
    attrs_dump();
}
