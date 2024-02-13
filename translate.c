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

void walk_and_emit_typedefs(Rule **rules, size_t num_rules) {
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

static inline void get_field_name(Field *f, int pos, char *buf) {
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
		printf("SEQUENCE_FIELD(%s, %s);\n",
				f->type_id, 
				get_field_name(f, p, &buf[0]));
		break;
	    OPTIONAL:
		printf("SEQUENCE_FIELD(%s, %s);\n",
				f->type_id, 
				get_field_name(f, p, &buf[0]));

	 	break;
	    NORMAL:
		printf("NORMAL_FIELD(%s, %s);\n",
				f->type_id,
				get_field_name(f, p, &buf[0]));
	        break;
	    default:
		break;
   }

}

void walk_and_emit_sum_type(Sum *sum) {
    printf("struct %s {\n", curr_id);
    

    printf("	enum {\n");
    for (size_t i = 0; i < sum->num_cons; i++)
	    printf("%s_kind,\n", sum->cons[i]);
    printf("	     } kind;\n");
    
    printf("	union {\n");
    for (size_t i = 0; i < sum->num_cons; i++) {
    	for (size_t p = 0; p < sum->cons[i]->num_field; p++)
		install_field(sum->cons[i]->fields[p], p);
	for (size_t p = 0; p < sum->num_attrs; p++)
		install_field(sum->attrs[p], p);
    }    
    printf("	\n} tyy;");

    printf("\n};\n");
}

void walk_and_emit_prod_type(Product *prod) {
   printf("union %s {\n", curr_id);

   for (size_t p = 0; p < prod->num_fields; p++)
	   install_field(prod->fields[i], p);

   printf("\n};\n");
}







