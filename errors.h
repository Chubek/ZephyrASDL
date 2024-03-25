#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int errcode_t;

size_t line_no = 1;
size_t token_no = 1;
size_t char_no = 0;

#define ERROR_UNKNOWN_SYMBOL "Unknown symbol"
#define ERROR_UNKNOWN_TOKEN "Unknown token"

static inline void error_out(const char *reason, char *context) {
  fprintf(stderr, "Error occurred at line %lu, token %lu, symbol %lu;\n",
          line_no, token_no, char_no);
  fprintf(stderr, "Reason: %s;\n", reason);

  if (context == NULL)
    fputs("Context: Not given;\n", stderr);
  else
    fprintf(stderr, "Context: %s;\n", context);

  fprintf(stderr, "This error was fatal, asdl(1) is exiting with status %d\n",
          EXIT_FAILURE);
  exit(EXIT_FAILURE);
}
