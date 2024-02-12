%{
#include "ast.h"
#include "y.tab.h"
%}

ident [a-zA-Z_][a-zA-Z0-9]*

%%

[A-Z]{ident}         { yylval.string_val = strdup(yytext); return CON_ID; }
[[a-z]{ident}        { yylval.string_val = strdup(yytext); return TYPE_ID; }
"attributes"         { return ATTRIBUTES; }
.                    { return *yytext; }

%%

int yywrap(void) {
    return 1;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at token: %s\n", s, yytext);
}
