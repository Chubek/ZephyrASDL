%{
#include "asdl.h"
#include "parse.tab.h"
%}

%x EMBED
%x APPEND
%x COMMENT

type_id [a-z_][a-z0-9_-]{1,32}
cons_id [A-Z][a-zA-Z0-9_-]{1,32}

%%

"#"[ \t]+		   { BEGIN(COMMENT); }
<COMMENT>.		   ;
<COMMENT>\n		   { BEGIN(INITIAL); }

^"%{"$			   { BEGIN(EMBED); }
<EMBED>^"%}"$		   { BEGIN(INITIAL); }
<EMBED>.|\n		   { emit_prelude(*yytext); }

^"%%"$			   { BEGIN(APPEND); }
<APPEND><<EOF>>		   { BEGIN(INITIAL); }
<APPEND>.|\n		   { emit_appendage(*yytext); }

"bool"			   { return BOOL; }
"size"			   { return SIZE; }
"usize"			   { return USIZE; }
"uint"			   { return UINT; }
"int"			   { return INT; }
"short"			   { return SHORT; }
"ushort"		   { return USHORT; }
"char"		           { return CHAR; }
"uchar"			   { return UCHAR; }
"float"			   { return FLOAT; }
"double"		   { return DOUBLE; }
"string"		   { return STRING; }
"identifier"		   { return IDENTIFIER; }
"attributes"               { return ATTRIBUTES; }
^{type_id}	   	   { yylval.str_val = gc_strndup(yytext, yyleng); return INIT_IDENT; }
{type_id}  	  	   { yylval.str_val = gc_strndup(yytext, yyleng); return TYPE_IDENT; }
{cons_id}    		   { yylval.str_val = gc_strndup(yytext, yyleng); return CONS_IDENT; }
[?*()|,:=]		   return *yytext;
[ \t\r\n]                  ;  // Skip whitespace
.                          ;
<<EOF>>			   yyterminate();


%%

int yywrap() {
    return 1;
}

