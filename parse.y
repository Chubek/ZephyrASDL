%{
#include "types.h"

extern int num_fields;
extern int num_attrs;
extern int num_cons;
extern int num_rules;

extern Field **fields;
extern Field **attributes;
extern Type **types;
extern Constructor **cons;
extern Sum* sumtype;
extern Product *prodtype;

extern FILE* yyin;
extern int yylex();
extern void translate_rule(char*, Type*);
void yyerror(const char* s);
%}

%union {
 char *str;
 int num;
 Rule *rulev;
 Rule **rulesv;
 Field *fieldv;
 Field **fieldsv;
 Sum *sumv;
 Product *productv;
 Constructor *conv;
Constructor **consv;
 Type *typev;
 Type **typesv;
}

%token <str> TYPE_ID
%token <str> CON_ID
%token <str> INIT_ID
%token ATTRIBUTES LPAREN RPAREN COMMA ASSIGN SEMICOLON PIPE QUESTION STAR

%type <str> ident_opt
%type <num> modifier_opt
%type <str> type_id
%type <str> con_id
%type <conv> constructor
%type <consv> constructors
%type <prodv> product_type
%type <sumv> sum_type
%type <fieldv> field
%type <fieldsv> fields
%type <fieldsv> fields_opt
%type <fieldsv> attr_opt
%type <typev> type

%start rules

%%

rules : rule SEMICOLON rules { }
      | /* empty */ { }
      ;

rule : INIT_ID ASSIGN type { translate_rule($1, $3); 
     			num_cons = 0;
			fields = NULL; 
			cons = NULL; 
			types = NULL; 
			attributes = NULL; 
		    }
     ;

type : sum_type {  }
     | product_type {  }
     ;

sum_type : constructors attr_opt { add_sum_type($1, 
					num_cons, 
					$2, 
					num_attrs); }

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

modifier_opt : STAR		{ $$ = '*'; }
	     | QUESTION 	{ $$ = '?'; }
	     | /* empty */ 	{ $$ = 128; }

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
