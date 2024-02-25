#include <alloca.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "asdl.h"

#alloc trans_heap, trans_alloc, trans_realloc, trans_dump
#hashfunc translate_hash

#define MAX_TYYNAME 8

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

#define INC_INDENT() indent_level++
#define DEC_INDENT() indent_level--

#ifndef INDENT
#define INDENT "    "
#endif

static const char *BOOL = "bool";
static const char *INT = "intmax_t";
static const char *UINT = "uintmax_t";
static const char *SIZE = "ssize_t";
static const char *USIZE = "size_t";
static const char *STRING = "uint8_t*";
static const char *IDENTIFIER = "char*";

static char *def_suffix = "def";
static char *fn_suffix = "create";
static char *arg_suffix = "arg";
static char *kind_suffix = "kind";
static int indent_level = 0;

Translator translator = {0};

void assign_suffixes(char *def, char *fn, char *arg, char *kind) {
  def_suffix = def;
  fn_suffix = fn;
  arg_suffix = arg;
  kind_suffix = kind;
}

void init_translator(char *outpath) {
  translator.prelude = tmpfile();
  translator.decls = tmpfile();
  translator.defs = tmpfile();
  translator.appendage = tmpfile();
  translator.rules = NULL;
  translator.outpath = outpath;
}

void emit_prelude(char c) { putc(c, translator.prelude); }

void finalize_translator(void) {
  FILE *outfile = stdout;
  int c;

  rewind(translator.prelude);
  rewind(translator.decls);
  rewind(translator.defs);
  rewind(translator.appendage);

  if (translator.outpath != NULL)
    if (access(translator.outpath, F_OK | W_OK) == 0)
      outfile = fopen(translator.outpath, "w");

  c = 0;
  while ((c = fgetc(translator.prelude)) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);

  c = 0;
  while ((c = fgetc(translator.decls)) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);

  c = 0;
  while ((c = fgetc(translator.defs)) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);

  c = 0;
  while ((c = fgetc(translator.appendage)) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);

  if (outfile != stdin)
    fclose(outfile);
}

void dump_translator(void) {
  fclose(translator.prelude);
  fclose(translator.decls);
  fclose(translator.defs);
  fclose(translator.appendage);
  trans_dump();
}

void print_indent(void) {
  for (int i = 0; i < indent_level; i++)
    fputs(INDENT, translator.defs);
}

char *gc_strdup(char *str) {
  size_t len = strlen(str);
  char *dup = trans_alloc(len);
  return memmove(dup, str, len);
}

char *to_lowercase(char *str) {
  char *original, *copy;

  original = copy = gc_strdup(str);

  while (*copy++) {
    *copy = tolower(*copy);
  }

  return original;
}

void install_include(const char *file) {
   EMIT_DECLS("#include <%s>", file);
}

void install_typedef(const char *original, const char *alias,
                                   bool pointer) {
  EMIT_DECLS("typedef %s%s%s;\n", original, pointer ? " *" : " ", alias);
}

void install_funcdecl_init(const char *returns,
                                         const char *name) {
  EMIT_DECLS("%s %s(", returns, name);
}

void install_funcdecl_arg(const char *type, const char *name,
                                        bool last) {
  EMIT_DECLS("%s %s%s", type, name, last ? ");\n" : ", ");
}

void install_macro(const char *name, const char *def) {
  EMIT_DEFS("#define %s %s\n", name, def);
}

void install_field(const char *type, const char *name) {
  print_indent();
  EMIT_DEFS("%s %s;", type, name);
}

void install_datatype_init(const char *kind, const char *name) {
  print_indent();
  fputs("%s %s {\n", translator.defs);
}

void install_datatype_field(const char *field, const char *end) {
  print_indent();
  EMIT_DEFS("%s%s", field, end);
}

void install_datatype_named_end(const char *name) {
  print_indent();
  EMIT_DEFS("} %s;\n", name);
}

void install_datatype_unnamed_end(void) {
  print_indent();
  fputs("};\n", translator.defs);
}

void install_funcdef_init(const char *returns, const char *name) {
  EMIT_DEFS("%s %s(", returns, name);
}

void install_funcdef_arg(const char *type, const char *name,
                                       bool last) {
  EMIT_DEFS("%s %s%s", type, name, last ? ") {\n" : ", ");
}

void install_function_alloc(const char *type) {
  print_indent();
  EMIT_DEFS("%s *p = ALLOC(sizeof(%s_%s));\n\n", type, type, def_suffix);
}

void install_function_assign(const char *field,
                                           const char *value) {
  print_indent();
  EMIT_DEFS("p->%s = %s;\n", field, value);
}

void install_function_return(void) {
  print_indent();
  fputs("return p;\n}\n\n", translator.defs);
}

const char *get_type_id(TypeId *tyyid) {
  switch (tyyid->kind) {
	case TYYNAME_BOOL:
		return BOOL;
	case TYYNAME_INT:
		return INT;
	case TYYNAME_UINT:
		return UINT;
	case TYYNAME_SIZE:
		return SIZE;
	case TYYNAME_USIZE:
		return USIZE;
	case TYYNAME_STRING:
		return STRING;
	case TYYNAME_IDENTIFIER:
		return IDENTIFIER;
	default:
		return tyyid->value;

  }
}

void translate_product_type(char *id, Product *product) {
  char *tyy = NULL;
  char *def = NULL;
  char *fn = NULL;
  STR_FORMAT(tyy, "union %s", id);
  STR_FORMAT(def, "%s_%s", id, def_suffix);
  STR_FORMAT(fn, "%s_%s", id, fn_suffix);

  install_typedef(tyy, def, true);
  install_funcdecl_init(def, fn);

  install_datatype_init("union", id);

  INC_INDENT();

  size_t n = 0;
  for (Field *f = product->fields; f != NULL; f = f->next, n++) {
    char *argtyy = NULL;
    char *argname = NULL;
    char *cache = NULL;
    STR_FORMAT(argtyy, "%s_%s", get_type_id(f->type_id), def_suffix);

    if (f->id == NULL) {
      STR_FORMAT(argname, "%s_%s%lu", argtyy, arg_suffix, n);
      STR_FORMAT(cache, "%s %s_%lu", argtyy, get_type_id(f->type_id), n);
    } else {
      STR_FORMAT(argname, "%s_%s", f->id, arg_suffix);
      STR_FORMAT(cache, "%s %s", argtyy, f->id);
    }

    install_funcdecl_arg(argtyy, argname, f->next == NULL);
    install_datatype_field(cache, ";");

    f->cache = cache;
  }

  DEC_INDENT();

  install_datatype_unnamed_end();
}

void install_asdl_field(Field *field, size_t num) {
  char *tyy = NULL;
  char *name = NULL;
  char *count = NULL;
  char *exists = NULL;

  switch (field->kind) {
  case FIELD_NORMAL:
    tyy = NULL;
    name = NULL;
    STR_FORMAT(tyy, "%s_%s", get_type_id(field->type_id), def_suffix);

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_type_id(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
    }

    install_field(tyy, name);
    break;
  case FIELD_SEQUENCE:
    tyy = NULL;
    name = NULL;

    STR_FORMAT(tyy, "%s_%s*", get_type_id(field->type_id), def_suffix);

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_type_id(field->type_id), num);
      STR_FORMAT(count, "%s_%lu_count", get_type_id(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
      STR_FORMAT(count, "%s_count", field->id);
    }

    install_field(tyy, name);
    install_field("long", count);

    break;
  case FIELD_OPTIONAL:
    tyy = NULL;
    name = NULL;

    STR_FORMAT(tyy, "%s_%s*", get_type_id(field->type_id), def_suffix);

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_type_id(field->type_id), num);
      STR_FORMAT(exists, "%s_%lu_exists", get_type_id(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
      STR_FORMAT(exists, "%s_exists", field->id);
    }

    install_field(tyy, name);
    install_field("int", exists);

    break;
  }
}

void install_constructor(Constructor *constructor) {
  INC_INDENT();

  size_t n = 0;
  for (Field *f = constructor->fields; f != NULL; f = f->next, n++) {
    install_asdl_field(f, n);
  }

  DEC_INDENT();
}

void install_attributes(Field *attributes) {
  INC_INDENT();

  size_t n = 0;
  for (Field *f = attributes; f != NULL; f = f->next, n++)
    install_asdl_field(f, n);

  DEC_INDENT();
}

void install_kinds(Constructor *constructors) {
  install_datatype_init("enum", " ");

  INC_INDENT();

  for (Constructor *c = constructors; c != NULL; c = c->next) {
    char *kind_name = NULL;
    STR_FORMAT(kind_name, "%s_%s", to_lowercase(c->id), kind_suffix);
    install_datatype_field(kind_name, ",");
  }

  DEC_INDENT();

  install_datatype_named_end("kind");
}

void install_constructor_function(char *id,
                                                Constructor *constructor,
                                                Field *attributes) {
  char *lc_ident = to_lowercase(constructor->id);

  char *returns = NULL;
  char *fnname = NULL;
  STR_FORMAT(returns, "%s_%s", id, def_suffix);
  STR_FORMAT(fnname, "create_%s", lc_ident);

  install_funcdecl_init(returns, fnname);
  install_funcdef_init(returns, fnname);

  char *argtyy = NULL;
  char *argname = NULL;

  size_t n = 0;
  for (Field *f = constructor->fields; f != NULL; f = f->next, n++) {
    argtyy = NULL;
    argname = NULL;

    STR_FORMAT(argtyy, "%s_%s", get_type_id(f->type_id), def_suffix);

    if (f->id == NULL) {
      STR_FORMAT(argname, "%s_%lu", get_type_id(f->type_id), n);
    } else {
      STR_FORMAT(argname, "%s", f->id);
    }

    f->cache = argname;

    install_funcdecl_arg(argtyy, argname, f->next == NULL);
    install_funcdef_arg(argtyy, argname, f->next == NULL);
  }

  n = 0;
  for (Field *f = attributes; f != NULL; f = f->next, n++) {
    argtyy = NULL;
    argname = NULL;

    STR_FORMAT(argtyy, "%s_%s", get_type_id(f->type_id), def_suffix);

    if (f->id == NULL) {
      STR_FORMAT(argname, "%s_%lu", get_type_id(f->type_id), n);
    } else {
      STR_FORMAT(argname, "%s", f->id);
    }

    f->cache = argname;

    install_funcdecl_arg(argtyy, argname, f->next == NULL);
    install_funcdef_arg(argtyy, argname, f->next == NULL);
  }

  INC_INDENT();

  install_function_alloc(id);

  char *assignname = NULL;

  for (Field *f = constructor->fields; f != NULL; f = f->next) {
    assignname = NULL;

    STR_FORMAT(assignname, "%s->%s", lc_ident, f->cache);

    install_function_assign(assignname, "NULL");
  }

  for (Field *f = attributes; f != NULL; f = f->next) {
    assignname = NULL;

    STR_FORMAT(assignname, "%s->%s", lc_ident, f->cache);

    install_function_assign(assignname, "NULL");
  }

  install_function_return();

  DEC_INDENT();
}

void translate_sum_type(char *id, Sum *sum) {
  char *struct_name = NULL;
  char *def_name = NULL;
  STR_FORMAT(struct_name, "struct %s", id);
  STR_FORMAT(def_name, "%s_%s", id, def_suffix);
  install_typedef(struct_name, def_name, true);

  install_datatype_init("struct", id);

  INC_INDENT();

  install_kinds(sum->constructors);

  for (Constructor *c = sum->constructors; c != NULL; c = c->next) {
    install_datatype_init("union", c->id);
    install_constructor(c);
    install_attributes(sum->attributes);
    install_datatype_named_end(c->id);
  }

  DEC_INDENT();

  install_datatype_unnamed_end();

  fputs("\n\n\n", translator.defs);

  for (Constructor *c = sum->constructors; c != NULL; c = c->next) {
    install_constructor_function(id, c, sum->attributes);
  }
}

void install_standard_includes(void) {
   install_include("stdio.h");
   install_include("stdlib.h");
   install_include("stddef.h");
   install_include("stdbool.h");
   install_include("stdint.h");
}

void translate_rule(Rule *rule) {
  if (rule->type->kind == TYPE_PRODUCT)
    translate_product_type(rule->id, rule->type->product);
  else
    translate_sum_type(rule->id, rule->type->sum);
}

void translate_rule_chain(Rule *rules) {
  install_standard_includes();

  for (Rule *r = rules; r != NULL; r = r->next)
    translate_rule(r);
}
