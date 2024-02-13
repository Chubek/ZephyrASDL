#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

extern void parse_and_translate(void);

FILE *yyin = NULL, *yyout = NULL;

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

   if (yyin == stdin && isatty(STDIN_FILENO)) {
	fprintf(stderr, "No input file was given, neither via STDIN nor commandline arguments\n");
	return 1;
   }

   parse_and_translate();

   if (yyin != stdin)
	   fclose(yyin);
   if (yyout != stdout)
	   fclose(yyout);

   return 0;
}

