#ifndef ABSYN_H
#define ABSYN_H

#include <stdlib.h>
#include <stdint.h>

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
    Attributes *attr;
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


static inline Rule *create_rule(void) {
   Rule *rule = (Rule*)calloc(1, sizeof(Rule));
   rule->types = NULL;
   rule->id = NULL;
   return rule;
}

static inline Rule *rule_add_type(Rule *rule, Type *type) {
    rule->types =
	(Type**)realloc(rule->types, (rule->num_types + 1) * sizeof(Rule*));
    rule->types[rule->num_types++] type;
    return rul;
}

static inline Sum *create_sum(void) {
    Sum *sum = (Sum*)calloc(1, sizeof(Sum));
    sum->cons = NULL;
    sum->num_cons = 0;
    return sum;
}

static inline Sum *sum_add_constructor(Sum *sum, Constructor *constructor) {
    sum->cons = (Constructor**)realloc(sum->cons, (sum->num_cons + 1) * sizeof(Constructor*));
    sum->cons[sum->num_cons++] = constructor;
    return sum;
}

static inline Product *create_product(void) {
    Product *product = (Product*)calloc(1, sizeof(Product));
    product->fields = NULL;
    product->num_fields = 0;
    return product;
}

static inline Product *product_add_field(Product *product, Field *field) {
    product->fields = (Field**)realloc(product->fields, (product->num_fields + 1) * sizeof(Field*));
    product->fields[product->num_fields++] = field;
    return product;
}

static inline Constructor *create_constructor(void) {
    Constructor *constructor = (Constructor*)calloc(1, sizeof(Constructor));
    constructor->fields = NULL;
    constructor->num_fields = 0;
    return constructor;
}

static inline Constructor *constructor_add_field(Constructor *constructor, Field *field) {
    constructor->fields = (Field**)realloc(constructor->fields, (constructor->num_fields + 1) * sizeof(Field*));
    constructor->fields[constructor->num_fields++] = field;
    return constructor;
}

static inline Field *create_field(void) {
    Field *field = (Field*)calloc(1, sizeof(Field));
    field->type_name = NULL;
    field->id = NULL;
    return field;
}

static inline void free_rule(Rule *rule) {
   if (rule == NULL)
	   return;

   for (size_t i = 0; i < rule->num_types; i++)
	    free_type(rule->type[rule->num_type]);

   free(rule->id);
   free(rule->types);
   free(rule);
}

static inline void free_sum(Sum *sum) {
    if (sum == NULL)
        return;

    for (size_t i = 0; i < sum->num_cons; ++i)
        free_constructor(sum->cons[i]);

    free(sum->cons);
    free(sum);
}

static inline void free_product(Product *product) {
    if (product == NULL)
        return;

    for (size_t i = 0; i < product->num_fields; ++i)
        free_field(product->fields[i]);


    free(product->fields);
    free(product);
}

static inline void free_constructor(Constructor *constructor) {
    if (constructor == NULL)
        return;

    for (size_t i = 0; i < constructor->num_fields; ++i)
        free_field(constructor->fields[i]);

    free(constructor->id);
    free(constructor->fields);
    free(constructor);
}

static inline void free_field(Field *field) {
   free(field->type_name);
   free(field->id);
   free(field);
}

