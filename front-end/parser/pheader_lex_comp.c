/*
 * ECE466: Compilers
 * By: Guy Bar Yosef
 * 
 * pheader_lex_comp.c - Contains the implementation
 * of the functions defined in pheader_lex_comp.h. 
 */

#include <stdio.h>

#include "pheader_lex_comp.h"

/* define a yyerror, not sure how this is going to quite fit in
   between the lexer and the parser */
void yyerror (char const *s) {
    fprintf (stderr, "Error in %s\n", s);
}
