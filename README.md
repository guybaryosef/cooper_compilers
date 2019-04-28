# cooper_compilers
ECE466: Compilers

Building a compiler for the C programming language.

### How to Run:

The compiler was built from the beginning in stages, and as such one could see the result of the compiler at different stages of completion, with each new stage building on top of the past stages. Each stage has a pdf associated with it, which was the assignment description and has now become a reference on what is included. 

The first stage was the lexer, and the assignment can be found */lexer/ltests/assign1.pdf*. To run the lexer by itself, use the Makefile inside the lexer directory.

Stages 2-5 involve the parser and the assignment specification can be found respectively in the */parser/tests_*** directories. There is one Makefile for all the parser stages, located in the */parser/* directory, and one can run the tests associated with the stage: Take for example stage 4: `make official-test-parser4`. One can add the optional flags `ast=[1,2,3] quad=[1,2]` which specify the verbosity of the ast and quads output respectively, with the value 1 not printing anything and 3 being the most verbose. By default the printing level defaults to 1.


### Description of the Stages:
1. Lexer - Lexical analysis of nearly the whole C language.
2. Parser - Parsing C expressions.
3. Parser - Parsing C declarations, symbols, and types.
4. Parser - Parsing C statements and functions.
5. Quad Generation - Generating intermediate representation. 
6. Back-end - Target code (x86-32 assembly) generation.

#### Status:
* Lexer working.
* Parser can parse a large subset of C language - for now we decided not to bother with typedefs, enums, function prototypes, old school function definitions, and a few other small grammars.
* Parser builds Abstract Syntax Trees for all C statements and function definitions. It parses declarators to maintain an up-to-date symbol table that incorporates scope stacks and namespaces. The parser uses the symbol table to resolve identifiers in the statements and function definitions.
* Completed quads for a medium-sized subset of the C language. This includes C expressions, function calls, conditional statements and loops. Excluded from this are function prototypes, function definitions with parameters, structure member operations, casts, ternary and comma operators, compound literals, switch statements, goto and labeled statements and initialized declarators.
* Currently working on target code generation.

#### General Todo:
* Implement a simple one-quad-window instruction selector, converting quads to assembly.
* Implement a primitive register allocator 

#### Step-by-step Todo:
* Assume that all variables are declared in the bss region and that there are no function calls, and that there is a return stmt.
* Implement logic to automatically add return stmt if not there.
* Then allow for function calls.
* Then allow for local variables.


##### Stretch goals:
* Type conversion quads.
* Implement grammars for function prototypes & function arguments, enums, struct bit fields, initialized declarators.
