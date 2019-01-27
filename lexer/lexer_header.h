/*
 * Lexer header file.
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * Includes the token name enum as well as
 * a function to get the inverse of the enum
 * (integer back to string).  
 * 
 */

#include <math.h>
#include <limits.h>
#include <stdlib.h>

#define LINESIZE 1024


/* token names, complying with tokens-manual.h file from hakner */
enum tokens {
	TOKEOF=0,
	IDENT=257,	/* This is where yacc will put it */
	CHARLIT,
	STRING,
	NUMBER,
	INDSEL,
	PLUSPLUS,
	MINUSMINUS,
	SHL,
	SHR,
	LTEQ,
	GTEQ,
	EQEQ,
	NOTEQ,
	LOGAND,
	LOGOR,
	ELLIPSIS,
	TIMESEQ,
	DIVEQ,
	MODEQ,
	PLUSEQ,
	MINUSEQ,
	SHLEQ,
	SHREQ,
	ANDEQ,
	OREQ,
	XOREQ,
	AUTO,
	BREAK,
	CASE,
	CHAR,
	CONST,
	CONTINUE,
	DEFAULT,
	DO,
	DOUBLE,
	ELSE,
	ENUM,
	EXTERN,
	FLOAT,
	FOR,
	GOTO,
	IF,
	INLINE,
	INT,
	LONG,
	REGISTER,
	RESTRICT,
	RETURN,
	SHORT,
	SIGNED,
	SIZEOF,
	STATIC,
	STRUCT,
	SWITCH,
	TYPEDEF,
	UNION,
	UNSIGNED,
	VOID,
	VOLATILE,
	WHILE,
	_BOOL,
	_COMPLEX,
	_IMAGINARY
};


/*
 * stringFromTokens - Reverses the action of the tokens enum.
 * 
 * This function takes in an integer from the tokens enum
 * and returns the string that the integer equals in the enum.  
 */
static inline char *stringFromTokens(enum tokens f) {
    static char *strings[] = { 
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


/*  'Number' Token Type masks:
    The 'num' struct inside of yylval has a variable calle 'types'.
    This variable contains the mask of whether the number is:
    - unsigned or signed.
    - regular, long, or long long.
    - int, float, or double.
   This will be recognized using the following masks */
#define NUMMASK_UNSIGN   1 /* return 0: signed,     1: unsigned */
#define NUMMASK_SIGN     2 /* return 0: unsigned,   1: signed   */
#define NUMMASK_LONG     4 /* return 0: not long,   1: long     */
#define NUMMASK_LL       8 /* return 0: not ll,     1: ll       */
#define NUMMASK_INT     16 /* return 0: not int,    1: int      */
#define NUMMASK_INTGR   32 /* return 0: not integer,1: integer  */
#define NUMMASK_FLOAT   64 /* return 0: not float,  1: float    */
#define NUMMASK_DOUBLE 128 /* return 0: not double, 1: double   */
    

/*
 * Structure definition of the number stuct that will be used
 * in yylval.
 */
struct num {
    unsigned long long val;
    int types; /* MASKS defined in header */
} num;


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
