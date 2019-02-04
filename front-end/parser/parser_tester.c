/*
 * ECE466: Compilers
 * By: Guy Bar Yosef
 *
 * parser_tester - A small program to run the parser
 * through some input.
 * 
 */

#include "./parser.c"


int main() {
 
    yyparse();

    return 0;
}