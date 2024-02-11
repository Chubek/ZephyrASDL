%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

extern int yylex(void);

%}

%union {
    char *string_val;
    ASTNode *node_val;
}

%token EQUALS
%token PIPE
%token ATTRIBUTES
%token LPAREN
%token RPAREN
%token COMMA
%token QUESTION_MARK
%token STAR

%token <string_val> CON_ID
%token <string_val> TYPE_ID
%token <string_val> IDENT

%type <node_val> definitions definition rhs lhs type product_type sum_type constructor_list
%type <node_val> attributes_opt constructor con_id fields fields_list field type_id id

%start definitions

%

%%

definitions : definitions definition	{ $$ = addChild($1, $2); }
	    | definition		{ $$ = $1; }
            | /* empty */		{ $$ = createNode(NODE_NONE, NULL); }
            ;

definition : lhs EQUALS rhs 	{ $$ = addChild($1, $3); }
	   ;

rhs : type		{ $$ = createNode(NODE_RHS_TYPE, $1); }
    ;

lhs : TYPE_ID    	{ $$ = createNode(NODE_LHS_IDENT, $1); }
    ;

type : sum_type		{ $$ = createNode(NODE_SUM_TYPE, $1);  }
     | product_type	{ $$ = createNode(NODE_PRODUCT_TYPE, $1); }
     ;

product_type : fields { $$ = $1; }
             ;

sum_type : constructor_list attributes_opt { $$ = addChild($1, $2); }
         | constructor			   { $$ = $1; }
         ;

constructor_list : constructor_list IPE constructor	{ $$ = addChild($1, $2); }
                 | constructor				{ $$ = $1 }
                 ;

attributes_opt : ATTRIBUTES fields	{ $$ = createNode(NODE_ATTRIBUTES, $2); }
               | /* empty */		{ $$ = createNode(NODE_NONE, NULL); }
               ;

constructor : con_id fields		{ $$ = addChild($1, $2); }
            | con_id			{ $$ = $1; }
            ;

con_id : CON_ID		{ $$ = createNode(NODE_CON_ID, $1); }
       ;

fields : LPAREN field_list RPAREN	{ $$ = $2;  }
       ;

field_list : field_list COMMA field	{ $$ = addChild($1, $2);	}
           | field				{ $$ = $1; }
           ;

field : type_id id	{ $$ = addChild($1, $2);	}
      ;

type_id : TYPE_ID QUESTION_MARK { $$ = createNode(NODE_TYPE_ID_OPT, $2); }
       | TYPE_ID STAR	       { $$ = createNode(NODE_TYPE_ID_KLEENE, $2); }
       | TYPEID		       { $$ = createNode(NODE_TYPE_ID_ORD, $2); }
       ;


id : /* empty */	{ $$ = createNode(NODE_NONE, NULL);  }
   | IDENT		{ $$ = createNode(NODE_IDENT, yytext); }
   ;

%%
 

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

