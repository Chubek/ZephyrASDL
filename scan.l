%option noyywrap

%{
#include "y.tab.h"
%}

%%

^[a-z][a-z0-9_]*	   { yylval.str = dup_str(yytext, yyleng); return INIT_ID; }
[a-z_][a-z0-9_]*  	   { yylval.str = dup_str(yytext, yyleng); return TYPE_ID; }
[A-Z][a-zA-Z0-9_]*         { yylval.str = dup_str(yytext, yyleng); return CON_ID; }
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

