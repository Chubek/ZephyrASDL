#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "asdl.h"

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

  fputc('\n', outfile);
  
  while ((c = fgetc(translator.decls) != EOF)
		  fputc(c, outfile);

  fputc('\n', outfile);

  while ((c = fgetc(translator.defs) != EOF)
		  fputc(c, outfile);

  fputc('\n', outfile);
  
  while ((c = fgetc(translator.appendage) != EOF)
		  fputc(c, outfile);

  fputc(c, outfile);
}

void dump_translator(void) {
  fclose(translator.prelude);
  fclose(translator.decls);
  fclose(translator.defs);
  fclose(translator.appendage);
}
