%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern FILE* yyin;
extern int yylex();
extern void translate_rule(Rule* rule);
void yyerror(const char* s);

#include "types.h"

int num_fields = 0;
int num_attrs = 0;
int num_cons = 0;

Field **fields = NULL;
Type **types == NULL;
Constructor **cons = NULL;

#include "absyn.gen.h"


%}

%union {
    char *str;
    int num;
    enum FieldKind fld_kind;
    Rule *rule;
    Rule **rules;
    Field *field;
    Filed **fields;
    Sum *sum;
    Product *product;
    Constructor *constructor;
    Consturctor **constructors;
}

%token <str> TYPE_ID
%token <str> CON_ID
%token <str> INIT_ID
%token ATTRIBUTES LPAREN RPAREN COMMA EQUALS COLON SEMICOLON PIPE QUESTION STAR

%type <str> ident_opt
%type <fld_kind> modifier_opt
%type <str> type_id
%type <str> con_id
%type <str> init_id
%type <constructor> constructor
%type <fields> field_pair
%type <fields> fields
%type <field> field

%%

rules : rule SEMICOLON rules { }
      | /* empty */ { }
      ;

rule : init_id type { translate_rule($1, $2); 
     			num_rules = 0; num_cons = 0;
			fields = NULL; cons = NULL; types = NULL; }
     ;

init_id : INIT_ID COLON { $$ = $1; }
        | INIT_ID EQUALS { $$ = $1; }
        ;

type : sum_type {  }
     | product_type {  }
     ;

sum_type : constructor PIPE constructors attr_opt { add_sum_type($2, 
	 						num_cons, 
							$4, 
							num_attrs); }
         ;

constructors : constructor PIPE constructors { }
            | constructor { num_cons++; }
            ;

constructor : con_id field_pair_opt { add_constructor($1, $2, num_fields); }
            ;

attr_opt : ATTRIBUTES fields { attributes = $2; num_attrs = num_fields; }
         | /* empty */ { }
         ;

product_type : fields { add_product_type($1, num_fields); }
             ;

fields : LPAREN fields_ RPAREN { }
       ;

fields_ : field COMMA fields_	{ }
        | field			{ num_fields++; }
        ;

field : TYPE_ID modifier_opt ident_opt { add_field($1, $2, $3); }
      ;

modifier_opt : STAR		{ $$ = SEQUENCE; }
	     | QUESTION 	{ $$ = OPTIONAL; }
	     | /* empty */ 	{ $$ = NORMAL; }

ident_opt : TYPE_ID { $$ = $1; }
          | /* empty */ { $$ = NULL; }
          ;

type_id : TYPE_ID { $$ = $1; }
        ;

con_id : CON_ID { $$ = $1; }
       ;

%%


void yyerror(const char* s) {
    printf("Error: %s\n", s);
}
