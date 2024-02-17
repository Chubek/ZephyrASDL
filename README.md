# Zephyr ASDL Implementation

This is an implmentation of the Zephyr ASDL language as described [in this paper](https://www.cs.princeton.edu/~appel/papers/asdl97.pdf).

One addition has been made, just like in Lex and Yacc, you can insert a prelude into the final file, just what you wish to include between a pair of `%%`s.

You can see `examples/basic.asdl` to get a clue. Please read the paper for more info on what ASDL is, as the people who came up with it explain it best.

## How to Install

```
sudo apt-get install flex bison
make && make install
```
