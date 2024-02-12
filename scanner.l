%{
#include "ast.h"
#include "y.tab.h"
%}

ident [a-zA-Z_][a-zA-Z0-9]*

%%

[A-Z]{ident}         { yylval.string_val = strdup(yytext); return CON_ID; }
[[a-z]{ident}        { yylval.string_val = strdup(yytext); return TYPE_ID; }
"="                  { return EQUALS; }
"|"                  { return PIPE; }
"attributes"         { return ATTRIBUTES; }
"("                  { return LPAREN; }
")"                  { return RPAREN; }
","                  { return COMMA; }
"?"                  { return QUESTION_MARK; }
"*"                  { return STAR; }
";"		     { return SEMICOLON; }
.                    { /* ignore all other characters */ }

%%

int yywrap(void) {
    return 1;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at token: %s\n", s, yytext);
}