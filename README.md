# cooper_compilers
ECE466: Compilers

Building a compiler for a subset of the C programming language.

### How to Run:

#### tldr:

To run using the Makefile (recommended), on, for example, a file called *tester.c* and the assembly output is to be saved to *output.s*, and then executed: 
```
make guycc input=tester.c output=output.s
```

Other *Make* specifications:
* If output is not specified, the assembly code is printed to stdout and the assembly code is not assembled or executed.
* You can choose the printing level of the abstract syntax tree and quads. By default, these are not printed. However one can specify in the *Make* command `ast=2/3` for different levels of verbosity. Likewise you can specify the printing level of the quads using `quad=1/3`.

#### The long, I'm-proud-of-this rundown:

The compiler was built from the beginning in stages, and as such one could see the result of the compiler at different stages of completion, with each new stage building on top of past stages. Each stage has a an assignment description in *pdf* format associated with it and can be used as a reference to see exactly what was included (and what wasn't). 

The first stage was the lexer, and the assignment can be found */front-end/lexer/ltests/assign1.pdf*. To run the lexer by itself, use the *Makefile* inside the lexer directory.

Stages 2-5 involve the parser and the assignment specification can be found respectively in the */front-end/parser/tests_*** directories. There is one Makefile for all the parser stages, located in the */parser/* directory, and one can run the tests associated with each stage: Take for example stage 4: `make official-test-parser4`. The parser's Makefile can also specify the printing level using the variables `quad=1/3` and `ast=1/2/3`. 

Stage 6 involves the backend (this compiler does conduct any optimizations) and its assignment sheet can be found at */back-end/tests_6/assign6.pdf*. As this stage wraps up this project, the Makefile associated with it is the main project Makefile located in the project's root directory. Description of how to use it is above in the tldr section.



### Description of the Stages:
1. Lexer - Lexical analysis of nearly the whole C language.
2. Parser - Parsing C expressions.
3. Parser - Parsing C declarations, symbols, and types.
4. Parser - Parsing C statements and functions.
5. Quad Generation - Generating intermediate representation. 
6. Back-end - Target code(x86-32 assembly) generation.


#### General Todo:
* Build an expression test case file for overall compiler.
* Build a pointer arithmetic test case file for overall compiler.
* Add some more break/continue statements in ctest1.
* Add some more functions in ctest2.

##### Stretch goals:
* Type conversion quads.
* Type-specific quad and target code generation, (for ex, different commands for signed and unsigned multiplication).
* Implement grammars for function prototypes & function arguments, enums, struct bit fields, initialized declarators.
