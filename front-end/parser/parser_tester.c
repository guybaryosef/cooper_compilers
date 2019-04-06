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


     

int main(int argc, char **argv) {

    if (argc == 1) 
        print_level = Minimal_Level;
    else if (argc == 3) {
        if (!strcmp(argv[2], "1"))
            print_level = Minimal_Level;
        else if (!strcmp(argv[2], "2"))
            print_level = Mid_Level;
        else if (!strcmp(argv[2], "3"))
            print_level = Verbose_Level;
        else {
            fprintf(stderr, "Correct Usage: %s [-p 1/2/3]\n", argv[0]);
            return -1;
        }    }
    else {
        fprintf(stderr, "Correct Usage: %s [-p 1/2/3]\n", argv[0]);
        return -1;
    }

    initializeFrontEnd();   /* initializes the front-end global vars */

    yyparse();
    return 0;
}