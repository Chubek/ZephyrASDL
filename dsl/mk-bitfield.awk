#!/usr/bin/awk -f

BEGIN {
    print "#include <stdint.h>\n\n"
    base_type = "";
    name = "";
}

/^BaseType: / { base_type = $2 }

/^Name: / { name = $2 }

/^-$/ {
    printf "typedef struct %s %s;\n", name, name
    printf "struct %s {\n", name
    next
}

/^[A-Za-z][a-zA-Z0-9_]+\([0-9]+\)/ {
    field = $1
    split(field, split1, /\(/)
    name = split1[1]
    bits = gensub(/\)/, "", "g", split1[2])

    printf "\t%s %s : %s;\n", base_type, name, bits
}


END {
    print "};\n\n"
}


