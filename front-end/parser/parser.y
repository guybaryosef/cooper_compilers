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

%type <astnode_p> expr
%type <astnode_p> primary-expr
%type <astnode_p> postfix-expr subscript-expr component-sel-expr function-call postinc-expr postdec-expr
%type <astnode_p> direct-comp-sel indirect-comp-sel
%type <astnode_p> expr-list assignment-expr

%type <astnode_p> unary-expr
%type <astnode_p> cast-expr type-name
%type <astnode_p> sizeof-expr unary-minus-expr unary-plus-expr logical-neg-expr bitwise-neg-expr address-expr indirection-expr preinc-expr predec-expr

%type <astnode_p> binary-expr
%type <astnode_p> mult-expr add-expr shift-expr relational-expr equality-expr bitwise-and-expr bitwise-xor-expr bitwise-or-expr

%%
/* grammars and actions */
expr: expr expr                 /* nothing (lets multiple rules through) */
    | primary-expr ';'          /* nothing */
    | postfix-expr ';'          /* nothing */
    | unary-expr ';'            /* nothing */
    | binary-expr ';'           /* nothing */
    ;


/* primary expressions */
primary-expr: IDENT             { $$ = newNode_str(IDENT, $1); }
            | CHARLIT           { $$ = newNode_str(CHARLIT, $1); }
            | STRING            { $$ = newNode_str(STRING, $1); }
            | '(' expr ')'      { $$ = $2; }
            ; 


/* postfix expressions */
postfix-expr: primary-expr          { $$ = $1; }               
            | subscript-expr        { $$ = $1; }    
            | component-sel-expr    { $$ = $1; }      
            | function-call         { $$ = $1; }
            | postinc-expr          { $$ = $1; }        
            | postdec-expr          { $$ = $1; } 
            ;   /* for now I skipped compound-literals */

subscript-expr: postfix-expr '[' expr ']'   { 
                                                /* TODO: equivalent to *(e1 + e2) */
                                            }
              ;

component-sel-expr: direct-comp-sel     { $$ = $1; }
                  | indirect-comp-sel   { $$ = $1; }
                  ;

direct-comp-sel: postfix-expr '.' IDENT         { 
                                                    $$ = newNode_slct();
                                                    $$->slct.left = $1;
                                                    $$->slct.right = $3;
                                                }
               ;

indirect-comp-sel: postfix-expr INDSEL IDENT    { 
                                                    $$ = newNode_slct();
                                                    /* TODO: equivalent to (*x).m */                                                            
                                                    $$->slct.right = $3;
                                                }
                 ;

/* function calls */
function-call: postfix-expr '(' expr-list ')'   { 
                                                    $$ = newNode_fnc();
                                                    $$->fnc.ident = $1;
                                                    $$->fnc.arguments = $3->arglist.list;
                                                    $$->func.arg_count =$3->arglist.size;
                                                    free($3);
                                                }
             ;

expr-list: assignment-expr                  { 
                                                $$ = newNode_arglist();
                                                expand_arglist($$);
                                                $$->arglist.list[0] = newNode_arg(1);
                                                $$->arglist.list[0].expr = $1;
                                                ++$$->arglist.size;
                                            }
         | expr-list ',' assignment-expr    { 
                                                $$ = $1;
                                                expand_arglist($$);
                                                $$->arglist.list[$$->arglist.size] = newNode_arg($$->arglist.size+1);
                                                $$->arglist.list[$$->arglist.size++].expr = $3;
                                            }
         ;

postinc-expr: postfix-expr PLUSPLUS     {
                                            $$ = newNode_unop("++");
                                            $$->unop.expr = $1;
                                        }
            ;

postdec-expr: postfix-expr MINUSMINUS   {
                                            $$ = newNode_unop("--");
                                            $$->unop.expr = $1;
                                        }
            ;


/* unary expressions */
cast-expr: unary-expr                   { $$ = $1; }
         | '(' type-name ')' cast-expr  { $$ = $4; } /* NOT CORRECT, NEEDS WORK */
         ;

unary-expr: postfix-expr        { $$ = $1; }
          | sizeof-expr         { $$ = $1; }
          | unary-minus-expr    { $$ = $1; }
          | unary-plus-expr     { $$ = $1; }
          | logical-neg-expr    { $$ = $1; }
          | bitwise-neg-expr    { $$ = $1; }
          | address-expr        { $$ = $1; }
          | indirection-expr    { $$ = $1; }
          | preinc-expr         { $$ = $1; }
          | predec-expr         { $$ = $1; }
          ;

type-name: INT          { $$ = newNode_type(INT);       }
         | LONG         { $$ = newNode_type(LONG);      }
         | UNSIGNED     { $$ = newNode_type(UNSIGNED);  }
         | CHAR         { $$ = newNode_type(CHAR);      }
         | CONST        { $$ = newNode_type(CONST);     }
         | DOUBLE       { $$ = newNode_type(DOUBLE);    }
         | FLOAT        { $$ = newNode_type(FLOAT);     }
         | STATIC       { $$ = newNode_type(STATIC);    }
         | SHORT        { $$ = newNode_type(SHORT);     }
         ;

sizeof-expr: SIZEOF '(' type-name ')'   { 
                                            $$ = newNode_unop(SIZEOF);
                                            $$->unop.expr = $3; 
                                        }
           | SIZEOF unary-expr          { 
                                            $$ = newNode_unop(SIZEOF);
                                            $$->unop.expr = $2; 
                                        }
           ;

unary-minus-expr: '-' cast-expr         {
                                            $$ = newNode_unop('-');
                                            $$->unop.expr = $2;
                                        }
                ;

unary-plus-expr: '+' cast-expr          {
                                            $$ = newNode_unop('+');
                                            $$->unop.expr = $2;
                                        }
               ;

logical-neg-expr: '!' cast-expr         {
                                            $$ = newNode_unop('!');
                                            $$->unop.expr = $2;
                                        }
                ;

bitwise-neg-expr: '~' cast-expr         {
                                            $$ = newNode_unop('~');
                                            $$->unop.expr = $2;                                            
                                        }
                ;

address-expr: '&' cast-expr             {
                                            $$ = newNode_unop('&');
                                            $$->unop.expr = $2;   
                                        }
            ;

indirection-expr: '*' cast-expr         {
                                            $$ = newNode_upnop('*');
                                            $$->unop.expr = $2;
                                        }
                ;

preinc-expr: PLUSPLUS unary-expr        { /* equivalent to unary-expr = unary-expr + 1 */
                                            $$ = newNode_binop('+');
                                            $$->binop.left = $2;

                                            struct YYnum tmp;
                                            tmp.val = 1;
                                            tmp.d_val = 0;
                                            tmp.types = NUMMASK_INTGR;

                                            $$->binop.right = newNode_num(tmp);
                                        } 
           ;

predec-expr: MINUSMINUS unary-expr      { /* equivalent to unary-expr = unary-expr - 1 */
                                            $$ = newNode_binop('-');
                                            $$->binop.left = $2;

                                            struct YYnum tmp;
                                            tmp.val = 1;
                                            tmp.d_val = 0;
                                            tmp.types = NUMMASK_INTGR;

                                            $$->binop.right = newNode_num(tmp);
                                        } 
           ;


/* Binary Operator Expressions */
binary-expr: mult-expr                  { $$ = $1; }  
           | add-expr                   { $$ = $1; }
           | shift-expr                 { $$ = $1; }
           | relational-expr            { $$ = $1; }  
           | equality-expr              { $$ = $1; }
           | bitwise-and-expr           { $$ = $1; }  
           | bitwise-xor-expr           { $$ = $1; }  
           | bitwise-or-expr            { $$ = $1; }
           ;  




%%

    /* my example of messing with grammers */
// stmt: stmt stmt           /* nothing */
//     | IDENT '=' expr ';'    {   $$ = newNode_binop('=');
//                                 $$->binop.left = newNode_str(IDENT, $1);
//                                 $$->binop.right = $3;
//                                 printAST($$, NULL);
//                                 freeTree($$);
//                             }
//     ;

// expr: NUMBER            { $$ = newNode_num(NUMBER, $1); }
//     | expr '+' NUMBER   { $$ = newNode_binop('+');
//                           $$->binop.left = $1;
//                           $$->binop.right = newNode_num(NUMBER, $3); }
//     ;