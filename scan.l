%option noyywrap

%{
#include "y.tab.h"
%}

%%

[a-zA-Z][a-zA-Z0-9_]*     { yylval.str = strdup(yytext); return TYPE_ID; }
[A-Z][a-zA-Z0-9_]*         { yylval.str = strdup(yytext); return CON_ID; }
"attributes"               { return ATTRIBUTES; }
"("                        { return LPAREN; }
")"                        { return RPAREN; }
","                        { return COMMA; }
"="                        { return EQUALS; }
":"                        { return COLON; }
";"                        { return SEMICOLON; }
"|"                        { return PIPE; }
"?"                        { return QUESTION; }
"*"                        { return STAR; }
[ \t\r\n]                  ;  // Skip whitespace
.                          ;

%%

int yywrap() {
    return 1;
}

