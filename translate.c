#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "types.h"

#ifndef MAX_ID
#define MAX_ID (1 << 8)
#endif

#ifndef MAX_TBL
#define MAX_TBL (1 << 14)
#endif

#ifndef MAX_SEQUENCE_SIZE
#define MAX_SEQUENCE_SIZE (1 << 10)
#endif

const char *macro_normal_field = "#define NORMAL_FIELD(type, name) type name;\n\n";

const char *macro_sequence_field = "#define SEQUENCE_FIELD(type, name) type name[MAX_SEQUENCE_SIZE]; int name##_count;\n\n";

const char *macro_optional_field = "#define OPTIONAL_FIELD(type, name) type name; int name##_present;\n\n";


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


static int indent = 0;
static fpos_t top;

static FILE *tmp_defs = NULL;
static FILE *tmp_decls = NULL;

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
        fprintf(tmp_defs, "  ");
    }
}

#define EMIT_DEF(s, ...)           			\
	do {					\
            print_indent();    	        	\
	    fprintf(tmp_defs, s, __VA_ARGS__);	\
	} while (0)

#define PUTS_DEF(s)					\
	do {					\
	   print_indent();			\
	   fputs(s, tmp_defs);			\
	} while (0)

#define EMIT_DECL(s, ...)           			\
	do {					\
            print_indent();    	        	\
	    fprintf(tmp_decls, s, __VA_ARGS__);	\
	} while (0)

#define PUTS_DECL(s)					\
	do {					\
	   print_indent();			\
	   fputs(s, tmp_decls);			\
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
      	    EMIT_DEF("SEQUENCE_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, &buff[0]));
            break;
	case OPTIONAL:
            EMIT_DEF("OPTIONAL_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, &buff[0]));
            break;
        case NORMAL:
            EMIT_DEF("NORMAL_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, &buff[0]));
            break;
        default:
            break;
    }
    indent--;
}


void install_consfn(Constructor *con) {
    char buff[MAX_ID];
    EMIT_DEF("%s_tyy create_%s(", curr_id, to_lowercase(con->id, &buff[0]));
    EMIT_DECL("%s_tyy create_%s(", curr_id, to_lowercase(con->id, &buff[0]));


    for (size_t p = 0; p < con->num_fields; p++) {
        EMIT_DEF("%s %s", con->fields[p]->type_id, get_field_name(con->fields[p], p, &buff[0]));
        EMIT_DECL("%s %s", con->fields[p]->type_id, get_field_name(con->fields[p], p, &buff[0]));
        if (p < con->num_fields - 1) {
	    PUTS_DEF(", ");
	    PUTS_DECL(", ");
	}
    }

    PUTS_DEF(") {\n");
    PUTS_DECL(");\n");

    indent++;
    
    EMIT_DEF("%s *p = NULL;\n", curr_id);
    EMIT_DEF("p = ALLOC(sizeof(%s));\n", curr_id);
    EMIT_DEF("p->kind = %s_kind;\n", con->id);

    for (size_t p = 0; p < con->num_fields; p++) {
        char *fldname = get_field_name(con->fields[p], p, &buff[0]);
        EMIT_DEF("p->v.%s.%s = %s;\n", con->id, fldname, fldname);
    }

    PUTS_DEF("return p;\n}\n\n");
    
    indent--;
}


void walk_and_emit_sum_type(Sum *sum) {
    char buff[MAX_ID];

    EMIT_DEF("struct _%s {\n", curr_id);

    indent++;
    PUTS_DEF("enum {\n");
    
    for (size_t i = 0; i < sum->num_cons; i++)
        EMIT_DEF("%s_kind,\n", sum->cons[i]->id);
    
    indent--;
    PUTS_DEF("} kind;\n");

    PUTS_DEF("union {\n");

    indent++;

    bool enumerative = false;
    for (size_t i = 0; i < sum->num_cons; i++) {
        if (sum->cons[i]->fields != NULL && !enumerative) {
            EMIT_DEF("struct %s_tyy {\n", sum->cons[i]->id);
	    indent++;
            for (size_t p = 0; p < sum->cons[i]->num_fields; p++)
                install_field(sum->cons[i]->fields[p], p);
            for (size_t p = 0; p < sum->num_attrs; p++)
                install_field(sum->attrs[p], p);
            EMIT_DEF("} %s;\n", to_lowercase(&sum->cons[i]->id[0], &buff[0]));
	    indent--;
            continue;
        } else if (sum->cons[i]->fields == NULL && !enumerative) {
            enumerative = true;
            EMIT_DEF("enum %s {\n", sum->cons[i]->id);
	    
        }

        if (enumerative)
            EMIT_DEF("%s_tyy,\n", sum->cons[i]->id);
    }
    EMIT_DEF("} %s;\n", enumerative ? "" : "v");
    indent--;
    

    PUTS_DEF("};\n\n");
    
}


void walk_and_emit_sum_consfn(Sum *sum) {
    for (size_t i = 0; i < sum->num_cons; i++)
        install_consfn(sum->cons[i]);
}


void walk_and_emit_prod_type(Product *prod) {
    EMIT_DEF("union %s {\n", curr_id);

    indent++;    
    for (size_t p = 0; p < prod->num_fields; p++)
        install_field(prod->fields[p], p);
    indent--;

    PUTS_DEF("};\n\n");
}

void emit_typedef(char *id, bool uni) {
   if (uni)
	EMIT_DECL("typedef union _%s %s_tyy;\n", id, id);
   else
        EMIT_DECL("typedef struct _%s %s_tyy;\n", id, id);
}



void translate_rule(char *id, Type *t) {     
    if (t->kind == TYPE_PRODUCT)
	emit_typedef(id, true);
    else
	emit_typedef(id, false);

    curr_id = id;

    if (t->kind == TYPE_SUM) {
        walk_and_emit_sum_type(t->sum);
        walk_and_emit_sum_consfn(t->sum);
    } else {
        walk_and_emit_prod_type(t->product);
    }

}


void initialize(void) {
    tmp_defs = tmpfile();
    tmp_decls = tmpfile();

    EMIT_DECL("#define MAX_SEQUENCE_SIZE %d\n\n", MAX_SEQUENCE_SIZE);
    PUTS_DECL(macro_normal_field);
    PUTS_DECL(macro_sequence_field);
    PUTS_DECL(macro_optional_field);
}

void merge_temp_files(void) {
   rewind(tmp_decls);
   rewind(tmp_defs);

   char c = 0;

   while ((c = getc(tmp_decls)) != EOF)
	fputc(c, yyout);

   fputs("\n\n", yyout);

   while ((c = getc(tmp_defs)) != EOF)
	fputc(c, yyout);

   fclose(tmp_defs);
   fclose(tmp_decls);
}

