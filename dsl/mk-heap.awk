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
        } else if ($1 == "HeapType")
	    heap_type $2;
	else if ($1 == "HeapAlloc:") {
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
    print macros

    # Emit heap
    "static struct %s {\n", heap_name;
    "	void *memory;\n";
    "	size_t size;\n";
    "	struct %s next\n;", heap_name;
    "} *%s__HEAP;\n\n", heap_name;

    # Emit allocate function
    "static inline void *%s(size_t num_members, size_t size_member) {\n", alloc_fun;
    "	struct %s *node = ALLOCATE(1, sizeof(struct %s));", heap_name;
    "	node->memory = ALLOCATE(num_members, size_members);"
    "   node->size = num_members * size_member;"
    "   node->next = %s__HEAP;"
    "   %s__HEAP = node;"
    "   return node->memory;"
    "};\n"

    # emit reallocate function
    "static inline void *%s(void *memory, size_t new_size) {\n", realloc_func;
    " 	struct %s *node = *%s__HEAP;\n", heap_name, heap_name;
    "	while (node != NULL) \n"	
    "		if (node->memory == memory) "
    "			break;	"
    "   if (node == NULL) {"
    "		fprintf(stderr, \"Memory realloc error at %s\");\n", heap_name;
    " 		exit(EXIT_FAILURE);	\n"
    "   }\n"
    "   node->size = new_size;\n"
    "   node->memory = REALLOC(node->memory, new_size);\n"
    "   return node->memory\n;"
    "}\n"

    # emit free function
    "static inline void *%s(void *memory, bool all) {\n", free_func;
    "   if (all) goto all;\n"
    " 	struct %s *node = *%s__HEAP\n;", heap_name, heap_name;
    "	while (node != NULL) \n"	
    "		if (node->memory == memory) \n"
    "			break;	\n"
    "   if (node == NULL) {\n"
    "		fprintf(stderr, \"Memory free error at %s\");\n", heap_name;
    " 		exit(EXIT_FAILURE);	\n"
    "   }\n"
    "   if (node->memory != NULL)\n"
    "            free(node->memory\n"
    "all:\n"
    " 	struct %s *node = *%s__HEAP\n;", heap_name, heap_name;
    "	while (node != NULL) \n"
    "		free(node); "
    "}\n"

 
    # Emit appendage
    printf "%s", appendage
}

