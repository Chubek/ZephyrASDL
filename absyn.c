#include "asdl.h"

Heap *absyn_heap = NULL;

#define GC_ALLOC(size) heap_alloc(absyn_heap, size)

void add_field(Field **prev, char *type_id, int modifier, char *id) {
  Field *field = (Field *)GC_ALLOC(sizeof(Field));
  field->type_id = type_id;
  field->modifier = modifier;
  field->id = id;
  field->next = *prev;
  *prev = field;
  return field;
}

void add_constructor(Constructor **prev, char *con_id, Field *fields,
                     int num_fields) {
  Constructor *constructor = (Constructor *)GC_ALLOC(sizeof(Constructor));
  constructor->fields = fields;
  constructor->num_fields = num_fields;
  constructor->next = *prev;
  *prev = constructor;
  return constructor;
}

void add_sum_type(Rule **prev, Constructor *constructors, int num_constructors,
                  Field *attributes, int num_attributes) {
  Rule *rule = (Rule *)GC_ALLOC(sizeof(Rule));
  rule->type = (Type *)GC_ALLOC(sizeof(Type));
  rule->type->sum->constructors = constructors;
  rule->type->sum->num_constructors = num_constructors;
  rule->type->sum->attributes = attributes;
  rule->type->sum->num_attributes = num_attributes;
  rule->kind = TYPE_SUM;
  rule->next = *prev;
  *prev = rule;
  return rule;
}

void add_product_type(Rule **prev, Field *fields, int num_fields) {
  Rule *rule = (Rule *)GC_ALLOC(sizeof(Rule));
  rule->type = (Type *)GC_ALLOC(sizeof(Type));
  rule->type->product->fields = fields;
  rule->type->product->num_fields = num_fields;
  rule->type->kind = TYPE_PRODUCT;
  rule->next = *prev;
  *prev = rule;
  return rule;
}

char *gc_strndup(const char *str, size_t n) {
  char *dup = (char *)GC_ALLOC(n);
  memmove(dup, str, n);
  return dup;
}
