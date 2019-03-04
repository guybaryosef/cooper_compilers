/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 *
 * parser.y - The Bison file, which will
 * build the parser.
 */


/* definitions and environment setup */
%{
    #include "../front_end_header.h"
    #include "symbol_table.h"
    #include "pheader_ast.h"
%}

/* Specify bison header file of token and YYSTYPE definitions.  */
/* This will be used by both the lexer and parser.              */
%defines "../lexer/lheader.h"

%union {
    int simple_int;

    struct YYnum num;
    struct YYstr str;

    struct astnode *astnode_p;      /* abstract syntax tree (AST) node pointer */
    astnode_list astnode_pp;    /* pointer to an array of AST node pointers*/

    enum possibleTypeQualifiers possible_type_qualifier;
    enum SymbolTableStorageClass storage_class;
    enum STEntry_Type ident_type;
}

/* getting some quality error handling up in here */
%error-verbose 
%locations      /* bison adds location code */



/***************************** TOKEN NAMES *****************************/
/* Used by both the lexer and the parser. For readability, over multiple lines. */
%token <str> IDENT CHARLIT STRING 
%token <num> NUMBER 
%token <simple_int> INDSEL PLUSPLUS MINUSMINUS SHL 
%token <simple_int> SHR LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR ELLIPSIS TIMESEQ 
%token <simple_int> DIVEQ MODEQ PLUSEQ MINUSEQ SHLEQ SHREQ ANDEQ OREQ XOREQ
%token <simple_int> AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE
%token <simple_int> ENUM EXTERN FLOAT FOR GOTO IF INLINE INT LONG REGISTER
%token <simple_int> RESTRICT RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF
%token <simple_int> UNION UNSIGNED VOID VOLATILE WHILE _BOOL _COMPLEX _IMAGINARY


/**************************** EXPRESSION GRAMMAR-TYPES ****************************/
%type <astnode_p> comma-expr expr
%type <astnode_p> conditional-expr
%type <astnode_p> logical-or-expr logical-and-expr
%type <astnode_p> mult-expr add-expr shift-expr relational-expr equality-expr bitwise-and-expr bitwise-xor-expr bitwise-or-expr
%type <astnode_p> sizeof-expr unary-minus-expr unary-plus-expr logical-neg-expr bitwise-neg-expr address-expr indirection-expr preinc-expr predec-expr
%type <astnode_p> unary-expr
%type <astnode_p> cast-expr type-name   /* need to get rid of this type-name after actually implemented */
%type <astnode_p> expr-list assignment-expr
%type <astnode_p> direct-comp-sel indirect-comp-sel
%type <astnode_p> postfix-expr subscript-expr component-sel-expr function-call postinc-expr postdec-expr
%type <astnode_p> primary-expr


/************************** TYPES GRAMMAR-TYPES (hehe) **************************/
%type <storage_class> storage-class-specifier 
%type <possible_type_qualifier> type-qualifier
%type <astnode_p> enum-type-specifier float-type-specifier int-type-specifier struct-type-specifier typedef-type-specifier union-type-specifier void-type-specifier

%type <astnode_p> signed-type-specifier unsigned-type-specifier character-type-specifier bool-type-specifier complex-type-specifier imag-type-specifier

%type <astnode_p> struct-type-definition struct-type-reference

%type <str> struct-tag

%type <ident_type> type-specifier

%type <simple_int> fnc-specifier
%type <astnode_p> simple-declarator


%type <astnode_p> pointer-declarator direct-declarator fnc-declarator array-declarator
%type <astnode_p> init-decl declarator /* initializer- technically here, but not integrated for now */
%type <TmpSymbolTableEntry *> decl-specifiers /* pretty sure about this one */

%type <astnode_pp> decl-init-list 
%type <st_entry> declaration


/*************************** TOP-LEVEL GRAMMAR-TYPES ****************************/
%type <astnode_p> compound-stmt stmt decl-or-stmt decl-or-stmt-list  
%start decl-or-stmt-list    /* last but most significant :) */


%%
/* grammars and actions */

/**********************************************************************
***************************** EXPRESSIONS *****************************
**********************************************************************/
/* primary expressions */
primary-expr: IDENT             { $$ = newNode_str(IDENT, $1);   }
            | CHARLIT           { $$ = newNode_str(CHARLIT, $1); }
            | NUMBER            { $$ = newNode_num($1);  }
            | STRING            { $$ = newNode_str(STRING, $1);  }
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
                                                $$ = newNode_unop('*');
                                                $$->unop.expr = newNode_binop('+');
                                                $$->unop.expr->binop.left = $1;
                                                $$->unop.expr->binop.right = $3;
                                            }
              ;

component-sel-expr: direct-comp-sel     { $$ = $1; }
                  | indirect-comp-sel   { $$ = $1; }
                  ;

direct-comp-sel: postfix-expr '.' IDENT         { 
                                                    $$ = newNode_slct();
                                                    $$->slct.left = $1;
                                                    $$->slct.right = newNode_str(IDENT, $3);
                                                }
               ;

indirect-comp-sel: postfix-expr INDSEL IDENT    { 
                                                    $$ = newNode_slct();
                                                    $$->slct.left = newNode_unop('*');
                                                    $$->slct.left->unop.expr = $1;                                                            
                                                    $$->slct.right = newNode_str(IDENT, $3);
                                                }
                 ;

/* function calls */
function-call: postfix-expr '(' expr-list ')'   { 
                                                    $$ = newNode_fnc();
                                                    $$->fnc.ident = $1;
                                                    $$->fnc.arguments = $3->arglist.list;
                                                    $$->fnc.arg_count = $3->arglist.size;
                                                    free($3);
                                                }
             ;

expr-list: assignment-expr                  { 
                                                $$ = newNode_arglist();
                                                expand_arglist($$);
                                                $$->arglist.list[0] = newNode_arg(1);
                                                $$->arglist.list[0]->arg.expr = $1;
                                                ++($$->arglist.size);
                                            }
         | expr-list ',' assignment-expr    { 
                                                $$ = $1;
                                                expand_arglist($$);
                                                $$->arglist.list[$$->arglist.size] = newNode_arg($$->arglist.size+1);
                                                $$->arglist.list[$$->arglist.size++]->arg.expr = $3;
                                            }
         ;

postinc-expr: postfix-expr PLUSPLUS     {
                                            $$ = newNode_unop(PLUSPLUS);
                                            $$->unop.expr = $1;
                                        }
            ;

postdec-expr: postfix-expr MINUSMINUS   {
                                            $$ = newNode_unop(MINUSMINUS);
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


sizeof-expr: SIZEOF '(' type-name ')'   {   /* type-name will be included as abstract type grammar */
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
                                            $$ = newNode_unop('*');
                                            $$->unop.expr = $2;
                                        }
                ;

preinc-expr: PLUSPLUS unary-expr        { /* equivalent to unary-expr = unary-expr + 1 */
                                            $$ = newNode_assment('=');
                                            $$->assignment.left = $2;
                                            $$->assignment.right = newNode_binop('+');
                                            $$->assignment.right->binop.left = $2;

                                            struct YYnum tmp;
                                            tmp.val = 1;
                                            tmp.d_val = 0;
                                            tmp.types = NUMMASK_INTGR;

                                            $$->assignment.right->binop.right = newNode_num(tmp);
                                        } 
           ;

predec-expr: MINUSMINUS unary-expr      { /* equivalent to unary-expr = unary-expr - 1 */
                                            $$ = newNode_assment('=');
                                            $$->assignment.left = $2;
                                            $$->assignment.right = newNode_binop('-');
                                            $$->assignment.right->binop.left = $2;

                                            struct YYnum tmp;
                                            tmp.val = 1;
                                            tmp.d_val = 0;
                                            tmp.types = NUMMASK_INTGR;

                                            $$->assignment.right->binop.right = newNode_num(tmp);
                                        } 
           ;


/* Binary Operator Expressions */
mult-expr: cast-expr                    { $$ = $1; }
         | mult-expr '*' cast-expr      { $$ = newNode_binop('*'); $$->binop.left = $1; $$->binop.right = $3; }
         | mult-expr '/' cast-expr      { $$ = newNode_binop('/'); $$->binop.left = $1; $$->binop.right = $3; }
         | mult-expr '%' cast-expr      { $$ = newNode_binop('%'); $$->binop.left = $1; $$->binop.right = $3; }
         ;

add-expr: mult-expr                     { $$ = $1; }
        | add-expr '+' mult-expr        { $$ = newNode_binop('+'); $$->binop.left = $1; $$->binop.right = $3; }
        | add-expr '-' mult-expr        { $$ = newNode_binop('-'); $$->binop.left = $1; $$->binop.right = $3; }
        ;

shift-expr: add-expr                    { $$ = $1; }
          | shift-expr SHL add-expr     { $$ = newNode_binop(SHL); $$->binop.left = $1; $$->binop.right = $3; }
          | shift-expr SHR add-expr     { $$ = newNode_binop(SHR); $$->binop.left = $1; $$->binop.right = $3; }
          ;

relational-expr: shift-expr                         { $$ = $1; }
               | relational-expr '<' shift-expr     { $$ = newNode_binop('<'); $$->binop.left = $1; $$->binop.right = $3; }
               | relational-expr '>' shift-expr     { $$ = newNode_binop('>'); $$->binop.left = $1; $$->binop.right = $3; }
               | relational-expr LTEQ shift-expr    { $$ = newNode_binop(LTEQ); $$->binop.left = $1; $$->binop.right = $3; }
               | relational-expr GTEQ shift-expr    { $$ = newNode_binop(GTEQ); $$->binop.left = $1; $$->binop.right = $3; }
               ;

equality-expr: relational-expr                      { $$ = $1; }
             | equality-expr EQEQ relational-expr   { $$ = newNode_binop(EQEQ); $$->binop.left = $1; $$->binop.right = $3; }
             | equality-expr NOTEQ relational-expr  { $$ = newNode_binop(NOTEQ); $$->binop.left = $1; $$->binop.right = $3; }
             ;

bitwise-and-expr: equality-expr                         { $$ = $1; }
                | bitwise-and-expr '&' equality-expr    { $$ = newNode_binop('&'); $$->binop.left = $1; $$->binop.right = $3; }
                ;

bitwise-or-expr: bitwise-xor-expr                       { $$ = $1; }
               | bitwise-or-expr '|' bitwise-xor-expr   { $$ = newNode_binop('|'); $$->binop.left = $1; $$->binop.right = $3; }
               ;

bitwise-xor-expr: bitwise-and-expr                         { $$ = $1; }
                | bitwise-xor-expr '^' bitwise-and-expr    { $$ = newNode_binop('^'); $$->binop.left = $1; $$->binop.right = $3; }
                ;


/* Logical Operator Expressions */
logical-or-expr: logical-and-expr                           { $$ = $1; }
               | logical-or-expr LOGOR logical-and-expr     { $$ = newNode_binop(LOGOR); $$->binop.left = $1; $$->binop.right = $3; }
               ;

logical-and-expr: bitwise-or-expr                           { $$ = $1; }
                | logical-and-expr LOGAND bitwise-or-expr   { $$ = newNode_binop(LOGAND); $$->binop.left = $1; $$->binop.right = $3; }
                ;


/* Conditional Expressions */
conditional-expr: logical-or-expr                               { $$ = $1; }
                | logical-or-expr '?' expr ':' conditional-expr {   
                                                                    $$ = newNode_ternary();
                                                                    $$->ternary.if_expr = $1;
                                                                    $$->ternary.then_expr = $3;
                                                                    $$->ternary.else_expr = $5;     
                                                                }
                ;


/* Assignment Operator */
assignment-expr: conditional-expr                   { $$ = $1; }
               | unary-expr '=' assignment-expr     { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = $3;
                                                    }
               | unary-expr PLUSEQ assignment-expr  { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop('+');
                                                        ($$->assignment.right)->binop.left = $1; 
                                                        ($$->assignment.right)->binop.right = $3; 
                                                    }
               | unary-expr MINUSEQ assignment-expr { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop('-');
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               | unary-expr TIMESEQ assignment-expr { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop('*');
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               | unary-expr DIVEQ assignment-expr   { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop('/');
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               | unary-expr MODEQ assignment-expr   { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop('%');
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               | unary-expr SHLEQ assignment-expr   { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop(SHL);
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               | unary-expr SHREQ assignment-expr   { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop(SHR);
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               | unary-expr ANDEQ assignment-expr   { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop('&');
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               | unary-expr XOREQ assignment-expr   { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop('^');
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               | unary-expr OREQ assignment-expr    { 
                                                        $$ = newNode_assment('='); 
                                                        $$->assignment.left = $1; 
                                                        $$->assignment.right = newNode_binop('|');
                                                        $$->assignment.right->binop.left = $1; 
                                                        $$->assignment.right->binop.right = $3; 
                                                    }
               ;


/* Sequential Expressions */
comma-expr: assignment-expr                     { $$ = $1; }
          | comma-expr ',' assignment-expr      { $$ = newNode_binop(','); $$->binop.left = $1; $$->binop.right = $3; }
          ;

expr: comma-expr { $$ = $1; }
    ;


stmt: expr ';'              { /* NOTHING */ }
    | compound-stmt         { /* NOTHING */ }
    | error ';'             { if (error_count > 10) exit(-1); }
    ;


/**********************************************************************
**************************** DECLARATIONS *****************************
**********************************************************************/

type-name:   { $$ = NULL; }
         ;


declaration: decl-specifiers ';'    { 
                                        /* valid but does NOTHING */ 
                                    }
           | decl-specifiers decl-init-list ';' {

                                        if (!isTmpSTableEntryValid($1)) {
                                            error("Error in declaration specifiers.");
                                        }
                                        else {
                                            for (int i = 0; i < $2->len; ++i) {
                                                /* create new symbol table entry */
                                                astnode *new_node = newNode_sTableEntry($1);
                                                new_node->stable_entry.ident = 
                                                    $2->list[i]->stable_entry.ident;
                                                switch(new_node->stable_entry.type) {
                                                    case VARIABLE_TYPE:
                                                    break;
                                                    case FUNCTION_TYPE:
                                                        break;
                                                    case SU_TAG_TYPE:
                                                        break;
                                                    case ENUM_TAG:
                                                        break;
                                                    case STATEMENT_LABEL:
                                                        break;
                                                    case ENUM_CONST_TYPE:
                                                        break;
                                                    case TYPEDEF_NAME:
                                                        break;
                                                    case SU_MEMBER_TYPE:
                                                        break;
                                                }
                                                new_node->stable_entry.node = 
                                                    $2->list[i]->stable_entry.node;
                                                
                                                /* add it to the symbol table */
                                                int ns_ind;
                                                if (new_node->stable_entry.type == STATEMENT_LABEL)
                                                    ns_ind = 1;  /* statment labels        */
                                                else if (new_node->stable_entry.type == ENUM_TAG ||
                                                         new_node->stable_entry.type == SU_TAG_TYPE)
                                                    ns_ind = 2;  /* tags (idents of struct/union/enum) */
                                                else if (new_node->stable_entry.type == SU_MEMBER_TYPE)
                                                    ns_ind = 3;  /* struct/union members */
                                                else
                                                    ns_ind = 4;  /* all other identifier classes */

                                                if(sTableInsert(scope_stack->innermost_scope[ns_ind], 
                                                                astnode *new_node) < 0)
                                                    error("Unable to insert variable into symbol table");
                                                
                                                free($2->list[i]);
                                            }
                                        }
                                        
                                        free($2);
                                        free($1);
                                    }
           ;

decl-specifiers: storage-class-specifier        { 
                                                    $$ = createTmpSTableEntry();
                                                    $$.var_fnc_storage_class = $1;
                                                }
               | storage-class-specifier decl-specifiers    { 
                                                    $$ = $2;
                                                    if ($$.var_fnc_storage_class)
                                                        error("Can't have multiple storage classes per declaration specifiers");
                                                    else
                                                        $$.var_fnc_storage_class = $1;
                                                }
               | type-specifier                 {
                                                    $$ = createTmpSTableEntry();
                                                    $$.type = $1;                                                                
                                                }
               | type-specifier decl-specifiers {
                                                    $$ = $2;
                                                    if ($$.type)
                                                        error("Can't have multiple storage classes per declaration specifiers");
                                                    else
                                                        $$.type = $1;                                                                
                                                }
               | type-qualifier                 {   
                                                    $$ = createTmpSTableEntry();
                                                    typeQualifierSTableEntry($$, $1);         
                                                }
               | type-qualifier decl-specifiers {   
                                                    $$ = $2;
                                                    typeQualifierSTableEntry($$, $1);         
                                                }
               | fnc-specifier                  {
                                                    $$ = createTmpSTableEntry();
                                                    $$.fnc_is_inline = 1;
                                                }               
               | fnc-specifier decl-specifiers  {
                                                    $$ = $2;
                                                    $$.fnc_is_inline = 1;
                                                }   
               ;

type-specifier: enum-type-specifier     { $$ = $1; }
              | float-type-specifier    { $$ = $1; }
              | int-type-specifier      { $$ = $1; }
              | struct-type-specifier   { $$ = $1; }
              | typedef-type-specifier  { $$ = $1; } 
              | union-type-specifier    { $$ = $1; }
              | void-type-specifier     { $$ = $1; }
              ;

int-type-specifier: signed-type-specifier       { $$ = $1; }
                  | unsigned-type-specifier     { $$ = $1; }
                  | character-type-specifier    { $$ = $1; }
                  | bool-type-specifier         { $$ = $1; }
                  ;

signed-type-specifier: SHORT            { $$ = newNode_scalarType(Short, 1);  }
                     | SHORT INT        { $$ = newNode_scalarType(Short, 1);  }
                     | SIGNED SHORT     { $$ = newNode_scalarType(Short, 1);  }
                     | SIGNED SHORT INT { $$ = newNode_scalarType(Short, 1);  }
                     | INT              { $$ = newNode_scalarType(Int, 1);    }
                     | SIGNED INT       { $$ = newNode_scalarType(Int, 1);    }
                     | SIGNED           { $$ = newNode_scalarType(Int, 1);    }
                     | LONG             { $$ = newNode_scalarType(Long, 1);   } 
                     | LONG INT         { $$ = newNode_scalarType(Long, 1);   }
                     | SIGNED LONG      { $$ = newNode_scalarType(Long, 1);   }
                     | SIGNED LONG INT  { $$ = newNode_scalarType(Long, 1);   }
                     | LONG LONG            { $$ = newNode_scalarType(LongLong, 1);}
                     | LONG LONG INT        { $$ = newNode_scalarType(LongLong, 1);}
                     | SIGNED LONG LONG     { $$ = newNode_scalarType(LongLong, 1);}
                     | SIGNED LONG LONG INT { $$ = newNode_scalarType(LongLong, 1);}
                     ;

unsigned-type-specifier: UNSIGNED SHORT        { $$ = newNode_scalarType(Short, 0);   }
                       | UNSIGNED SHORT INT    { $$ = newNode_scalarType(Short, 0);   }
                       | UNSIGNED              { $$ = newNode_scalarType(Int, 0);     }
                       | UNSIGNED INT          { $$ = newNode_scalarType(Int, 0);     }
                       | UNSIGNED LONG         { $$ = newNode_scalarType(Long, 0);    }
                       | UNSIGNED LONG INT     { $$ = newNode_scalarType(Long, 0);    }
                       | UNSIGNED LONG LONG    { $$ = newNode_scalarType(LongLong, 0);}
                       | UNSIGND LONG LONG INT { $$ = newNode_scalarType(LongLong, 0);}
                       ;

/* a plain 'char' was chosen to be an 'unsigned char' */
character-type-specifier: CHAR              { $$ = newNode_scalarType(Char, 0);}
                        | SIGNED CHAR       { $$ = newNode_scalarType(LongLong, 1);}
                        | UNSIGNED CHAR     { $$ = newNode_scalarType(LongLong, 1);}
                        ;

bool-type-specifier: _BOOL { $$ = newNode_scalarType(Bool, 0); }
                   ;

float-type-specifier: FLOAT                  { $$ = newNode_scalarType(Float, 0);       }
                    | DOUBLE                 { $$ = newNode_scalarType(Double, 0);      }
                    | LONG DOUBLE            { $$ = newNode_scalarType(LongDouble, 0);  }
                    | complex-type-specifier { $$ = $1; }
                    | imag-type-specifier    { $$ = $1; }
                    ;

complex-type-specifier: FLOAT _COMPLEX       { $$ = newNode_scalarType(FloatComplex, 0);     }
                      | DOUBLE _COMPLEX      { $$ = newNode_scalarType(DoubleComplex, 0);    }
                      | LONG DOUBLE _COMPLEX { $$ = newNode_scalarType(LongDoubleComplex, 0);}
                      ;

imag-type-specifier: FLOAT _IMAGINARY       { $$ = newNode_scalarType(FloatImag, 0);     }
                   | DOUBLE _IMAGINARY      { $$ = newNode_scalarType(DoubleImag, 0);    }
                   | LONG DOUBLE _IMAGINARY { $$ = newNode_scalarType(LongDoubleImag, 0);}
                   ;

/* for now not implementing enums, will be completed with more time */
enum-type-specifier: ENUM   { $$ = newNode_scalarType(Int, 0); }
                   ;

struct-type-specifier: struct-type-definition   { $$ = $1; }
                     | struct type-reference    { $$ = $1; }
                     ;

struct-type-definition: STRUCT '{' field-list '}'
                      | STRUCT struct-tag '{' field-list '}'
                      ;

struct-type-reference: STRUCT struct-tag   { $$ = searchStackScope(3, $20>str); }
                     ;

struct-tag: IDENT   { $$ = $1; }
          ;

field-list: component-declaration
          | field-list component-declaration
          ;

component-declaration: type-specifier component-declarator-list ';'
                     ;

component-declarator-list: component-declarator
                         | component-declarator-list ',' component-declarator
                         ;

component-declarator: declarator
                    /* we will not be implementing bit-fields in struct definitions */
                    ;



/* due to the possibility for a list of declarators, we implement the 
   decl-init-list nontoken as pointer to a pointer to an astnode_p.  */
decl-init-list: init-decl       { $$ = newASTnodeList(1, NULL); $$->list[0] = $1; }  
              | decl-init-list ',' init-decl    {
                                                    $$ = newASTnodeList($1->len+1, $1);
                                                    $$->list[$$->len-1] = $3;
                                                }
              ; 

init-decl: declarator                   { $$ = $1; }
         /* For simplicity we will not implement initialized declerations. */  
         ;

storage-class-specifier: AUTO           { $$ = Auto;        }
                       | EXTERN         { $$ = Extern;      }
                       | REGISTER       { $$ = Register;    }
                       | STATIC         { $$ = Static;      }
                       | TYPEDEF        { $$ = Typedef;     }
                       ;



fnc-specifier: INLINE  { $$ = 1; }
             ;


type-qualifier: CONST           { $$ = Const;    }
              | VOLATILE        { $$ = Volatile; }
              | RESTRICT        { $$ = Restrict; }
              ;


declarator: pointer-declarator  { $$ = $1; }
          | direct-declarator   { $$ = $1; }
          ;


pointer-declarator: pointer direct-declarator
                  ;

pointer: '*'                        { /* what do we do? */ }
       | '*' type-qualifier-list    { /* what do we do? */ }
       | '*' pointer                        { /* what do we do? */ }
       | '*' type-qualifier-list pointer    { 
                                                $$ = $1;
                                                $$.node = newNode_ptr();
                                            }
       ;

type-qualifier-list: type-qualifier                     {   
                                                            $$ = createTmpSTableEntry();
                                                            typeQualifierSTableEntry($$, $1);         
                                                        }
                   | type-qualifier-list type-qualifier {   
                                                            $$ = $2;
                                                            typeQualifierSTableEntry($$, $1);         
                                                        }
                   ;

direct-declarator: simple-declarator    { $$ = $1; }
                 | '(' declarator ')'   { $$ = $2; }
                 | fnc-declarator       { $$ = $1; }
                 | array-declarator     { $$ = $1; }
                 ;


simple-declarator: IDENT    { $$ = $1; } /* need to figure out what to do */
                 ;

array-declarator: direct-declarator '[' ']'         {
                                                        $$ = newNode_arr(-1);
                                                        $$
                                                    }
                | direct-declarator '[' NUMBER ']'
                /* for now only allow these type of array declarations. We will 
                   also simplify by not implementing variable-length arrays. */
                ;

fnc-declarator: direct-declarator '(' ')'
              /* simpilify function parameters to only include (). */
              ;



/**********************************************************************
************************** TOP-LEVEL GRAMMAR **************************
**********************************************************************/


decl-or-stmt: declaration   { $$ = $1; }
            | stmt          { $$ = $1; }
            ;


decl-or-stmt-list: /* empty */                      { /* NOTHING */ }
                 | decl-or-stmt-list decl-or-stmt   { /* NOTHING */ }
                 ;


compound-stmt: '{' decl-or-stmt-list '}'    { /* NOTHING */ }
             ;



%%
