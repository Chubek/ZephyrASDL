#include <alloca.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "asdl.h"

#define STR_FORMAT(dest, fmt, ...)                                             \
  do {                                                                         \
    size_t l = strlen(fmt) + UCHAR_MAX;                                        \
    dest = alloca(l);                                                          \
    snprintf(dest, l, fmt, __VA_ARGS__);                                       \
  } while (0)

#define EMIT_PRELUDE(fmt, ...) fprintf(translator.prelude, fmt, __VA_ARGS__)
#define EMIT_DECLS(fmt, ...) fprintf(translator.decls, fmt, __VA_ARGS__)
#define EMIT_DEFS(fmt, ...) fprintf(translator.defs, fmt, __VA_ARGS__)
#define EMIT_APPENDAGE(fmt, ...) fprintf(translator.appendage, fmt, __VA_ARGS__)

#ifndef INDENT
#define INDENT "    "
#endif

char *def_suffix = "def";
char *fn_suffix = "create";
char *arg_suffix = "arg";
char *kind_suffix = "kind";
int indent_level = 0;
Translator translator = {0};

void init_translator(void) {
  translator.prelude = tmpfile();
  translator.decls = tmpfile();
  translator.defs = tmpfile();
  translator.appendage = tmpfile();
  translator.rules = NULL;
  translator.outpath = NULL;
}

void finalize_translator(void) {
  FILE *outfile = stdout;
  int c;

  if (translator.outpath != NULL)
    if (access(translator.outpath, F_OK | W_OK) == 0)
      outfile = fopen(translator.outpath, "w");

  while ((c = fgetc(translator.prelude) != EOF)
     fputc(c, outfile);

  fputc('\n', outfile);
  
  while ((c = fgetc(translator.decls) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);

  while ((c = fgetc(translator.defs) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);
  
  while ((c = fgetc(translator.appendage) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);
  fclose(outfile);
}

void dump_translator(void) {
  fclose(translator.prelude);
  fclose(translator.decls);
  fclose(translator.defs);
  fclose(translator.appendage);
}

static inline void install_typedef(const char *original, const char *alias,
                                   bool pointer) {
  EMIT_DECLS("typedef %s%s%s;\n", original, pointer ? " *" : " ", alias);
}

static inline void install_funcdecl_init(const char *returns,
                                         const char *name) {
  EMIT_DECLS("%s %s(", returns, name);
}

static inline void install_funcdecl_arg(const char *type, const char *name,
                                        bool last) {
  EMIT_DECLS("%s %s%s", type, name, last ? ");\n" : ", ");
}

static inline void install_macro(const char *name, const char *def) {
  EMIT_DEFS("#define %s %s\n", name, def);
}

static inline void install_field(const char *type, const char *name) {
  EMIT_DEFS("%s %s;", type, name);
}

static inline void install_datatype_init(const char *kind, const char *name) {
  EMIT_DEFS("%s %s {\n");
}

static inline void install_datatype_field(const char *field, const char *end) {
  EMIT_DEFS("%s%s", field, end);
}

static inline void install_datatype_named_end(const char *name) {
  EMIT_DEFS("} %s;\n", name);
}

static inline void install_datatype_unnamed_end(void) { EMIT_DEFS("};\n"); }

static inline void install_function_signature_init(const char *returns,
                                                   const char *name) {
  EMIT_DEFS("%s %s(", returns, name);
}

static inline void install_function_signature_arg(const char *type,
                                                  const char *name, bool last) {
  EMIT_DEFS("%s %s%s", type, name, last ? ") {\n" : ", ");
}

static inline void install_function_alloc(const char *type) {
  EMIT_DEFS("%s *p = ALLOC(sizeof(%s));\n\n", type, type);
}

static inline void install_function_assign(const char *field,
                                           const char *value) {
  EMIT_DEFS("p->%s = %s;\n", field, value);
}

static inline void install_function_return(void) {
  EMIT_DEFS("return p;\n}\n\n");
}

static inline void print_indent(void) {
  for (int i = 0; i < indent_level; i++)
    fputs(translator.defs, INDENT);
}

static inline void translate_product_type(char *id, Product *product) {
  char *tyy = NULL;
  char *def = NULL;
  char **fn = NULL;
  STR_FORMAT(tyy, "union %s", id);
  STR_FORMAT(def, "%s_%s", id, def_suffix);
  STR_FORMAT(fn, "%s_%s", id, fn_suffix);

  install_typedef(tyy, def, true);
  install_funcdecl_init(def, fn);

  install_datatype_init("union", id);

  size_t n = 0;
  for (Field *f = product->fields; f != NULL; f = f->next, n++) {
    char *argtyy = NULL;
    char *argname = NULL;
    char *cache = NULL;
    STR_FORMAT(argtyy, "%s_%s", f->type_id, def_suffix);

    if (f->id == NULL) {
      STR_FORMAT(argname, "%s_%s%lu", argtyy, arg_suffix, n);
      STR_FORMAT(cache, "%s %s_%lu", argtyy, f->type_id, n);
    } else {
      STR_FORMAT(argname, "%s_%s", f->id, arg_suffix);
      STR_FORMAT(cache, "%s %s", argtyy, f->id);
    }

    install_funcdecl_arg(argtyy, argname, f->next == NULL);
    install_datatype_field(cache, ";");

    f->cache = cache;
  }

  install_datatype_unnamed_end();
}

static inline void install_field(Field *field, size_t num) {
  switch (field->kind) {
  case FIELD_NORMAL:
    char *tyy = NULL;
    char *name = NULL;
    STR_FORMAT(tyy, "%s_%s", field->type_id, def_suffix);

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", field->type_id, num);
    } else {
      STR_FORMAT(name, "%s", filed->id);
    }

    install_field(tyy, tyy, name);
    break;
  case FIELD_SEQUENCE:
    char *tyy = NULL;
    char *name = NULL;
    char *count = NULL;

    STR_FORMAT(tyy, "%s_%s*", field->type_id, def_suffix);

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", field->type_id, num);
      STR_FORMAT(count, "%s_%lu_count", field->type_id, num);
    } else {
      STR_FORMAT(name, "%s", field->id);
      STR_FORMAT(count, "%s_count", field->id);
    }

    install_field(tyy, name);
    install_field("long", count);

    break;
  case FIELD_OPTIONAL:
    char *tyy = NULL;
    char *name = NULL;
    char *exists = NULL;

    STR_FORMAT(tyy, "%s_%s*", field->type_id, def_suffix);

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", field->type_id, count);
      STR_FORMAT(exits, "%s_%lu_exists", field->type_id, count);
    } else {
      STR_FORMAT(name, "%s", field->id);
      STR_FORMAT(exists, "%s_exists", field->id);
    }

    install_field(tyy, name);
    install_field("int", exists);

    break;
  }
}

static inline void install_constructor(Constructor *constructor) {
  size_t n = 0;
  for (Field f = constructor->fields; f != NULL; f = f->next, n++) {
    install_field(f, n);
  }
}

static inline void install_attributes(Field *attributes) {
  size_t n = 0;
  for (Field f = attributes; f != NULL; f = f->next, n++)
    install_field(f, n);
}

static inline void install_kinds(Constructor *constructors) {
  install_datatype_init("enum", " ");

  for (Constructor *c = constructors; c != NULL; c = c->next) {
    char *kind_name = NULL;
    STR_FORMAT(kind_name, "%s_%s", c->id, kind_suffix);
    install_datatype_field(kind_name, ",");
  }

  install_datatype_named_end("kind");
}

static inline void translate_sum_type(char *id, Sum *sum) {
  char *struct_name = NULL;
  char *def_name = NULL;
  STR_FORMAT(struct_name, "struct %s", id);
  STR_FORMAT(def_name, "%s_%s", id, def_suffix);
  install_typedef(struct_name, def_name);

  install_datatype_init("struct", id);

  install_kinds(sum->constructors);

  for (Constructor *c = sum->constructors; c != NULL; c = c->next) {
    install_datatype_init("union", cr->id);
    install_constructor(c);
    install_attributes(sum->attributes);
    install_datatype_named_end(c->id);
  }

  install_datatype_unnamed_end();
}

static inline void translate_rule(Rule *rule) {
  if (rule->type->kind == TYPE_PRODUCT)
    translate_sum_type(rule->id, rule->type->sum);
  else
    translate_product_type(rule->id, rule->type->product);
}

void translate_rule_chain(Rule *rules) {
  for (Rule *r = rules; r != NULL; r = r->next)
    translate_rule(r);
}
