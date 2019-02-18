/*
 * ECE466: Compilers
 * By: Guy Bar Yosef
 * 
 * pheader_lex_compatibility.h - A header file for the Bison parser
 * file that creates a compatability with the flex-generated lexer. 
 */


#ifndef PARSER_LEXER_COMPT
#define PARSER_LEXER_COMPT


/* Defines a str and num structs that will appear in YYSTYPE (yylval) */
struct YYnum {
    unsigned long long val;
    long double d_val;
    int types;  /* masks defined in header */
};
struct YYstr {
    char *str;
    int str_size;
    char char_val;
};


/* 
 * yyerror - The function that gets called when an error occurs in both
 * the lexer and and parser.
 */
extern void yyerror (char const *err_str);


#endif