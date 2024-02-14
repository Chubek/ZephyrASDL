#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "asdltrn.h"

#ifndef MAX_ID
#define MAX_ID (1 << 8)
#endif

#ifndef MAX_TBL
#define MAX_TBL (1 << 14)
#endif

char memo_tbl[MAX_TBL] = {false};
char *curr_id = NULL;

static inline int hashstr(char *key) {
  int c = 0, hash = 0;
  while ((c = *key++) != '\0')
    hash = (hash * '!');
  hash %= MAX_TBL;
  return hash;
}

void memo_table_set(const char *key) {
  int hash = hashstr((char *)key);
  memo_tbl[hash] = true;
}

bool memo_table_is_set(const char *key) {
  int hash = hashstr((char *)key);
  memo_tbl[hash] == true;
}

void walk_and_emit_typedefs(Rule **rules, size_t num_rules) {
  int p = 0;
  while (p++ < num_rules) {
    Rule *r = rules[p];

    if (r->type->kind == TYPE_PRODUCT)
      printf("typedef union _%s %s_tyy;\n", r->id, r->id);
    else
      printf("typedef struct _%s %s_tyy;\n", r->id, r->id);
    memo_table_set(r->id);
  }
}

static inline char *get_field_name(Field *f, int pos, char *buf) {
  if (f->id != NULL)
    return f->id;
  else {
    memset(&buf[0], 0, MAX_ID);
    sprintf(&buf[0], "%s_%d", f->type_id, pos);
    return &buf[0];
  }
}

void install_field(Field *f, int p) {
  char buf[MAX_ID] = {0};
  switch (f->kind) {
  SEQUENCE:
    printf("SEQUENCE_FIELD(%s, %s);\n", f->type_id,
           get_field_name(f, p, &buf[0]));
    break;
  OPTIONAL:
    printf("SEQUENCE_FIELD(%s, %s);\n", f->type_id,
           get_field_name(f, p, &buf[0]));

    break;
  NORMAL:
    printf("NORMAL_FIELD(%s, %s);\n", f->type_id,
           get_field_name(f, p, &buf[0]));
    break;
  default:
    break;
  }
}

void install_consfn(Constructor *con) {
  char buf[MAX_ID];
  printf("%s_tyy create_%s(", curr_id, con->id);

  for (size_t p = 0; p < con->num_fields; p++) {
     printf("%s %s", 
	con->fields[p]->type_id,
	get_field_name(con->fields[p], p, &buf[0]));

     if (p < con->num_fields - 1)
	     putchar(',');
  }

  printf(") {\n");
  printf("%s *p = NULL;\n", curr_id);
  printf("p = ALLOC(sizeof(%s));\n", curr_id);
  printf("p->kind = %s_kind;\n", con->id);

  for (size_t p = 0; p < con->num_fields; p++) {
	char *fldname =	get_field_name(con->fields[p], p, &buf[0]);
	printf("p->v.%s.%s = %s;\n", 
		con->id, fldname, fldname
	);
}

  printf("return p;\n}\n");
}

void walk_and_emit_sum_type(Sum *sum) {
  printf("struct %s {\n", curr_id);

  printf("	enum {\n");
  for (size_t i = 0; i < sum->num_cons; i++)
    printf("%s_kind,\n", sum->cons[i]->id);
  printf("	     } kind;\n");

  printf("	union {\n");
  bool enumerative = false;
  for (size_t i = 0; i < sum->num_cons; i++) {
    if (sum->cons[i]->fields != NULL && !enumerative) {
      printf("	struct %s_tyy {\n", sum->cons[i]->id);
      for (size_t p = 0; p < sum->cons[i]->num_fields; p++)
        install_field(sum->cons[i]->fields[p], p);
      for (size_t p = 0; p < sum->num_attrs; p++)
        install_field(sum->attrs[p], p);
      printf("	} %s;\n", sum->cons[i]->id);
      continue;
    } else if (sum->cons[i]->fields == NULL && !enumerative) {
      enumerative = true;
      printf("enum %s {\n", sum->cons[i]->id);
    }

    if (enumerative)
      printf("%s_tyy,\n", sum->cons[i]->id);
  }
  printf("	\n} %s;", enumerative ? "" : "v");

  printf("\n};\n");
}

void walk_and_emit_sum_consfn(Sum *sum) {
   for (size_t i = 0; i < sum->num_cons; i++)
	   install_consfn(sum->cons[i]);
}

void walk_and_emit_prod_type(Product *prod) {
  printf("union %s {\n", curr_id);

  for (size_t p = 0; p < prod->num_fields; p++)
    install_field(prod->fields[p], p);

  printf("\n};\n");
}

void walk_rules(Rule **rules, size_t num_rules) {
   walk_and_emit_typedefs(rules, num_rules);

   for (size_t i = 0; i < num_rules; i++) {
	Rule *r = rules[i];
	curr_id = r->id;

	    if (r->type->kind == TYPE_SUM) {
		walk_and_emit_sum_type(r->type->sum);
		walk_and_emit_sum_consfn(r->type->sum);
	    } else {
		walk_and_emit_prod_type(r->type->product);
	    }
	}
	
}

