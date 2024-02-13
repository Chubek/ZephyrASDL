#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

extern void pare_and_translate(const char *infile const char *outfile);

int main(int argc, char **argv) {
   const char *infile = NULL;
   const char *outfile = NULL;

   int c = 0;
   while ((c = getopt(argc, argv, "i:o:")) != -1) {
	switch (c) {
		case 'i':
			infile = optarg;
			break;
		case 'o':
			outfile = optarg;
			break;
		default:
			break;
	}
   }

   if (infile == NULL && isatty(STDIN_FILENO)) {
	fprintf(stderr, "No file givne, neither via STDIN or arguments\n");
	return 1;
   }

   parse_and_translate(infile, outfile);

   return 0;
}

