#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asdl.h"

#define MAX_KEY 32

#alloc absyn_heap, absyn_alloc, absyn_realloc, absyn_dump
#hashfunc absyn_hash

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

TypeId *create_typeid(TypeIdKind kind, char *value) {
  TypeId *id = absyn_alloc(sizeof(TypeId));
  id->kind = kind;
  id->value = value;
}

Field *add_field(TypeId *type_id, int modifier, char *id) {
  Field *field = (Field *)absyn_alloc(sizeof(Field));
  field->type_id = type_id;
  field->kind = modifier;
  field->id = id;
  field->cache[0] = field->cache[1] = NULL;
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
  rule->type->sum = (Sum *)absyn_alloc(sizeof(Sum));
  rule->type->sum->constructors = constructors;
  rule->type->sum->attributes = attributes;
  rule->type->kind = TYPE_SUM;
  rule->next = rules;
  rules = rule;
  return rule;
}

Rule *add_product_type(Field *fields) {
  Rule *rule = (Rule *)absyn_alloc(sizeof(Rule));
  rule->type = (Type *)absyn_alloc(sizeof(Type));
  rule->type->product = (Product *)absyn_alloc(sizeof(Product));
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


Symtable *stab = NULL;


Symtable *symtable_init(void) {
   stab = (Symtable *)absyn_alloc(sizeof(Symtable));
}

void symtable_insert(const char *key, const void *value) {
   Symtable *node = (Symtable *)absyn_alloc(sizeof(Symtable));
   node->key = gc_strndup(key, strlen(key));
   node->value = value;
   node->next = stab;
   stab = node;
}

void *symtable_retrieve(const char *key) {
   for (Symtable *st = stab; st != NULL; st = st->next)
	   if (!strncmp(key, st->key, MAX_KEY))
		   return st->value;
   return NULL;
}

