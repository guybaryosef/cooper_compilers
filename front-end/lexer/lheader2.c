/*
 * Lexer header implementation file.
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * lheader.c - Contains the implementation of the functions
 * defined in lheader.h.
 */


#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "../front_end_header.h"
#include "lheader.h"
#include "lheader2.h"


/*
 * stringFromTokens - Reverses the action of the tokens enum.
 * 
 * This function takes in an integer from the tokens enum
 * and returns the string that the integer equals in the enum.  
 */
char *stringFromTokens(enum yytokentype f) {
    char *strings[] = { 
        "IDENT",	/* This is where yacc will put it */
        "CHARLIT",
        "STRING",
        "NUMBER",
        "INDSEL",
        "PLUSPLUS",
        "MINUSMINUS",
        "SHL",
        "SHR",
        "LTEQ",
        "GTEQ",
        "EQEQ",
        "NOTEQ",
        "LOGAND",
        "LOGOR",
        "ELLIPSIS",
        "TIMESEQ",
        "DIVEQ",
        "MODEQ",
        "PLUSEQ",
        "MINUSEQ",
        "SHLEQ",
        "SHREQ",
        "ANDEQ",
        "OREQ",
        "XOREQ",
        "AUTO",
        "BREAK",
        "CASE",
        "CHAR",
        "CONST",
        "CONTINUE",
        "DEFAULT",
        "DO",
        "DOUBLE",
        "ENUM",
        "EXTERN",
        "FLOAT",
        "FOR",
        "GOTO",
        "IF",
        "ELSE",
        "INLINE",
        "INT",
        "LONG",
        "REGISTER",
        "RESTRICT",
        "RETURN",
        "SHORT",
        "SIGNED",
        "SIZEOF",
        "STATIC",
        "STRUCT",
        "SWITCH",
        "TYPEDEF",
        "UNION",
        "UNSIGNED",
        "VOID",
        "VOLATILE",
        "WHILE",
        "_BOOL",
        "_COMPLEX",
        "_IMAGINARY"
    };

    return strings[f-258];
}


/*
 * checkNumberTypes - given a number lexem, this function checks
 * and updates its token value regarding its number type (unsigned
 * vs signed, int/long/longlong, int/float/double).
 */
void checkNumberTypes(YYSTYPE *yylval, char *yytext) {
    /* check and confirm type of integer */
    if (strstr(yytext, "u") || strstr(yytext, "U"))
        yylval->num.types |= NUMMASK_UNSIGN;
    else
        yylval->num.types |= NUMMASK_SIGN;

    /* confirm that number type is compatible */
    if (strstr(yytext, "ll") || strstr(yytext, "LL"))
        yylval->num.types |= NUMMASK_LL;
    else if (strstr(yytext, "l") || strstr(yytext, "L")) {
        if (((yylval->num.types & NUMMASK_UNSIGN) && 
                        (yylval->num.val > ULONG_MAX)) || 
                                (yylval->num.val > LONG_MAX))
            yylval->num.types |= NUMMASK_LL;
        else
            yylval->num.types |= NUMMASK_LONG;
    }
    else { /* int type by default */
        if (((yylval->num.types & NUMMASK_UNSIGN) && 
                        (yylval->num.val > ULONG_MAX)) || 
                                (yylval->num.val > LONG_MAX))
            yylval->num.types |= NUMMASK_LL;
        if (((yylval->num.types & NUMMASK_UNSIGN) && 
                        (yylval->num.val > UINT_MAX)) || 
                                (yylval->num.val > INT_MAX))
            yylval->num.types |= NUMMASK_LONG;
        else
            yylval->num.types |= NUMMASK_INT;
    }
}


/* 
 * yyerror - The function that gets called when an error occurs in both
 * the lexer and and parser.
 */
int yyerror (char const *err_str) {
    fprintf(stderr, "%s:%d:%d Error: %s\n", 
                cur_file_name, cur_line_num, yylloc.last_column, err_str);

    return 0;
}