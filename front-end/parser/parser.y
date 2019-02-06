/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 *
 * parser.y - The Bison file, which will
 * build the parser.
 */


/* definitions and environment setup */
%{
    #include "../lexer/lexer.c"
    #include "./pheader_ast.h"
    #include "./pheader_lex_comp.h"

%}

/* Specify bison header file of token and YYSTYPE definitions.  */
/* This will be used by both the lexer and parser.              */
%defines "../lexer/lheader.h"

%union {
    struct num {
        unsigned long long val;
        long double d_val;
        int types;  /* masks defined in header */
    } num;     
    
    struct str {
        char *str;
        int str_size;
        char char_val;
    } str;

    struct astnode *astnode_p; /* abstract syntax node pointer */
}

/*  Defining the token names (and order) which will be used by both 
    the lexer and the parser. For readability, over multiple lines.  */
%token  IDENT CHARLIT STRING NUMBER INDSEL PLUSPLUS MINUSMINUS SHL 
%token  SHR LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR ELLIPSIS TIMESEQ 
%token  DIVEQ MODEQ PLUSEQ MINUSEQ SHLEQ SHREQ ANDEQ OREQ XOREQ
%token  AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE
%token  ENUM EXTERN FLOAT FOR GOTO IF INLINE INT LONG REGISTER
%token  RESTRICT RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF
%token  UNION UNSIGNED VOID VOLATILE WHILE _BOOL _COMPLEX _IMAGINARY

%type <int> assn expr

%%
/* grammars and actions */
assn: IDENT '=' expr {printf("aaa\n");}
   ;

expr: NUMBER    {$$ = yylval.num.val;
                printf("%d\n", $$);}
    | expr '+' NUMBER {$$ += yylval.num.val;}
    ;

%%