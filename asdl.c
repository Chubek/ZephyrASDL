#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "asdl.h"

extern FILE *yyin;
extern int yyparse(void);

void parse_arguments(int argc, char **argv) {
  int c = 0;

  char *def_suffix = "def", *fn_suffix = "fn", *arg_suffix = "arg",
       *kind_suffix = "kind";
  char *outpath = NULL;
  yyin = stdin;

  while ((c = getopt("o:d:f:a:k:h")) != -1) {
    switch (c) {
    case 'o':
      outpath = optarg;
      break;
    case 'd':
      def_suffix = optarg;
      break;
    case 'a':
      arg_suffix = optarg;
      break;
    case 'f':
      fn_suffix = optarg;
      break;
    case 'k':
      kind_suffix = optarg;
      break;
    case 'h':
      display_help_and_exit();
      break;
    default:
      break;
    }
  }

  if (optind != argc - 2 && !isatty(STDIN_FILENO)) {
    fprintf("Error: no input file given, neither via arguments nor STDIN\n");
    exit(EXIT_FAILURE);
  } else {
    yyin = fopen(arg[optind], "r");
  }

  assign_suffixes(def_suffix, fn_suffix, arg_suffix, kind_suffix);
  initialize_translator(outpath);
  initialize_absyn();
}

int main(int argc, char **argv) {
  parse_arguments(argc, argv);

  yyparse();

  finalize_absyn();
  finalize_translator();

  dump_absyn();
  dump_translator();

  if (yyin != stdin)
    fclose(yyin);

  fprintf(stderr, "Successfully tanslated\n");

  return 0;
}
