#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "types.h"

extern FILE *yyin;
extern FILE *yyout;
extern size_t seq_size;
extern int yyparse(void);
extern void initialize(void);
extern void merge_temp_files(void);
extern void dump_heaps(void);

int main(int argc, char **argv) {
  int c = 0;
  yyin = stdin;
  yyout = stdout;

  while ((c = getopt(argc, argv, "i:o:s:")) != -1) {
    switch (c) {
    case 'i':
      yyin = fopen(optarg, "r");
      break;
    case 'o':
      yyout = fopen(optarg, "w");
      break;
    case 's':
      seq_size = strtoul(optarg, NULL, 10);
      break;
    default:
      break;
    }
  }

  initialize();

  yyparse();

  merge_temp_files();

  if (yyin != stdin)
    fclose(yyin);
  if (yyout != stdout)
    ;
  fclose(yyout);

  dump_heaps();
}
