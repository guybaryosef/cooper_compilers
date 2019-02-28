# cooper_compilers
ECE466: Compilers

Building a compiler for the C programming language.


## Todo:
* Decide how to implement the construction of the symbol table entries through all the optional declaration specifiers.
* Incorporate symbol table into parser.
* Build functionality to print out the symbol table entires as they appear in similar format to AST nodes being printed.
* Build grammar for C declarations and finish up SIZEOF and casting expressions.

## summary of where I am in assignment 3:

So I have built an architecture where a symbol table is basically a hash table, and the symbol table entries lead to different variables. The entries hold declaration specifiers (different specifiers for each type of variable) and also a pointer to an AST node, which will contain the actual, and often complicated, 'type' of the variable. I also expanded the AST nodes to be able to represent any C types.


However while beginning to work my way through the declarations grammar, I am still a bit fishy on some of the details:

For example, we could get a const pointer to a const array of ints. In this situation, the symbol table entry will say that the type qualifier is const and have a pointer to an AST node that is of pointer type. However that pointer needs to point to ANOTHER symbol table entry, because it needs to specify that it is pointing to a CONST int, not just an int (if it was a pointer to an int, it could just point to an AST node with the scalar value of an int). 
As such pointers could point to symbol table entries OR AST nodes. 

The question becomes should we differentiate them, or should we keep expanding AST node definitions so that it itself could be the entry in the symbol table.