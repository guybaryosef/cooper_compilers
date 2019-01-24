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

    