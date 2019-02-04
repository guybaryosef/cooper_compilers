/*
 * ECE466: Compilers
 * By: Guy Bar Yosef
 * 
 * pheader_lex_compatibility.h - A header file for the Bison parser
 * file that creates a compatability with the flex-generated lexer. 
 */


#ifndef PARSER_LEXER_COMPT
#define PARSER_LEXER_COMPT

#define YYSTYPE YYSTYPE /* yystype was defined in the lexer */

/* define a yyerror, not sure how this is going to quite fit in
   between the lexer and the parser */
void yyerror (char const *s);

#endif