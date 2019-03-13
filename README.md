# cooper_compilers
ECE466: Compilers

Building a compiler for the C programming language.


## Assignment 3 Todo:
* Incorporate symbol table into parser.
* Build functionality to print out the symbol table entires as they appear in similar format to AST nodes being printed.
* Build grammar for C declarations and finish up SIZEOF and casting expressions.

#### Status and next on the agenda:
* Got most of the required functionality to work (scalars, arrays, functions, pointers). Next steps:
* Build intermediate symbol table insert function & implement both regular insert as well as resize insert using this new function. Afterwards check that duplicates do not get added to the symbol table.
* Implement the large type checking function to make sure crazyness isn't happening (not sure if it is needed though).
* Implement structs & unions.
* Implement scope.
* Implement abstract types & use them in sizeof expressions and cast expressions.

##### Stretch goals:
* Implement function prototypes.
* Implement enums.

