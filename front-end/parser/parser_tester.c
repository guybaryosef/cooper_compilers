/*
 * ECE466: Compilers
 * By: Guy Bar Yosef
 *
 * parser_tester - A small program to run the parser
 * through whatever input is piped into it.
 * 
 */


/* Defines the global variables here. All other header files
   will see these variables as extern vars.  */
#define EXTERN_VAR 


#include "../front_end_header.h"
#include "../lexer/lexer.c"
#include "./parser.c"


     

int main() {
    
    initializeFrontEnd();   /* initializes the front-end global vars */

    yyparse();
    return 0;
}