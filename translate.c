#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "types.h"

#alloc typedef_heap, typedef_alloc, typedef_realloc, typedef_dump

#hashfunc  typdef_hash


#ifndef MAX_ID
#define MAX_ID (1 << 8)
#endif

#ifndef MAX_TBL
#define MAX_TBL (1 << 14)
#endif

#ifndef MAX_SEQ_COUNT
#define MAX_SEQ_COUNT (1 << 14)
#endif

#ifndef MAX_TYDEF
#define MAX_TYDEF (1 << 12)
#endif

const char *macro_normal_field = "#define NORMAL_FIELD(type, name) type name;\n\n";

const char *macro_sequence_field = "#define SEQUENCE_FIELD(type, name) type name[MAX_SEQUENCE_SIZE]; \\ \n\tint name##_count;\n\n";

const char *macro_optional_field = "#define OPTIONAL_FIELD(type, name) type name; \\ \n\tint name##_present;\n\n";


static char upper_to_lower_map[SCHAR_MAX] = {
    ['A'] = 'a',
    ['B'] = 'b',
    ['C'] = 'c',
    ['D'] = 'd',
    ['E'] = 'e',
    ['F'] = 'f',
    ['G'] = 'g',
    ['H'] = 'h',
    ['I'] = 'i',
    ['J'] = 'j',
    ['K'] = 'k',
    ['L'] = 'l',
    ['M'] = 'm',
    ['N'] = 'n',
    ['O'] = 'o',
    ['P'] = 'p',
    ['Q'] = 'q',
    ['R'] = 'r',
    ['S'] = 's',
    ['T'] = 't',
    ['U'] = 'u',
    ['V'] = 'v',
    ['W'] = 'w',
    ['X'] = 'x',
    ['Y'] = 'y',
    ['Z'] = 'z'
};

static char *curr_id = NULL;
static char *type_defs = NULL;
static int type_defs_len = MAX_TYDEF;
static int type_defs_curs = 0;
static int indent = 0;
static fpos_t top;


extern FILE *yyout;

static inline char *to_lowercase(char *s, char *buff) {
   memset(buff, 0, MAX_ID);
   for (int i = 0; i < strlen(s); i++) {
	if (s[i] <= 'Z' && s[i] >= 'A') {
	     buff[i] = upper_to_lower_map[s[i]];
	} else {
	     buff[i] = s[i];
	}
   }

   return &buff[0];
}

static inline void print_indent() {
    for (int i = 0; i < indent; i++) {
        fprintf(yyout, "  ");
    }
}

#define EMIT(s, ...)           			\
	do {					\
            print_indent();    	        	\
	    fprintf(yyout, s, __VA_ARGS__);	\
	} while (0)

#define PUTS(s, f)				\
	do {					\
	   print_indent();			\
	   fputs(s, f);				\
	} while (0)

static inline char *get_field_name(Field *f, int pos, char *buff) {
    if (f->id != NULL)
        return f->id;
    else {
        memset(buff, 0, MAX_ID);
        snprintf(buff, MAX_ID, "%s_%d", f->type_id, pos);
        return buff;
    }
}

void install_field(Field *f, int p) {
    indent++;
    char buff[MAX_ID] = {0};
    switch (f->kind) {
        case SEQUENCE:
      	    EMIT("SEQUENCE_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, &buff[0]));
            break;
	case OPTIONAL:
            EMIT("OPTIONAL_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, &buff[0]));
            break;
        case NORMAL:
            EMIT("NORMAL_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, &buff[0]));
            break;
        default:
            break;
    }
    indent--;
}


void install_consfn(Constructor *con) {
    char buff[MAX_ID];
    EMIT("%s_tyy create_%s(", curr_id, to_lowercase(con->id, &buff[0]));

    for (size_t p = 0; p < con->num_fields; p++) {
        EMIT("%s %s", con->fields[p]->type_id, get_field_name(con->fields[p], p, &buff[0]));
        if (p < con->num_fields - 1) {
            fputc(',', yyout);
	    fputc(' ', yyout);
	}
    }

    PUTS(") {\n", yyout);

    indent++;
    
    EMIT("%s *p = NULL;\n", curr_id);
    EMIT("p = ALLOC(sizeof(%s));\n", curr_id);
    EMIT("p->kind = %s_kind;\n", con->id);

    for (size_t p = 0; p < con->num_fields; p++) {
        char *fldname = get_field_name(con->fields[p], p, &buff[0]);
        EMIT("p->v.%s.%s = %s;\n", con->id, fldname, fldname);
    }

    PUTS("return p;\n}\n\n", yyout);
    
    indent--;
}


void walk_and_emit_sum_type(Sum *sum) {
    char buff[MAX_ID];

    EMIT("struct _%s {\n", curr_id);

    indent++;
    PUTS("enum {\n", yyout);
    
    for (size_t i = 0; i < sum->num_cons; i++)
        EMIT("%s_kind,\n", sum->cons[i]->id);
    
    indent--;
    PUTS("} kind;\n", yyout);

    PUTS("union {\n", yyout);

    indent++;

    bool enumerative = false;
    for (size_t i = 0; i < sum->num_cons; i++) {
        if (sum->cons[i]->fields != NULL && !enumerative) {
            EMIT("struct %s_tyy {\n", sum->cons[i]->id);
	    indent++;
            for (size_t p = 0; p < sum->cons[i]->num_fields; p++)
                install_field(sum->cons[i]->fields[p], p);
            for (size_t p = 0; p < sum->num_attrs; p++)
                install_field(sum->attrs[p], p);
            EMIT("} %s;\n", to_lowercase(&sum->cons[i]->id[0], &buff[0]));
	    indent--;
            continue;
        } else if (sum->cons[i]->fields == NULL && !enumerative) {
            enumerative = true;
            EMIT("enum %s {\n", sum->cons[i]->id);
	    
        }

        if (enumerative)
            EMIT("%s_tyy,\n", sum->cons[i]->id);
    }
    EMIT("} %s;\n", enumerative ? "" : "v");
    indent--;
    

    PUTS("};\n\n", yyout);
    
}


void walk_and_emit_sum_consfn(Sum *sum) {
    for (size_t i = 0; i < sum->num_cons; i++)
        install_consfn(sum->cons[i]);
}


void walk_and_emit_prod_type(Product *prod) {
    EMIT("union %s {\n", curr_id);

    indent++;    
    for (size_t p = 0; p < prod->num_fields; p++)
        install_field(prod->fields[p], p);
    indent--;

    PUTS("};\n\n", yyout);
}

void save_typedef(char *id, bool uni) {
   char buff[MAX_TYDEF] = {0};
   int def_len = 0;

   if (uni)
        def_len = snprintf(&buff[0], MAX_TYDEF, "typedef union _%s %s_tyy;\n", id, id);
   else
        def_len = snprintf(&buff[0], MAX_TYDEF, "typedef struct _%s %s_tyy;\n", id, id);


   type_defs = typedef_realloc(type_defs, type_defs_len + def_len + 1);
   memmove(&type_defs[type_defs_curs], &buff[0], def_len);
   type_defs_len += def_len;
   type_defs_curs += def_len;
}



void translate_rule(char *id, Type *t) {     
    if (t->kind == TYPE_PRODUCT)
	save_typedef(id, true);
    else
	save_typedef(id, false);

    curr_id = id;

    if (t->kind == TYPE_SUM) {
        walk_and_emit_sum_type(t->sum);
        walk_and_emit_sum_consfn(t->sum);
    } else {
        walk_and_emit_prod_type(t->product);
    }

}


void initialize(void) {
    PUTS(macro_normal_field, yyout);
    PUTS(macro_sequence_field, yyout);
    PUTS(macro_optional_field, yyout);
    
    fputs("\n\n", yyout);

    fgetpos(yyout, &top);

    type_defs = typedef_alloc(type_defs_len);
	
}

void finish_up_translate(void) {
   rewind(yyout);
   fputs(type_defs, yyout);
   fputs("\n\n", yyout);
   typedef_dump();
}

