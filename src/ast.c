#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

struct Asdl {
       Type **types;
       size_t num_types;
};

struct Type {
    const char *name;
    Constructor **cons;
    size_t num_cons;
    Attributes *attrs;
};

struct Field {
	const char *type_id;
	const char *name;
	int kind;
};


struct Constructor {
        const char *con_id;
	Fields **fields;
	size_t num_fields;
	int kind;
};

struct Attributes {
	Field **fields;
	size_t num_fields;
};




Field* createField(Type *type, const char *name, int kind) {
    Field* field = (Field*)malloc(sizeof(Field));
    if (field) {
        field->type = type;
        field->name = strdup(name);
        field->kind = kind;
    }
    return field;
}


Constructor* createConstructor(const char *name, int kind) {
    Constructor* cons = (Constructor*)malloc(sizeof(Constructor));
    if (cons) {
        cons->name = strdup(name);
        cons->num_fields = 0;
        cons->fields = NULL;
        cons->kind = kind;
    }
    return cons;
}


void insertField(Constructor *cons, Field *field) {
    cons->num_fields++;
    cons->fields = realloc(cons->fields, cons->num_fields * sizeof(Field*));
    if (cons->fields) {
        cons->fields[cons->num_fields - 1] = field;
    }
}

void insertAttribute(Attributes *attrs, Field *field) {
    attrs->num_fields++;
    attrs->fields = realloc(attrs->fields, attrs->num_fields * sizeof(Field*));
    if (attrs->fields) {
        attrs->fields[attrs->num_fields - 1] = field;
    }
}


Type* createType(const char *name) {
    Type* type = (Type*)malloc(sizeof(Type));
    if (type) {
        type->name = strdup(name);
        type->num_cons = 0;
        type->cons = NULL;
        type->attrs.num_fields = 0;
        type->attrs.fields = NULL;
    }
    return type;
}


void insertConstructor(Type *type, Constructor *cons) {
    type->num_cons++;
    type->cons = realloc(type->cons, type->num_cons * sizeof(Constructor*));
    if (type->cons) {
        type->cons[type->num_cons - 1] = cons;
    }
}

Attributes* createAttributes() {
    Attributes* attrs = (Attributes*)malloc(sizeof(Attributes));
    if (attrs) {
        attrs->num_fields = 0;
        attrs->fields = NULL;
    }
    return attrs;
}



Asdl* createAsdl() {
    Asdl* asdl = (Asdl*)malloc(sizeof(Asdl));
    if (asdl) {
        asdl->num_types = 0;
        asdl->types = NULL;
    }
    return asdl;
}


void insertType(Asdl *asdl, Type *type) {
    asdl->num_types++;
    asdl->types = realloc(asdl->types, asdl->num_types * sizeof(Type*));
    if (asdl->types) {
        asdl->types[asdl->num_types - 1] = type;
    }
}


void freeField(Field *field) {
    if (field) {
        free((void*)field->name);
        free(field);
    }
}


void freeConstructor(Constructor *cons) {
    if (cons) {
        for (size_t i = 0; i < cons->num_fields; i++) {
            freeField(cons->fields[i]);
        }
        free(cons->fields);
        free((void*)cons->name);
        free(cons);
    }
}


void freeType(Type *type) {
    if (type) {
        for (size_t i = 0; i < type->num_cons; i++) {
            freeConstructor(type->cons[i]);
        }
        free(type->cons);
        free((void*)type->name);
        free(type);
    }
}

void freeAttributes(Attributes *attrs) {
    if (attrs) {
        for (size_t i = 0; i < attrs->num_fields; i++) {
            freeField(attrs->fields[i]);
        }
        free(attrs->fields);
        free(attrs);
    }
}

void freeAsdl(Asdl *asdl) {
    if (asdl) {
        for (size_t i = 0; i < asdl->num_types; i++) {
            freeType(asdl->types[i]);
        }
        free(asdl->types);
        free(asdl);
    }
}

