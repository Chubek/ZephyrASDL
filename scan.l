%{
#include "asdl.h"
#include "parse.tab.h"
%}

%x EMBED

%%
"%{"			   { BEGIN(EMBED); fputs("/* embed-begin */\n", yyout); }
<EMBED>"%}"		   { BEGIN(INITIAL); fputs("\n/* embed-end */\n\n", yyout); }
<EMBED>.		   { emit_prelude(*yytext); }

"attributes"               { return ATTRIBUTES; }
^[a-z][a-z0-9_]*	   { yylval.str_val = gc_strndup(yytext, yyleng); return INIT_IDENT; }
[a-z_][a-z0-9_]*  	   { yylval.str_val = gc_strndup(yytext, yyleng); return TYPE_IDENT; }
([A-Z][a-z]+)*[A-Z][a-z]*  { yylval.str_val = gc_strndup(yytext, yyleng); return CONS_IDENT; }
[A-Za-z0-9_]+		   { yylval.str_val = gc_strndup(yytext, yyleng); return NAME; }
[?*()|,:=]		   return *yytext;
[ \t\r\n]                  ;  // Skip whitespace
.                          ;
<<EOF>>			   yyterminate();


%%

int yywrap() {
    return 1;
}

