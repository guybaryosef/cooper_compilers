# cooper_compilers
ECE466: Compilers

Building a compiler for the C programming language.

#### Status:
* Lexer working.
* Parser can parse a large subset of C language - for now we decided not to bother with typedefs, enums, function prototypes, old school function definitions, and a few other small grammars.
* Parser builds Abstract Syntax Trees for all C statements and function definitions. It parses declarators to maintain an up-to-date symbol table that incorporates scope stacks and namespaces. The parser uses the symbol table to resolve identifiers in the statements and function definitions.
* Need to begin assignment 5.

#### Todo:
* Add flags to whether to print the variable declarator definitions.
* Fix structs and unions to be defined after the declarator instead of after their body.
* Assignment 5 - Quads baby.

##### Stretch goals:
* Implement function prototypes & function arguments.
* Implement enums.
* Implement struct bit fields.
* Implement initialized declarators.
