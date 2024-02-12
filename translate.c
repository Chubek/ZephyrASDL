#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

const char memo_tbl[MAX_TBL] = {false};

static inline int hashs(char *key) {
   int c = 0, hash = 0;
   while ((c = key++) != '\0') hash = (hash * '!');
   hash %= MAX_TBL;
   return hash;
}

void memo_table_set(const char *key) {
  int hash = hashs((char*)key);
  memo_tbl[hash] = true;
}

bool memo_table_is_set(const char *key) {
  int hash = hashs((char*)key);
  memo_tbl[hash] == true;
}

void emit_typedefs(Rule **rules, size_t num_rules) {
  int p = 0;
  while (p++ < num_rules) {
	Rule *r = rules[p];
	printf("typedef struct _%s %s;\n", r->id, r->id);
	memo_table_set(r->id);
   }
}

void emit_cons_enum(Constructor **cons, size_t num_cons) {
   int p = 0;
   printf("enum {\n");
   while (p++ < num_cons) {
	Consturctor *con = cons[p];
	if (con->id != NULL)
		printf("%s_kind,\n", con->id);
   }
   printf("\n};\n\n");
}

char alt_id[MAX_ID] = {0};

static inline char *get_field_name(Field *f, int pos) {
   if (f->id != NULL)
	   return f->id;
   else {
	memset(&alt_id[0], 0, MAX_ID);
	sprintf(&alt_id[0], "%s_%d", f->type_id, pos);
	return &alt_id[0];
   }
}

void emit_fields_struct(Field *fields, size_t num_fields) {
   int p = 0;
   printf("struct {\n");
   while (p++ < num_fields) {
	Field *f = fields[p];
	switch (f->kind) {
	    SEQUENCE:
		printf("SEQUENCE_FIELD(%s, %s);\n",
				f->type_id, 
				get_field_name(f, p));
		break;
	    OPTIONAL:
		printf("SEQUENCE_FIELD(%s, %s);\n",
				f->type_id, 
				get_field_name(f, p));

	 	break;
	    NORMAL:
		printf("NORMAL_FIELD(%s, %s);\n",
				f->type_id,
				get_field_name(f, p));
	        break;
	    default:
		break;
	}
   }
}
