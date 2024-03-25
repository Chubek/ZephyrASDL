# Zephyr ASDL Implementation

This is an implmentation of the Zephyr ASDL language as described [in this 1997 paper](https://www.cs.princeton.edu/~appel/papers/asdl97.pdf) by Wang, Appel, Korn and Serra.

ASDL is a domain-specific language that is used to describe syntax of a language. You specify tree grammars, and these tree grammars are translated into C code. This C code contains the Abstract Syntax Tree declarations and definitions. 

There are plans to add more translations. D, Rust and Python would be good.

You can read the documentation for this program at `extras/MANPAGE.md`.

# How to Install

You need:

1- An implementation of Lex, e.g. Flex

2- An implementation of Yacc, e.g. Bison

3- Perl

After installing these (for example, with `apt-get`) you can run:

```
make && sudo make install
```

After that, bring up the manpage and see what is added to the language:

```
man 1 asdl
```

You can also view the Texinfo with:

```
info asdl
```

The manpage and the Texinfo are available in `extras` under `manual.html` and `guide.html`. After installing, these files, along with examples, and everything within `extras` will be copied to `/usr/local/share/doc/asdl`. If you install via a package manager, omitt `/local`.


If you need to contact me, you can do so via email: `chubakbidpaa [at] riseup [dot] net`.

Thanks to anyone who uses this program.

