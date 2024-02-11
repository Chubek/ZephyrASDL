%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

extern ASTNode *yytree;
extern int yylex(void);

void yyerror(const char *s);

%}

%union {
    char *string_val;
    ASTNode *node_val;
}

%token EQUALS PIPE ATTRIBUTES LPAREN RPAREN COMMA QUESTION_MARK STAR
%token <string_val> CON_ID TYPE_ID IDENT

%type <node_val> definitions definition rhs lhs type product_type sum_type constructor_list
%type <node_val> attributes_opt constructor con_id fields field type_id id

%start definitions

%%

definitions : definitions definition   { $$ = yytree = add_child($1, $2); }
            | definition               { $$ = yytree = $1; }
            | /* empty */              { $$ = create_node(NODE_NONE, NULL); }
            ;

definition : lhs EQUALS rhs          { $$ = add_child($1, $3); }
          ;

rhs : type                          { $$ = $1; }
    ;

lhs : TYPE_ID                     { $$ = create_node(NODE_LHS_IDENT, $1); }
    ;

type : sum_type                    { $$ = $1;  }
     | product_type                 { $$ = $1; }
     ;

product_type : fields              { $$ = $1; }
             ;

sum_type : constructor_list attributes_opt { $$ = add_child($1, $2); }
         | constructor                    { $$ = $1; }
         ;

constructor_list : constructor_list PIPE constructor   { $$ = add_child($1, $3); }
                 | constructor                          { $$ = $1; }
                 ;

attributes_opt : ATTRIBUTES fields  { $$ = create_node(NODE_ATTRIBUTES, $2); }
               | /* empty */         { $$ = create_node(NODE_NONE, NULL); }
               ;

constructor : con_id fields         { $$ = add_child($1, $2); }
            | con_id                 { $$ = $1; }
            ;

con_id : CON_ID                     { $$ = create_node(NODE_CON_ID, $1); }
       ;

fields : LPAREN field_list RPAREN  { $$ = $2; }
       ;

field_list : field_list COMMA field   { $$ = add_child($1, $3); }
           | field                     { $$ = $1; }
           ;

field : type_id id                { $$ = add_child($1, $2); }
      ;

type_id : TYPE_ID QUESTION_MARK    { $$ = create_node(NODE_TYPE_ID_OPT, $1); }
       | TYPE_ID STAR               { $$ = create_node(NODE_TYPE_ID_KLEENE, $1); }
       | TYPE_ID                    { $$ = create_node(NODE_TYPE_ID_ORD, $1); }
       ;


id : IDENT                        { $$ = create_node(NODE_IDENT, $1); }
   | /* empty */                  { $$ = create_node(NODE_NONE, NULL); }
   ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at token: %s\n", s, yytext);
}


