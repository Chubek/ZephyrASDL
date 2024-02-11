#include <getopt.h>

#include "ast.h"


ASTNode *yytree = NULL;
extern int yyparse(void);
extern FILE* yyin;

int main(int argc, char **argv) {
   int c = 0;
   FILE *input = NULL;
   FILE *output = NULL;

   while ((c = getopt(argv, "i:o:")) != -1) {
	switch (c) {
		case 'i':
			input = fopen(optarg, "r");
			break;
		case 'o':
			output = fopen(optarg, "w");
			break;
		default:
			break;
	}
   }

   if (input == NULL && !isatty(STDIN_FILENO))
	   input = yyin = stdin;
   else
	   input = yyin = fopen("examples/simple.asdl", "r");

   if (output == NULL)
	   output = stdout;

   yyparse();

   print_ast(output, yytree);

}
