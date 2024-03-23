#include <alloca.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

#define PRINTF_PRELUDE(fmt, ...) fprintf(translator.prelude, fmt, __VA_ARGS__)
#define PRINTF_DECLS(fmt, ...) fprintf(translator.decls, fmt, __VA_ARGS__)
#define PRINTF_DEFS(fmt, ...) fprintf(translator.defs, fmt, __VA_ARGS__)
#define PRINTF_APPENDAGE(fmt, ...)                                             \
  fprintf(translator.appendage, fmt, __VA_ARGS__)

#define PUTC_PRELUDE(c) fputc(c, translator.prelude)
#define PUTC_DECLS(c) fputc(c, translator.decls)
#define PUTC_DEFS(c) fputc(c, translator.defs)
#define PUTC_APPENDAGE(c) fputc(c, translator.appendage)

#define PUTS_PRELUDE(s) fputs(s, translator.prelude)
#define PUTS_DECLS(s) fputs(s, translator.decls)
#define PUTS_DEFS(s) fputs(s, translator.defs)
#define PUTS_APPENDAGE(s) fputs(s, translator.appendage)

#define NEWLINE_PRELUDE() fputs("\n", translator.prelude)
#define NEWLINE_DECLS() fputs("\n", translator.decls)
#define NEWLINE_DEFS() fputs("\n", translator.defs)
#define NEWLINE_APPENDAGE() fputs("\n", translator.appendage)

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
static const char *STRING = "string_t";
static const char *IDENTIFIER = "identifier_t";

static const char *BOOL_NAME = "boolean";
static const char *INT_NAME = "int";
static const char *UINT_NAME = "uint";
static const char *SIZE_NAME = "size";
static const char *USIZE_NAME = "usize";
static const char *STRING_NAME = "string";
static const char *IDENTIFIER_NAME = "identifier";

static char *fn_prefix = NULL;

static char *def_suffix = "def";
static char *fn_suffix = "create";
static char *arg_suffix = "arg";
static char *kind_suffix = "kind";

static int indent_level = 0;

Translator translator = {0};

static inline void print_time_signature(FILE *out) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fprintf(out,
          "/* Generated by ZephyrASDL at: %02d-%02d-%d %02d:%02d:%02d */\n",
          tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min,
          tm.tm_sec);
}

void assign_suffixes(char *def, char *fn, char *arg, char *kind) {
  def_suffix = def;
  fn_suffix = fn;
  arg_suffix = arg;
  kind_suffix = kind;
}

void assign_prefixes(char *fn) { fn_prefix = fn; }

void init_translator(char *outpath) {
  translator.prelude = tmpfile();
  translator.decls = tmpfile();
  translator.defs = tmpfile();
  translator.appendage = tmpfile();
  translator.rules = NULL;
  translator.outpath = outpath;
}

void emit_prelude(char c) { PUTC_PRELUDE(c); }
void emit_appendage(char c) { PUTC_APPENDAGE(c); }

void finalize_translator(void) {
  FILE *outfile = stdout;
  int c;

  rewind(translator.prelude);
  rewind(translator.decls);
  rewind(translator.defs);
  rewind(translator.appendage);

  if (translator.outpath != NULL)
    outfile = fopen(translator.outpath, "w");

  fputc('\n', outfile);

  print_time_signature(outfile);

  fputs("\n/*----Begin Prelude---*/\n", outfile);

  c = 0;
  while ((c = fgetc(translator.prelude)) != EOF)
    fputc(c, outfile);

  fputs("\n/*----End Prelude---*/\n", outfile);

  fputc('\n', outfile);

  fputs("\n/*----Begin Declarations---*/\n", outfile);

  c = 0;
  while ((c = fgetc(translator.decls)) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);

  fputs("\n/*----End Declarations---*/\n", outfile);

  fputs("\n/*----Begin Definitions---*/\n", outfile);

  c = 0;
  while ((c = fgetc(translator.defs)) != EOF)
    fputc(c, outfile);

  fputc('\n', outfile);

  fputs("\n/*----End Definitions---*/\n", outfile);

  fputs("\n/*----Begin Appendage---*/\n", outfile);

  c = 0;
  while ((c = fgetc(translator.appendage)) != EOF)
    fputc(c, outfile);

  fputs("\n/*----End Prelude---*/\n", outfile);

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

static inline void print_indent(void) {
  for (int i = 0; i < indent_level; i++)
    PUTS_DEFS(INDENT);
}

char *to_lowercase(char *str) {
  size_t len = strlen(str);
  char *dup = trans_alloc(len);
  memmove(dup, str, len);

  while (len--)
    dup[len] = tolower(dup[len]);

  return dup;
}

char *to_uppercase(char *str) {
  size_t len = strlen(str);
  char *dup = trans_alloc(len);
  memmove(dup, str, len);

  while (len--)
    dup[len] = toupper(dup[len]);

  return dup;
}

void install_include(const char *file) {
  PRINTF_PRELUDE("#include <%s>\n", file);
}

void install_typedef(const char *original, const char *alias, bool pointer) {
  PRINTF_PRELUDE("typedef %s%s%s;\n", original, pointer ? " *" : " ", alias);
}

void install_funcdecl_init(const char *returns, const char *name) {
  if (fn_prefix != NULL) {
    PUTS_DECLS(fn_prefix);
  }
  PRINTF_DECLS("%s %s(", returns, name);
}

void install_funcdecl_arg(const char *type, const char *name, bool last) {
  PRINTF_DECLS("%s %s%s", type, name, last ? ");\n" : ", ");
}

void install_pp_directive(const char *dir) { PRINTF_DECLS("#%s\n", dir); }

void install_macro(const char *name, const char *def) {
  PRINTF_DECLS("#define %s %s\n", name, def);
}

void install_appendage_macro(const char *name, const char *def) {
  PRINTF_APPENDAGE("#define %s %s\n", name, def);
}

void install_field(const char *type, const char *name) {
  print_indent();
  PRINTF_DEFS("%s %s;\n", type, name);
}

void install_sequence_field(char *type, char *name) {
  print_indent();
  PUTS_DEFS("struct {\n");
  INC_INDENT();
  print_indent();
  PRINTF_DEFS("%s %s;\n", type, name);
  print_indent();
  PRINTF_DEFS("ssize_t %s_count;\n", name);
  DEC_INDENT();
  print_indent();
  PRINTF_DEFS("} %s_seq;\n", name);
}

void install_optional_field(char *type, char *name) {
  print_indent();
  PUTS_DEFS("struct {\n");
  INC_INDENT();
  print_indent();
  PRINTF_DEFS("%s %s;\n", type, name);
  print_indent();
  PRINTF_DEFS("bool %s_exists;\n", name);
  DEC_INDENT();
  print_indent();
  PRINTF_DEFS("} %s_opt;\n", name);
}

void install_datatype_init(const char *kind, const char *name) {
  print_indent();
  PRINTF_DEFS("%s %s {\n", kind, name);
}

void install_datatype_field(const char *field, const char *end) {
  print_indent();
  PRINTF_DEFS("%s%s\n", field, end);
}

void install_datatype_named_end(const char *name) {
  print_indent();
  PRINTF_DEFS("} %s;\n", name);
}

void install_datatype_unnamed_end(void) {
  print_indent();
  PUTS_DEFS("};\n");
}

void install_funcdef_init(const char *returns, const char *name) {
  if (fn_prefix != NULL) {
    PUTS_DECLS(fn_prefix);
  }
  PRINTF_DEFS("%s %s(", returns, name);
}

void install_funcdef_arg(const char *type, const char *name, bool last) {
  PRINTF_DEFS("%s %s%s", type, name, last ? ") {\n" : ", ");
}

void install_function_alloc(const char *type) {
  print_indent();
  PRINTF_DEFS("%s p = ALLOC(sizeof(%s));\n\n", type, type);
}

void install_function_assign(const char *field, const char *value) {
  print_indent();
  PRINTF_DEFS("p->%s = %s;\n", field, value);
}

void install_function_return(void) {
  print_indent();
  PUTS_DEFS("return p;\n}\n\n");
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

const char *get_argname(TypeId *tyyid) {
  switch (tyyid->kind) {
  case TYYNAME_BOOL:
    return BOOL_NAME;
  case TYYNAME_INT:
    return INT_NAME;
  case TYYNAME_UINT:
    return UINT_NAME;
  case TYYNAME_SIZE:
    return SIZE_NAME;
  case TYYNAME_USIZE:
    return USIZE_NAME;
  case TYYNAME_STRING:
    return STRING_NAME;
  case TYYNAME_IDENTIFIER:
    return IDENTIFIER_NAME;
  default:
    return tyyid->value;
  }
}

static inline int random_integer(void) { return rand(); }

void install_stub(char *id) {
  char *enumeration = NULL;
  char *macro_name = NULL;

  STR_FORMAT(enumeration, "%d", random_integer());
  STR_FORMAT(macro_name, "ENUM_%s", to_uppercase(id));

  install_macro(macro_name, enumeration);
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

    if (f->type_id->kind == TYYNAME_ID) {
      STR_FORMAT(argtyy, "%s_%s", get_type_id(f->type_id), def_suffix);
    } else {
      STR_FORMAT(argtyy, "%s", get_type_id(f->type_id));
    }

    if (f->id == NULL) {
      STR_FORMAT(argname, "%s_%s%lu", get_argname(f->type_id), arg_suffix, n);
    } else {
      STR_FORMAT(argname, "%s_%s", f->id, arg_suffix);
    }

    install_funcdecl_arg(argtyy, argname, f->next == NULL);
    install_datatype_field(cache, ";");
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
    if (field->type_id->kind == TYYNAME_ID) {
      STR_FORMAT(tyy, "%s_%s", get_type_id(field->type_id), def_suffix);
    } else {
      STR_FORMAT(tyy, "%s", get_type_id(field->type_id));
    }

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_argname(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
    }

    install_field(tyy, name);
    break;
  case FIELD_SEQUENCE:
    tyy = NULL;
    name = NULL;

    if (field->type_id->kind == TYYNAME_ID) {
      STR_FORMAT(tyy, "%s_%s*", get_type_id(field->type_id), def_suffix);
    } else {
      STR_FORMAT(tyy, "%s*", get_type_id(field->type_id));
    }

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_argname(field->type_id), num);
      STR_FORMAT(count, "%s_%lu_count", get_argname(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
      STR_FORMAT(count, "%s_count", field->id);
    }

    install_sequence_field(tyy, name);
    break;
  case FIELD_OPTIONAL:
    tyy = NULL;
    name = NULL;

    if (field->type_id->kind == TYYNAME_ID) {
      STR_FORMAT(tyy, "%s_%s", get_type_id(field->type_id), def_suffix);
    } else {
      STR_FORMAT(tyy, "%s", get_type_id(field->type_id));
    }

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_argname(field->type_id), num);
      STR_FORMAT(exists, "%s_%lu_exists", get_argname(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
      STR_FORMAT(exists, "%s_exists", field->id);
    }

    install_optional_field(tyy, name);
    break;
  }
}

void install_constructor(Constructor *constructor) {
  if (constructor->fields == NULL) {
    install_stub(constructor->id);
    return;
  }

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
    STR_FORMAT(kind_name, "%s_%s", to_uppercase(c->id), kind_suffix);
    install_datatype_field(kind_name, ",");
  }

  DEC_INDENT();

  install_datatype_named_end("kind");
}

void install_constructor_function(char *id, Constructor *constructor,
                                  Field *attributes) {

  if (constructor->fields == NULL)
    return;

  char *lc_ident = to_lowercase(constructor->id);

  char *returns = NULL;
  char *fnname = NULL;
  STR_FORMAT(returns, "%s_%s", id, def_suffix);
  STR_FORMAT(fnname, "create_%s", lc_ident);

  install_funcdecl_init(returns, fnname);
  install_funcdef_init(returns, fnname);

  char *argtyy = NULL;
  char *argname = NULL;
  char *swap = NULL;

  size_t n = 0;
  for (Field *f = constructor->fields; f != NULL; f = f->next, n++) {
    argtyy = NULL;
    argname = NULL;
    swap = NULL;

    if (f->type_id->kind == TYYNAME_ID) {
      STR_FORMAT(argtyy, "%s_%s", get_type_id(f->type_id), def_suffix);
    } else {
      STR_FORMAT(argtyy, "%s", get_type_id(f->type_id));
    }

    if (f->id == NULL) {
      STR_FORMAT(argname, "%s_%lu", get_argname(f->type_id), n);
    } else {
      STR_FORMAT(argname, "%s", f->id);
    }

    if (f->kind == FIELD_SEQUENCE) {
      swap = argtyy;
      STR_FORMAT(argtyy, "%s*", swap);
    }

    f->cache[0] = argtyy;
    f->cache[1] = argname;

    install_funcdecl_arg(argtyy, argname,
                         f->next == NULL && f->kind == FIELD_NORMAL);
    install_funcdef_arg(argtyy, argname,
                        f->next == NULL && f->kind == FIELD_NORMAL);

    if (f->kind == FIELD_OPTIONAL) {
      STR_FORMAT(argname, "%s_exists", f->cache[1]);
      install_funcdecl_arg("bool", argname, f->next == NULL);
      install_funcdef_arg("bool", argname, f->next == NULL);
    } else if (f->kind == FIELD_SEQUENCE) {
      STR_FORMAT(argname, "%s_count", f->cache[1]);
      install_funcdecl_arg("ssize_t", argname, f->next == NULL);
      install_funcdef_arg("ssize_t", argname, f->next == NULL);
    }
  }

  n = 0;
  for (Field *f = attributes; f != NULL; f = f->next, n++) {
    argtyy = NULL;
    argname = NULL;
    swap = NULL;

    if (f->type_id->kind == TYYNAME_ID) {
      STR_FORMAT(argtyy, "%s_%s", get_type_id(f->type_id), def_suffix);
    } else {
      STR_FORMAT(argtyy, "%s", get_type_id(f->type_id));
    }

    if (f->id == NULL) {
      STR_FORMAT(argname, "%s_%lu", get_argname(f->type_id), n);
    } else {
      STR_FORMAT(argname, "%s", f->id);
    }

    if (f->kind == FIELD_SEQUENCE) {
      swap = argtyy;
      STR_FORMAT(argtyy, "%s*", swap);
    }

    f->cache[0] = argtyy;
    f->cache[1] = argname;

    install_funcdecl_arg(argtyy, argname,
                         f->next == NULL && f->kind == FIELD_NORMAL);
    install_funcdef_arg(argtyy, argname,
                        f->next == NULL && f->kind == FIELD_NORMAL);

    if (f->kind == FIELD_OPTIONAL) {
      STR_FORMAT(argname, "%s_exists", f->cache[1]);
      install_funcdecl_arg("bool", argname, f->next == NULL);
      install_funcdef_arg("bool", argname, f->next == NULL);
    } else if (f->kind == FIELD_SEQUENCE) {
      STR_FORMAT(argname, "%s_count", f->cache[1]);
      install_funcdecl_arg("ssize_t", argname, f->next == NULL);
      install_funcdef_arg("ssize_t", argname, f->next == NULL);
    }
  }

  char *ptyy = NULL;

  STR_FORMAT(ptyy, "%s_%s", id, def_suffix);

  INC_INDENT();

  install_function_alloc(ptyy);

  PUTS_DEFS("\n");

  char *assignname = NULL;
  char *assignval = NULL;

  char *macro_name = NULL;
  char *macro_def = NULL;

  STR_FORMAT(assignname, "%s_%s", to_uppercase(constructor->id), kind_suffix);

  install_function_assign("kind", assignname);

  for (Field *f = constructor->fields; f != NULL; f = f->next) {
    assignname = NULL;
    assignval = NULL;

    if (f->kind == FIELD_SEQUENCE) {
      STR_FORMAT(assignname, "value.%s.%s_seq.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      STR_FORMAT(assignval, "(%s)%s", f->cache[0], f->cache[1]);
      install_function_assign(assignname, assignval);
      STR_FORMAT(assignname, "value.%s.%s_seq.%s_count", lc_ident, f->cache[1],
                 f->cache[1]);
      install_function_assign(assignname, "0");

      STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_seq.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_appendage_macro(macro_name, macro_def);

      STR_FORMAT(macro_name, "GET_%s_%s_count(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_seq.%s_count", lc_ident,
                 f->cache[1], f->cache[1]);
      install_appendage_macro(macro_name, macro_def);

      continue;
    } else if (f->kind == FIELD_OPTIONAL) {
      STR_FORMAT(assignname, "value.%s.%s_opt.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_function_assign(assignname, f->cache[1]);
      STR_FORMAT(assignname, "value.%s.%s_opt.%s_exists", lc_ident, f->cache[1],
                 f->cache[1]);
      STR_FORMAT(assignval, "%s_exists", f->cache[1]);
      install_function_assign(assignname, assignval);

      STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_opt.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_appendage_macro(macro_name, macro_def);

      STR_FORMAT(macro_name, "GET_%s_%s_exists(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_opt.%s_exists", lc_ident,
                 f->cache[1], f->cache[1]);
      install_appendage_macro(macro_name, macro_def);

      continue;
    }

    STR_FORMAT(assignname, "value.%s.%s", lc_ident, f->cache[1]);
    install_function_assign(assignname, f->cache[1]);

    STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
    STR_FORMAT(macro_def, "v->value.%s.%s", lc_ident, f->cache[1]);
    install_appendage_macro(macro_name, macro_def);
  }

  for (Field *f = attributes; f != NULL; f = f->next) {
    assignname = NULL;
    assignval = NULL;

    if (f->kind == FIELD_SEQUENCE) {
      STR_FORMAT(assignname, "value.%s.%s_seq.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      STR_FORMAT(assignval, "(%s)%s", f->cache[0], f->cache[1]);
      install_function_assign(assignname, assignval);
      STR_FORMAT(assignname, "value.%s.%s_seq.%s_count", lc_ident, f->cache[1],
                 f->cache[1]);
      install_function_assign(assignname, "0");

      STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_seq.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_appendage_macro(macro_name, macro_def);

      STR_FORMAT(macro_name, "GET_%s_%s_count(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_seq.%s_count", lc_ident,
                 f->cache[1], f->cache[1]);
      install_appendage_macro(macro_name, macro_def);

      continue;
    } else if (f->kind == FIELD_OPTIONAL) {
      STR_FORMAT(assignname, "value.%s.%s_opt.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_function_assign(assignname, f->cache[1]);
      STR_FORMAT(assignname, "value.%s.%s_opt.%s_exists", lc_ident, f->cache[1],
                 f->cache[1]);
      STR_FORMAT(assignval, "%s_exists", f->cache[1]);
      install_function_assign(assignname, assignval);

      STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_opt.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_appendage_macro(macro_name, macro_def);

      STR_FORMAT(macro_name, "GET_%s_%s_exists(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_opt.%s_exists", lc_ident,
                 f->cache[1], f->cache[1]);
      install_appendage_macro(macro_name, macro_def);

      continue;
    }

    STR_FORMAT(assignname, "value.%s.%s", lc_ident, f->cache[1]);
    install_function_assign(assignname, f->cache[1]);

    STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
    STR_FORMAT(macro_def, "v->value.%s.%s", lc_ident, f->cache[1]);
    install_appendage_macro(macro_name, macro_def);
  }

  install_function_return();

  DEC_INDENT();
}

void install_append_function(const char *id, const char *def_name) {
  char *fn_name = NULL;
  char *argtyy_head = NULL;
  char *argname_head = NULL;
  char *argname_append = NULL;

  STR_FORMAT(fn_name, "append_%s_chain", id);
  STR_FORMAT(argtyy_head, "%s*", def_name);
  STR_FORMAT(argname_head, "%s_head", def_name);
  STR_FORMAT(argname_append, "%s_append", id);

  install_funcdecl_init("void", fn_name);
  install_funcdef_init("void", fn_name);

  install_funcdecl_arg(argtyy_head, argname_head, false);
  install_funcdef_arg(argtyy_head, argname_head, false);

  install_funcdecl_arg(def_name, argname_append, true);
  install_funcdef_arg(def_name, argname_append, true);

  INC_INDENT();

  print_indent();
  PRINTF_DEFS("%s->next = *%s;\n", argname_append, argname_head);

  print_indent();
  PRINTF_DEFS("*%s = %s;\n", argname_head, argname_append);

  DEC_INDENT();

  PUTS_DEFS("\n}\n\n");
}

void install_destroy_function(const char *id, const char *def_name) {
  char *fn_name = NULL;
  char *arg_tyy = NULL;
  char *arg_name = NULL;

  STR_FORMAT(fn_name, "destroy_%s_chain", id);
  STR_FORMAT(arg_tyy, "%s*", def_name);
  STR_FORMAT(arg_name, "%s_head", id);

  install_funcdecl_init("void", fn_name);
  install_funcdef_init("void", fn_name);

  install_funcdecl_arg(arg_tyy, arg_name, true);
  install_funcdef_arg(arg_tyy, arg_name, true);

  INC_INDENT();
  print_indent();
  PRINTF_DEFS("for (%s item = *%s; item != NULL; item = item->next) {\n",
              def_name, arg_name);
  INC_INDENT();
  print_indent();
  PUTS_DEFS("FREE(item);\n");
  DEC_INDENT();
  print_indent();
  PUTS_DEFS("}\n");
  DEC_INDENT();
  PUTS_DEFS("}\n\n");
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

  if (sum->constructors == NULL) {
    fprintf(stderr, "Error: empty tree\n");
    exit(EXIT_FAILURE);
  }

  install_datatype_init("union", "");

  INC_INDENT();

  for (Constructor *c = sum->constructors; c != NULL; c = c->next) {
    install_datatype_init("struct", c->id);
    install_constructor(c);
    install_attributes(sum->attributes);
    install_datatype_named_end(to_lowercase(c->id));
  }

  DEC_INDENT();

  install_datatype_named_end("value");

  install_field(def_name, "next");

  DEC_INDENT();

  install_datatype_unnamed_end();

  PUTS_DEFS("\n\n\n");

  for (Constructor *c = sum->constructors; c != NULL; c = c->next) {
    install_constructor_function(id, c, sum->attributes);
  }

  PUTS_DEFS("\n\n");

  install_append_function(id, def_name);
  install_destroy_function(id, def_name);
}

void install_standard_includes(void) {
  install_include("stdio.h");
  install_include("stdlib.h");
  install_include("stddef.h");
  install_include("stdbool.h");
  install_include("stdint.h");
  fputc('\n', translator.prelude);
}

void install_standard_typedefs(void) {
  install_typedef("uint8_t", "string_t", true);
  install_typedef("char", "identifier_t", true);
}

void install_alloc_macro(void) {
  install_pp_directive("ifndef ALLOC");
  install_macro("ALLOC(size)", "malloc(size)");
  install_pp_directive("endif");
}

void install_realloc_macro(void) {
  install_pp_directive("ifndef REALLOC");
  install_macro("REALLOC(mem, size)", "realloc(mem, size)");
  install_pp_directive("endif");
}

void install_free_macro(void) {
  install_pp_directive("ifndef FREE");
  install_macro("FREE(mem)", "free(mem)");
  install_pp_directive("endif");
}

void translate_rule(Rule *rule) {
  if (rule->type->kind == TYPE_PRODUCT)
    translate_product_type(rule->id, rule->type->product);
  else
    translate_sum_type(rule->id, rule->type->sum);
}

void translate_rule_chain(Rule *rules) {
  install_standard_includes();
  install_standard_typedefs();
  install_alloc_macro();
  install_realloc_macro();
  install_free_macro();

  PUTS_DECLS("\n\n");

  for (Rule *r = rules; r != NULL; r = r->next)
    translate_rule(r);
}
