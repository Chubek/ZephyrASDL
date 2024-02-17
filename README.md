# Zephyr ASDL Implementation

This is an implmentation of the Zephyr ASDL language as described [in this paper](https://www.cs.princeton.edu/~appel/papers/asdl97.pdf).

One addition has been made, just like in Lex and Yacc, you can insert a prelude into the final file, just put what you wish to include between a pair of `%%`s (with a newline after the first delimiter, and before the second one).

You can see `examples/basic.asdl` to get a clue. Please read the paper for more info on what ASDL is, as the people who came up with it explain it best.

## How to Install

```
sudo apt-get install flex bison
make && make install
man 1 asdl
```

## Uses for Zephyr ASDL

This domain-specific language is mostly used to serialize a tree description into an Abstract Syntax Tree (AST) in the target language.

## Notice

Default types described by paper (`identifier` and `int`) are not (or yet to be) included. You can use a couple of `typedef` statements to have them in your file by inserting them as preludes between two `%%`s.
