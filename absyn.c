#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#alloc field_heap, field_alloc, field_realloc, field_dump
#alloc con_heap, con_alloc, con_realloc, con_dump
#alloc type_heap, type_alloc, type_realloc, type_dump
#alloc rule_heap, rule_alloc, rule_realloc, rule_dump
#alloc attrs_heap, attrs_alloc, attrs_realloc, attrs_dump
#alloc prod_heap, prod_alloc, prod_realloc, prod_dump
#alloc sum_heap, sum_alloc, sum_realloc, sum_dump
#alloc strn_heap, strn_alloc, strn_realloc, strn_dump
#hashfunc tree_hash

#include "asdltrn.h"


Rule *rule = NULL, **rules = NULL;
Type *type = NULL, **types = NULL;
Sum  *sumtype = NULL, **sumtypes = NULL;
Product *prodtype = NULL, **prodtypes = NULL;
Constructor *con = NULL, **cons = NULL;
Field *field = NULL, **fields = NULL, **attrs = NULL;

char modifier = 0, *lowercase_id = NULL, *pascalcase_id = NULL, *cstyle_id = NULL, *cons_id = NULL, *type_id = NULL, *identifier = NULL;

enum FieldKind MOD_LUT['~'] = {
  ['*'] = SEQUENCE,
  ['?'] = OPTIONAL,
};

size_t num_fields = 0, num_cons = 0, num_types = 0, num_rules = 0, num_attrs = 0;

char *dup_str(char *s, size_t n) {
    char *d = strn_alloc(n + 1);
    return memmove(&d[0], &s[0], n);
}

int last_type_kind = 0;

#define SUM 1
#define PRODUCT 2

extern FILE *yyin, *yyout;
extern void walk_rules(Rule**, size_t);

#define YY_INPUT(buf, result, max_size)        		\
{                                       	      	\
   int yyc = fgetc(yyin);              	        	\
   result = (EOF == yyc) ? -1 : (*(buf) = yyc, 1); 	\
}


#include "parse.peg.h"


void parse_and_translate(void) {
    while (yyparse());

    printf("2");

    walk_rules(rules, num_rules);
	
    field_dump();
    con_dump();
    type_dump();
    rule_dump();
    attrs_dump();
    prod_dump();
    sum_dump();
    strn_dump();
}
