# GNU Texinfo Documentation for ASDL

asdl(1) is an implementation of the ZephyrASDL language, specified in 1997 by Appel, Wang, Korn and Serra in paper 'Zephyr Abstract Syntax Description Language'. The following document is guide on how to write ASDL specs, and use the asdl(1) translator utility which translates ASDL specs to C code.

**Note**: When we are referring to our implementation of ASDL, we'll use asdl(1). We will use 'ASDL' when referring to the language as a whole.

# Navigation

* [Using asdl(1)](#using-asdl-1): How to use the utility
* [The ASDL and asdl(1) Grammar](#the-asdl-and-asdl-1-grammar): The grammar that asdl(1) accepts, in EBNF Format
* [Details of ASDL](#details-of-asdl): How to Define ASTs in ASDiL


# Using asdl(1)

asdl(1) translator utility has several options and one argument. The syntax for invoking it is:

```
asdl [-o output\_file] [-d def\_suffix] [-f fn\_suffix] [-k kind\_suffix] [-a arg\_suffix] [-p fn\_prefix] INPUT_FILE
```

To explain each option in detail

* output\_file: (default: STDOUT) This option specifies the output file, for the translated C source. If it is not given, STDOUT is used.
 
* def\_suffix: (default: "def") This option specifies the suffix for typedefs. Basically, as we'll see, every tree has a name, and this name is translated into a nested data structure. Each structure is referred to by its typedef (which is a pointer alias). For example, for a structure named `stmt`, we get:

```
typedef struct stmt *stmt_def;
```

* fn\_suffix: (default: "create") This is actually the 'prefix', in a way, but since there's also an fn\_prefix, we call it that. This marks the fragment of text which precedes the name of sub-tree in constructor functions. So if we have a sub-tree named 'Add' for the `stmt` tree, we get:

```
stmt_def create_add(/* arguments */);
```

(Keep in mind that constructor functions are also declared).


* kind\_suffix: (default "kind") This is the suffix for enumerations. The translator emits two types of enumerations, one for stubs, one for each sub-tree in the tree. So the 'Add' subtree will have the kind:

```
ADD_kind
```

* arg\_suffix: (default "arg") Basically, the argument name suffix. Nothing special about this one.

* fn\_prefix: (empty by default) You can use this to declare `static` or `static inline` functions. Just pass what you want prefixed before the function declaration.

# The ASDL and asdl(1) Grammar

asdl(1) defines a superset for Zephyr ASDL. This superset includes extended pre-defined types, and Yacc-like embeds. The grammar which asdl(1) acceps can be described in EBNF format as:

```
# Syntactic Grammar

definitions	::= prelude { definition } "%%" c-code
definition	::= type_id '=' type

type		::= sum_type | product_type

product_type	::= fields

sum_type   	::= constructor { '|' constructor } [ "attributes" fields ]

constructor	::= con_id [ fields ]

fields		::= '(' field { ',' field } ')'

field 		::= ( type_id | predef-types ) [ '?' | '*' ] [ id ]

prelude		::= "%{" c-code "%}"


# Lexical Grammar
 
predef-types	::= "int" 
		| "uint"
 		| "size" 
		| "usize"
		| "boolean" 
		| "string"
		| "identifier"

id		::= con_id | type_id

con_id		::= [A-Z][a-zA-Z0-9_]*
type_id		::= [a-z][a-z0-9_]*

c-code		::= ? valid-c-code ?
```

# Details of ASDL

ASDL language
