#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "asdl.h"

extern FILE *yyin;
extern int yyparse(void);

static inline void print_help_and_exit() {
  printf("Usage: asdl [-o output] [-d def_suffix] [-f fn_suffix] [-k "
         "kind_suffix] [-a arg_suffix] FILE\n");
  printf("\n");
  printf("Options:\n");
  printf("  -o output      Specify the output file\n");
  printf("  -d def_suffix  Specify the suffix for typedefs\n");
  printf("  -f fn_suffix   Specify the suffix for functions\n");
  printf("  -k kind_suffix Specify the suffix for kind types\n");
  printf("  -a arg_suffix  Specify the suffix for arguments\n");
  printf("\n");
  exit(EX_USAGE);
}

void parse_arguments(int argc, char **argv) {
  int c = 0;

  char *def_suffix = "def", *fn_suffix = "fn", *arg_suffix = "arg",
       *kind_suffix = "kind";
  char *outpath = NULL;
  yyin = stdin;

  while ((c = getopt(argc, argv, "o:d:f:a:k:h")) != -1) {
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
      print_help_and_exit();
      break;
    default:
      fprintf(stderr, "Error: wrong argument, pass -h to view help\n");
      exit(EXIT_FAILURE);
    }
  }

  if (optind >= argc && isatty(STDIN_FILENO)) {
    fprintf(stderr,
            "Error: no input file given, neither via arguments nor STDIN\n");
    exit(EXIT_FAILURE);
  } else if (optind < argc && isatty(STDIN_FILENO)) {
    yyin = fopen(argv[optind], "r");
  } else if (optind < argc && !isatty(STDIN_FILENO)) {
    yyin = stdin;
  }

  assign_suffixes(def_suffix, fn_suffix, arg_suffix, kind_suffix);
  init_translator(outpath);
  init_absyn();
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
