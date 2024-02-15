#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "types.h"

extern FILE *yyin;
extern int yyparse(void);
extern void dump_heaps(void);

int main(int argc, char **argv) {
   int c = 0;
   yyin = stdin;

   while ((c = getopt(argc, argv, "i:o:")) != -1) {
  	switch (c) {
		case 'i':
			yyin = fopen(optarg, "r");
			break;
		default:
			break;
	}
   }

   yyparse();

   if (yyin != stdin)
	   fclose(yyin);

   dump_heaps();

}

