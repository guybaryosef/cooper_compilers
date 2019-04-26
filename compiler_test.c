/*
 * ECE466: Compilers
 * By: Guy Bar Yosef
 *
 * compiler_test - A program to run the compiler
 * through whatever input is piped in.
 */


/* Defines the global variables here. All other header files
   will see these variables as extern vars.  */
#define EXTERN_VAR 


#include "./front-end/front_end_header.h"
#include "./front-end/lexer/lexer.c"
#include "./front-end/parser/parser.c"


     
int main(int argc, char **argv) {
    if (argc == 2) {
        ast_pl = Minimal_Level; 
        quads_pl = Minimal_Level;
    }
    else if (argc == 4) {
        if (!strcmp(argv[2], "1"))
            ast_pl = Minimal_Level;
        else if (!strcmp(argv[2], "2"))
            ast_pl = Mid_Level;
        else if (!strcmp(argv[2], "3"))
            ast_pl = Verbose_Level;
        else {
            fprintf(stderr, "Correct Usage: %s [-p 1/2/3]\n", argv[0]);
            return -1;
        }

        if (!strcmp(argv[3], "1"))
            quads_pl = Minimal_Level;
        else if (!strcmp(argv[3], "2") || !strcmp(argv[3], "3"))
            quads_pl = Mid_Level;
        else {
            fprintf(stderr, "Correct Usage: %s [-p 1/2/3 1/2/3]\n", argv[0]);
            return -1;
        }   
    }
    else {
        fprintf(stderr, "Correct Usage: %s [-p 1/2/3 1/2]\n", argv[0]);
        return -1;
    }

    initializeFrontEnd();   /* initializes the front-end global vars */

    yyparse();
    return 0;
}