#ifndef ASDL_TRN_H
#define ASDL_TRN_H

#include <string.h>

typedef struct Rule Rule;
typedef struct Type Type;
typedef struct Sum Sum;
typedef struct Product Product;
typedef struct Constructor Constructor;
typedef struct Field Field;


struct Rule {
    char *id;
    Type *type;
};


struct Type {
   enum TypeKind {
	TYPE_SUM,
	TYPE_PRODUCT,
   } kind;
   union {
	Product *product;
	Sum *sum;
   };
};

struct Sum {
    Constructor **cons;
    size_t num_cons;
    Field **attrs;
    size_t num_attrs;
};

struct Product {
   Field **fields;
   size_t num_fields;
};

struct Constructor {
    char *id;
    Field **fields;
    size_t num_fields;
};

struct Field {
   enum FieldKind {
	SEQUENCE, OPTIONAL, NORMAL,
   } kind;
   char *type_id;
   char *id;
};





#endif
