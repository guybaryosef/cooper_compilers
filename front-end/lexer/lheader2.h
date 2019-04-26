/*
 * Lexer header file.
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * Includes the token name enum as well as
 * a function to get the inverse of the enum
 * (integer back to string).  
 */


#ifndef LEXERHEADER
#define LEXERHEADER


/*
 * stringFromTokens - Reverses the action of the tokens enum.
 * 
 * This function takes in an integer from the tokens enum
 * and returns the string that the integer equals in the enum.  
 */
char *stringFromTokens(enum yytokentype f);


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
#define NUMMASK_LDBLE  256 /* return 0: not long double, 1: long double   */


/*
 * checkNumberTypes - given a number lexem, this function checks
 * and updates its token value regarding its number type (unsigned
 * vs signed, int/long/longlong, int/float/double).
 */
void checkNumberTypes(YYSTYPE *yylval, char *yytext);


/* 
 * yyerror - The function that gets called when an error occurs in both
 * the lexer and and parser.
 */
int yyerror(char const *err_str);   


/* 
 * yyerror - The function that gets called when an error occurs in both
 * the lexer and and parser.
 */
int yywarn (char const *err_str);



#endif