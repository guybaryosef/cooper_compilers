/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 *
 * parser.y - The Bison file, which will
 * build the parser.
 */


/* definitions and environment setup */
%{
    #include <stdio.h>

    #include "./pheader_lex_comp.h"
    #include "../lexer/lexer.c"
    #include "./pheader_ast.h"

%}

/* Specify bison header file of token and YYSTYPE definitions.  */
/* This will be used by both the lexer and parser.              */
%defines "../lexer/lheader.h"

%union {
    int simple_int;

    struct YYnum num;
    struct YYstr str;

    struct astnode *astnode_p; /* abstract syntax node pointer */
}

/*  Defining the token names (and order) which will be used by both 
    the lexer and the parser. For readability, over multiple lines.  */
%token <str> IDENT CHARLIT STRING 
%token <num> NUMBER 
%token <simple_int> INDSEL PLUSPLUS MINUSMINUS SHL 
%token <simple_int> SHR LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR ELLIPSIS TIMESEQ 
%token <simple_int> DIVEQ MODEQ PLUSEQ MINUSEQ SHLEQ SHREQ ANDEQ OREQ XOREQ
%token <simple_int> AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE
%token <simple_int> ENUM EXTERN FLOAT FOR GOTO IF INLINE INT LONG REGISTER
%token <simple_int> RESTRICT RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF
%token <simple_int> UNION UNSIGNED VOID VOLATILE WHILE _BOOL _COMPLEX _IMAGINARY

%type <astnode_p> stmt expr

%%
/* grammars and actions */
stmt: IDENT '=' expr    {   $$ = newNode_binop('=');
                            $$->binop.left = newNode_str(IDENT, $1);
                            $$->binop.right = $3;
                            printAST($$, NULL); }
    ;

expr: NUMBER            { $$ = newNode_num(NUMBER, $1); }
    | expr '+' NUMBER   { $$ = newNode_binop('+');
                          $$->binop.left = $1;
                          $$->binop.right = newNode_num(NUMBER, $3); }
    ;

%%

//  printf("%d\n", 2);
// {$$ += yylval.num.val;}