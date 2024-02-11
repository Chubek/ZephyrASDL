#!/usr/bin/awk -f

/^$/	{ next; }

BEGIN {
    # Define variables to store heap name and function names
    heap_name = ""
    heap_type = ""
    alloc_func = ""
    realloc_func = ""
    free_func = ""

    # Define variables to store macros and appendage
    macros = ""
    appendage = ""

    # Set state variables for parsing
    in_specs = 0
    in_macros = 0
    in_appendage = 0
}

# Process input line by line
{
    # Handle specs section
    if ($1 == "%Specs:") {
        in_specs = 1
        next
    }
    if (in_specs) {
        if ($1 == "HeapName:") {
            heap_name = $2
        } else if ($1 == "HeapType:") {
            heap_type = $2
        } else if ($1 == "HeapAlloc:") {
            alloc_func = $2
        } else if ($1 == "HeapRealloc:") {
            realloc_func = $2
        } else if ($1 == "HeapFree:") {
            free_func = $2
        }
    }

    # Handle macros section
    if ($1 == "%Macros:") {
        in_macros = 1
        next
    }
    if (in_macros) {
        macros = macros $0 "\n"
        next
    }

    # Handle appendage section
    if ($1 == "%Appendage:") {
        in_appendage = 1
        next
    }
    if (in_appendage) {
        appendage = appendage $0 "\n"
        next
    }
}

# Generate output
END {
    # Emit macros
    printf "%s", macros # Fixed to use printf instead of print

    # Emit heap
    printf "static struct %s {\n", heap_name;
    printf "    void *memory;\n";
    printf "    size_t size;\n";
    printf "    struct %s *next;\n", heap_name;
    printf "} *%s__HEAP;\n\n", heap_name;

    # Emit allocate function
    printf "static inline void *%s(size_t num_members, size_t size_member) {\n", alloc_func; # Fixed variable name
    printf "    struct %s *node = ALLOCATE(1, sizeof(struct %s));\n", heap_name, heap_name;
    printf "    node->memory = ALLOCATE(num_members, size_member);\n";
    printf "    node->size = num_members * size_member;\n";
    printf "    node->next = %s__HEAP;\n", heap_name;
    printf "    %s__HEAP = node;\n", heap_name;
    printf "    return node->memory;\n";
    printf "};\n\n";

    # Emit reallocate function
    printf "static inline void *%s(void *memory, size_t new_size) {\n", realloc_func;
    printf "    struct %s *node = %s__HEAP;\n", heap_name, heap_name;
    printf "    while (node != NULL) {\n";
    printf "        if (node->memory == memory) {\n";
    printf "            break;\n";
    printf "        }\n";
    printf "        node = node->next;\n";
    printf "    }\n";
    printf "    if (node == NULL) {\n";
    printf "        fprintf(stderr, \"Memory realloc error at %s\");\n", heap_name;
    printf "        exit(EXIT_FAILURE);\n";
    printf "    }\n";
    printf "    node->size = new_size;\n";
    printf "    node->memory = REALLOC(node->memory, new_size);\n";
    printf "    return node->memory;\n";
    printf "}\n\n";

    # Emit free function
    printf "static inline void %s(void *memory, bool all) {\n", free_func;
    printf "    if (all) goto all;\n";
    printf "    struct %s *node = %s__HEAP;\n", heap_name, heap_name;
    printf "    while (node != NULL) {\n";
    printf "        if (node->memory == memory) {\n";
    printf "            break;\n";
    printf "        }\n";
    printf "        node = node->next;\n";
    printf "    }\n";
    printf "    if (node == NULL) {\n";
    printf "        fprintf(stderr, \"Memory free error at %s\");\n", heap_name;
    printf "        exit(EXIT_FAILURE);\n";
    printf "    }\n";
    printf "    if (node->memory != NULL) {\n";
    printf "        free(node->memory);\n";
    printf "    }\n";
    printf "all:\n";
    printf "    while (%s__HEAP != NULL) {\n", heap_name;
    printf "        node = %s__HEAP;\n", heap_name;
    printf "        %s__HEAP = %s__HEAP->next;\n", heap_name, heap_name;
    printf "        free(node);\n";
    printf "    }\n";
    printf "}\n\n";

    # Emit appendage
    printf "%s", appendage
}

