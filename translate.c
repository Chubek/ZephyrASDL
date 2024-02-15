#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#ifndef MAX_ID
#define MAX_ID (1 << 8)
#endif

#ifndef MAX_TBL
#define MAX_TBL (1 << 14)
#endif


char *curr_id = NULL;
int indent = 0;
fpos_t top;
fpos_t curr;

extern FILE *yyout;

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

static inline char *get_field_name(Field *f, int pos, char *buf) {
    if (f->id != NULL)
        return f->id;
    else {
        memset(buf, 0, MAX_ID);
        snprintf(buf, MAX_ID, "%s_%d", f->type_id, pos);
        return buf;
    }
}

void install_field(Field *f, int p) {
    char buf[MAX_ID] = {0};
    switch (f->kind) {
        case SEQUENCE:
      	    EMIT("SEQUENCE_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, buf));
            break;
	case OPTIONAL:
            EMIT("OPTIONAL_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, buf));
            break;
        case NORMAL:
            EMIT("NORMAL_FIELD(%s, %s);\n", f->type_id, get_field_name(f, p, buf));
            break;
        default:
            break;
    }
}


void install_consfn(Constructor *con) {
    char buf[MAX_ID];
    EMIT("%s_tyy create_%s(", curr_id, con->id);

    for (size_t p = 0; p < con->num_fields; p++) {
        EMIT("%s %s", con->fields[p]->type_id, get_field_name(con->fields[p], p, buf));
        if (p < con->num_fields - 1)
            putchar(',');
    }

    fputs(") {\n", yyout);
    indent++;
    EMIT("%s *p = NULL;\n", curr_id);
    EMIT("p = ALLOC(sizeof(%s));\n", curr_id);
    EMIT("p->kind = %s_kind;\n", con->id);

    for (size_t p = 0; p < con->num_fields; p++) {
        char *fldname = get_field_name(con->fields[p], p, buf);
        EMIT("p->v.%s.%s = %s;\n", con->id, fldname, fldname);
    }

    fputs("return p;\n}\n", yyout);
    indent--;
    
}


void walk_and_emit_sum_type(Sum *sum) {
    EMIT("struct %s {\n", curr_id);

    fputs("enum {\n", yyout);
    indent++;
    for (size_t i = 0; i < sum->num_cons; i++)
        EMIT("%s_kind,\n", sum->cons[i]->id);
    indent--;
    fputs("} kind;\n", yyout);

    fputs("union {\n", yyout);
    indent++;
    bool enumerative = false;
    for (size_t i = 0; i < sum->num_cons; i++) {
        if (sum->cons[i]->fields != NULL && !enumerative) {
            EMIT("struct %s_tyy {\n", sum->cons[i]->id);
            for (size_t p = 0; p < sum->cons[i]->num_fields; p++)
                install_field(sum->cons[i]->fields[p], p);
            for (size_t p = 0; p < sum->num_attrs; p++)
                install_field(sum->attrs[p], p);
            EMIT("} %s;\n", sum->cons[i]->id);
            continue;
        } else if (sum->cons[i]->fields == NULL && !enumerative) {
            enumerative = true;
            EMIT("enum %s {\n", sum->cons[i]->id);
	    indent++;
        }

        if (enumerative)
            EMIT("%s_tyy,\n", sum->cons[i]->id);
    }
    EMIT("} %s;\n", enumerative ? "" : "v");
    indent--;

    fputs("};\n", yyout);
    indent--;
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

    fputs("};\n", yyout);
}


void translate_rule(char *id, Type *t) {
    fgetpos(yyout, &curr);
    fsetpos(yyout, &top);

    if (t->kind == TYPE_PRODUCT)
        EMIT("typedef union _%s %s_tyy;\n", id, id);
    else
        EMIT("typedef struct _%s %s_tyy;\n", id, id);

    fsetpos(yyout, &curr);

    curr_id = id;

    if (t->kind == TYPE_SUM) {
        walk_and_emit_sum_type(t->sum);
        walk_and_emit_sum_consfn(t->sum);
    } else {
        walk_and_emit_prod_type(t->product);
    }
}

