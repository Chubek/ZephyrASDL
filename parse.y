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

%token <strval> CONS_IDENT TYPE_IDENT INIT_IDENT

%%
