%{
#include "y.tab.h"
%}

%%

[A-Z][a-zA-Z0-9_]*   { yylval.string_val = strdup(yytext); return CON_ID; }
[a-z][a-z0-9_]*      { yylval.string_val = strdup(yytext); return TYPE_ID; }
[A-Za-z_][a-zA-Z0-9_] { yylva.string_val = strdup(yytext); return IDENT;  }
"="                  { return EQUALS; }
"|"                  { return PIPE; }
"attributes"         { return ATTRIBUTES; }
"("                  { return LPAREN; }
")"                  { return RPAREN; }
","                  { return COMMA; }
"?"                  { return QUESTION_MARK; }
"*"                  { return STAR; }
.                    { /* ignore all other characters */ }

%%

int yywrap(void) {
    return 1;
}

