#!/usr/bin/awk -f

BEGIN {
    type_name = "";
    name = "";
    cap = 0;
}

/^VectorType: / { type_name = $2; }
/^VectorName: / { name = $2; lcname = tolower($2); }
/^Capacity: /   { cap = $2; }

/^%%$/ {
    printf "#include <stdio.h>\n";
    printf "#include <stdlib.h>\n\n";

    while (getline) {
       if(match($0, /#define .+/))
	      print $0;
    }

    print "\n\n"

    printf "typedef %s* (*%s_insert_t)(%s*, %s);\n", type_name, name, name, type_name;
    printf "typedef void (*%s_delete_t)(%s*, size_t);\n", name, name;
    printf "typedef %s* (*%s_retrieve_t)(%s*, size_t);\n\n", type_name, name, name;


    printf "typedef struct %s {\n", name;
    printf "    %s **items;\n", type_name;
    printf "    size_t num_items;\n";
    printf "    ssize_t cap;\n";
    printf "    %s_insert_t insert;\n", name;
    printf "    %s_delete_t delete;\n", name;
    printf "    %s_retrieve_t retrieve;\n", name;
    printf "} %s;\n\n", name;

    printf "%s* %s_insert(%s* vector, %s item) {\n", type_name, name, name, type_name;
    printf "    if (vector->num_items == vector->cap) {\n";
    printf "        if (vector->cap == 0)\n";
    printf "            vector->cap = 1;\n";
    printf "        else\n";
    printf "            vector->cap *= 2;\n";
    printf "        vector->items = REALLOC(vector->items, vector->cap * sizeof(%s*));\n", type_name;
    printf "    }\n";
    printf "    vector->items[vector->num_items++] = ALLOCATE(sizeof(%s));\n", type_name;
    printf "    *(vector->items[vector->num_items - 1]) = item;\n";
    printf "    return *(vector->items[vector->num_items - 1]);\n";
    printf "}\n\n";

    printf "void %s_delete(%s* vector, size_t index) {\n", name, name;
    printf "    if (index >= vector->num_items) {\n";
    printf "        printf(\"Index out of bounds\\n\");\n";
    printf "        return;\n";
    printf "    }\n";
    printf "    DEALLOCATE(vector->items[index]);\n";
    printf "    for (size_t i = index; i < vector->num_items - 1; ++i) {\n";
    printf "        vector->items[i] = vector->items[i + 1];\n";
    printf "    }\n";
    printf "    vector->num_items--;\n";
    printf "}\n\n";

    printf "%s* %s_retrieve(%s* vector, size_t index) {\n", type_name, name, name;
    printf "    if (index >= vector->num_items) {\n";
    printf "        printf(\"Index out of bounds\\n\");\n";
    printf "        return NULL;\n";
    printf "    }\n";
    printf "    return *(vector->items[index]);\n";
    printf "}\n\n";

    printf "void %s_deallocate(%s* vector) {\n", name, name;
    printf "    for (size_t i = 0; i < vector->num_items; ++i) {\n";
    printf "        DEALLOCATE(vector->items[i]);\n";
    printf "    }\n";
    printf "    DEALLOCATE(vector->items);\n";
    printf "}\n\n";

    printf "%s* %s_create(void) {\n", name, lcname;
    printf "    %s* %s = ALLOCATE(sizeof(%s));\n", name, lcname, name;
    printf "    %s->items = NULL;\n", lcname;
    printf "    %s->num_items = 0;\n", lcname;
    printf "    %s->insert = %s_insert;\n", lcname, name;
    printf "    %s->delete = %s_delete;\n", lcname, name;
    printf "    %s->retrieve = %s_retrieve;\n", lcname, name;
    printf "    return %s;\n", lcname;
    printf "}\n\n";

}

