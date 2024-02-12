#ifndef ABSYN_H
#define ABSYN_H


typedef enum AbsynType AbsynType;
typedef struct AbsynNode AbsynNode;
typedef struct AbsynNodeList AbsynNodeList;

enum AbsynType {
  IDENT,
  CONSTRUCTOR,
  ATTRIBUTE,
  TUPLE,
  MARKER,
};

struct AbsynNode {
   AbsynType type;
   const char *value;
   bool is_leaf;
   AbsynNode *next;
};

