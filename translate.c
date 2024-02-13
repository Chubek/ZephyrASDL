#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

char memo_tbl[MAX_TBL] = {false};
char *curr_id = NULL;

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

	if (r->num_types == 1 && r->types[0]->kind = TYPE_PRODUCT)
	    printf("typedef union _%s %s;\n", r->id, r->id);
	else
	    printf("typedef struct _%s %s;\n", r->id, r->id);
	memo_table_set(r->id);
   }
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

void emit_single_field(Field *f, int p) {
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

void emit_con(Constructor *con, Attributes *attrs) {
    printf("		struct {\n");
    for (size_t p = 0; p < con->num_fields; p++)
	    print_single_field(cons->fields[p], p);
    printf("		} %s;\n", con->id)
}

void emit_sum_type(Sum *sum) {
    printf("struct %s {\n", curr_id);
    

    printf("	enum {\n");
    for (size_t i = 0; i < sum->num_cons; i++)
	    printf("%s_kind, ", sum->cons[i]);
    printf("	     } kind;\n");
    
    printf("	union {\n");
    for (size_t i = 0; i < sum->num_cons; i++)
	    emit_con(sum->cons[i], sum->attrs);
    printf("	\n};");

    printf("\n};\n");
}


