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

#define ERROR_LOOSE_TYPEID "Loose type name"
#define ERROR_LOOSE_CONSID "Loose constructor name"
#define ERROR_LOOSE_MODIFIER "Loose modifier symbol"
#define ERROR_LOOSE_SEPARATOR "Loose separator symbol" 
#define ERROR_LOOSE_PARENTHESIS "Loose parenthesis"
#define ERROR_LOOSE_EQUALSIGN "Loose equal sign"

#define ERROR_PASCALCASE_FIELDTYPE "PascalCase identifiers cannot be used as field types"
#define ERROR_PASCALCASE_FIELDNAME "PascalCase identifiers cannot be used as field names"
#define ERROR_PASCALCASE_RULENAME "PascalCase identifiers cannot be used as rule names"
#define ERROR_LOWERCASE_CONSNAME "lower_case identifiers cannot be used as constructor names"

#define ERROR_DOUBLE_MODIFIER "Sequence (*) and Optional (*) modifiers cannot be used in tandem, please use product types as redirection"
#define ERROR_DOUBLE_PRODUCT "Product types are only allowed one record"
#define ERROR_RECORD_SUM "Records are not allowed in sum types"

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
