%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "absyn.h"

extern int yylex(void);
extern void yyerror(const char *s);

void yyerror(const char *s);

%}

%union {
    char *string_val;
    ASTNode *node_val;
    ASTNodeList *node_list;
}

%token EQUALS PIPE ATTRIBUTES LPAREN RPAREN COMMA QUESTION_MARK STAR SEMICOLON
%token <string_val> CON_ID TYPE_ID

%type <string_val> name_opt lhs
%type <node_list> fields rhs defns
%type <node_val> constr field attrs defn tuple

%start asdl

%%

defns    : defn				{ $$ = $1; }
	 | defns ';' defn		{ $$ = append_list(&$1, $3); }
	 ;

defn     : lhs '=' rhs			{ $$ = create_defn($1, $3); }

lhs      : TYPE_ID			{ $$ = $1; }
	 ;

rhs      : rhs_item			{ $$ = $1; }
	 | rhs '|' rhs_item		{ $$ = append_list(&$1, $3); }
	 ;

rhs_item : constr			{ $$ = $1; }
         | attrs			{ $$ = $1; }
	 | tuple			{ $$ = $1; }
	 ;
        

attrs    : ATTRIBUTES '(' fields ')'    { $$ = create_attr($3); }
	 ;

tuple    : '(' field ',' field ')'	{ $$ = create_tuple($2, $4); }
	 ;

constr   : CON_ID			{ $$ = create_constructor($1, NULL); }
	 | CON_ID '(' fields ')'	{ $$ = create_constructor($1, $3); }
	 ;

fields   : field		{ $$ = $1; }
	 | fields ',' field     { $$ = append_list(&$1, $3); }
	 ;

field    : TYPE_ID name_opt { $$ = create_field($1, $2); }

name_opt : /* empty */		{ $$ = NULL; }
         | TYPE_ID		{ $$ = create_ident($1, 0);   }
	 | TYPE_ID '*'		{ $$ = create_ident_marker($1, '*'); }
	 | TYPE_ID '?'		{ $$ = create_ident_marker($1, '?'); }
         ;





