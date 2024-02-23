#include "asdl.h"

#define GC_ALLOC(size) heap_alloc(absyn_heap, size)

Heap *absyn_heap = NULL;
Field *fields;
Constructor *constructors;
Rule *rules;

void init_absyn(void) {
  constructors = NULL;
  fields = NULL;
  rules = NULL;
}

void finalize_absyn(void) { translate_rule_chain(rules_head); }

void dump_absyn(void) { dump_heap(absyn_heap); }

Field *add_field(char *type_id, int modifier, char *id) {
  Field *field = (Field *)GC_ALLOC(sizeof(Field));
  field->type_id = type_id;
  field->modifier = modifier;
  field->id = id;
  field->next = fields;
  fields = field;
  return field;
}

Constructor *add_constructor(char *con_id, Field *fields) {
  Constructor *constructor = (Constructor *)GC_ALLOC(sizeof(Constructor));
  constructor->fields = fields;
  constructor->num_fields = num_fields;
  constructor->next = constructors;
  constructors = constructor;
  return constructor;
}

Rule *add_sum_type(Constructor *constructors, Field *attributes) {
  Rule *rule = (Rule *)GC_ALLOC(sizeof(Rule));
  rule->type = (Type *)GC_ALLOC(sizeof(Type));
  rule->type->sum->constructors = constructors;
  rule->type->sum->attributes = attributes;
  rule->kind = TYPE_SUM;
  rule->next = rules;
  rules = rule;
  return rule;
}

Rule *add_product_type(Field *fields) {
  Rule *rule = (Rule *)GC_ALLOC(sizeof(Rule));
  rule->type = (Type *)GC_ALLOC(sizeof(Type));
  rule->type->product->fields = fields;
  rule->type->kind = TYPE_PRODUCT;
  rule->next = rules;
  rules = rule;
  return rule;
}

char *gc_strndup(const char *str, size_t n) {
  char *dup = (char *)GC_ALLOC(n);
  return memmove(dup, str, n);
}
