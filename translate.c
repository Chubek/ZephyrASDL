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

#define MAX_TYPENAME 8
#define HEADER_GUARD_LEN 5

#define STR_FORMAT(dest, fmt, ...)                                             \
  do {                                                                         \
    size_t l = strlen(fmt) + UCHAR_MAX;                                        \
    dest = trans_alloc(l);                                                     \
    snprintf(dest, l, fmt, __VA_ARGS__);                                       \
  } while (0)

#define PRINTF_LOCATORS(fmt, ...) fprintf(translator.locators, fmt, __VA_ARGS__)
#define PRINTF_PRELUDE(fmt, ...) fprintf(translator.prelude, fmt, __VA_ARGS__)
#define PRINTF_TYDEFS(fmt, ...) fprintf(translator.tydefs, fmt, __VA_ARGS__)
#define PRINTF_TYSPECS(fmt, ...) fprintf(translator.tyspecs, fmt, __VA_ARGS__)
#define PRINTF_DECLS(fmt, ...) fprintf(translator.decls, fmt, __VA_ARGS__)
#define PRINTF_DEFS(fmt, ...) fprintf(translator.defs, fmt, __VA_ARGS__)
#define PRINTF_APPENDAGE(fmt, ...)                                             \
  fprintf(translator.appendage, fmt, __VA_ARGS__)

#define PUTC_LOCATORS(c) fputc(c, translator.locators)
#define PUTC_PRELUDE(c) fputc(c, translator.prelude)
#define PUTC_TYDEFS(c) fputc(c, translator.tydefs)
#define PUTC_TYSPECS(c) fputc(c, translator.tyspecs)
#define PUTC_DECLS(c) fputc(c, translator.decls)
#define PUTC_DEFS(c) fputc(c, translator.defs)
#define PUTC_APPENDAGE(c) fputc(c, translator.appendage)

#define PUTS_LOCATORS(s) futs(s, translator.locators)
#define PUTS_PRELUDE(s) fputs(s, translator.prelude)
#define PUTS_TYDEFS(s) fputs(s, translator.tydefs)
#define PUTS_TYSPECS(s) fputs(s, translator.tyspecs)
#define PUTS_DECLS(s) fputs(s, translator.decls)
#define PUTS_DEFS(s) fputs(s, translator.defs)
#define PUTS_APPENDAGE(s) fputs(s, translator.appendage)

#define NEWLINE_LOCATORS() fputs("\n", translator.locators)
#define NEWLINE_PRELUDE() fputs("\n", translator.prelude)
#define NEWLINE_TYDEFS() fputs("\n", translator.tydefs)
#define NEWLINE_TYSPECS() fputs("\n", translator.tyspecs)
#define NEWLINE_DECLS() fputs("\n", translator.decls)
#define NEWLINE_DEFS() fputs("\n", translator.defs)
#define NEWLINE_APPENDAGE() fputs("\n", translator.appendage)

#define INC_INDENT() indent_level++
#define DEC_INDENT() indent_level--

#ifndef INDENT
#define INDENT "    "
#endif

static const char *BOOL = "bool";
static const char *INT8 = "int8_t";
static const char *UINT8 = "uint8_t";
static const char *INT16 = "int16_t";
static const char *UINT16 = "uint16_t";
static const char *INT32 = "int32_t";
static const char *UINT32 = "uint32_t";
static const char *INT64 = "int64_t";
static const char *UINT64 = "uint64_t";
static const char *CHAR = "char";
static const char *UCHAR = "unsigned char";
static const char *FLOAT32 = "float";
static const char *FLOAT64 = "double";
static const char *FLOAT80 = "long double";
static const char *SIZE = "ssize_t";
static const char *USIZE = "size_t";
static const char *STRING = "string_t";
static const char *IDENTIFIER = "identifier_t";
static const char *BYTEARRAY = "bytearray_t";

static const char *BOOL_kEYWORD = "bool";
static const char *INT8_kEYWORD = "int8";
static const char *UINT8_kEYWORD = "uint8";
static const char *INT16_kEYWORD = "int16";
static const char *UINT16_kEYWORD = "uint16";
static const char *INT32_kEYWORD = "int32";
static const char *UINT32_kEYWORD = "uint32";
static const char *INT64_kEYWORD = "int64";
static const char *UINT64_kEYWORD = "uint64";
static const char *CHAR_kEYWORD = "char";
static const char *UCHAR_kEYWORD = "uchar";
static const char *FLOAT32_kEYWORD = "float32";
static const char *FLOAT64_kEYWORD = "float64";
static const char *FLOAT80_kEYWORD = "float80";
static const char *SIZE_kEYWORD = "size";
static const char *USIZE_kEYWORD = "usize";
static const char *STRING_kEYWORD = "string";
static const char *IDENTIFIER_kEYWORD = "identifier";
static const char *BYTEARRAY_kEYWORD = "bytearray";

static char *fn_prefix = NULL;

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

void assign_prefixes(char *fn) { fn_prefix = fn; }

void init_translator(char *outpath, char *sympath) {
  translator.locators = tmpfile();
  translator.prelude = tmpfile();
  translator.tydefs = tmpfile();
  translator.tyspecs = tmpfile();
  translator.decls = tmpfile();
  translator.defs = tmpfile();
  translator.appendage = tmpfile();
  translator.rules = NULL;
  translator.outpath = outpath;
  translator.sympath = sympath;
}

void emit_prelude(char *s) { PUTS_PRELUDE(s); }
void emit_appendage(char *s) { PUTS_APPENDAGE(s); }

static inline void print_outfile_time_signature(FILE *outfile) {
  if (outfile == NULL)
    return;

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fprintf(outfile,
          "\n\n/* Generated by ZephyrASDL (github.com/Chubek/ZephyrASDL) at: "
          "%02d-%02d-%d %02d:%02d:%02d */\n\n",
          tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min,
          tm.tm_sec);
}

static inline void print_outfile_start_section(char *marker, FILE *outfile) {
  if (outfile == NULL)
    return;
  fprintf(outfile, "\n\n/*---BEGIN-SECTION: %s---*/\n\n", marker);
}

static inline void print_outfile_end_section(char *marker, FILE *outfile) {
  if (outfile == NULL)
    return;
  fprintf(outfile, "\n\n/*---END-SECTION: %s---*/\n\n", marker);
}

static inline void print_outfile_license(FILE *outfile) {
  if (outfile == NULL)
    return;

  char *name = NULL;
  if ((name = getenv("USER")) != NULL)
    fprintf(outfile,
            "\n\n/* Generated by: %s; License belongs to the author "
            "of the ASDL file */\n\n",
            name);
  else
    fprintf(outfile, "\n\n/* Generated file; License belongs to the author of "
                     "the ASDL file */\n\n");
}

static inline void print_symfile_header_guard_start(FILE *symfile, char *name) {
  char *name_dup = strndup(name, FILENAME_MAX + 1);
  name_dup[FILENAME_MAX] = '\0';
  size_t n;
  for (n = 0; n < FILENAME_MAX && name_dup[n] != '.' && name_dup[n] != '\0';
       n++)
    name_dup[n] =
        (ispunct(name[n]) || isblank(name[n])) ? '_' : toupper(name_dup[n]);
  name_dup[n] = '\0';

  fprintf(symfile, "#ifndef %s_H\n", &name_dup[0]);
  fprintf(symfile, "#define %s_H\n\n", &name_dup[0]);

  fprintf(symfile, "\n/* Symbols Declaration File: %s */\n\n", name);

  free(name_dup);
}

static inline void print_symfile_header_guard_end(FILE *symfile, char *name) {
  fprintf(symfile, "\n#endif /* %s */\n\n", name);
}

void finalize_translator(void) {
  FILE *outfile = stdout;
  FILE *symfile = NULL;
  int c;

  rewind(translator.locators);
  rewind(translator.prelude);
  rewind(translator.tydefs);
  rewind(translator.tyspecs);
  rewind(translator.decls);
  rewind(translator.defs);
  rewind(translator.appendage);

  if (translator.outpath != NULL)
    outfile = fopen(translator.outpath, "w");

  if (translator.sympath != NULL)
    symfile = fopen(translator.sympath, "w");

  print_outfile_time_signature(outfile);

  if (symfile != NULL)
    print_symfile_header_guard_start(symfile, translator.sympath);

  print_outfile_start_section("LOCATORS", outfile);
  print_outfile_start_section("LOCATORS", symfile);
  c = 0;

  int rand_guard = rand();
  fprintf(outfile, "\n#ifndef LOCATORS_%d\n#define LOCATORS_%d\n\n", rand_guard,
          rand_guard);
  if (symfile != NULL)
    fprintf(symfile, "\n#ifndef LOCATORS_%d\n#define LOCATORS_%d\n\n",
            rand_guard, rand_guard);

  while ((c = fgetc(translator.locators)) != EOF) {
    fputc(c, outfile);
    if (symfile != NULL)
      fputc(c, symfile);
  }

  fputs("\n#endif\n\n", outfile);
  if (symfile != NULL)
    fputs("\n#endif\n\n", symfile);

  print_outfile_end_section("LOCATORS", outfile);
  print_outfile_end_section("LOCATORS", symfile);

  print_outfile_start_section("PRELUDE", outfile);
  print_outfile_start_section("PRELUDE", symfile);
  c = 0;
  while ((c = fgetc(translator.prelude)) != EOF) {
    fputc(c, outfile);
    if (symfile != NULL)
      fputc(c, symfile);
  }
  print_outfile_end_section("PRELUDE", outfile);
  print_outfile_end_section("PRELUDE", symfile);

  print_outfile_start_section("TYPEDEFS", outfile);
  print_outfile_start_section("TYPEDEFS", symfile);
  c = 0;
  while ((c = fgetc(translator.tydefs)) != EOF) {
    fputc(c, outfile);
    if (symfile != NULL)
      fputc(c, symfile);
  }
  print_outfile_end_section("TYPEDEFS", outfile);
  print_outfile_end_section("TYPEDEFS", symfile);

  print_outfile_start_section("TYPESPECS", outfile);
  print_outfile_start_section("TYPESPECS", symfile);
  c = 0;
  while ((c = fgetc(translator.tyspecs)) != EOF) {
    fputc(c, outfile);
    if (symfile != NULL)
      fputc(c, symfile);
  }
  print_outfile_end_section("TYPESPECS", outfile);
  print_outfile_end_section("TYPESPECS", symfile);

  print_outfile_start_section("DECLARATIONS", outfile);
  print_outfile_start_section("DECLARATIONS", symfile);
  c = 0;
  while ((c = fgetc(translator.decls)) != EOF) {
    fputc(c, outfile);
    if (symfile != NULL)
      fputc(c, symfile);
  }
  print_outfile_end_section("DECLARATIONS", outfile);
  print_outfile_end_section("DECLARATIONS", symfile);

  if (symfile != NULL) {
    print_symfile_header_guard_end(symfile, translator.sympath);
    fclose(symfile);
  }

  print_outfile_start_section("DEFINITIONS", outfile);
  c = 0;
  while ((c = fgetc(translator.defs)) != EOF)
    fputc(c, outfile);
  print_outfile_end_section("DEFINITIONS", outfile);

  print_outfile_start_section("APPENDAGE", outfile);
  c = 0;
  while ((c = fgetc(translator.appendage)) != EOF)
    fputc(c, outfile);
  print_outfile_end_section("APPENDANGE", outfile);

  print_outfile_license(outfile);

  if (outfile != stdout)
    fclose(outfile);
}

void dump_translator(void) {
  fclose(translator.locators);
  fclose(translator.prelude);
  fclose(translator.tydefs);
  fclose(translator.tyspecs);
  fclose(translator.decls);
  fclose(translator.defs);
  fclose(translator.appendage);
  trans_dump();
}

static inline void print_indent(void) {
  for (int i = 0; i < indent_level; i++)
    PUTS_DEFS(INDENT);
}

static inline void print_tyspecs_indent(void) {
  for (int i = 0; i < indent_level; i++)
    PUTS_TYSPECS(INDENT);
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
  PRINTF_TYDEFS("typedef %s%s%s;\n", original, pointer ? " *" : " ", alias);
}

void install_funcdecl_init(const char *func_returns, const char *name) {
  if (fn_prefix != NULL) {
    PUTS_DECLS(fn_prefix);
  }
  PRINTF_DECLS("%s %s(", func_returns, name);
}

void install_funcdecl_param(const char *type, const char *name, bool last) {
  PRINTF_DECLS("%s %s%s", type, name, last ? ");\n" : ", ");
}

void install_pp_directive(const char *dir) { PRINTF_DECLS("#%s\n", dir); }

void install_macro(const char *name, const char *def) {
  PRINTF_DECLS("#define %s %s\n", name, def);
}

void install_locator_macro(const char *name, const char *def) {
  PRINTF_LOCATORS("#define %s %s\n", name, def);
}

void install_field(const char *type, const char *name) {
  print_tyspecs_indent();
  PRINTF_TYSPECS("%s %s;\n", type, name);
}

void install_sequence_field(char *type, char *name) {
  print_tyspecs_indent();
  PUTS_TYSPECS("struct {\n");
  INC_INDENT();
  print_tyspecs_indent();
  PRINTF_TYSPECS("%s %s;\n", type, name);
  print_tyspecs_indent();
  PRINTF_TYSPECS("ssize_t %s_count;\n", name);
  DEC_INDENT();
  print_tyspecs_indent();
  PRINTF_TYSPECS("} %s_seq;\n", name);
}

void install_optional_field(char *type, char *name) {
  print_tyspecs_indent();
  PUTS_TYSPECS("struct {\n");
  INC_INDENT();
  print_tyspecs_indent();
  PRINTF_TYSPECS("%s %s;\n", type, name);
  print_tyspecs_indent();
  PRINTF_TYSPECS("bool %s_exists;\n", name);
  DEC_INDENT();
  print_tyspecs_indent();
  PRINTF_TYSPECS("} %s_opt;\n", name);
}

void install_datatype_init(const char *kind, const char *name) {
  print_tyspecs_indent();
  PRINTF_TYSPECS("%s %s {\n", kind, name);
}

void install_datatype_field(const char *field, const char *end) {
  print_tyspecs_indent();
  PRINTF_TYSPECS("%s%s\n", field, end);
}

void install_datatype_named_end(const char *name) {
  print_tyspecs_indent();
  PRINTF_TYSPECS("} %s;\n", name);
}

void install_datatype_unnamed_end(void) {
  print_tyspecs_indent();
  PUTS_TYSPECS("};\n\n\n");
}

void install_funcdef_init(const char *func_returns, const char *name) {
  if (fn_prefix != NULL) {
    PUTS_DECLS(fn_prefix);
  }
  PRINTF_DEFS("%s %s(", func_returns, name);
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

const char *get_type_id(TypeId *typeid) {
  switch (typeid->kind) {
  case TYPENAME_BOOL:
    return BOOL;
  case TYPENAME_INT8:
    return INT8;
  case TYPENAME_UINT8:
    return UINT8;
  case TYPENAME_INT16:
    return INT16;
  case TYPENAME_UINT16:
    return UINT16;
  case TYPENAME_INT32:
    return INT32;
  case TYPENAME_UINT32:
    return UINT32;
  case TYPENAME_INT64:
    return INT64;
  case TYPENAME_UINT64:
    return UINT64;
  case TYPENAME_FLOAT32:
    return FLOAT32;
  case TYPENAME_FLOAT64:
    return FLOAT64;
  case TYPENAME_FLOAT80:
    return FLOAT80;
  case TYPENAME_CHAR:
    return CHAR;
  case TYPENAME_UCHAR:
    return UCHAR;
  case TYPENAME_SIZE:
    return SIZE;
  case TYPENAME_USIZE:
    return USIZE;
  case TYPENAME_STRING:
    return STRING;
  case TYPENAME_IDENTIFIER:
    return IDENTIFIER;
  case TYPENAME_BYTEARRAY:
    return BYTEARRAY;
  default:
    char *mentioned = NULL;
    STR_FORMAT(mentioned, "%s_MENTIONED", typeid->value);
    if (symtable_exists(typeid->value) == false && !symtable_exists(mentioned)) {
      fprintf(stderr, "Warning: undefined type `%s`\n", typeid->value);
      symtable_insert(mentioned, NULL);
    }
    return typeid->value;
  }
}

const char *get_arg_name(TypeId *typeid) {
  switch (typeid->kind) {
  case TYPENAME_BOOL:
    return BOOL_kEYWORD;
  case TYPENAME_INT8:
    return INT8_kEYWORD;
  case TYPENAME_UINT8:
    return UINT8_kEYWORD;
  case TYPENAME_INT16:
    return INT16_kEYWORD;
  case TYPENAME_UINT16:
    return UINT16_kEYWORD;
  case TYPENAME_INT32:
    return INT32_kEYWORD;
  case TYPENAME_UINT32:
    return UINT32_kEYWORD;
  case TYPENAME_INT64:
    return INT64_kEYWORD;
  case TYPENAME_UINT64:
    return UINT64_kEYWORD;
  case TYPENAME_FLOAT32:
    return FLOAT32_kEYWORD;
  case TYPENAME_FLOAT64:
    return FLOAT64_kEYWORD;
  case TYPENAME_FLOAT80:
    return FLOAT80_kEYWORD;
  case TYPENAME_CHAR:
    return CHAR_kEYWORD;
  case TYPENAME_UCHAR:
    return UCHAR_kEYWORD;
  case TYPENAME_SIZE:
    return SIZE_kEYWORD;
  case TYPENAME_USIZE:
    return USIZE_kEYWORD;
  case TYPENAME_STRING:
    return STRING_kEYWORD;
  case TYPENAME_IDENTIFIER:
    return IDENTIFIER_kEYWORD;
  case TYPENAME_BYTEARRAY:
    return BYTEARRAY_kEYWORD;
  default:
    return typeid->value;
  }
}

static inline int random_integer(void) { return rand(); }

void install_product_function(char *id, Product *product) {
  char *func_name = NULL;
  char *return_type = NULL;

  STR_FORMAT(func_name, "create_%s", id);
  STR_FORMAT(return_type, "%s_%s", id, def_suffix);

  install_funcdecl_init(return_type, func_name);
  install_funcdef_init(return_type, func_name);

  char *arg_type = NULL;
  char *arg_name = NULL;

  for (Field *f = product->fields; f != NULL; f = f->next) {
    install_funcdecl_param(f->cache[0], f->cache[1],
                           f->next == NULL && f->cache[2] == NULL);
    install_funcdef_arg(f->cache[0], f->cache[1],
                        f->next == NULL && f->cache[2] == NULL);

    if (f->cache[2] != NULL) {
      arg_type = NULL;
      arg_name = NULL;

      if (!strncmp(f->cache[2], "ssize_t", MAX_TYPENAME)) {
        arg_type = "ssize_t";
        STR_FORMAT(arg_name, "%s_count", f->cache[1]);
      } else {
        arg_type = "bool";
        STR_FORMAT(arg_name, "%s_exists", f->cache[1]);
      }

      install_funcdecl_param(arg_type, arg_name, f->next == NULL);
      install_funcdef_arg(arg_type, arg_name, f->next == NULL);
      f->cache[2] = arg_name;
    }
  }

  INC_INDENT();

  install_function_alloc(return_type);

  for (Field *f = product->fields; f != NULL; f = f->next) {
    print_indent();
    PRINTF_DEFS("p->%s = %s;\n", f->cache[1], f->cache[1]);

    if (f->cache[2] != NULL) {
      print_indent();
      PRINTF_DEFS("p->%s = %s;\n", f->cache[2], f->cache[2]);
    }
  }

  print_indent();
  PUTS_DEFS("p->next = NULL;\n\n");

  install_function_return();

  DEC_INDENT();

  NEWLINE_DEFS();
}

void install_product_seq_field_append_function(char *id, char *type,
                                               char *name) {
  char *func_name = NULL;
  char *arg1_type = NULL;
  char *arg1_name = NULL;
  char *arg2_type = NULL;
  char *arg2_name = NULL;
  char *locator = NULL;

  STR_FORMAT(func_name, "%s_%s_append", id, name);
  STR_FORMAT(arg1_type, "%s_%s", id, def_suffix);
  STR_FORMAT(arg1_name, "%s_instance", id);
  STR_FORMAT(arg2_type, "%s", type);
  STR_FORMAT(arg2_name, "%s_appendage", name);

  install_funcdecl_init("void", func_name);
  install_funcdecl_param(arg1_type, arg1_name, false);
  install_funcdecl_param(arg2_type, arg2_name, true);

  install_funcdef_init("void", func_name);
  install_funcdef_arg(arg1_type, arg1_name, false);
  install_funcdef_arg(arg2_type, arg2_name, true);

  STR_FORMAT(locator, "%s->%s", arg1_name, name);

  INC_INDENT();

  print_indent();
  PRINTF_DEFS("%s =\n%s(%s*)REALLOC(%s, (%s_count + 1) * sizeof(%s));\n",
              locator, INDENT, arg2_type, locator, locator, arg2_type);
  print_indent();
  PRINTF_DEFS("%s[%s_count++] = %s;\n", locator, locator, arg2_name);

  DEC_INDENT();

  PUTS_DEFS("}\n");

  NEWLINE_DEFS();
}

void install_product_seq_field_free_function(char *id, char *type, char *name) {
  char *func_name = NULL;
  char *arg_type = NULL;
  char *arg_name = NULL;
  char *locator = NULL;

  STR_FORMAT(func_name, "%s_%s_free", id, name);
  STR_FORMAT(arg_type, "%s_%s", id, def_suffix);
  STR_FORMAT(arg_name, "%s_instance", id);

  STR_FORMAT(locator, "%s->%s", arg_name, name);

  install_funcdecl_init("void", func_name);
  install_funcdecl_param(arg_type, arg_name, true);
  install_funcdef_init("void", func_name);
  install_funcdef_arg(arg_type, arg_name, true);

  INC_INDENT();

  print_indent();
  PRINTF_DEFS("while (--%s_count) {\n", locator);

  INC_INDENT();

  print_indent();
  PRINTF_DEFS("FREE(%s[%s_count]);\n", locator, locator);

  DEC_INDENT();

  print_indent();
  PUTS_DEFS("}\n");

  DEC_INDENT();

  PUTS_DEFS("}\n");

  NEWLINE_DEFS();
}

void install_product_type_chain_append(char *id) {
  char *func_name = NULL;
  char *arg1_type = NULL;
  char *arg1_name = NULL;
  char *arg2_type = NULL;
  char *arg2_name = NULL;
  char *locator = NULL;

  STR_FORMAT(func_name, "%s_chain_append", id);
  STR_FORMAT(arg1_type, "%s_%s*", id, def_suffix);
  STR_FORMAT(arg1_name, "%s_head", id);
  STR_FORMAT(arg2_type, "%s_%s", id, def_suffix);
  STR_FORMAT(arg2_name, "%s_appendage", id);

  STR_FORMAT(locator, "%s->next", arg2_name);

  install_funcdecl_init("void", func_name);
  install_funcdef_init("void", func_name);

  install_funcdecl_param(arg1_type, arg1_name, false);
  install_funcdecl_param(arg2_type, arg2_name, true);

  install_funcdef_arg(arg1_type, arg1_name, false);
  install_funcdef_arg(arg2_type, arg2_name, true);

  INC_INDENT();

  print_indent();
  PRINTF_DEFS("%s = *%s;\n", locator, arg1_name);

  print_indent();
  PRINTF_DEFS("*%s = %s;\n", arg1_name, arg2_name);

  DEC_INDENT();

  PUTS_DEFS("\n}\n");

  NEWLINE_DEFS();
}

void install_product_type_chain_dump(char *id) {
  char *func_name = NULL;
  char *arg_type = NULL;
  char *arg_name = NULL;

  STR_FORMAT(func_name, "%s_chain_dump", id);
  STR_FORMAT(arg_type, "%s_%s", id, def_suffix);
  STR_FORMAT(arg_name, "%s_chain", id);

  install_funcdecl_init("void", func_name);
  install_funcdef_init("void", func_name);

  install_funcdecl_param(arg_type, arg_name, true);
  install_funcdef_arg(arg_type, arg_name, true);

  INC_INDENT();

  print_indent();
  PRINTF_DEFS("if (%s == NULL) return;\n", arg_name);

  print_indent();
  PRINTF_DEFS("%s(%s->next);\n", func_name, arg_name);

  print_indent();
  PRINTF_DEFS("FREE(%s);\n", arg_name);

  DEC_INDENT();

  PUTS_DEFS("\n}\n");

  NEWLINE_DEFS();
}

void translate_product_type(char *id, Product *product) {
  char *type = NULL;
  char *def = NULL;

  STR_FORMAT(type, "struct %s", id);
  STR_FORMAT(def, "%s_%s", id, def_suffix);

  install_typedef(type, def, true);

  install_datatype_init("struct", id);

  char *field_type = NULL;
  char *field_name = NULL;
  char *field = NULL;

  INC_INDENT();

  size_t n = 0;
  for (Field *f = product->fields; f != NULL; f = f->next, n++) {
    field_name = NULL;
    field = NULL;

    if (f->type_id->kind == TYPENAME_ID) {
      STR_FORMAT(field_type, "%s_%s", get_type_id(f->type_id), def_suffix);
    } else {
      STR_FORMAT(field_type, "%s", get_type_id(f->type_id));
    }

    if (f->id == NULL) {
      STR_FORMAT(field_name, "%s%lu", get_arg_name(f->type_id), n);
    } else {
      STR_FORMAT(field_name, "%s", f->id);
    }

    f->cache[0] = field_type;
    f->cache[1] = field_name;

    if (f->kind == FIELD_SEQUENCE) {
      STR_FORMAT(field, "%s* %s;\n%sssize_t %s_count", field_type, field_name,
                 INDENT, field_name);
      STR_FORMAT(f->cache[0], "%s*", field_type);
      STR_FORMAT(f->cache[3], "%s", field_type);
      f->cache[2] = "ssize_t";
    } else if (f->kind == FIELD_OPTIONAL) {
      STR_FORMAT(field, "%s %s;\n%sbool %s_exists", field_type, field_name,
                 INDENT, field_name);
      f->cache[2] = "bool";
    } else {
      STR_FORMAT(field, "%s %s", field_type, field_name);
    }

    install_datatype_field(field, ";");
  }

  char *next_field = NULL;

  STR_FORMAT(next_field, "%s_%s next", id, def_suffix);
  install_datatype_field(next_field, ";");

  DEC_INDENT();

  install_datatype_unnamed_end();

  NEWLINE_DEFS();

  install_product_function(id, product);

  for (Field *f = product->fields; f != NULL; f = f->next) {
    if (f->kind == FIELD_SEQUENCE && f->cache[3] != NULL) {
      install_product_seq_field_append_function(id, f->cache[3], f->cache[1]);
      install_product_seq_field_free_function(id, f->cache[3], f->cache[1]);
    }
  }

  install_product_type_chain_append(id);
  install_product_type_chain_dump(id);
}

void install_seq_field_append(char *id, char *constructor_name,
                              char *field_type, char *field_name) {
  char *func_name = NULL;
  char *arg1_type = NULL;
  char *arg1_name = NULL;
  char *arg2_type = NULL;
  char *arg2_name = NULL;
  char *assign_name = NULL;

  STR_FORMAT(func_name, "append_%s_%s_%s_field", id,
             to_lowercase(constructor_name), field_name);

  install_funcdecl_init("void", func_name);
  install_funcdef_init("void", func_name);

  STR_FORMAT(arg1_type, "%s_%s", id, def_suffix);
  STR_FORMAT(arg1_name, "%s_value", id);

  install_funcdecl_param(arg1_type, arg1_name, false);
  install_funcdef_arg(arg1_type, arg1_name, false);

  STR_FORMAT(arg2_type, "%s", field_type);
  STR_FORMAT(arg2_name, "%s_append", field_name);

  install_funcdecl_param(arg2_type, arg2_name, true);
  install_funcdef_arg(arg2_type, arg2_name, true);

  STR_FORMAT(assign_name, "%s->value.%s.%s_seq.%s", arg1_name,
             to_lowercase(constructor_name), field_name, field_name);

  INC_INDENT();

  print_indent();
  PRINTF_DEFS("%s =\n (%s*)REALLOC(%s,\n (%s_count + 1) * sizeof(%s*));\n",
              assign_name, field_type, assign_name, assign_name, field_type);
  print_indent();
  PRINTF_DEFS("%s[%s_count++] = %s;\n", assign_name, assign_name, arg2_name);

  PUTS_DEFS("\n}\n\n");

  DEC_INDENT();
}

void install_seq_field_dump(char *id, char *constructor_name, char *field_type,
                            char *field_name) {
  char *func_name = NULL;
  char *arg1_type = NULL;
  char *arg1_name = NULL;
  char *field_link = NULL;

  STR_FORMAT(func_name, "dump_%s_%s_%s_field", id,
             to_lowercase(constructor_name), field_name);

  install_funcdecl_init("void", func_name);
  install_funcdef_init("void", func_name);

  STR_FORMAT(arg1_type, "%s_%s", id, def_suffix);
  STR_FORMAT(arg1_name, "%s_dump", id);

  install_funcdecl_param(arg1_type, arg1_name, true);
  install_funcdef_arg(arg1_type, arg1_name, true);

  STR_FORMAT(field_link, "%s->value.%s.%s_seq.%s", arg1_name,
             to_lowercase(constructor_name), field_name, field_name);

  INC_INDENT();
  print_indent();
  PRINTF_DEFS("while (--%s_count) {\n", field_link);
  INC_INDENT();
  print_indent();
  PRINTF_DEFS("FREE(%s[%s_count]);\n", field_link, field_link);
  DEC_INDENT();
  print_indent();
  PUTS_DEFS("}\n");

  PUTS_DEFS("}\n\n");

  DEC_INDENT();
}

void install_asdl_field(Field *field, size_t num) {
  char *type = NULL;
  char *name = NULL;
  char *count = NULL;
  char *exists = NULL;

  switch (field->kind) {
  case FIELD_NORMAL:
    type = NULL;
    name = NULL;
    if (field->type_id->kind == TYPENAME_ID) {
      STR_FORMAT(type, "%s_%s", get_type_id(field->type_id), def_suffix);
    } else {
      STR_FORMAT(type, "%s", get_type_id(field->type_id));
    }

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_arg_name(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
    }

    install_field(type, name);
    break;
  case FIELD_SEQUENCE:
    type = NULL;
    name = NULL;

    if (field->type_id->kind == TYPENAME_ID) {
      STR_FORMAT(type, "%s_%s*", get_type_id(field->type_id), def_suffix);
    } else {
      STR_FORMAT(type, "%s*", get_type_id(field->type_id));
    }

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_arg_name(field->type_id), num);
      STR_FORMAT(count, "%s_%lu_count", get_arg_name(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
      STR_FORMAT(count, "%s_count", field->id);
    }

    install_sequence_field(type, name);
    break;
  case FIELD_OPTIONAL:
    type = NULL;
    name = NULL;

    if (field->type_id->kind == TYPENAME_ID) {
      STR_FORMAT(type, "%s_%s", get_type_id(field->type_id), def_suffix);
    } else {
      STR_FORMAT(type, "%s", get_type_id(field->type_id));
    }

    if (field->id == NULL) {
      STR_FORMAT(name, "%s_%lu", get_arg_name(field->type_id), num);
      STR_FORMAT(exists, "%s_%lu_exists", get_arg_name(field->type_id), num);
    } else {
      STR_FORMAT(name, "%s", field->id);
      STR_FORMAT(exists, "%s_exists", field->id);
    }

    install_optional_field(type, name);
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

void install_kinds(char *id, Constructor *constructors, bool had_enums) {
  install_datatype_init("enum", " ");
  char *kind_name = NULL;

  INC_INDENT();

  if (had_enums) {
    STR_FORMAT(kind_name, "%s_VARIANT_%s", to_uppercase(id), kind_suffix);
    install_datatype_field(kind_name, ",");
  }

  for (Constructor *c = constructors; c != NULL; c = c->next) {
    kind_name = NULL;
    STR_FORMAT(kind_name, "%s_%s", to_uppercase(c->id), kind_suffix);
    install_datatype_field(kind_name, ",");
  }

  DEC_INDENT();

  install_datatype_named_end("kind");
}

void install_field_declarators(char *id, char *lc_ident, Field *fields) {
  char *arg_type = NULL;
  char *arg_name = NULL;
  char *swap = NULL;

  size_t n = 0;
  for (Field *f = fields; f != NULL; f = f->next, n++) {
    arg_type = NULL;
    arg_name = NULL;
    swap = NULL;

    if (f->type_id->kind == TYPENAME_ID) {
      STR_FORMAT(arg_type, "%s_%s", get_type_id(f->type_id), def_suffix);
    } else {
      STR_FORMAT(arg_type, "%s", get_type_id(f->type_id));
    }

    if (f->id == NULL) {
      STR_FORMAT(arg_name, "%s_%lu", get_arg_name(f->type_id), n);
    } else {
      STR_FORMAT(arg_name, "%s", f->id);
    }

    f->cache[2] = arg_type;

    if (f->kind == FIELD_SEQUENCE) {
      swap = arg_type;
      STR_FORMAT(arg_type, "%s*", swap);
    }

    f->cache[0] = arg_type;
    f->cache[1] = arg_name;

    install_funcdecl_param(arg_type, arg_name,
                           f->next == NULL && f->kind == FIELD_NORMAL);
    install_funcdef_arg(arg_type, arg_name,
                        f->next == NULL && f->kind == FIELD_NORMAL);

    if (f->kind == FIELD_OPTIONAL) {
      STR_FORMAT(arg_name, "%s_exists", f->cache[1]);
      install_funcdecl_param("bool", arg_name, f->next == NULL);
      install_funcdef_arg("bool", arg_name, f->next == NULL);
    } else if (f->kind == FIELD_SEQUENCE) {
      STR_FORMAT(arg_name, "%s_count", f->cache[1]);
      install_funcdecl_param("ssize_t", arg_name, f->next == NULL);
      install_funcdef_arg("ssize_t", arg_name, f->next == NULL);
    }
  }
}

void install_field_assigners(char *id, char *lc_ident, Field *fields) {
  char *assign_name = NULL;
  char *assign_val = NULL;
  char *macro_name = NULL;
  char *macro_def = NULL;

  for (Field *f = fields; f != NULL; f = f->next) {
    assign_name = NULL;
    assign_val = NULL;
    macro_name = NULL;
    macro_def = NULL;

    if (f->kind == FIELD_SEQUENCE) {
      STR_FORMAT(assign_name, "value.%s.%s_seq.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      STR_FORMAT(assign_val, "(%s)%s", f->cache[0], f->cache[1]);
      install_function_assign(assign_name, assign_val);
      STR_FORMAT(assign_name, "value.%s.%s_seq.%s_count", lc_ident, f->cache[1],
                 f->cache[1]);
      install_function_assign(assign_name, "0");

      STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_seq.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_locator_macro(macro_name, macro_def);

      STR_FORMAT(macro_name, "GET_%s_%s_count(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_seq.%s_count", lc_ident,
                 f->cache[1], f->cache[1]);
      install_locator_macro(macro_name, macro_def);

      continue;
    } else if (f->kind == FIELD_OPTIONAL) {
      STR_FORMAT(assign_name, "value.%s.%s_opt.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_function_assign(assign_name, f->cache[1]);
      STR_FORMAT(assign_name, "value.%s.%s_opt.%s_exists", lc_ident,
                 f->cache[1], f->cache[1]);
      STR_FORMAT(assign_val, "%s_exists", f->cache[1]);
      install_function_assign(assign_name, assign_val);

      STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_opt.%s", lc_ident, f->cache[1],
                 f->cache[1]);
      install_locator_macro(macro_name, macro_def);

      STR_FORMAT(macro_name, "GET_%s_%s_exists(v)", lc_ident, f->cache[1]);
      STR_FORMAT(macro_def, "v->value.%s.%s_opt.%s_exists", lc_ident,
                 f->cache[1], f->cache[1]);
      install_locator_macro(macro_name, macro_def);

      continue;
    }

    STR_FORMAT(assign_name, "value.%s.%s", lc_ident, f->cache[1]);
    install_function_assign(assign_name, f->cache[1]);

    STR_FORMAT(macro_name, "GET_%s_%s(v)", lc_ident, f->cache[1]);
    STR_FORMAT(macro_def, "v->value.%s.%s", lc_ident, f->cache[1]);
    install_locator_macro(macro_name, macro_def);
  }
}

void install_constructor_function(char *id, Constructor *constructor,
                                  Field *attributes) {

  char *lc_ident = to_lowercase(constructor->id);

  char *func_returns = NULL;
  char *func_name = NULL;
  STR_FORMAT(func_returns, "%s_%s", id, def_suffix);
  STR_FORMAT(func_name, "create_%s", lc_ident);

  install_funcdecl_init(func_returns, func_name);
  install_funcdef_init(func_returns, func_name);

  if (constructor->is_enum) {
    PUTS_DECLS("void);\n");
    PUTS_DEFS("void) {\n");
    INC_INDENT();
    install_function_alloc(func_returns);
    print_indent();
    PRINTF_DEFS("p->kind = %s_VARIANT_%s;\n", to_uppercase(id), kind_suffix);
    print_indent();
    PRINTF_DEFS("p->variant = %s_variant;\n", to_uppercase(constructor->id));
    install_function_return();
    DEC_INDENT();
    return;
  }

  install_field_declarators(id, lc_ident, constructor->fields);
  install_field_declarators(id, lc_ident, attributes);

  char *swap = NULL;

  STR_FORMAT(swap, "%s_%s", id, def_suffix);

  INC_INDENT();

  install_function_alloc(swap);
  NEWLINE_DEFS();
  STR_FORMAT(swap, "%s_%s", to_uppercase(constructor->id), kind_suffix);
  install_function_assign("kind", swap);

  print_indent();
  PUTS_DEFS("p->next = NULL;\n\n");

  install_field_assigners(id, lc_ident, constructor->fields);
  install_field_assigners(id, lc_ident, attributes);
  install_function_return();

  DEC_INDENT();
}

bool install_variant_constructors(char *id, Sum *sum) {
  bool had_enums = false;
  char *name = NULL;
  char *type = NULL;

  for (Constructor *c = sum->constructors; c != NULL; c = c->next) {
    if (!had_enums && c->is_enum) {
      had_enums = true;
      STR_FORMAT(name, "enum %s_VARIANTS", to_uppercase(id));
      STR_FORMAT(type, "%s_variants", id);
      install_typedef(name, type, false);
      symtable_insert(type, NULL);
      STR_FORMAT(name, "%s_VARIANTS", to_uppercase(id));
      install_datatype_init("enum", name);
      INC_INDENT();
    }

    if (c->is_enum) {
      STR_FORMAT(name, "%s_variant", to_uppercase(c->id));
      install_datatype_field(name, ",");
    }
  }

  if (had_enums) {
    DEC_INDENT();
    install_datatype_named_end("variant");

    char *macro_name = NULL;

    STR_FORMAT(macro_name, "GET_%s_variant(v)", id);
    install_locator_macro(macro_name, "v->variant");

    STR_FORMAT(macro_name, "VARIANT_%s_is(v, is)", id);
    install_locator_macro(macro_name, "(v->variant == is)");
  }

  return had_enums;
}

void install_append_function(const char *id, const char *def_name) {
  char *fn_name = NULL;
  char *arg_type_head = NULL;
  char *arg_name_head = NULL;
  char *arg_name_append = NULL;

  STR_FORMAT(fn_name, "append_%s_chain", id);
  STR_FORMAT(arg_type_head, "%s*", def_name);
  STR_FORMAT(arg_name_head, "%s_head", def_name);
  STR_FORMAT(arg_name_append, "%s_append", id);

  install_funcdecl_init("void", fn_name);
  install_funcdef_init("void", fn_name);

  install_funcdecl_param(arg_type_head, arg_name_head, false);
  install_funcdef_arg(arg_type_head, arg_name_head, false);

  install_funcdecl_param(def_name, arg_name_append, true);
  install_funcdef_arg(def_name, arg_name_append, true);

  INC_INDENT();

  print_indent();
  PRINTF_DEFS("%s->next = *%s;\n", arg_name_append, arg_name_head);

  print_indent();
  PRINTF_DEFS("*%s = %s;\n", arg_name_head, arg_name_append);

  DEC_INDENT();

  PUTS_DEFS("\n}\n\n");
}

void install_destroy_function(const char *id, const char *def_name) {
  char *fn_name = NULL;
  char *arg_type = NULL;
  char *arg_name = NULL;

  STR_FORMAT(fn_name, "destroy_%s_chain", id);
  STR_FORMAT(arg_type, "%s*", def_name);
  STR_FORMAT(arg_name, "%s_head", id);

  install_funcdecl_init("void", fn_name);
  install_funcdef_init("void", fn_name);

  install_funcdecl_param(arg_type, arg_name, true);
  install_funcdef_arg(arg_type, arg_name, true);

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

void install_special_field_functions(char *id, Constructor *constructor,
                                     Field *attributes) {
  for (Field *f = constructor->fields; f != NULL; f = f->next) {
    if (f->kind == FIELD_SEQUENCE) {
      install_seq_field_append(id, constructor->id, f->cache[2], f->cache[1]);
      install_seq_field_dump(id, constructor->id, f->cache[2], f->cache[1]);
    }
  }

  for (Field *f = attributes; f != NULL; f = f->next) {
    if (f->kind == FIELD_SEQUENCE) {
      install_seq_field_append(id, constructor->id, f->cache[2], f->cache[1]);
      install_seq_field_dump(id, constructor->id, f->cache[2], f->cache[1]);
    }
  }
}

void translate_sum_type(char *id, Sum *sum) {
  char *struct_name = NULL;
  char *def_name = NULL;
  STR_FORMAT(struct_name, "struct %s", id);
  STR_FORMAT(def_name, "%s_%s", id, def_suffix);
  install_typedef(struct_name, def_name, true);

  install_datatype_init("struct", id);

  INC_INDENT();

  bool had_enums = install_variant_constructors(id, sum);
  install_kinds(id, sum->constructors, had_enums);

  if (sum->constructors == NULL) {
    fprintf(stderr, "Error: empty tree\n");
    exit(EXIT_FAILURE);
  }

  install_datatype_init("union", "");

  INC_INDENT();

  for (Constructor *c = sum->constructors; c != NULL; c = c->next) {
    if (c->is_enum)
      continue;

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

  for (Constructor *c = sum->constructors; c != NULL; c = c->next) {
    install_special_field_functions(id, c, sum->attributes);
  }

  PUTS_DEFS("\n\n");

  char *macro_name = NULL;

  STR_FORMAT(macro_name, "GET_%s_kind(v)", id);
  install_locator_macro(macro_name, "v->kind");

  STR_FORMAT(macro_name, "KIND_%s_is(v, is)", id);
  install_locator_macro(macro_name, "(v->kind == is)");

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
  install_typedef("uint8_t", "bytearray_t", true);
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
