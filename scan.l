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

static inline void error_out(char *fmt, ...) {
   va_list ap;
   va_start(ap, fmt);
   fprintf(stderr, "Lexical scanning error at line %lu, token %lu, symbol %lu:\n", line_no, token_no, symbol_no);
   vfprintf(stderr, fmt, ap);
   va_end(ap);
   fprintf(stderr, "\nFatal error, ZephyrASDL is exiting with status %d\n", EXIT_FAILURE);
   exit(EXIT_FAILURE);
}

#define MODE_REC (1 << 1)
#define MODE_SUM (1 << 2)

static int mode = -1;


%}

type_id [a-z_][a-z0-9_-]{1,32}
type_name [a-zA-Z_][a-zA-Z0-9-]{1,32}
cons_id [A-Z][a-zA-Z0-9_-]{1,32}

%x EMBED
%x APPEND
%x COMMENT
%x TYINIT
%x SUM_VARIANT
%x REC
%x CONS
%x ATTRS
%x SUM
%x FIELDTYPE
%x FIELDNAME
%x REC_VARIANT


%%

<EMBED>[ \t]+		   { emit_prelude(yytext); INC_TOKCHARNO(); }
<*>[ \t]+		   { INC_TOKCHARNO(); }


<*>"#"[ \t]+		   { BEGIN(COMMENT); INC_TOKCHARNO(); }
<COMMENT>[\r\n]		   { BEGIN(YY_START); INC_LINECHARNO(); }
<COMMENT>.		   ;
<COMMENT><<EOF>>	   { error_out("Unexpected <EOF> at comment when expecting <NEWLINE> to terminate it"); }


^{type_id}	   	   { yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); BEGIN(TYINIT); return RULE_NAME; }
^{cons_id}		   { error_out("Unexpected constructor/variant name, when expecting a rule name"); }
[ \t]+			   { INC_TOKCHARNO(); }
[\r\n]			   { INC_LINENO(); }
.			   { error_out("Unexpected symbol `%c`, when expecting a rule name", *yytext); }
<INITIAL,APPEND><<EOF>>	   { yyterminate(); }

^"%{"$			   { BEGIN(EMBED); INC_LINECHARNO(); }
<EMBED>^"%}"$		   { BEGIN(INITIAL); INC_CHARNO(); }
<EMBED>[\r\n]		   { emit_prelude(yytext); INC_LINENO(); }
<EMBED>.+	 	   { emit_prelude(yytext); INC_CHARNO(); }
<EMBED><<EOF>>		   { error_out("Unexpcted <EOF> when expecting more C code, or `%}`"); }

^"%%"$			   { BEGIN(APPEND); INC_LINECHARNO(); }
<APPEND>[ \t]+		   { emit_appendage(yytext); INC_TOKCHARNO(); }
<APPEND>[\r\n]		   { emit_appendage(yytext); INC_LINECHARNO(); }
<APPEND>.+		   { emit_appendage(yytext); INC_CHARNO(); }

<TYINIT>"::="|[:=]	   { INC_CHARNO(); BEGIN(SUM_VARIANT); return INIT_ASSIGN; }
<TYINIT>[\r\n]		   { error_out("Unexpected <NEWLINE>, when expecting `=`, `:`, or `::=`"); }
<TYINIT><<EOF>>		   { error_out("Unexpected <EOF>, when expecting `=`, `:`, or `::=`"); }


<SUM_VARIANT>"attributes"  { INC_TOKCHARNO(); BEGIN(ATTRS); return ATTRIBUTES; }
<SUM_VARIANT>{cons_id}	   { yylval.str_val = gc_strndup(yytext, yyleng); INC_TOKCHARNO(); BEGIN(CONS); return VARIANT_NAME; }
<SUM_VARIANT>[(]	   { BEGIN(REC); BEGIN(FIELDTYPE); INC_CHARNO(); mode = MODE_REC; return REC_LPAREN; }
<SUM_VARIANT>[|]	   { INC_TOKCHARNO(); return *yytext; }
<SUM_VARIANT>[\r\n]{1}	   { INC_LINECHARNO(); }
<SUM_VARIANT>[\r\n]{2}|[;] { INC_LINECHARNO(); BEGIN(INITIAL); return SUM_TERM; }
<SUM_VARIANT><<EOF>>	   { error_out("Unexpected <EOF>, when expecting records, variants, two <NEWLINE>s or `;`"); }


<CONS>[(]		   { BEGIN(SUM); BEGIN(FIELDTYPE); INC_CHARNO(); mode = MODE_SUM; return SUM_LPAREN; }
<CONS>[|]		   { BEGIN(SUM_VARIANT); return *yytext; }
<CONS>[\r\n]{1}		   { INC_LINECHARNO(); }
<CONS>[\r\n]{1}[(]	   { error_out("Unexpected <NEWLINE> before `(`"); }
<CONS>[\r\n]{2}|[;]	   { BEGIN(INITIAL); INC_LINECHARNO(); return SUM_TERM; }
<CONS>.			   { error_out("Unexpected symbol `%c`, when expecting `(`, `|`, two <NEWLINE>s or `;`", *yytext); }
<CONS><<EOF>>		   { error_out("Unexpected <NEWLINE>, when expecting `(`, `|`, two <NEWLINE>s or `;`"); }

<ATTRS>[(]		   { BEGIN(SUM); BEGIN(FIELDTYPE); INC_CHARNO(); return ATTR_LPAREN; }
<ATTRS>.		   { error_out("Unexpected symbol `%c`, when expecting `(`", *yytext); }


<REC,SUM,FIELDTYPE>"int"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT32; }
<REC,SUM,FIELDTYPE>"uint"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return UINT32; }
<REC,SUM,FIELDTYPE>"int8"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT8; }
<REC,SUM,FIELDTYPE>"uint8"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT8; }
<REC,SUM,FIELDTYPE>"int16"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT16; }
<REC,SUM,FIELDTYPE>"uint16"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT16; }
<REC,SUM,FIELDTYPE>"int32"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT32; }
<REC,SUM,FIELDTYPE>"uint32"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT32; }
<REC,SUM,FIELDTYPE>"int64"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT64; }
<REC,SUM,FIELDTYPE>"uint65"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT64; }
<REC,SUM,FIELDTYPE>"char"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return CHAR; }
<REC,SUM,FIELDTYPE>"uchar"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return UCHAR; }
<REC,SUM,FIELDTYPE>"float32"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return FLOAT32; }
<REC,SUM,FIELDTYPE>"float64"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return FLOAT64; }
<REC,SUM,FIELDTYPE>"float80"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return FLOAT80; }
<REC,SUM,FIELDTYPE>"size"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return SIZE; }
<REC,SUM,FIELDTYPE>"usize"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return USIZE; }
<REC,SUM,FIELDTYPE>"string"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return STRING; }
<REC,SUM,FIELDTYPE>"identifier"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return IDENTIFIER; }
<REC,SUM,FIELDTYPE>"bytearray"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return BYTEARRAY; }

<REC,SUM,FIELDTYPE>{type_id}	{ 
					INC_TOKCHARNO(); 
					BEGIN(FIELDNAME); 
					yylval.str_val = gc_strndup(yytext, yyleng);
					return FIELD_TYPE; 
				}

<REC,SUM,FIELDNAME>[*?]		{ INC_TOKCHARNO(); return *yytext; }
<REC,SUM,FIELDTYPE>[*?]		{ error_out("Unexpected type modifier `%c` when expecting field/record type name, `,` or `)`", *yytext); }

<REC,SUM,FIELDNAME>("u")?("int")("8"|"16"|"32"|"64")|"float"("32"|"64"|"80")|("u")?"size"|("u")?"char"|"string"|"identifier"|"bytearray"		{ 
 			error_out("Unexpected field name `%s`, Built-in field type names cannot serve as field names", yytext); 
                 }
<REC,SUM,FIELDNAME>{type_name}	       { 
						INC_TOKCHARNO(); 
						yylval.str_val = gc_strndup(yytext, yyleng); 
						return FIELD_NAME; 
					}

<REC,SUM,FIELDTYPE>{cons_id}		{ error_out("Unexpected variant name `%s`, expected field type, or type modifier", yytext); }
<REC,SUM,FIELDNAME>{cons_id} 		{ error_out("Unexpected variant name `%s`, expected field name, or `,`", yytext); }

<REC,SUM,FIELDTYPE,FIELDNAME>[,]	{ INC_TOKCHARNO(); BEGIN(FIELDTYPE); return *yytext; }


<REC,SUM,FIELDTYPE,FIELDNAME>[)]	{
					   if (mode == MODE_REC) {
						mode = -1;
						BEGIN(REC_VARIANT);
						return REC_RPAREN;
					   } else if (mode == MODE_SUM) {
						mode = -1;
						BEGIN(SUM_VARIANT);
						return SUM_RPAREN;
					   } else {
						error_out("Logistics error (mistmatched DFA states)");
					   }

					}


<REC,SUM>[\r\n]				{ error_out("Unexpected <NEWLINE> at field/record, expected field type, type modifier, or field name"); }
<REC,SUM><<EOF>>			{ error_out("Unexpected <EOF> at field/record, expected field type, type modifier, or field name"); }


<REC,SUM,FIELDTYPE,FIELDNAME>.		{ error_out("Unexpected symbol `%c` at field/recor", *yytext);  }

<REC_VARIANT>[\r\n]{2}|[;]		{ INC_LINECHARNO(); BEGIN(INITIAL); return REC_TERM; }
<REC_VARIANT>[(]			{ error_out("Unexpected symbol `(`, Note: multiple records cannot be assigned to one rule"); }
<REC_VARIANT>.				{ error_out("Unexpected symbol `%c`, expected `;`, or two <NEWLINE>s", *yytext); }
<REC_VARIANT><<EOF>>			{ error_out("Unexpected <EOF> when expecting termination of record type"); }

%%

int yywrap() {
    return 1;
}


