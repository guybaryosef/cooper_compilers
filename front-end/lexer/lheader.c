/*
 * Lexer header implementation file.
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * lheader.c - Contains the implementation of the functions
 * defined in lheader.h.
 */

#include <string.h>

#include "lheader.h"

/*
 * stringFromTokens - Reverses the action of the tokens enum.
 * 
 * This function takes in an integer from the tokens enum
 * and returns the string that the integer equals in the enum.  
 */
char *stringFromTokens(enum tokens f) {
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
        "ELSE",
        "ENUM",
        "EXTERN",
        "FLOAT",
        "FOR",
        "GOTO",
        "IF",
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

    return strings[f-257];
}


/*
 * checkNumberTypes - given a number lexem, this function checks
 * and updates its token value regarding its number type (unsigned
 * vs signed, int/long/longlong, int/float/double).
 */
void checkNumberTypes(struct num *yylval, char * yytext) {
    /* check and confirm type of integer */
    if (strstr(yytext, "u") || strstr(yytext, "U"))
        yylval->types |= NUMMASK_UNSIGN;
    else
        yylval->types |= NUMMASK_SIGN;

    /* confirm that number type is compatible */
    if (strstr(yytext, "ll") || strstr(yytext, "LL"))
        yylval->types |= NUMMASK_LL;
    else if (strstr(yytext, "l") || strstr(yytext, "L")) {
        if (((yylval->types & NUMMASK_UNSIGN) && 
                        (yylval->val > ULONG_MAX)) || 
                                (yylval->val > LONG_MAX))
            yylval->types |= NUMMASK_LL;
        else
            yylval->types |= NUMMASK_LONG;
    }
    else { /* int type by default */
        if (((yylval->types & NUMMASK_UNSIGN) && 
                        (yylval->val > ULONG_MAX)) || 
                                (yylval->val > LONG_MAX))
            yylval->types |= NUMMASK_LL;
        if (((yylval->types & NUMMASK_UNSIGN) && 
                        (yylval->val > UINT_MAX)) || 
                                (yylval->val > INT_MAX))
            yylval->types |= NUMMASK_LONG;
        else
            yylval->types |= NUMMASK_INT;
    }
}
