/*
 * ECE466: Compilers
 * By: Guy Bar Yosef
 *
 * parser_tester - A small program to run the parser
 * through whatever input is piped into it.
 * 
 */


#include "./parser.c"


int main() {
    yyparse();
    return 0;
}