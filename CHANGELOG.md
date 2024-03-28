# History and Creation

ZephyrASDL was created in February of 2024 by Chubak Bidpaa, an implementation of Appel, Wang et al's **Zephyr Abstract Syntax Description Language** describe in 1997 paper of the same name. ASDL brings the robust typing system of ML-like languages to any language with less forgiving type syntax.

Known Variants:
* pyasdl: A library for Python which implements a parser for ASDL. This library is used in CPython's source code to generate the language's tree.
* Original implementation by the authors: This implementation targets more languages, such as SML. But It's just too outdated, and clunky to build. Worse is, it is bootstrapped upon itself, so building it is almost impossible.


# Revision 1: First Release

The first release predates the creation of this document. The first release was rife with issues.


# Revision 2: Linked Lists

* In revision 2, besides major improvements, linked lists were added --- both to the main code, and the generated code.
* More built-in types were added in revision 2.


# Revision 3: Major Improvements, New Features

Major improvements were made to the revision 3 of ZephyrASDL, the include:

* Better error reporting during lexical analysis
* A lot more built-in types
* Improvements to variant types
* Generating `append` and `dump` functions for sequential items
* 


These are just a few features added to rev. 3; please check out the git logs (`git log`) for a detailed list of changes.
