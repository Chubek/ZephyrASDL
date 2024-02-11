#ifndef AST_H
#define AST_H

typedef struct Field Field;
typedef struct Constructor Constructor;
typedef struct Attributes Attributes;
typedef struct Type Type;
typedef struct Asdl Asdl;


Field* createField(Type *type, const char *name, int kind);
Constructor* createConstructor(const char *name, int kind);
void insertField(Constructor *cons, Field *field);
void insertAttribute(Attributes *attrs, Field *field);
Type* createType(const char *name);
void insertConstructor(Type *type, Constructor *cons);
Attributes* createAttributes();
Asdl* createAsdl();
void insertType(Asdl *asdl, Type *type);
void freeField(Field *field);
void freeConstructor(Constructor *cons);
void freeType(Type *type);
void freeAttributes(Attributes *attrs);
void freeAsdl(Asdl *asdl);










#endif
