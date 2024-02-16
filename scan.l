
%{

typedef struct Rule Rule;
typedef struct Type Type;
typedef struct Sum Sum;
typedef struct Product Product;
typedef struct Constructor Constructor;
typedef struct Field Field;

extern char *dup_str(char *, int);

#include "y.tab.h"

%}

%x EMBED

%%

"attributes"               { return ATTRIBUTES; }
^[a-z][a-z0-9_]*	   { yylval.str = dup_str(yytext, yyleng); return INIT_ID; }
[a-z_][a-z0-9_]*  	   { yylval.str = dup_str(yytext, yyleng); return TYPE_ID; }
[A-Z][a-zA-Z0-9_]*         { yylval.str = dup_str(yytext, yyleng); return CON_ID; }
"("                        { return LPAREN; }
")"                        { return RPAREN; }
","                        { return COMMA; }
"|"                        { return PIPE; }
"?"                        { return QUESTION; }
"*"                        { return STAR; }
"%%"			   { BEGIN(EMBED); }
<EMBED>"%%"		   { BEGIN(INITIAL); }
<EMBED>.		   { fputc(*yytext, yyout); }
[=:]			   { return ASSIGN; }
[ \t\r\n]                  ;  // Skip whitespace
.                          ;
<<EOF>>			   yyterminate();


%%

int yywrap() {
    return 1;
}

