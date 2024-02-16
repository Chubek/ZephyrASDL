#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "types.h"

extern fpos_t top;
extern FILE *yyin;
extern FILE *yyout;
extern int yyparse(void);
extern void dump_heaps(void);

extern const char *macro_normal_field;
extern const char *macro_sequence_field;
extern const char *macro_optional_field;

int main(int argc, char **argv) {
  int c = 0;
  yyin = stdin;
  yyout = stdout;

  while ((c = getopt(argc, argv, "i:o:")) != -1) {
    switch (c) {
    case 'i':
      yyin = fopen(optarg, "r");
      break;
    case 'o':
      yyout = fopen(optarg, "w");
      break;
    default:
      break;
    }
  }

  fputs(macro_normal_field, yyout);
  fputs(macro_sequence_field, yyout);
  fputs(macro_optional_field, yyout);
  fputs("\n", yyout);
  fgetpos(yyout, &top);

  yyparse();

  if (yyin != stdin)
    fclose(yyin);

  dump_heaps();
}
