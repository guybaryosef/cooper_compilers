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


/* define a yyerror, not sure how this is going to quite fit in
   between the lexer and the parser */
extern void yyerror (char const *s);


#endif