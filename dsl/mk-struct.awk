# Check if the line starts with "Struct:" or "Union:"
BEGIN {
    print "#include <stdio.h>\n#include <stdlib.h>\n#include <stdint.h>\n#include <stdbool.h>\n#include <stddef.h>\n#include <string.h>\n\n"
    print "#include \"hattix.h\"\n\n"
}

/^Struct:/ {
    structName = $2
    printf "typedef struct %s %s;\n", structName, structName
    printf "struct %s {\n", structName
    i = 0;
    next
}
/^Union:/ {
    unionName = $2
    printf "typedef union %s %s;\n", unionName, unionName
    printf "union %s {\n", unionName
    i = 0;
    next
}

# Skip lines starting with "-"
/^-/ { next }

# For field lines, print them out as C struct members
{
    deallocs[i] = $3 ~ /y/ ? 1 : 0;
    fields[i] = $2;
    types[i++] = $1;
    printf "    %s %s;\n", $1, $2
}

# Close the struct or union
END {
    printf "};\n\n"
    typeName = length(structName) > 0 ? structName : unionName;
    print_constructor(typeName);
    print_destructor(typeName);
}


function print_constructor(name) {
    printf "%s* create_%s(void) {\n", name, tolower(name)
    printf "\t%s* %s = (%s*)calloc(1, sizeof(%s));\n", name, tolower(name), name, name
    for (k = 0; k < i; k++) {
        if (match(types[k], /.+\*/))
                printf "\t%s->%s = NULL;\n", tolower(name), fields[k]
    }
    printf "\treturn %s;\n}\n\n", tolower(name);
}

function print_destructor(name) {
   printf "void destroy_%s(%s* %s) {\n", tolower(name), name, tolower(name)
   printf "\tif (%s == NULL) return;\n", tolower(name)
   for (k = 0; k < i; k++) {
        if (deallocs[k]) 
                printf "\tif (%s->%s != NULL)\n\t\tfree(%s->%s);\n", tolower(name), fields[k], tolower(name), fields[k]
    }
    printf "\tfree(%s);\n}\n", tolower(name)
}

