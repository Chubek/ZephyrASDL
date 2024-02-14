#ifndef TREE_FN_H
#define TREE_FN_H

#include <stdlib.h>
#include <string.h>

#include "types.h"

#alloc fields_heap, fields_alloc, fields_realloc, fields_dump
#alloc cons_heap, cons_alloc, cons_realloc, cons_dump
#alloc sums_heap, sums_alloc, sums_realloc, sums_dump
#alloc prod_heap, prod_alloc, prod_realloc, prod_dump
#alloc str_heap, str_alloc, str_realloc, str_dump

#hashfunc tree_hash

char *dup_str(char *s, int n) {
    char *d = str_alloc(n);
    return memmove(&d[0], s, n);
}

void dump_heap(void) {
   fields_dump();
   cons_dump();
   sums_dump();
   prod_dump();
   str_dump();
}

static inline void add_field(char* type_id, enum FieldKind kind, char* id) {
    fields = fields_realloc(fields, (num_fields + 1) * sizeof(Field*));
    fields[num_fields] = fields_alloc(sizeof(Field));
    fields[num_fields]->type_id = type_id;
    fields[num_fields]->kind = kind;
    fields[num_fields]->id = id;
    num_fields++;
}

static inline void add_constructor(char* con_id, Field** fields, int num_fields) {
    cons = cons_realloc(cons, (num_cons + 1) * sizeof(Constructor*));
    cons[num_cons] = cons_alloc(sizeof(Constructor));
    cons[num_cons]->id =  con_id;
    cons[num_cons]->fields = fields;
    cons[num_cons]->num_fields = num_fields;
    num_cons++;
}

static inline void add_sum_type(Constructor** constructors, int num_constructors, Field** attributes, int num_attributes) {
    sumtype = sums_alloc(sizeof(Sum));
    sumtype->cons = constructors;
    sumtype->num_cons = num_constructors;
    sumtype->attrs = attributes;
    sumtype->num_attrs = num_attributes;
}

static inline void add_product_type(Field** fields, int num_fields) {
    prodtype = prod_alloc(sizeof(Product));
    prodtype->fields = fields;
    prodtype->num_fields = num_fields;
}



#endif
