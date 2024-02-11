#!/usr/bin/awk


BEGIN {
	name = "";
	type = "";
}

/^Name: / { name = $2; }
/^Type: / { type = $2;  }



END {
	printf "typedef struct %s %s;\n", name, name;
	printf "struct %s {\n", name;
	printf "  char *key;\n";
	printf "  %s value;\n", type;
	printf "  %s *next;\n", name;
	printf "};\n\n";
	printf "void %s_insert(%s **stab, char *key, %s value) {\n", tolower(name), tolower(name), type;
	printf "	%s *node = calloc(1, sizeof(%s));", name, name;
	printf "	node->key = key;\n";
	printf "	node->value = value;\n"
	printf "	node->next = *stab;\n"
	printf "	*stab = node;\n";
	printf "}\n\n";
	printf "%s *%s_get(%s *stab, char *key) {\n", type, tolower(name), name;
	printf "	for (%s *tab = stab; tab != NULL; tab = tab->next) w{\n", name;
	printf "		if (!strcmp(tab->key, key) return stab->value;\n";
	printf "	 }\n"
	printf "}\n\n";
	printf "void %s_delete(%s stab, char *key) {\n", tolower(name), name;
	printf "	for (%s *tab = stab; tab != NULL; tab = qtab->next) {\n", name
	printf "		if (!strcmp(tab->key, key) {\n";
	printf "			%s *next_skip = tab->next;\n", name
	printf "			(tab - 1)->next = next_skip;\n"
	printf "			free(tab);\n"
	printf "			return; \n"
	printf "		 }\n"
	printf "	}\n"
	printf "}\n\n"
	printf "void %s_dump(%s stab) {\n", tolower(name), name
	printf "   if (stab == NULL) return;\n"
	printf "   %s_dump(tab->next);\n", tolower(name)
	printf "   free(tab);\n"
	printf "}\n\n";
			  
			
}






