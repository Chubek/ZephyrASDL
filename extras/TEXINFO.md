# Guide for ASDL and asdl(1)

`asdl(1)` is an implementation of the ZephyrASDL language, specified in 1997 by Appel, Wang, Korn, and Serra in the paper 'Zephyr Abstract Syntax Description Language'. The following document is a guide on how to write ASDL specs and use the `asdl(1)` translator utility, which translates ASDL specs to C code.

**Note**: When referring to our implementation of ASDL, we'll use `asdl(1)`. We will use 'ASDL' when referring to the language as a whole.


* [Building asdl(1)](#building-asdl-1): How to build the package with make(1) and companion files
* [Using asdl(1)](#using-asdl-1): How to use the utility
* [The ASDL and asdl(1) Grammar](#the-asdl-and-asdl-1-grammar): The grammar that `asdl(1)` accepts, in EBNF Format
* [The Built-in Types](#the-built-in-types): The types which have been added to asdl(1) which may be used by default
* [Details of ASDL Language](#details-of-asdl-language): How to Define ASTs in ASDL
* [The Constructor Functions](#the-constructor-functions): Constructor functions in generated files
* [Optional and Seuential Fields](#optional-and-sequential-fields): Fields with more than one members, and optional fields
* [Using Generated Files](#using-generated-files): How to use the generated files
* [Future of asdl(1)](#future-of-asdl-1): ASDL is an abandoned language, but asdl(1) is not!


### Building asdl(1) <a id="building-asdl-1">[#]</a>

To build asdl(1), just run `make`. To install it, run `make install`.

In order to build asdl(1), you will need:

* An implementation of Yacc and Lex (preferably Bison and Flex);
* A C compiler (like GCC)
* Perl

After installation, you can browse the man page by typing `man 1 asdl` into terminal. You can page this document by `info asdl`. Examples and companions will be located in `/usr/local/share/doc/asdl` (emit `/local` if installed from a package manager).

asdl(1) also comes with a VimScript which you can use with any implementation of Vim to have syntax highlighting. This file is called `asdl.vim`. Please consult Vim's documentation to see what needs to be done to add it to your editor.

### Using asdl(1) <a id="using-asdl-1">[#]</a>

The `asdl(1)` translator utility has several options and one argument. The syntax for invoking it is:

```
asdl [-o output_file] [-d def_suffix] [-f fn_suffix] [-k kind_suffix] [-a arg_suffix] [-p fn_prefix] INPUT_FILE
```

To explain each option in detail:

* `output_file`: (default: STDOUT) This option specifies the output file for the translated C source. If it is not given, STDOUT is used.
 
* `def_suffix`: (default: "def") This option specifies the suffix for typedefs. Basically, as we'll see, every tree has a name, and this name is translated into a nested data structure. Each structure is referred to by its typedef (which is a pointer alias). For example, for a structure named `stmt`, we get:

```
typedef struct stmt *stmt_def;
```

* `fn_suffix`: (default: "create") This is actually the 'prefix', in a way, but since there's also an `fn_prefix`, we call it that. This marks the fragment of text which precedes the name of sub-tree in constructor functions. So if we have a sub-tree named 'Add' for the `stmt` tree, we get:

```
stmt_def create_add(/* arguments */);
```

(Keep in mind that constructor functions are also declared).

* `kind_suffix`: (default "kind") This is the suffix for enumerations. The translator emits two types of enumerations, one for stubs, one for each sub-tree in the tree. So the 'Add' subtree will have the kind:

```
ADD_kind
```

* `arg_suffix`: (default "arg") Basically, the argument name suffix. Nothing special about this one.

* `fn_prefix`: (empty by default) You can use this to declare `static` or `static inline` functions. Just pass what you want prefixed before the function declaration.

### The ASDL and asdl(1) Grammar <a id="the-asdl-and-asdl-1-grammar">[#]</a>

`asdl(1)` defines a superset for Zephyr ASDL. This superset includes extended pre-defined types and Yacc-like embeds. The grammar that `asdl(1)` accepts can be described in EBNF format as:

```ebnf
### Syntactic Grammar

definitions ::= prelude { definition | comment } "%%" c-code
definition ::= type_id '=' type

type ::= sum_type | product_type

product_type ::= fields

sum_type ::= constructor { '|' constructor } [ "attributes" fields ]

constructor ::= con_id [ fields ]

fields ::= '(' field { ',' field } ')'

field ::= ( type_id | predef-types ) [ '?' | '*' ] [ id ]

prelude ::= "%{" c-code "%}"


### Lexical Grammar
 
predef-types ::= "int" 
        | "uint"
        | "size" 
        | "usize"
        | "boolean" 
        | "string"
        | "identifier"

id ::= con_id | type_id

con_id ::= [A-Z][a-zA-Z0-9_]*
type_id ::= [a-z][a-z0-9_]*

c-code ::= ? valid-c-code ?

comment ::= "### " .* \n

```

### Built-in Types <a id="the-built-in-types">[#]</a>

ASDL only supports 2 built-in types (int and identifier). But asdl(1) supports:

* int, uint
* size, usize,
* boolean
* identifier
* string

`size` is translated to `ssize_t`. `usize` to `size_t`. `int` and `uint` are translated to `intmax_t` and `uintmax_t`. `identifier` is translated into `char*`. string to `uint8_t*`. And `boolean` to `bool`.

### Details of ASDL Language <a id="details-of-asdl-language">[#]</a>

Based on the grammar above, the AST for `m4(1)` syntax would be:

```
m4 = Define(string name, m4 definition)
   | ArgumentRef(int num)
   | Token(string*)
```

Here, `Define`, `ArgumentRef`, and `Token` are each called *Constructors*. What's between open parenthesis and close parenthesis are called *Fields*. A Field has a *Type ID* and an OPTIONAL *Id*. The *Type ID* may be referred to recursively. The entire thing is a *Rule*.

`asdl(1)` will generate a data structure for the `m4` rule. This structure would be:

```
typedef struct m4 *m4_def; // this will be actually emitted before any rule is defined
struct m4 {
   enum {
        DEFINE_kind,
        ARGUMENTREF_kind,
        TOKEN_kind,
   };
   union {
        struct Define {
          uint8_t *name;
          m4_def *definition;
        } define;
        struct ArgumentRef {
          int num;
        };
        struct Token {
           struct {
             uint8_t **string_0;
             ssize_t num_string_0;
          } token_seq;
        };
   } value;
   m4_def *next;
};
```

### The Constrcutor Functions <a id="the-constructor-functions">[#]</a>

With each constructor, a `create_<constructor>` function is generated, with its arguments being the fields. These functions use the `ALLOC` macro to allocate space for the structure. You can define your own `ALLOC` in the prelude:

```
%{
#define ALLOC(size) calloc(1, size)
%}

m4 = ...
```

Now, as said before, you can always append your own C code after everything is done, à la Yacc:

```
m4 = ...
%%

int main(void) {
  return 0;
}
```

### Optional and Sequential Fields <a id="optional-and-sequential-fields">[#]</a>

One thing to note is, in ASDL, you can use `*` after a type id (for example, `string*`) to denote sequential fields and `?` after one to denote optional. In such cases, `asdl(1)` will emit a structure in place of these fields:

* For sequential fields, the structure adds a reference to the type and emits `ssize_t` variable to keep the count of it. You can see this in action in the example above.

* For the optional fields, the second option is a `bool` field, denoting if the item exists. No additional ref markers are generated.

### The Append and Destroy Chain Functions

After `asdl(1)` generates the constructor functions, it emits an `append` function, which takes:

1. A double-ref to the rule type.
2. A single-ref to the rule type.

Then, it uses the `next` item in the rule type (refer to the above example) to append item 2 to item 1. This way, you can use this function to keep a chain of singly-linked lists. A `destroy` function is also emitted so you can destroy the heap chain. The names of these functions are `append_<rule>_chain` and `destroy_<rule>_chain`. The `destroy` function uses the `FREE(mem)` macro which just like `ALLOC` you may re-define. By default, it's `free(mem)`.

**Note**: At this current version, you cannot change the suffixes and prefixes for generated functions unless for the ones specified in the [first section](#using-asdl-1). The ability will be added as soon as it is viable.

### Using the Generated Files <a id="using-generated-files">[#]</a>

You are free to use the generated files as you wish. One way is to generate the file as a header file (.h) and include it on top of your main file so C preprocessor can append it. 

Another way is to declare functions as extern, and compile the file alongside your main file.

### Future of asdl(1) <a id="future-of-asdl-1">[#]</a>

In the future versions, tree-walker functions will be added. It was hesitant to add them at the moment because people would like to walk the trees with their own use in mind.

Another feature which will be added in the future is generation of a separate header file that declares the functions and types.


