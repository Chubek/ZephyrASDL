%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "asdl.h"

extern int yylex(void);
int yyerror(const char *);

extern Field *fields;
extern Constructor *constructors;
extern Rule *rules;

%}

%union {
   char *str_val;
   Field *field_val;
   Constructor *cons_val;
   Rule *rule_val;
}

%token <str_val> CONS_IDENT TYPE_IDENT INIT_IDENT NAME
%token ATTRIBUTES

%type <str_val> name_opt
%type <field_val> fields fields_opt attrs
%type <cons_val> constructors
%type <rule_val> sum prod type

%start asdl

%%

asdl : rules
     |;

rules : rule
      | rules rule
      ;

rule : INIT_IDENT assign type semi_opt	{ $3->id = $1; }
     ;

semi_opt : ';'
	 |;

assign : '=' | ':'
       ;


type  : prod	{ $$ = $1; }
      | sum	{ $$ = $1; }
      ;

prod : fields				{ $$ = add_product_type($1);  }
     ;

sum : constructors			{ $$ = add_sum_type($1, NULL); }
    | constructors attrs		{ $$ = add_sum_type($1, $2); }
    ;

attrs : ATTRIBUTES fields		{ $$ = fields; fields = NULL;  }
      ;

constructors : constrs			{ $$ = constructors; constructors = NULL; }
	     ;

constrs : constr
	| constrs '|' constr
	;

constr : CONS_IDENT fields_opt		{ add_constructor($1, $2); }
       ;

fields_opt : '(' items ')'		{ $$ = fields; fields = NULL; }
           |				{ $$ = NULL; }
	   ;

fields : '(' items ')'			{ $$ = fields; fields = NULL; }
       ;

items : item
      | items ',' item
      ;

item : TYPE_IDENT  name_opt	 	{ add_field($1, FIELD_NORMAL, $2);	}
     | TYPE_IDENT '*' name_opt		{ add_field($1, FIELD_SEQUENCE, $3); 	}
     | TYPE_IDENT '?' name_opt		{ add_field($1, FIELD_OPTIONAL, $3);    }
     ;

name_opt : NAME		{ $$ = $1; }
	 |		{ $$ = NULL; }
	 ;

%%


int yyerror(const char *msg) {
   fputs("Parsing error occurred: ", stderr);
   fputs(msg, stderr);
   fputs("\n", stderr);
   exit(EXIT_FAILURE);
}













