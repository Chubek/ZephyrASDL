%{
#include "asdl.h"
#include "parse.tab.h"
#include "errors.h"

#define INC_TOKNO() token_no++
#define INC_LINENO() line_no++
#define INC_CHARNO() char_no += yyleng
#define INC_TOKCHARNO() token_no++; char_no += yyleng
#define INC_TOKLINENO() line_no++; char_no += yyleng

%}

%x EMBED
%x APPEND
%x COMMENT

type_id [a-z_][a-z0-9_-]{1,32}
cons_id [A-Z][a-zA-Z0-9_-]{1,32}

%%

"#"[ \t]+		   { BEGIN(COMMENT); INC_TOKCHARNO(); }
<COMMENT>[^ \t\r\n]+	   { INC_CHARNO(); }
<COMMENT>[ \t]+		   { INC_TOKCHARNO(); }
<COMMENT>[\r\n]		   { BEGIN(INITIAL); INC_LINECHARNO(); }

^"%{"$			   { BEGIN(EMBED); INC_LINECHARNO(); }
<EMBED>^"%}"$		   { BEGIN(INITIAL); INC_CHARNO(); }
<EMBED>[^ \t\r\n]+	   { emit_prelude(yytext); INC_CHARNO(); }
<EMBED>[ \t]+		   { emit_prelude(yytext); INC_TOKCHARNO(); }
<EMBED>[\r\n]		   { emit_prelude(yytext); INC_LINENO(); }

^"%%"$			   { BEGIN(APPEND); INC_LINECHARNO(); }
<APPEND><<EOF>>		   { BEGIN(INITIAL); }
<APPEND>[^ \t\r\n]+	   { emit_appendage(yytext); INC_CHARNO(); }
<APPEND>[ \t]+		   { emit_appendage(yytext); INC_TOKCHARNO(); }
<APPEND>[\r\n]		   { emit_appendage(yytext); INC_LINECHARNO(); }

"boolean"                  { INC_CHARNO(); return BOOLEAN; }
"int"                      { INC_CHARNO(); return INT32; }
"uint"                     { INC_CHARNO(); return UINT32; }
"int8"                     { INC_CHARNO(); return INT8; }
"uint8"                    { INC_CHARNO(); return UINT8; }
"int16"                    { INC_CHARNO(); return INT16; }
"uint16"                   { INC_CHARNO(); return UINT16; }
"int32"                    { INC_CHARNO(); return INT32; }
"uint32"                   { INC_CHARNO(); return UINT32; }
"int64"                    { INC_CHARNO(); return INT64; }
"uint64"                   { INC_CHARNO(); return UINT64; }
"float32"                  { INC_CHARNO(); return FLOAT32; }
"float64"                  { INC_CHARNO(); return FLOAT64; }
"float80"                  { INC_CHARNO(); return FLOAT80; }
"size"                     { INC_CHARNO(); return SIZE; }
"usize"                    { INC_CHARNO(); return USIZE; }
"char"                     { INC_CHARNO(); return CHAR; }
"uchar"                    { INC_CHARNO(); return UCHAR; }
"string"                   { INC_CHARNO(); return STRING; }
"bytearray"                { INC_CHARNO(); return BYTEARRAY; }
"identifier"               { INC_CHARNO(); return IDENTIFIER; }
"attributes"               { INC_CHARNO(); return ATTRIBUTES; }

^{type_id}	   	   { yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); return INIT_IDENT; }
{type_id}  	  	   { yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); return TYPE_IDENT; }
{cons_id}    		   { yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); return CONS_IDENT; }
[?*()|,:=]		   { INC_CHARNO(); return *yytext; }

[ \t\r\n]                  { INC_TOKCHARNO(); }
.			   { error_out(ERROR_UNKNOWN_SYMBOL, *yytext); }
.+[ \t]+		   { error_out(ERROR_UNKNOWN_TOKEN, yytext); }
<<EOF>>			   yyterminate();


%%

int yywrap() {
    return 1;
}


