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
    #include "./symbol_table.h"
    #include "./pheader_ast.h"
%}

/* Specify bison header file of token and YYSTYPE definitions.  */
/* This will be used by both the lexer and parser.              */
%defines "../lexer/lheader.h"

%union {
    int simple_int;

    struct YYnum num;
    struct YYstr str;

    astnode *astnode_p;   /* abstract syntax tree (AST) node pointer */
    astnode_list *astnode_pp;    /* pointer to an array of AST node pointers*/

    enum possibleTypeQualifiers possible_type_qualifier;
    enum SymbolTableStorageClass storage_class;
    enum STEntry_Type ident_type;
    TmpSymbolTableEntry *tmp_stable_entry;
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

%type <astnode_p> signed-type-specifier unsigned-type-specifier character-type-specifier bool-type-specifier 
%type <astnode_p> complex-type-specifier imag-type-specifier

%type <simple_int> prev-specifiers unsigned-intermediate signed-intermediate 
%type <simple_int> type-intermediate


%type <astnode_p> struct-type-definition struct-type-reference
%type <str> struct-tag
%type <astnode_pp> field-list member-declaration member-declarator-list
%type <astnode_p> member-declarator


%type <astnode_p> type-specifier

%type <simple_int> fnc-specifier
%type <astnode_p> simple-declarator


%type <astnode_p> pointer-declarator pointer direct-declarator fnc-declarator array-declarator
%type <astnode_p> init-decl declarator /* initializer- technically here, but not integrated for now */
%type <tmp_stable_entry> type-qualifier-list decl-specifiers /* pretty sure about this one */

%type <astnode_pp> decl-init-list 
%type <astnode_pp> declaration


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
                        yyerror("Error in declaration specifiers.");
                    }
                    else {
                        /* create the new symbol table entries */
                        $$ = combineSpecifierDeclarator($1, $2); 

                        /* add the new entries to the symbol table */
                        int ns_ind;
                        for (int i = 0 ; i < $$->len; ++i) {
                            if ($$->list[i]->stable_entry.type == STATEMENT_LABEL)
                                ns_ind = 1;  /* statment labels        */
                            else if ($$->list[i]->stable_entry.type == ENUM_TAG ||
                                     $$->list[i]->stable_entry.type == SU_TAG_TYPE)
                                ns_ind = 2;  /* tags (idents of struct/union/enum) */
                            else if ($$->list[i]->stable_entry.type == SU_MEMBER_TYPE)
                                ns_ind = 3;  /* struct/union members */
                            else
                                ns_ind = 4;  /* all other identifier classes */

                            if(sTableInsert(scope_stack.innermost_scope->tables[ns_ind], $$->list[i], 1) < 0)
                                yyerror("Unable to insert variable into symbol table");
                        }
                    }
                }
           ;

decl-specifiers: storage-class-specifier { 
                        $$ = createTmpSTableEntry();
                        $$->var_fnc_storage_class = $1;
                    }
               | storage-class-specifier decl-specifiers { 
                        $$ = $2;
                        if ($$->var_fnc_storage_class)
                            yyerror("Can't have multiple storage classes per declaration specifiers");
                        else
                            $$->var_fnc_storage_class = $1;
                    }
               | type-specifier {
                        $$ = createTmpSTableEntry();
                        $$->node = $1;                                                                
                    }
               | type-specifier decl-specifiers {
                        $$ = $2;
                        if ($$->node)
                            yyerror("Can't have multiple type specifiers for a declaration specifiers");
                        else
                            $$->node = $1;                                                                
                    }
               | type-qualifier {   
                        $$ = createTmpSTableEntry();
                        typeQualifierSTableEntry($$, $1);         
                    }
               | type-qualifier decl-specifiers {   
                        $$ = $2;
                        typeQualifierSTableEntry($$, $1);         
                    }
               | fnc-specifier      {
                        $$ = createTmpSTableEntry();
                        $$->fnc_is_inline = 1;
                    }               
               | fnc-specifier decl-specifiers  {
                        $$ = $2;
                        $$->fnc_is_inline = 1;
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

signed-type-specifier: signed-intermediate type-intermediate { 
                            switch($2) {
                                case 1:
                                    $$ = newNode_scalarType(Short, 1);
                                    break;
                                case 2:
                                    $$ = newNode_scalarType(Int, 1);
                                    break;
                                case 3:
                                    $$ = newNode_scalarType(Long, 1);
                                    break;
                                case 4:
                                    $$ = newNode_scalarType(LongLong, 1);
                                    break;
                            }  
                        }
                     | type-intermediate { 
                            switch($1) {
                                case 1:
                                    $$ = newNode_scalarType(Short, 1);
                                    break;
                                case 2:
                                    $$ = newNode_scalarType(Int, 1);
                                    break;
                                case 3:
                                    $$ = newNode_scalarType(Long, 1);
                                    break;
                                case 4:
                                    $$ = newNode_scalarType(LongLong, 1);
                                    break;
                            }  
                        }
                     ;

unsigned-intermediate: UNSIGNED { /* NOTHING */ }
                     ;

signed-intermediate: SIGNED     { /* NOTHING */ }
                   ;

/* todo: break these into a more readable and intuitive enum */
prev-specifiers: /* empty */    { $$ = 2; }
               |  SHORT         { $$ = 1; }
               | LONG           { $$ = 3; }
               | LONG LONG      { $$ = 4; }
               ;

type-intermediate: prev-specifiers INT { $$ = $1;}
                 ;

unsigned-type-specifier: unsigned-intermediate type-intermediate { 
                                switch($2) {
                                    case 1:
                                        $$ = newNode_scalarType(Short, 0); 
                                        break;
                                    case 2:
                                        $$ = newNode_scalarType(Int, 0);
                                        break;
                                    case 3:
                                        $$ = newNode_scalarType(Long, 0);
                                        break;
                                    case 4:
                                        $$ = newNode_scalarType(LongLong, 0);
                                        break;
                                }
                            }
                       ;

/* a plain 'char' was chosen to be an 'unsigned char' */
character-type-specifier: CHAR              { $$ = newNode_scalarType(Char, 0);}
                        | signed-intermediate CHAR       { $$ = newNode_scalarType(LongLong, 1);}
                        | unsigned-intermediate CHAR     { $$ = newNode_scalarType(LongLong, 1);}
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

/* for now not implementing enums, typedefs, or union.
   With enough time, will see how many more can be included. */
enum-type-specifier: ENUM        { $$ = newNode_scalarType(Int, 0); }
                   ;

typedef-type-specifier: TYPEDEF  { $$ = newNode_scalarType(Int, 0); } 
                      ;

union-type-specifier: UNION      { $$ = newNode_scalarType(Int, 0); }
                    ;

void-type-specifier: VOID        { $$ = newNode_scalarType(Int, 0); }
                   ;

/* struct-type-specifier is a symbol table entry, not just a astnode type */
struct-type-specifier: struct-type-definition   { $$ = $1; }
                     | struct-type-reference    { $$ = $1; }
                     ;

struct-type-definition: STRUCT '{' field-list '}' {   
                                $$ = newNode_sTableEntry(NULL);
                                $$->nodetype = STABLE_SU_TAG;
                                $$->stable_entry.node = newNode_strctType();

                                for (int i = 0; i < $3->len; ++i)
                                    if (!sTableInsert($$->stable_entry.node->strct.stable, 
                                                      $3->list[i], 1) ) 
                                        yyerror("Inserting members into struct symbol table.");
                            }
                      | STRUCT struct-tag '{' field-list '}' { 
                                $$ = newNode_sTableEntry(NULL);
                                $$->nodetype = STABLE_SU_TAG;
                                $$->stable_entry.node = newNode_strctType();
                                $$->stable_entry.ident = $2.str;

                                for (int i = 0; i < $4->len; ++i)
                                    if (!sTableInsert($$->stable_entry.node->strct.stable, 
                                                      $4->list[i], 1) ) 
                                        yyerror("Inserting members into struct symbol table.");
                            }
                      ;

struct-type-reference: STRUCT struct-tag   { $$ = searchStackScope(3, $2.str); }
                     ;

struct-tag: IDENT   { $$ = $1; }
          ;

field-list: member-declaration              { $$ = $1; }
          | field-list member-declaration   { 
                    $$ = newASTnodeList($1->len + $2->len, $1->list);
                    
                    for (int i = $1->len, k=0 ; i < $$->len ; ++i, ++k)  
                        $$->list[i] = $2->list[k];
                }
          ;

member-declaration: type-specifier member-declarator-list ';' { 
                            TmpSymbolTableEntry *tmp_entry = createTmpSTableEntry();
                            tmp_entry->node = $1;
                            if (!isTmpSTableEntryValid(tmp_entry))
                                yyerror("Invalid struct declaration specifiers.");
                            else
                                $$ = combineSpecifierDeclarator(tmp_entry, $2); 
                        }
                  ;

member-declarator-list: member-declarator { 
                                $$ = newASTnodeList(1, NULL); 
                                $$->list[0] = $1;
                            }
                      | member-declarator-list ',' member-declarator {
                                $$ = newASTnodeList($1->len+1, $1->list);
                                $$->list[$1->len] = $3;
                            }
                      ;

member-declarator: declarator   { $$ = $1; }
                 /* we will not be implementing bit-fields in struct definitions */
                 ;



/* due to the possibility for a list of declarators, we implement the 
   decl-init-list non-token as pointer to a pointer to an astnode_p.  */
decl-init-list: init-decl { 
                        $$ = newASTnodeList(1, NULL); 
                        $$->list[0] = $1;
                    }  
              | decl-init-list ',' init-decl    {
                        $$ = newASTnodeList($1->len+1, $1->list);
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
                                                /* what do we do? */
                                            }
       ;

type-qualifier-list: type-qualifier                     {   
                                                            $$ = createTmpSTableEntry();
                                                            typeQualifierSTableEntry($$, $1);         
                                                        }
                   | type-qualifier-list type-qualifier {   
                                                            $$ = $1;
                                                            typeQualifierSTableEntry($$, $2);         
                                                        }
                   ;

direct-declarator: simple-declarator    { $$ = $1; }
                 | '(' declarator ')'   { $$ = $2; }
                 | fnc-declarator       { $$ = $1; }
                 | array-declarator     { $$ = $1; }
                 ;


simple-declarator: IDENT    { /* no answer yet */ } /* need to figure out what to do */
                 ;

array-declarator: direct-declarator '[' ']'         {
                                                        $$ = newNode_arr(-1);
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


decl-or-stmt: declaration   { /* NOTHING */ }
            | stmt          { /* NOTHING */ }
            ;


decl-or-stmt-list: /* empty */                      { /* NOTHING */ }
                 | decl-or-stmt-list decl-or-stmt   { /* NOTHING */ }
                 ;


compound-stmt: '{' decl-or-stmt-list '}'    { /* NOTHING */ }
             ;



%%
