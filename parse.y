%{
#include "types.h"

extern int num_fields;
extern int num_attrs;
extern int num_cons;
extern int num_rules;

extern Field **fields;
extern Field **attributes;
extern Type *type;
extern Type **types;
extern Constructor *con;
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
}

%token <str> TYPE_ID
%token <str> CON_ID
%token <str> INIT_ID
%token ATTRIBUTES LPAREN RPAREN COMMA ASSIGN SEMICOLON PIPE QUESTION STAR

%type <str> ident_opt
%type <num> modifier_opt
%type <str> type_id
%type <str> con_id

%start rules

%%

rules : rule rules { }
      | /* empty */ { }
      ;

rule : INIT_ID ASSIGN type semi_opt  { translate_rule($1, type);   }     
     ;

semi_opt : SEMICOLON
	 | /* empty */

type : sum_type {  }
     | product_type {  }
     ;

sum_type : constructors attr_opt { add_sum_type(cons, 
					num_cons, 
					attributes, 
					num_attrs);
					num_cons = 0;
					cons = NULL;
					attributes = NULL;}

constructors : constructor PIPE constructors { }
            | constructor {  }
            ;

constructor : con_id fields_opt { add_constructor($1, fields, num_fields); fields = NULL; num_fields = 0; }
            ;

attr_opt : ATTRIBUTES fields { attributes = dup_mem(fields, num_fields, sizeof(Field*)); num_attrs = num_fields; fields = NULL; num_fields = 0; }
         | /* empty */ { }
         ;

product_type : fields_par { add_product_type(fields, num_fields); }
             ;

fields_opt : LPAREN fields RPAREN { }
	   | /* empty */	{ }
           ;

fields_par : LPAREN fields RPAREN { }
	   ;

fields : field COMMA fields	{ }
        | field			{ }
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
