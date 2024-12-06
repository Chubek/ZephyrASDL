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

extern size_t line_no;

%}

%union {
   char *str_val;
   Field *field_val;
   Constructor *cons_val;
   Rule *rule_val;
   TypeId *typeid_val;
}

%token <str_val> VARIANT_NAME FIELD_TYPE FIELD_NAME RULE_NAME
%token ATTRIBUTES BOOL SIZE USIZE INT8 UINT8 INT16 UINT16 INT32 UINT32 INT64 UINT64 FLOAT32 FLOAT64 FLOAT80 CHAR UCHAR STRING IDENTIFIER BYTEARRAY
%token SUM_LPAREN SUM_RPAREN
%token REC_LPAREN REC_RPAREN
%token INIT_ASSIGN
%token REC_TERM SUM_TERM

%type <str_val> name_opt
%type <field_val> fields_attr fields_prod fields_sum_opt attrs
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

rule : RULE_NAME INIT_ASSIGN type term	{ $3->id = dash_to_underscore($1); symtable_insert($3->id, NULL); }
     ;

term : REC_TERM
     | SUM_TERM
     ;


type  : prod	{ $$ = $1; }
      | sum	{ $$ = $1; }
      ;

prod : fields_prod			{ $$ = add_product_type($1);  }
     ;

sum : constructors			{ $$ = add_sum_type($1, NULL); }
    | constructors attrs		{ $$ = add_sum_type($1, $2); }
    ;

attrs : ATTRIBUTES fields_attr		{ $$ = fields; fields = NULL;  }
      ;

constructors : variants			{ $$ = constructors; constructors = NULL; }
	     ;

variants : variant
	| variants '|' variant
	;

variant : VARIANT_NAME fields_sum_opt		{ add_constructor(dash_to_underscore($1), $2, $2 == NULL); }
       ;

fields_sum_opt : SUM_LPAREN items SUM_RPAREN		{ $$ = fields; fields = NULL; }
               |					{ $$ = NULL; }
	       ;


fields_prod : REC_LPAREN items REC_RPAREN { $$ = fields; fields = NULL; }
	   ;

fields_attr : SUM_LPAREN items SUM_RPAREN { $$ = fields; fields = NULL; }
	    ;

items : item
      | items ',' item
      ;

item : type_id  name_opt	 	{ add_field($1, FIELD_NORMAL, $2);	}
     | type_id '*' name_opt		{ add_field($1, FIELD_SEQUENCE, $3); 	}
     | type_id '?' name_opt		{ add_field($1, FIELD_OPTIONAL, $3);    }
     ;

type_id : FIELD_TYPE	{ $$ = create_typeid(TYPENAME_ID, dash_to_underscore($1)); }
	| BOOL	{ $$ = create_typeid(TYPENAME_BOOL, NULL); }
	| INT8		{ $$ = create_typeid(TYPENAME_INT8, NULL); }
	| UINT8		{ $$ = create_typeid(TYPENAME_UINT8, NULL); }
	| INT16		{ $$ = create_typeid(TYPENAME_INT16, NULL); }
	| UINT16	{ $$ = create_typeid(TYPENAME_UINT16, NULL); }
	| INT32		{ $$ = create_typeid(TYPENAME_INT32, NULL); }
	| UINT32	{ $$ = create_typeid(TYPENAME_UINT32, NULL); }
	| INT64		{ $$ = create_typeid(TYPENAME_INT64, NULL); }
	| UINT64	{ $$ = create_typeid(TYPENAME_UINT64, NULL); }
	| FLOAT32	{ $$ = create_typeid(TYPENAME_FLOAT32, NULL); }
	| FLOAT64	{ $$ = create_typeid(TYPENAME_FLOAT64, NULL); }
	| FLOAT80	{ $$ = create_typeid(TYPENAME_FLOAT80, NULL); }
	| CHAR		{ $$ = create_typeid(TYPENAME_CHAR, NULL); }
	| UCHAR		{ $$ = create_typeid(TYPENAME_UCHAR, NULL); }
	| SIZE		{ $$ = create_typeid(TYPENAME_SIZE, NULL); }
	| USIZE		{ $$ = create_typeid(TYPENAME_USIZE, NULL); }
	| STRING	{ $$ = create_typeid(TYPENAME_STRING, NULL); }
	| IDENTIFIER    { $$ = create_typeid(TYPENAME_IDENTIFIER, NULL); }
	| BYTEARRAY     { $$ = create_typeid(TYPENAME_BYTEARRAY, NULL); }
	;

name_opt : FIELD_NAME	{ $$ = dash_to_underscore($1); }
	 |		{ $$ = NULL; }
	 ;

%%


int yyerror(const char *msg) {
   fputs("Parsing error occurred: ", stderr);
   fputs(msg, stderr);
   fputs("\n", stderr);
   fprintf(stderr, "At line: %lu\n", line_no);
   exit(EXIT_FAILURE);
}













