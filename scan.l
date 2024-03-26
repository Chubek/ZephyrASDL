%{
#include <stdarg.h>
#include <stdint.h>

#include "asdl.h"
#include "parse.tab.h"

int before_comment = -1;

size_t line_no = 1;
size_t token_no = 1;
size_t symbol_no = 0;

#define INC_TOKNO() token_no++
#define INC_LINENO() line_no++
#define INC_CHARNO() symbol_no += yyleng
#define INC_TOKCHARNO() token_no++; symbol_no += yyleng
#define INC_TOKLINENO() line_no++; token_no++
#define INC_LINECHARNO() line_no++; symbol_no += yyleng

%}

type_id [a-z_][a-z0-9_-]{1,32}
type_name [a-zA-Z_][a-zA-Z0-9-]{1,32}
cons_id [A-Z][a-zA-Z0-9_-]{1,32}

%x EMBED
%x APPEND
%x COMMENT
%x TYINIT
%x ASSIGN
%x REC
%x CONS
%x SUM
%x TYPEID
%x TYPEMOD
%x TYPENAME


%%

^{type_id}	   	   { yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); BEGIN(TYINIT); return INIT_IDENT; }
^{cons_id}		   { error_out("Unexpected constructor/variant name, expecting a rule name"); }
^[^ #\r\n\t]		   { error_out("Unexpected symbol, expecting a comment, or a rule name"); }
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

<TYINIT>"::="|[:=]	   { INC_CHARNO(); BEGIN(ASSIGN); return INIT_ASSIGN; }
<TYINIT>[ \t]+		   { INC_TOKCHARNO(); }
<TYINIT>[\r\n]		   { error_out("Unexpected <NEWLINE>, expecting `=`, `:`, or `::=`"); }
<TYINIT><<EOF>>		   { error_out("Unexpected <EOF>, expecting `=`, `:`, or `::=`"); }

<ASSIGN>{cons_id}	   { yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); BEGIN(CONS); return CONST_IDENT; }
<ASSIGN>[(]		   { BEGIN(REC); BEGIN(TYPENAME); INC_CHARNO(); return REC_LPAREN; }
<ASSIGN>[|]		   { error_out("Unexpected alternator (|) character before any variants given"); }
<ASSIGN>[;]		   { error_out("Unexpected semicolon (;) before any variants given"); }
<ASSIGN>[ \t]+		   { INC_TOKCHARNO(); }
<ASSIGN>[\r\n]		   { INC_LINECHARNO(); }


<CONS>[(]		   { BEGIN(SUM); BEGIN(TYPENAME); INC_CHARNO(); return FLD_LPAREN; }
<CONS>[|]		   { BEGIN(SUM_VARIANT); return *yytext; }
<CONS>[ \t]+		   { INC_TOKCHARNO(); }
<CONS>[\r\n]{1}		   { error_out("Unexpected <NEWLINE>, expecting `(`, two <NEWLINE>s or `;`"); }
<CONS>[\r\n]{2}|[;]	   { BEGIN(INITIAL); INC_TOKCHARNO(); return SUM_TERM; }
<CONS>.			   { error_out("Unexpected symbol `%c`, expecting `(`, two <NEWLINE>s or `;`", *yytext); }
<CONS><<EOF>>		   { error_out("Unexpected <NEWLINE>, expecting `(`, two <NEWLINE>s or `;`"); }


<REC,SUM,TYPEID>"int"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT; }
<REC,SUM,TYPEID>"uint"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return UINT; }
<REC,SUM,TYPEID>"int8"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT8; }
<REC,SUM,TYPEID>"uint8"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT8; }
<REC,SUM,TYPEID>"int16"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT16; }
<REC,SUM,TYPEID>"uint16"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT16; }
<REC,SUM,TYPEID>"int32"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT32; }
<REC,SUM,TYPEID>"uint32"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT32; }
<REC,SUM,TYPEID>"int64"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT64; }
<REC,SUM,TYPEID>"uint65"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return INT64; }
<REC,SUM,TYPEID>"char"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return CHAR; }
<REC,SUM,TYPEID>"uchar"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return UCHAR; }
<REC,SUM,TYPEID>"float32"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return FLOAT32; }
<REC,SUM,TYPEID>"float64"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return FLOAT64; }
<REC,SUM,TYPEID>"float80"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return FLOAT80; }
<REC,SUM,TYPEID>"size"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return SIZE; }
<REC,SUM,TYPEID>"usize"		{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return USIZE; }
<REC,SUM,TYPEID>"string"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return STRING; }
<REC,SUM,TYPEID>"identifier"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return IDENTIFIER; }
<REC,SUM,TYPEID>"bytearray"	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return BYTEARRAY; }
<REC,SUM,TYPEID>{type_id}	{ INC_TOKCHARNO(); BEGIN(TYPEMOD); return TYPE_IDENT; }

<REC,SUM,TYPEMOD>[*?]		{ INC_TOKCHARNO(); BEGIN(TYPENAME); return *yytext; }
<REC,SUM,TYPEID>[*?]		{ error_out("Unexpected type modifier `%c` when expecting field/record type", *yytext); }
<REC,SUM,TYPENAME>[*?]		{ error_out("Unexpected type modifier `%c` when expecting field/record name", *yytext); }

<REC,SUM,TYPENAME>("u")?("int")("8"|"16"|"32"|"64")|"float"("32"|"64"|"80")|("u")?"size"|("u")?"char"|"string"|"identifier"|"bytearray"				 { 
				error_out("Built-in type names cannot serve as field names"); 
			  }
<REC,SUM,TYPENAME>{type_name}	       { 
						INC_TOKCHARNO(); 
						yylval.str_val = gc_strndup(yytext, yyleng); 
						return SUM_NAME; 
					}

<REC,SUM,TYPEID,TYPENAME>[,]		{ INC_TOKCHARNO(); BEGIN(TYPEID); return *yytext; }


<REC,TYPEID,TYPENAME>[)]		{ INC_TOKCHARNO(); BEGIN(REC_VARIANT); return REC_RPAREN; }
<SUM,TYPEID,TYPENAME>[)]		{ INC_TOKCHARNO(); BEGIN(SUM_VARIANT); return FLD_RPAREN; }


<REC,SUM>[ \t]+				{ INC_TOKCHARNO(); }
<REC,SUM>[\r\n]				{ error_out("Unexpected <NEWLINE> at field/record"); }
<REC,SUM><<EOF>>			{ error_out("Unexpected <EOF> at field/record"); }


<REC,SUM,TYPEID,TYPENAME,TYPEMOD>.	{ error_out("Unexpected symbol `%c` at field/recor", *yytext);  }

<REC_VARIANT>[\r\n]{2}|[;]		{ INC_TOKCHARNO(); BEGIN(INITIAL); return REC_TERM; }
<REC_VARIANT>.				{ error_out("Unexpected symbol `%c`, expected `;`, or two <NEWLINE>s", *yytext); }


<SUM_VARIANT>[|]			{ INC_TOKCHARNO(); BEGIN(ASSIGN); return *yytext; }
<SUM_VARIANT>[\r\n]			{ INC_TOKCHARNO(); }
<SUM_VARIANT>[\r\n]{2}|[;]		{ INC_TOKCHARNO(); BEGIN(INITIAL); return SUM_TERM; }
<SUM_VARIANT>.				{ error_out("Unexpected symbol `%c`, expected `|` followed by a variant, two <NEWLINE>s or `;`"); }

%%

int yywrap() {
    return 1;
}


