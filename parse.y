%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "asdl.h"

static inline char *dash_to_underscore(char *ident) {
   char *replaced = ident;

   while (*replaced++)
   	*replaced = *replaced == '-' ? '_' : *replaced;

   return ident;   
}

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
   TypeId *typeid_val;
}

%token <str_val> CONS_IDENT TYPE_IDENT INIT_IDENT
%token ATTRIBUTES BOOL SIZE USIZE INT UINT SHORT USHORT CHAR UCHAR FLOAT DOUBLE STRING IDENTIFIER

%type <str_val> name_opt
%type <field_val> fields fields_opt attrs
%type <cons_val> constructors
%type <rule_val> sum prod type
%type <typeid_val> type_id

%start asdl

%%

asdl : rules
     |;

rules : rule
      | rules rule
      ;

rule : INIT_IDENT assign type semi_opt	{ $3->id = dash_to_underscore($1); symtable_insert($1, NULL); }
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

constr : CONS_IDENT fields_opt		{ add_constructor(dash_to_underscore($1), $2); }
       ;

fields_opt : '(' items ')'		{ $$ = fields; fields = NULL; }
           |				{ $$ = NULL; }
	   ;

fields : '(' items ')'			{ $$ = fields; fields = NULL; }
       ;

items : item
      | items ',' item
      ;

item : type_id  name_opt	 	{ add_field($1, FIELD_NORMAL, $2);	}
     | type_id '*' name_opt		{ add_field($1, FIELD_SEQUENCE, $3); 	}
     | type_id '?' name_opt		{ add_field($1, FIELD_OPTIONAL, $3);    }
     ;

type_id : TYPE_IDENT	{ $$ = create_typeid(TYYNAME_ID, dash_to_underscore($1)); }
	| BOOL		{ $$ = create_typeid(TYYNAME_BOOL, NULL); }
	| SIZE		{ $$ = create_typeid(TYYNAME_SIZE, NULL); }
	| USIZE		{ $$ = create_typeid(TYYNAME_USIZE, NULL); }
	| INT		{ $$ = create_typeid(TYYNAME_INT, NULL); }
	| UINT		{ $$ = create_typeid(TYYNAME_UINT, NULL); }
	| SHORT		{ $$ = create_typeid(TYYNAME_SHORT, NULL); }
	| USHORT	{ $$ = create_typeid(TYYNAME_USHORT, NULL); }
	| CHAR		{ $$ = create_typeid(TYYNAME_CHAR, NULL); }
	| UCHAR		{ $$ = create_typeid(TYYNAME_UCHAR, NULL); }
	| FLOAT		{ $$ = create_typeid(TYYNAME_FLOAT, NULL); }
	| DOUBLE	{ $$ = create_typeid(TYYNAME_DOUBLE, NULL); }
	| STRING	{ $$ = create_typeid(TYYNAME_STRING, NULL); }
	| IDENTIFIER    { $$ = create_typeid(TYYNAME_IDENTIFIER, NULL); }
	;

name_opt : TYPE_IDENT	{ $$ = dash_to_underscore($1); }
	 |		{ $$ = NULL; }
	 ;

%%


int yyerror(const char *msg) {
   fputs("Parsing error occurred: ", stderr);
   fputs(msg, stderr);
   fputs("\n", stderr);
   exit(EXIT_FAILURE);
}













