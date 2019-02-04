/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 *
 * parser.y - The Bison file, which will
 * build the parser.
 */


/* definitions and environment setup */
%{
    #include "../lexer/lexer.c"
    #include "./pheader_ast.h"
    #include "./pheader_lex_comp.h"

%}

%union {
    struct astnode *astnode_p;
}

%type <astnode_p> expr;


%%
/* grammars and embedded actions */
expr: '('
    ;

%%