# ASDL: An implementation of the Zephyr ASDL Language, Targeting C
## Version 1.1


## SYNTAX

```
asdl [-o output_file] [-d def_suffix] [-f fn_suffix] [-k kind_suffix] [-a arg_suffix] [-p fn_prefix] INPUT_FILE
```

## OPTIONS

* output\_file: The output file (STDOUT by default)
* def\_suffix: Suffix used for typedef aliases (*def* by default).
* fn\_suffix: Suffix used for function names (*fn* by default).
* kind\_sufifx: Suffix used for kind enumeration names (*kind* by default).
* arg\_suffix: Suffix used for argument names (*arg* by default).
* fn\_prefix: Suffix used for function signatures (empty by default, you can use `static` or `static inline`).

## ARGUMENTS

* INPUT\_FILE: The ASDL specification file (STDIN by default)

## DESCRIPTION


ASDL is an implementation of the Zephyr ASDL language, as specified by Wang, Appel, Korn, Serra in 1997 paper 'The Zephyr Abstract Syntax Description Language' (linked below). 

Zephyr ASDL was originally implemented in C, and its targts included C, C++, Stnadard ML and so on. However, this implementation was bootstrapped onto itself, and besides that, it relied on SUIF for compilation. This version has lite dependencies and is much more hassle-free. However, it just targets C (at this moment).

ASDL language is a tree grammar used to describe language syntax, the output can be used to construct Abstract Syntax Trees.

The current version (1.1) is stable. It outputs correct code, with zero warnings or errors (as far as tested). 

## ASDL DOCUMENTATION

The documentation for ASDL language can be found at the original papger ([https://www.cs.princeton.edu/~appel/papers/asdl97.pdf](https://www.cs.princeton.edu/~appel/papers/asdl97.pdf)). 

This implementation adds two YACC-like extensions to the file:

1- You can enclose C code between `%{` and `%}`, this will be inserted at the BEGINNING of the file before the translation.
2- You can insert a `%%` after the tree grammar, and anything you insert after it, C code that is, will be inserted AFTER the translation.

There are now several more built-in types: `int`, `uint`, `string`, `identifier`, `size`, `usize`, `boolean`

Also, see EXAMPLE to see how you can insert comments.


## EXAMPLE

This is a very basic ASDL that describes the syntax for **m4(1)** (the appendages are nonesense):

```
%{

#include <stddef.h>

static int IN = 0;

%}

# This is a comment

m4 = Macro(identifier name, macro definition) # Another comment
   | NumberedArgument(int num)
   | WildCardArgument
   | CommaSepArgument
   ;


%%

int main() {
  return IN;
}

```

## FILES

* **MANPAGE.md**: This documentation in form of Markdown.
* **manual.html**: This documentation in form of HTML.
* **asdl.1**: This documentation in ROFF man page format.
* **TEXINFO.md**: The Texinfo documentation in form of Markdown.
* **asdl.texi**: The Texinfo documentation in native format.
* **guide.html**: The Texinfo documentation in form of HTML.
* **asdl.vim**: VimScript syntax highlighter (you can copy it to the `/syntax` directory of NeoVim or Vim).
* **GRAMMAR.ebnf**: The grammar for ASDL in EBNF syntax
* **examples/regex.asdl**: Usage example #1
* **examples/basicl.asdl**: Usage example #2

## COPYRIGHT

2024 - Chubak Bidpaa (chubakbidpaa@riseup.net)

Released under GPL Version 3-or-later license.

Redistribution of this program complies with the conventions of above mentioned license.

## SEE ALSO

**yacc(1)**, **lex(1)**, **peg(1)**, **bnfc(1)**






