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
%token ATTRIBUTES BOOLEAN SIZE USIZE INT8 UINT8 INT16 UINT16 INT32 UINT32 INT64 UINT64 FLOAT32 FLOAT64 FLOAT80 CHAR UCHAR STRING IDENTIFIER BYTEARRAY

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

rule : INIT_IDENT assign type semi_opt	{ $3->id = dash_to_underscore($1); symtable_insert($3->id, NULL); }
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

constr : CONS_IDENT fields_opt		{ add_constructor(dash_to_underscore($1), $2, $2 == NULL); }
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
	| INT8		{ $$ = create_typeid(TYYNAME_INT8, NULL); }
	| UINT8		{ $$ = create_typeid(TYYNAME_UINT8, NULL); }
	| INT16		{ $$ = create_typeid(TYYNAME_INT16, NULL); }
	| UINT16	{ $$ = create_typeid(TYYNAME_UINT16, NULL); }
	| INT32		{ $$ = create_typeid(TYYNAME_INT32, NULL); }
	| UINT32	{ $$ = create_typeid(TYYNAME_UINT32, NULL); }
	| INT64		{ $$ = create_typeid(TYYNAME_INT64, NULL); }
	| UINT64	{ $$ = create_typeid(TYYNAME_UINT64, NULL); }
	| FLOAT32	{ $$ = create_typeid(TYYNAME_FLOAT32, NULL); }
	| FLOAT64	{ $$ = create_typeid(TYYNAME_FLOAT64, NULL); }
	| FLOAT80	{ $$ = create_typeid(TYYNAME_FLOAT80, NULL); }
	| CHAR		{ $$ = create_typeid(TYYNAME_CHAR, NULL); }
	| UCHAR		{ $$ = create_typeid(TYYNAME_UCHAR, NULL); }
	| SIZE		{ $$ = create_typeid(TYYNAME_SIZE, NULL); }
	| USIZE		{ $$ = create_typeid(TYYNAME_USIZE, NULL); }
	| STRING	{ $$ = create_typeid(TYYNAME_STRING, NULL); }
	| IDENTIFIER    { $$ = create_typeid(TYYNAME_IDENTIFIER, NULL); }
	| BYTEARRAY     { $$ = create_typeid(TYYNAME_BYTEARRAY, NULL); }
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













