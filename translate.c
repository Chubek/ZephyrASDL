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

#define EMIT_PRELUDE(fmt, ...) fprintf(translator.prelude, fmt, __VA_ARGS__)
#define EMIT_DECLS(fmt, ...) fprintf(translator.decls, fmt, __VA_ARGS__)
#define EMIT_DEFS(fmt, ...) fprintf(translator.defs, fmt, __VA_ARGS__)
#define EMIT_APPENDAGE(fmt, ...) fprintf(translator.appendage, fmt, __VA_ARGS__)

#ifndef INDENT
#define INDENT "    "
#endif

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

static inline void install_typedef(const char *original, const char *alias) {
  EMIT_DECLS("typedef %s %s;\n", original, alias);
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

static inline void install_datatype_init(const char *kind, const char *name) {
  EMIT_DEFS("%s %s {\n");
}

static inline void install_datatype_typed_field(const char *type,
                                                const char *name) {
  EMIT_DEFS("%s %s;\n", type, name);
}

static inline void install_datatype_untyped_field(const char *name) {
  EMIT_DEFS("%s,\n", name);
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

static inline print_indent(void) {
  for (int i = 0; i < indent_level; i++)
    fputs(translator.defs, INDENT);
}
