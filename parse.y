%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "asdl.h"


extern Field *fields;
extern Constructor *constructors;
extern Rule *rules;

%}

%union {
   char *strval;
   Field *field_val;
   Constructor *cons_val;
   Rule *rule_val;
}

%token <strval> CONS_IDENT TYPE_IDENT INIT_IDENT NAME

%token ATTRIBUTES

%%

rules : rule
      | rules rule

rule : INIT_IDENT assign type semi_opt

semi_opt : ';'
	 |;

assign : '=' | ':'


type  : prod
      | sum

prod : fields

sum : constrs
    | constrs attrs

attrs : ATTRIBUTES fields

constrs : constr
	| constrs '|' constr

constr : CONS_IDENT fields_opt

fields_opt : '(' items ')'
           |;

items : item
      | items ',' item

item : term
     | term name

term : TYPE_IDENT
     | TYPE_IDENT '*'
     | TYPE_IDENT '?'












