# cooper_compilers
ECE466: Compilers

Building a compiler for the C programming language.

### How to Run:

The compiler was built from the beginning in stages, and as such one could see the result of the compiler at different stages of completion, with each new stage building on top of the past stages. Each stage has a pdf associated with it, which was the assignment description and has now become a reference on what is included. 

The first stage was the lexer, and the assignment can be found */lexer/ltests/assign1.pdf*. To run the lexer by itself, use the Makefile inside the lexer directory.

Stages 2-5 involve the parser and can be found respectively in the */parser/tests_*** directories. There is one Makefile for all the parser stages, and one can run the tests associated with the stage: Take for example stage 4: `make official-test-parser4`. One can add an optional flag `-PL [1,2,3]` flag which specifies the verbosity of the output, with 1 being the least verbose AST output and 3 being the most, with level 4 symbolizing AST output and QUADS output.

### Description of the Stages:
1. Lexer - Lexical analysis of nearly the whole C language.
2. Parser - Parsing C expressions.
3. Parser - Parsing C declarations, symbols, and types.
4. Parser - Parsing C statements and functions.
5. Quad Generation - Generating intermediate representation. 

#### Status:
* Lexer working.
* Parser can parse a large subset of C language - for now we decided not to bother with typedefs, enums, function prototypes, old school function definitions, and a few other small grammars.
* Parser builds Abstract Syntax Trees for all C statements and function definitions. It parses declarators to maintain an up-to-date symbol table that incorporates scope stacks and namespaces. The parser uses the symbol table to resolve identifiers in the statements and function definitions.
* Knee-deep in QUADS. Completed quads for C expressions and function calls. Basic blocks and quad storage is implemented. Working on some loops.

#### Todo:
* Implement IR for loops.
* Test quads.
* Type conversion quads.

##### Stretch goals:
* Implement function prototypes & function arguments.
* Implement enums.
* Implement struct bit fields.
* Implement initialized declarators.
