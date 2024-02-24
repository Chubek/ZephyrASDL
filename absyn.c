#include "asdl.h"

#alloc absyn_heap, absyn_alloc, absyn_realloc, absyn_dump

Field *fields;
Constructor *constructors;
Rule *rules;

void init_absyn(void) {
  constructors = NULL;
  fields = NULL;
  rules = NULL;
}

void finalize_absyn(void) { translate_rule_chain(rules); }

void dump_absyn(void) { absyn_dump(); }

Field *add_field(char *type_id, int modifier, char *id) {
  Field *field = (Field *)absyn_alloc(sizeof(Field));
  field->type_id = type_id;
  field->modifier = modifier;
  field->id = id;
  field->cache = NULL;
  field->next = fields;
  fields = field;
  return field;
}

Constructor *add_constructor(char *id, Field *fields) {
  Constructor *constructor = (Constructor *)absyn_alloc(sizeof(Constructor));
  constructor->id = id;
  constructor->fields = fields;
  constructor->next = constructors;
  constructors = constructor;
  return constructor;
}

Rule *add_sum_type(Constructor *constructors, Field *attributes) {
  Rule *rule = (Rule *)absyn_alloc(sizeof(Rule));
  rule->type = (Type *)absyn_alloc(sizeof(Type));
  rule->type->sum->constructors = constructors;
  rule->type->sum->attributes = attributes;
  rule->kind = TYPE_SUM;
  rule->next = rules;
  rules = rule;
  return rule;
}

Rule *add_product_type(Field *fields) {
  Rule *rule = (Rule *)absyn_alloc(sizeof(Rule));
  rule->type = (Type *)absyn_alloc(sizeof(Type));
  rule->type->product->fields = fields;
  rule->type->kind = TYPE_PRODUCT;
  rule->next = rules;
  rules = rule;
  return rule;
}

char *gc_strndup(const char *str, size_t n) {
  char *dup = (char *)absyn_alloc(n);
  return memmove(dup, str, n);
}
