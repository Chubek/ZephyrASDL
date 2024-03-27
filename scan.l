%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <alloca.h>
#include <string.h>
#include <ctype.h>

#include "asdl.h"
#include "parse.tab.h"

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

static inline void is_c_keyword(char *type, size_t len) {
   if (len < 2)
	return;

   char *type_dup = alloca(len + 1);
   memset(&type_dup[0], 0, len + 1);
   strncat(&type_dup[0], type, len);

   for (size_t i = 0; i < len; i++)
   	type_dup[i] = tolower(type_dup[i]);


   if (!strcmp(type_dup, "int")
   	|| !strcmp(type_dup, "char")
	|| !strcmp(type_dup, "unsigned")
	|| !strcmp(type_dup, "long")
	|| !strcmp(type_dup, "short")
	|| !strcmp(type_dup, "float")
	|| !strcmp(type_dup, "double")
	|| !strcmp(type_dup, "struct")
	|| !strcmp(type_dup, "enum")
	|| !strcmp(type_dup, "union")
	|| !strcmp(type_dup, "if")
	|| !strcmp(type_dup, "while")
	|| !strcmp(type_dup, "do")
	|| !strcmp(type_dup, "for")
	|| !strcmp(type_dup, "switch")
	|| !strcmp(type_dup, "case")
	|| !strcmp(type_dup, "default")
	|| !strcmp(type_dup, "else")
	|| !strcmp(type_dup, "static")
	|| !strcmp(type_dup, "inline")
	|| !strcmp(type_dup, "void")
	|| !strcmp(type_dup, "const")
	|| !strcmp(type_dup, "register")
	|| !strcmp(type_dup, "volatile")
	|| !strcmp(type_dup, "extern")
	|| !strcmp(type_dup, "auto")
	|| !strcmp(type_dup, "bool")
	|| !strcmp(type_dup, "int8_t")
	|| !strcmp(type_dup, "uint8_t")
	|| !strcmp(type_dup, "int16_t")
	|| !strcmp(type_dup, "uint16_t")
	|| !strcmp(type_dup, "int32_t")
	|| !strcmp(type_dup, "uint32_t")
	|| !strcmp(type_dup, "int64_t")
	|| !strcmp(type_dup, "uint64_t")
	|| !strcmp(type_dup, "size_t")
	|| !strcmp(type_dup, "ssize_t")
	|| !strcmp(type_dup, "false")
	|| !strcmp(type_dup, "true")) {
		error_out("Type name, field name or variant name token `%s` matches C keyword (regardless of case)", type);
	}
}


#define MODE_REC_PRAWL (1 << 1)
#define MODE_SUM_PRAWL (1 << 2)
#define MODE_REC_DEFINED (1 << 3)
#define MODE_SUM_DEFINED (1 << 4)

static int mode = -1;
static bool variants_defined = false, seek_attrs = false;

%}

type_id [a-z_][a-z0-9_-]{0,16}
type_name [a-zA-Z_][a-zA-Z0-9_-]{0,16}
cons_id [A-Z][a-zA-Z0-9_-]{0,16}

%x EMBED
%x APPEND
%x COMMENT
%x TYINIT
%x PRAWL
%x FIELDTYPE
%x FIELDNAME


%%

<APPEND>[ \t]+		   { emit_appendage(yytext); INC_TOKCHARNO(); }
<EMBED>[ \t]+		   { emit_prelude(yytext); INC_TOKCHARNO(); }
<*>[ \t]+		   { INC_TOKCHARNO(); }


<*>"#"[ \t]+		   { BEGIN(COMMENT); INC_TOKCHARNO(); }
<COMMENT>[\r\n]		   { BEGIN(YY_START); INC_LINECHARNO(); }
<COMMENT>.		   ;
<COMMENT><<EOF>>	   { error_out("Unexpected <EOF> at comment when expecting <NEWLINE> to terminate it"); }


^{type_id}	   	   { is_c_keyword(yytext, yyleng);
				yylval.str_val = gc_strndup(yytext, yyleng); INC_CHARNO(); BEGIN(TYINIT); return RULE_NAME; }
^{cons_id}		   { error_out("Unexpected constructor/variant name, when expecting a rule name"); }
[\r\n]			   { INC_LINENO(); }
.			   { error_out("Unexpected symbol `%c`, when expecting a rule name", *yytext); }
<INITIAL,APPEND><<EOF>>	   { yyterminate(); }

^"%{"$			   { BEGIN(EMBED); INC_LINECHARNO(); }
<EMBED>^"%}"$		   { BEGIN(INITIAL); INC_CHARNO(); }
<EMBED>[\r\n]		   { emit_prelude(yytext); INC_LINENO(); }
<EMBED>.+	 	   { emit_prelude(yytext); INC_CHARNO(); }
<EMBED><<EOF>>		   { error_out("Unexpcted <EOF> when expecting more C code, or `%}`"); }

^"%%"$			   { BEGIN(APPEND); INC_LINECHARNO(); }
<APPEND>[\r\n]		   { emit_appendage(yytext); INC_LINECHARNO(); }
<APPEND>.+		   { emit_appendage(yytext); INC_CHARNO(); }

<TYINIT>"::="|[:=]	   { INC_CHARNO(); BEGIN(PRAWL); return INIT_ASSIGN; }
<TYINIT>[\r\n]		   { error_out("Unexpected <NEWLINE>, when expecting `=`, `:`, or `::=`"); }
<TYINIT><<EOF>>		   { error_out("Unexpected <EOF>, when expecting `=`, `:`, or `::=`"); }


<PRAWL>"attributes"  	   { 
				if (!variants_defined)
					error_out("Unexpected <ATTRIBUTES> token, no other variants defined");

				INC_TOKCHARNO(); seek_attrs = true; return ATTRIBUTES; 
			   }
<PRAWL>{cons_id}	   { 
				is_c_keyword(yytext, yyleng);
				variants_defined = true;
			   	yylval.str_val = gc_strndup(yytext, yyleng); INC_TOKCHARNO(); return VARIANT_NAME; 
			   }
<PRAWL>[(]	   	   { 
				if (mode == MODE_REC_DEFINED) {
				    error_out("Unexpected `(`, cannot define more than one product/record type");
				}
				else if (variants_defined || seek_attrs) {
				    BEGIN(FIELDTYPE); 
				    INC_CHARNO(); 
				    mode = MODE_SUM_PRAWL; 
				    return SUM_LPAREN;
				} else {			
				    BEGIN(FIELDTYPE); 
				    INC_CHARNO(); 
				    mode = MODE_REC_PRAWL; 
				    return REC_LPAREN; 
				}
			   }
<PRAWL>[|]	   	   { 
				if (mode == MODE_REC_DEFINED)
					error_out("Unexpected `|`, cannot define more than one product/record type");

				if (!variants_defined)
					error_out("Unexpected `|`, no variants are defined yet");
				
				mode = -1;
				INC_TOKCHARNO(); return *yytext; 
			   }
<PRAWL>[\r\n]{1}	   { INC_LINECHARNO(); }
<PRAWL>[\r\n]{2}|[;] 	   { 
				if (!variants_defined && mode != MODE_REC_DEFINED)
					error_out("Unexpected <RULE-TERMINATE>, no variants or records defined");
				
			 	INC_LINECHARNO(); 
				BEGIN(INITIAL); 
				mode = -1; 
				variants_defined = seek_attrs = false; 
				return SUM_TERM; 
			   }
<PRAWL><<EOF>>	  	   { error_out("Unexpected <EOF>, when expecting records, variants, two <NEWLINE>s or `;`"); }


<FILEDTYPE>"boolean"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return BOOLEAN; }
<FIELDTYPE>"int"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT32; }
<FIELDTYPE>"uint"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return UINT32; }
<FIELDTYPE>"int8"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT8; }
<FIELDTYPE>"uint8"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT8; }
<FIELDTYPE>"int16"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT16; }
<FIELDTYPE>"uint16"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT16; }
<FIELDTYPE>"int32"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT32; }
<FIELDTYPE>"uint32"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT32; }
<FIELDTYPE>"int64"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT64; }
<FIELDTYPE>"uint65"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return INT64; }
<FIELDTYPE>"char"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return CHAR; }
<FIELDTYPE>"uchar"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return UCHAR; }
<FIELDTYPE>"float32"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return FLOAT32; }
<FIELDTYPE>"float64"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return FLOAT64; }
<FIELDTYPE>"float80"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return FLOAT80; }
<FIELDTYPE>"size"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return SIZE; }
<FIELDTYPE>"usize"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return USIZE; }
<FIELDTYPE>"string"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return STRING; }
<FIELDTYPE>"identifier"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return IDENTIFIER; }
<FIELDTYPE>"bytearray"	{ INC_TOKCHARNO(); BEGIN(FIELDNAME); return BYTEARRAY; }

<FIELDTYPE>{type_id}	{ 
					INC_TOKCHARNO(); 
					BEGIN(FIELDNAME); 
					yylval.str_val = gc_strndup(yytext, yyleng);
					return FIELD_TYPE; 
			}

<FIELDNAME>[*?]		{ INC_TOKCHARNO(); return *yytext; }
<FIELDTYPE>[*?]		{ error_out("Unexpected type modifier `%c` when expecting field/record type name, `,` or `)`", *yytext); }

<FIELDNAME>("u")?("int")("8"|"16"|"32"|"64")|"float"("32"|"64"|"80")|("u")?"size"|("u")?"char"|"string"|"identifier"|"bytearray"		{ 
 			error_out("Unexpected field name `%s`, Built-in field type names cannot serve as field names", yytext); 
                 }
<FIELDNAME>{type_name}	       { 
						is_c_keyword(yytext, yyleng);
						INC_TOKCHARNO(); 
						yylval.str_val = gc_strndup(yytext, yyleng); 
						return FIELD_NAME; 
                               }

<FIELDTYPE>{cons_id}		{ error_out("Unexpected variant name `%s`, expected field type, or type modifier", yytext); }
<FIELDNAME>{cons_id} 		{ error_out("Unexpected variant name `%s`, expected field name, or `,`", yytext); }

<FIELDTYPE,FIELDNAME>[,]	{ INC_TOKCHARNO(); BEGIN(FIELDTYPE); return *yytext; }


<FIELDTYPE,FIELDNAME>[)]	{
					if (mode == MODE_REC_PRAWL) {
						mode = MODE_REC_DEFINED;
						BEGIN(PRAWL);
						return REC_RPAREN;
					   } else if (mode == MODE_SUM_PRAWL) {
						mode = MODE_SUM_DEFINED;
						BEGIN(PRAWL);
						return SUM_RPAREN;
					   } else {
						error_out("Logistics error (mistmatched DFA states)");
					   }

				}


<FIELDTYPE,FIELDNAME>[\r\n]	{ error_out("Unexpected <NEWLINE> when defining fields"); }

<FIELDTYPE,FIELDNAME>.		{ error_out("Unexpected symbol `%c` at field/recor", *yytext);  }

%%

int yywrap() {
    return 1;
}


