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
#include <string.h>

#include "./front-end/front_end_header.h"
#include "./front-end/lexer/lexer.c"
#include "./front-end/parser/parser.c"
#include "./back-end/assemb_gen.h"

#include "./back-end/back_end_header.h"


int main(int argc, char **argv) {

    // figure out file flags
    char *output_name;
    if (argc == 2) {
        ast_pl = Minimal_Level; 
        quads_pl = Minimal_Level;
    }
    else if (argc == 4 && !strcmp("p", argv[2])) {
        if (!strcmp(argv[2], "1"))
            ast_pl = Minimal_Level;
        else if (!strcmp(argv[2], "2"))
            ast_pl = Mid_Level;
        else if (!strcmp(argv[2], "3"))
            ast_pl = Verbose_Level;
        else {
            fprintf(stderr, "Correct Usage: %s [-p 1/2/3] [-n output_name]\n", argv[0]);
            return -1;
        }

        if (!strcmp(argv[3], "1"))
            quads_pl = Minimal_Level;
        else if (!strcmp(argv[3], "2") || !strcmp(argv[3], "3"))
            quads_pl = Mid_Level;
        else {
            fprintf(stderr, "Correct Usage: %s [-p 1/2/3 1/2/3] [-n output_name]\n", argv[0]);
            return -1;
        }   
    }
    else if (argc == 3 && !strcmp("-n", argv[2])) {

        if (!strcmp(argv[2], "stdout"))
            output_name = NULL;
        else 
            output_name = argv[2];    
    }
    else if (argc == 6) {
        if (!strcmp(argv[2], "1"))
            ast_pl = Minimal_Level;
        else if (!strcmp(argv[2], "2"))
            ast_pl = Mid_Level;
        else if (!strcmp(argv[2], "3"))
            ast_pl = Verbose_Level;
        else {
            fprintf(stderr, "Correct Usage: %s [-p 1/2/3] [-n output_name]\n", argv[0]);
            return -1;
        }

        if (!strcmp(argv[3], "1"))
            quads_pl = Minimal_Level;
        else if (!strcmp(argv[3], "2") || !strcmp(argv[3], "3"))
            quads_pl = Mid_Level;
        else {
            fprintf(stderr, "Correct Usage: %s [-p 1/2/3 1/2/3] [-n output_name]\n", argv[0]);
            return -1;
        }   

        if (!strcmp(argv[5], "stdout"))
            output_name = NULL;
        else 
            output_name = argv[5];
    }
    else {
        fprintf(stderr, "Correct Usage: %s [-p 1/2/3 1/2] [-n output_name]\n", argv[0]);
        return -1;
    }

    /* initializes the front-end global vars */
    initializeFrontEnd();   

    /* run front-end */
    yyparse();  

    /* run back-end */
    initializeRegisterCollection();

    generateAssemb32(output_name);   

    return 0;
}