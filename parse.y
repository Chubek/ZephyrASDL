%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "types.h"

extern FILE* yyin;
extern int yylex();
extern void translate_rule(char*, Type*);
void yyerror(const char* s);

int num_fields = 0;
int num_attrs = 0;
int num_cons = 0;
int num_rules = 0;

Field **fields = NULL;
Field **attributes = NULL;
Type **types = NULL;
Constructor **cons = NULL;
Sum* sumtype = NULL;
Product *prodtype = NULL;

#include "absyn.gen.h"


%}

%union {
    char *str;
    int num;
    enum FieldKind fld_kind;
    Rule *rule;
    Rule **rules;
    Field *field;
    Field **fields;
    Sum *sum;
    Product *product;
    Constructor *con;
    Constructor **cons;
    Type *type;
    Type **types;
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
%type <con> constructor
%type <cons> constructors
%type <prod> product_type
%type <sum> sum_type
%type <field> field
%type <fields> fields
%type <fields> fields_opt
%type <fields> attr_opt
%type <type> type

%%

rules : rule SEMICOLON rules { }
      | /* empty */ { }
      ;

rule : init_id type { translate_rule($1, $2); 
     			num_cons = 0;
			fields = NULL; 
			cons = NULL; 
			types = NULL; 
			attributes = NULL; 
		    }
     ;

init_id : INIT_ID COLON { $$ = $1; }
        | INIT_ID EQUALS { $$ = $1; }
        ;

type : sum_type {  }
     | product_type {  }
     ;

sum_type : constructors attr_opt { add_sum_type($1, 
					num_cons, 
					$2, 
					num_attrs); }
         ;

constructors : constructor PIPE constructors { }
            | constructor { num_cons++; }
            ;

constructor : con_id fields_opt { add_constructor($1, $2, num_fields); }
            ;

attr_opt : ATTRIBUTES fields { attributes = $2; num_attrs = num_fields; }
         | /* empty */ { }
         ;

product_type : fields { add_product_type($1, num_fields); }
             ;

fields_opt : LPAREN fields RPAREN { }
	   | /* empty */	{ }
           ;

fields : field COMMA fields	{ }
        | field			{ num_fields++; }
        ;

field : type_id modifier_opt ident_opt { add_field($1, $2, $3); }
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
