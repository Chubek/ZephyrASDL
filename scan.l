%{
#include "asdl.h"
#include "parse.tab.h"
#include "errors.h"

#define INC_TOKNO() token_no++
#define INC_LINENO() line_no++
#define INC_CHARNO() char_no += yyleng
#define INC_TOKCHARNO() token_no++; char_no += yyleng
#define INC_TOKLINENO() line_no++; token_no++
#define INC_LINECHARNO() line_no++; char_no += yyleng

%}

%x EMBED
%x APPEND
%x COMMENT

type_id [a-z_][a-z0-9_-]{1,32}
type_name [a-zA-Z_][a-zA-Z0-9-]{1,32}
cons_id [A-Z][a-zA-Z0-9_-]{1,32}


%x TYINIT
%x ASSIGN
%x RECORD
%x CONS
%x FIELD
%x TYPEID
%x TYPEMOD
%x TYPENAME


%%

^{type_id}	   	   { yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); BEGIN(TYINIT); return INIT_IDENT; }
^{cons_id}		   { error_out(ERROR_UNEXPECTED_TOKEN, get_toktok_expn_msg("Type Name", "Constructor Name")); }
^[^ #\r\n\t]		   { error_out(ERROR_UNEXPECTED_SYMBOL, get_strsym_expn_msg("Lowercase letters", *yytext)); }
<<EOF>>			   { yyterminate(); }


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

<TYINIT>[=]		   { INC_CHARNO(); BEGIN(ASSIGN); return INIT_ASSIGN; }
<TYINIT>[ \t]+		   { INC_TOKCHARNO(); }
<TYINIT>[\r\n]		   { error_out(ERROR_UNEXPECTED_SYMBOL, get_strstr_expn_msg("=", "<NEWLINE>")); }
<TYINIT><<EOF>>		   { error_out(ERROR_UNEXPECTED_SYMBOL, get_strstr_expn_msg("=", "<EOF>")); }

<ASSIGN>{cons_id}	   { yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); BEGIN(CONS); return CONST_IDENT; }
<ASSIGN>[(]		   { BEGIN(RECORD); BEGIN(TYPENAME); INC_CHARNO(); return REC_LPAREN; }
<ASSIGN>[|]		   { error_out(ERROR_UNEXPECTED_SYMBOL, "Unexpected alternator (|) character before any variants given"); }
<ASSIGN>[;]		   { error_out(ERROR_UNEXPECTED_SYMBOL, "Unexpected semicolon (;) before any variants given"); }
<ASSIGN>[ \t]+		   { INC_TOKCHARNO(); }
<ASSING>[\r\n]		   { INC_LINECHARNO(); }


<CONS>[(]		   { BEGIN(FIELD); BEGIN(TYPENAME); INC_CHARNO(); return FLD_LPAREN; }
<CONS>[ \t]+		   { INC_TOKCHARNO(); }
<CONS>[\r\n]		   { error_out(ERROR_UNEXPECTED_SYMBOL, get_str_expn_msg("(", "<NEWLINE>")); }
<CONS>[^ \t\r\n]	   { error_out(ERROR_UNEXPECTED_SYMBOL, get_sym_expn_msg('(', *yytext)); }
<CONS><<EOF>>		   { error_out(ERROR_UNEXPECTED_SYMBOL, get_str_expn_msg("(", "<EOF>")); }


<RECORD,FIELD,TYPEID>"int"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT; }
<RECORD,FIELD,TYPEID>"uint"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return UINT; }
<RECORD,FIELD,TYPEID>"int8"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT8; }
<RECORD,FIELD,TYPEID>"uint8"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT8; }
<RECORD,FIELD,TYPEID>"int16"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT16; }
<RECORD,FIELD,TYPEID>"uint16"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT16; }
<RECORD,FIELD,TYPEID>"int32"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT32; }
<RECORD,FIELD,TYPEID>"uint32"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT32; }
<RECORD,FIELD,TYPEID>"int64"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT64; }
<RECORD,FIELD,TYPEID>"uint65"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT64; }
<RECORD,FIELD,TYPEID>"char"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return CHAR; }
<RECORD,FIELD,TYPEID>"uchar"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return UCHAR; }
<RECORD,FIELD,TYPEID>"float32"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return FLOAT32; }
<RECORD,FIELD,TYPEID>"float64"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return FLOAT64; }
<RECORD,FIELD,TYPEID>"float80"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return FLOAT80; }
<RECORD,FIELD,TYPEID>"size"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return SIZE; }
<RECORD,FIELD,TYPEID>"usize"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return USIZE; }
<RECORD,FIELD,TYPEID>"string"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return STRING; }
<RECORD,FIELD,TYPEID>"identifier"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return IDENTIFIER; }
<RECORD,FIELD,TYPEID>"bytearray"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return BYTEARRAY; }
<RECORD,FIELD,TYPEID>{type_id}		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return TYPE_IDENT; }

<RECORD,FIELD,TYPEMOD>[*?]		{ INC_TOKCHARNO(); BEGIN(TYPENAME); return *yytext; }
<RECORD,FIELD,TYPEID>[*?]		{ error_out(ERROR_UNEXPECTED_SYMBOL, get_strsym_expn("Type Identifier", *yytext)); }
<RECORD,FIELD,TYPENAME>[*?]		{ error_out(ERROR_UNEXPECTED_SYMBOL, get_strsym_expn("Type Name", *yytext)); }

<RECORD,FIELD,TYPENAME>("u")?("int")("8"|"16"|"32"|"64")|"float"("32"|"64"|"80")|("u")?"size"|("u")?"char"|"string"|"identifier"|"bytearray"				 { 
					error_out(ERROR_UNEXPECTED_TOKEN, "Built-in type names cannot serve as field names"); 
				  }
<RECORD,FIELD,TYPENAME>{type_name}	{ 
						INC_TOKCHARNO(); 
						yylval.str_val = gc_strndup(yytext, yyleng); 
						BEGIN(TYPEID); 
						return FIELD_NAME; 
					}
%%

int yywrap() {
    return 1;
}


