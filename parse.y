%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Field {
    char* type_id;
    char* id;
    char modifier;
} Field;

typedef struct Constructor {
    char* con_id;
    Field** fields;
    int num_fields;
} Constructor;

typedef struct SumType {
    Constructor** cons;
    int num_cons;
    Field** attrs;
    int num_attrs;
} SumType;

typedef struct ProductType {
    Field** fields;
    int num_fields;
} ProductType;

typedef struct Rule {
    char* type_id;
    void* type;
} Rule;

extern FILE* yyin;
extern int yylex();
void yyerror(const char* s);

Rule** rules;
int num_rules = 0;

Field** fields;
int num_fields = 0;

Constructor** cons;
int num_cons = 0;

SumType* sumtype;
ProductType* prodtype;

void add_field(char* type_id, char modifier, char* id) {
    fields = realloc(fields, (num_fields + 1) * sizeof(Field*));
    fields[num_fields] = malloc(sizeof(Field));
    fields[num_fields]->type_id = strdup(type_id);
    fields[num_fields]->modifier = modifier;
    fields[num_fields]->id = strdup(id);
    num_fields++;
}

void add_constructor(char* con_id, Field** fields, int num_fields) {
    cons = realloc(cons, (num_cons + 1) * sizeof(Constructor*));
    cons[num_cons] = malloc(sizeof(Constructor));
    cons[num_cons]->con_id = strdup(con_id);
    cons[num_cons]->fields = fields;
    cons[num_cons]->num_fields = num_fields;
    num_cons++;
}

void add_sum_type(Constructor** constructors, int num_constructors, Field** attributes, int num_attributes) {
    sumtype = malloc(sizeof(SumType));
    sumtype->cons = constructors;
    sumtype->num_cons = num_constructors;
    sumtype->attrs = attributes;
    sumtype->num_attrs = num_attributes;
}

void add_product_type(Field** fields, int num_fields) {
    prodtype = malloc(sizeof(ProductType));
    prodtype->fields = fields;
    prodtype->num_fields = num_fields;
}

%}

%union {
    char* str;
}

%token <str> TYPE_ID
%token <str> CON_ID
%token ATTRIBUTES LPAREN RPAREN COMMA EQUALS COLON SEMICOLON PIPE QUESTION STAR

%type <str> ident_opt
%type <str> mod_type
%type <str> type_id
%type <str> con_id
%type <str> lhs_type_name
%type <str> constructor
%type <str> field_pair
%type <str> fields
%type <str> field

%%

rules : rule SEMICOLON rules { }
      | /* empty */ { }
      ;

rule : lhs_type_name EQUALS type { printf("Rule: %s\n", $1); }
     ;

lhs_type_name : TYPE_ID COLON { $$ = $1; }
              | TYPE_ID EQUALS { $$ = $1; }
              ;

type : sum_type { }
     | product_type { }
     ;

sum_type : constructor PIPE constructors attr_opt { add_sum_type($2, $3, $4, $5); }
         ;

constructors : constructor PIPE constructors { }
            | constructor { }
            ;

constructor : con_id field_pair_opt { add_constructor($1, $2, $3); }
            ;

attr_opt : ATTRIBUTES fields { }
         | /* empty */ { }
         ;

product_type : fields { add_product_type($1, $2); }
             ;

fields : LPAREN field_pairs RPAREN { }
       ;

field_pairs : field_pair COMMA field_pairs { }
            | field_pair { }
            ;

field_pair : field { }
           ;

field : mod_type type_id ident_opt { add_field($2, $1[0], $3); }
      ;

mod_type : TYPE_ID STAR { $$ = $1; }
         | TYPE_ID QUESTION { $$ = $1; }
         | /* empty */ { }
         ;

ident_opt : TYPE_ID { $$ = $1; }
          | /* empty */ { }
          ;

type_id : TYPE_ID { $$ = $1; }
        ;

con_id : CON_ID { $$ = $1; }
       ;

%%


void yyerror(const char* s) {
    printf("Error: %s\n", s);
}
