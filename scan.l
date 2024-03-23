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

"boolean"		   { return BOOLEAN; }
"int"			   { return INT32; }
"uint"			   { return UINT32; }
"int8"			   { return INT8; }
"uint8"			   { return UINT8; }
"int16"			   { return INT16; }
"uint16"		   { return UINT16; }
"int32"			   { return INT32; }
"uint32"		   { return UINT32; }
"int64"			   { return INT64; }
"uint64"		   { return UINT64; }
"float32"		   { return FLOAT32; }
"float64"		   { return FLOAT64; }
"float80"		   { return FLOAT80; }
"size"			   { return SIZE; }
"usize"			   { return USIZE; }
"char"		           { return CHAR; }
"uchar"			   { return UCHAR; }
"string"		   { return STRING; }
"bytearray"		   { return BYTEARRAY; }
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

