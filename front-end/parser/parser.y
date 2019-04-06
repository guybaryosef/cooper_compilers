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
    struct ScopeStackLayer *scope_layer;
    struct AstnodeLinkedList *astnode_ll;
    struct astnode_scope_contents *cmpnd_stmt;
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
%token <simple_int> AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE 
%token <simple_int> ENUM EXTERN FLOAT FOR GOTO
%left IF 
%left ELSE  
%token <simple_int> INLINE INT 
%token <simple_int> LONG REGISTER
%token <simple_int> RESTRICT RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF
%token <simple_int> UNION UNSIGNED VOID VOLATILE WHILE _BOOL _COMPLEX _IMAGINARY


/**************************** EXPRESSION GRAMMAR-TYPES ****************************/
%type <astnode_p> comma-expr expr
%type <astnode_p> conditional-expr
%type <astnode_p> logical-or-expr logical-and-expr
%type <astnode_p> mult-expr add-expr shift-expr relational-expr equality-expr bitwise-and-expr bitwise-xor-expr bitwise-or-expr
%type <astnode_p> sizeof-expr unary-minus-expr unary-plus-expr logical-neg-expr bitwise-neg-expr address-expr indirection-expr preinc-expr predec-expr
%type <astnode_p> unary-expr
%type <astnode_p> cast-expr   /* need to get rid of this type-name after actually implemented */
%type <astnode_p> expr-list assignment-expr
%type <astnode_p> direct-comp-sel indirect-comp-sel
%type <astnode_p> postfix-expr subscript-expr component-sel-expr function-call postinc-expr postdec-expr
%type <astnode_p> primary-expr


/**************************** STATEMENT GRAMMAR-TYPES ****************************/
%type <astnode_p> expr-stmt labeled-stmt conditional-stmt iterative-stmt switch-stmt break-stmt continue-stmt return-stmt goto-stmt null-stmt

%type <tmp_stable_entry> label named-label case-label default-label
%type <astnode_p> compound-stmt stmt decl-or-stmt 
%type <astnode_ll> decl-or-stmt-list
%type <astnode_p> if-stmt if-else-stmt 
%type <astnode_p> while-stmt do-stmt for-stmt for-expr
%type <astnode_pp>  initial-clause

/************************** TYPE GRAMMAR-TYPES (hehe) **************************/
%type <storage_class> storage-class-specifier 
%type <possible_type_qualifier> type-qualifier type-qualifier-list
%type <astnode_p> enum-type-specifier float-type-specifier int-type-specifier struct-type-specifier typedef-type-specifier union-type-specifier void-type-specifier


%type <astnode_p> signed-type-specifier unsigned-type-specifier character-type-specifier bool-type-specifier
%type <astnode_p> short-signed-type reg-signed-type long-signed-type longlong-signed-type short-unsigned-type int-unsigned-type long-unsigned-type longlong-unsigned-type
%type <astnode_p> complex-type-specifier imag-type-specifier

%type <astnode_p> struct-type-def struct-type-ref union-type-ref union-type-def
%type <str> struct-tag union-tag
%type <astnode_pp> field-list member-declaration member-declarator-list
%type <astnode_p> member-declarator

%type <tmp_stable_entry> type-specifier 

%type <simple_int> fnc-specifier
%type <astnode_p> simple-declarator

%type <astnode_p> pointer-declarator pointer direct-declarator 
%type <astnode_p> fnc-declarator array-declarator
%type <astnode_p> init-decl declarator  /* initializer- technically here, but not integrated for now */
%type <tmp_stable_entry> decl-specifiers /* pretty sure about this one */

%type <astnode_pp> decl-init-list 
%type <astnode_pp> declaration

%type <astnode_p> type-name abstract-declarator direct-abstract-declarator


/*************************** TOP-LEVEL GRAMMAR-TYPES ****************************/
%type <astnode_p> function-body
%type <astnode_p>  function-def 
%type <astnode_p> declaration_or_fndef 
%start declaration_or_fndef


%%
/* grammars and actions */

/**********************************************************************
***************************** EXPRESSIONS *****************************
**********************************************************************/
/* primary expressions */
primary-expr: IDENT     { 
                    /* resolve identifier from the symbol table */
                    if (!($$ = searchStackScope(GENERAL_NAMESPACE, $1.str))) {
                        $$ = newNode_sTableEntry(NULL);
                        $$->stable_entry.ident = $1.str;
                    }
                }
            | CHARLIT       { $$ = newNode_str(CHARLIT, $1); }
            | NUMBER        { $$ = newNode_num($1);          }
            | STRING        { $$ = newNode_str(STRING, $1);  }
            | '(' expr ')'  { $$ = $2; }
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
             | postfix-expr '(' ')' {
                    $$ = newNode_fnc();
                    $$->fnc.ident = $1;
                    $$->fnc.arguments = NULL;
                    $$->fnc.arg_count = 0;
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

/**********************************************************************
***************************** STATEMENTS ******************************
**********************************************************************/

stmt: expr-stmt         { $$ = $1; }
    | labeled-stmt      { $$ = $1; }
    | compound-stmt     { $$ = $1; }
    | conditional-stmt  { $$ = $1; }
    | iterative-stmt    { $$ = $1; }
    | switch-stmt       { $$ = $1; }
    | break-stmt        { $$ = $1; }
    | continue-stmt     { $$ = $1; }
    | return-stmt       { $$ = $1; }
    | goto-stmt         { $$ = $1; }
    | null-stmt         { $$ = $1; }
    ;

expr-stmt: expr ';'     { $$ = $1; }
         | error ';'    { if (error_count > 10) exit(-1); }
         ;

labeled-stmt: label ':' stmt    {
                    $1->type = Statement_Label;

                    $$ = newNode_sTableEntry($1);
                    $$->stable_entry.node = $3;
                    switch($$->stable_entry.stmtlabel.label_type) {
                        astnode *tmp;
                        case NAMED_LABEL:
                            $$->stable_entry.ident = $1->ident;

                            // check if already defined, if not, add to scope label namespace
                            if (!(tmp = searchStackScope(LABEL_NAMESPACE, $$->stable_entry.ident))) {
                                sTableInsert(scope_stack.innermost_scope->tables[LABEL_NAMESPACE], $$, 0);
                            }
                            else {
                                // if a forward declared label, define it, otherwise an error
                                if (tmp->stable_entry.node == NULL) {

                                    tmp->stable_entry.node = $3;
                                    free($$);
                                    $$ = tmp;
                                }
                                else
                                    yyerror("Multiple labels with the same name!");
                            }
                            break;
                        case CASE_LABEL:
                            // for now not doing anything
                            break;
                        case DEFAULT_LABEL:
                            // nothing to put into scope label namespace
                            break;
                    }
                }
            ;

label: named-label      { $$ = $1; }
     | case-label       { $$ = $1; }
     | default-label    { $$ = $1; }
     ;

compound-stmt: '{' {createNewScope(Block);} decl-or-stmt-list '}' {
                        $$ = newNode_compoundStmt();

                        /* connect compound stmt to its astnodes */
                        $$->compound_stmt.astnode_ll = $3;

                        /* update scope stacks */
                        $$->compound_stmt.scope_layer = scope_stack.innermost_scope;
                        scope_stack.innermost_scope = scope_stack.innermost_scope->child;
                }
             ;

decl-or-stmt-list: /* empty */                      { 
                        $$ = newASTnodeLinkedList(NULL);
                    }
                 | decl-or-stmt-list decl-or-stmt   { 
                        $$ = $1;
                        addASTnodeLinkedList($$, $2);

                        /* if a label, push also its corresponding stmt to the ,
                           AST linked list and also make label point to the linked 
                           list node containing the stmt instead of the stmt itself. */
                        if ($2->nodetype == STABLE_STMT_LABEL) {
                            struct AstnodeLinkedListNode *tmp1 = $$->last;
                            astnode *tmp2 = tmp1->node->stable_entry.node;

                            addASTnodeLinkedList($$, $2->stable_entry.node);
                            tmp1->node->stable_entry.node = newNode_labelHack($$->last);

                            while (tmp2->nodetype == STABLE_STMT_LABEL) {
                                tmp1 = $$->last;

                                addASTnodeLinkedList($$, tmp2->stable_entry.node);
                                tmp1->node->stable_entry.node = newNode_labelHack($$->last);

                                tmp2 = $$->last->node;
                            }        
                        }
                    }
                 ;

decl-or-stmt: declaration   { $$ = $1->list[0]; }
                                        /* doesn't matter what we input, 
                                         because we won't include these 
                                         into the astnode linked list */
            | stmt          { $$ = $1; }
            ;

conditional-stmt: if-stmt       { $$ = $1; }
                | if-else-stmt  { $$ = $1; }
                ;

if-stmt: IF '(' expr ')' stmt %prec IF {
            $$ = newNode_conditionalStmt($3, $5, NULL);
        }
       ;

if-else-stmt: IF '(' expr ')' stmt ELSE stmt %prec ELSE {
                    $$ = newNode_conditionalStmt($3, $5, $7);
                }
            ;

iterative-stmt: while-stmt  { $$ = $1; }
              | do-stmt     { $$ = $1; }
              | for-stmt    { $$ = $1; }
              ;

while-stmt: WHILE '(' expr ')' stmt {
                $$ = newNode_whileStmt($3, $5);
            }
          ;

do-stmt: DO stmt WHILE '(' expr ')' ';' {
            $$ = newNode_doWhileStmt($5, $2);
        }
       ;

for-stmt: FOR for-expr stmt { 
                $$ = $2;
                $$->for_stmt.stmt = $3;
            }
        ;

for-expr: '(' initial-clause ';' expr   ';' expr    ')' {
                $$ = newNode_forLoop();
                $$->for_stmt.initial_clause = $2;
                $$->for_stmt.check_expr = $4;
                $$->for_stmt.iteration_expr = $6;
            }
        | '(' initial-clause ';' expr   ';'         ')' {
                $$ = newNode_forLoop();
                $$->for_stmt.initial_clause = $2;
                $$->for_stmt.check_expr = $4;
            }
        | '(' initial-clause ';'        ';' expr    ')' {
                $$ = newNode_forLoop();
                $$->for_stmt.initial_clause = $2;
                $$->for_stmt.iteration_expr = $5;
            }
        | '(' initial-clause ';' ';' ')' {
            printf("aa\n"); 
                $$ = newNode_forLoop();
                $$->for_stmt.initial_clause = $2;
            }
        | '('                ';' expr   ';' expr    ')' {
                $$ = newNode_forLoop();
                $$->for_stmt.check_expr = $3;
                $$->for_stmt.iteration_expr = $5;
            }
        | '('                ';' expr   ';'         ')' {
                $$ = newNode_forLoop();
                $$->for_stmt.check_expr = $3;
            }
        | '('                ';'        ';' expr    ')' {
                $$ = newNode_forLoop();
                $$->for_stmt.iteration_expr = $4;
            }
        | '('                ';'        ';'         ')' {
                $$ = newNode_forLoop();
            }
        ;

initial-clause: expr        { $$ = newASTnodeList(1, NULL); $$->list[0] = $1; }
              /* ignoring c++ like feature that includes declarations here */
              ;

switch-stmt: SWITCH '(' expr ')' stmt { $$ = newNode_switch($3, $5); }
           ;

case-label: CASE NUMBER {
                $$ = createTmpSTableEntry();
                $$->stmt_case_label_value = $2.val;
                $$->stmt_label_type = CASE_LABEL;    
            }
          ;

default-label: DEFAULT  {
                    $$ = createTmpSTableEntry();
                    $$->stmt_label_type = DEFAULT_LABEL;                
                }
             ;

break-stmt: BREAK ';' { $$ = newNode_flowControl(); $$->nodetype = BREAK_STMT; }
          ;

continue-stmt: CONTINUE ';' { $$ = newNode_flowControl(); $$->nodetype = CONTINUE_STMT; }
             ;

return-stmt: RETURN ';'      { $$ = newNode_returnStmt(); }
           | RETURN expr ';' { $$ = newNode_returnStmt(); $$->return_stmt.expr = $2; }
           ;

goto-stmt: GOTO named-label ';' {
                $$ = newNode_gotoStmt();
                
                astnode *tmp;
                // search for the label in scope, if not found, add it!
                if (!(tmp = searchStackScope(LABEL_NAMESPACE, $2->ident))) {
                    // TODO: implement forward label declaration

                    // create a new label symbol table entry
                    $2->type = Statement_Label;

                    astnode *label_entry = newNode_sTableEntry($2);
                    label_entry->stable_entry.node = NULL;
                    label_entry->stable_entry.ident = $2->ident;

                    // // insert label into the current scope
                    sTableInsert(scope_stack.innermost_scope->
                                    tables[LABEL_NAMESPACE], label_entry, 0);
  
                    $$->goto_stmt.label_stmt = label_entry;
                }
                else
                    $$->goto_stmt.label_stmt = tmp;
            }
         ;

named-label: IDENT { 
                    $$ = createTmpSTableEntry(); 
                    $$->ident = $1.str;
                    $$->stmt_label_type = NAMED_LABEL;
                }
           ;

null-stmt: ';' { $$ = newNode_gotoStmt(); $$->nodetype = NULL_STMT; }
         ;

/**********************************************************************
**************************** DECLARATIONS *****************************
**********************************************************************/

type-name: decl-specifiers {
                if ($1->var_fnc_storage_class)
                    yyerror("Specifying storage class for abstract type");
                else {
                    astnode *tmp = newNode_sTableEntry($1);
                    $$ = tmp->stable_entry.node;
                    free(tmp);
                }
            }
         | decl-specifiers abstract-declarator {
                if ($1->var_fnc_storage_class)
                    yyerror("Specifying storage class for abstract type");
                else {
                    astnode *tmp  = $2;
                    astnode *tmp2;
                    // get to the end of the pointer/array/func path
                    while ( tmp && (
                            tmp->nodetype == PTR_TYPE ||
                            tmp->nodetype == ARRAY_TYPE ||
                            tmp->nodetype == FNC_TYPE
                            ) ) {
                        tmp2 = tmp;
                        switch(tmp->nodetype) {
                            case PTR_TYPE:   tmp = tmp->ptr.pointee;            break;
                            case ARRAY_TYPE: tmp = tmp->arr.ptr->ptr.pointee;   break;
                            case FNC_TYPE:   tmp = tmp->fnc_type.return_type;   break;
                        }
                    }

                    // incorporate the decl-specifier into the declarator
                    astnode *tmp3 = newNode_sTableEntry($1);
                    switch(tmp2->nodetype) {
                        case PTR_TYPE: tmp2->ptr.pointee = tmp3->stable_entry.node;           break;
                        case ARRAY_TYPE: tmp2->arr.ptr->ptr.pointee = tmp3->stable_entry.node;  break;
                        case FNC_TYPE: tmp2->fnc_type.return_type = tmp3->stable_entry.node;  break;
                    }

                    $$ = $2;
                    free(tmp3);
                }
            }
         ;

abstract-declarator: pointer                            { $$ = $1; }
                   | direct-abstract-declarator         { $$ = $1; }
                   | pointer direct-abstract-declarator {
                        /* get to the end of the pointer nodes */
                        astnode *tmp  = $2;
                        astnode *tmp2;
                        // get to the end of the pointer/array/func path
                        while ( tmp && (
                                tmp->nodetype == PTR_TYPE ||
                                tmp->nodetype == ARRAY_TYPE ||
                                tmp->nodetype == FNC_TYPE
                                ) ) {
                            tmp2 = tmp;
                            switch(tmp->nodetype) {
                                case PTR_TYPE:   tmp = tmp->ptr.pointee;            break;
                                case ARRAY_TYPE: tmp = tmp->arr.ptr->ptr.pointee;   break;
                                case FNC_TYPE:   tmp = tmp->fnc_type.return_type;   break;
                            }
                        }

                        // add a pointer node to the sequence at the end
                        switch(tmp2->nodetype) {
                            case PTR_TYPE:   tmp2->ptr.pointee = newNode_ptr(None);          break;
                            case ARRAY_TYPE: tmp2->arr.ptr->ptr.pointee = newNode_ptr(None); break;
                            case FNC_TYPE:   tmp2->fnc_type.return_type = newNode_ptr(None); break;
                        }   
                        $$ = $2;
                    }
                   ;

direct-abstract-declarator: '(' abstract-declarator ')'               { $$ = $2; }
                          | direct-abstract-declarator '[' NUMBER ']' {
                                $$ = newNode_arr($3.val);
                                $$->arr.ptr->ptr.pointee = $1;
                            }
                          | direct-abstract-declarator '[' ']' {
                                $$ = newNode_arr(-1);
                                $$->arr.ptr->ptr.pointee = $1;
                            }
                          | '[' NUMBER ']'  { $$ = newNode_arr($2.val); }
                          | '[' ']'         { $$ = newNode_arr(-1);     }
                          | direct-abstract-declarator '(' ')'  {
                                if ($1->nodetype == FNC_TYPE)
                                    yyerror("Invalid type: function returning function");
                                else {

                                    astnode *tmp  = $1;
                                    astnode *tmp2;
                                    // get to the end of the pointer/array/func path
                                    while ( tmp && (
                                            tmp->nodetype == PTR_TYPE ||
                                            tmp->nodetype == ARRAY_TYPE
                                            ) ) {
                                        tmp2 = tmp;
                                        switch(tmp->nodetype) {
                                            case PTR_TYPE:   tmp = tmp->ptr.pointee;            break;
                                            case ARRAY_TYPE: tmp = tmp->arr.ptr->ptr.pointee;   break;
                                        }
                                    }

                                    // add a pointer node to the sequence at the end
                                    switch(tmp2->nodetype) {
                                        case PTR_TYPE:   tmp2->ptr.pointee = newNode_fncType(-1);          break;
                                        case ARRAY_TYPE: tmp2->arr.ptr->ptr.pointee = newNode_fncType(-1); break;
                                        case FNC_TYPE:   tmp2->fnc_type.return_type = newNode_fncType(-1); break;
                                    }
                                    $$ = $1;
                                }
                            }
                          | '(' ')' { 
                                $$ = newNode_fncType(-1);
                            }
                          ;





declaration: decl-specifiers ';'    { 
                $$ = newASTnodeList(1, NULL); 
                $$->list[0] = $1->node;
            }
           | decl-specifiers decl-init-list ';' {
            

                if (!($1->node)) {
                    $1->node = newNode_scalarType(Int, 1);
                    $1->type = Variable_Type;
                }

                if (isTmpSTableEntryValid($1)) {

                    /* create the new symbol table entries */
                    $$ = combineSpecifierDeclarator($1, $2); 
                    /* add the new entries to the symbol table */
                    int ns_ind;

                    for (int i = 0 ; i < $$->len; ++i) {

                        if  (   $$->list[i]->stable_entry.type == Enum_Tag   ||
                                $$->list[i]->stable_entry.type == S_Tag_Type ||
                                $$->list[i]->stable_entry.type == U_Tag_Type
                            )
                            ns_ind = SU_TAG_NAMESPACE;  /* tags (idents of struct/union/enum) */
                        else
                            ns_ind = GENERAL_NAMESPACE;  /* all other identifier classes */


                        /* check if we are in global scope, as this will make
                        variables extern by default instead of auto. */
                        if  (   scope_stack.innermost_scope->scope_type == File &&
                                $$->list[i]->stable_entry.type == Variable_Type ||
                                $$->list[i]->stable_entry.type == Function_Type
                            )
                            $$->list[i]->stable_entry.var.storage_class = Extern;

                        if(sTableInsert(scope_stack.innermost_scope->tables[ns_ind], $$->list[i], 0) < 0) {
                            
                            free($$->list[i]);
                            $$->list[i] = NULL;
                        }
                    }
                }
                else
                    yyerror("Error in declaration specifiers");
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
                        $$ = $1;
                    }
               | type-specifier decl-specifiers {
                        $$ = $1;
                        if ($$->node)
                            yyerror("Can't have multiple type specifiers for a declaration specifiers");
                        else {
                            $$->node = $1->node;
                            $$->type = $1->type;
                        }                                                                
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

type-specifier: enum-type-specifier {                         
                        $$ = createTmpSTableEntry();
                        $$->node = $1; 
                        $$->type =  Enum_Tag;
                    }
              | float-type-specifier  {                         
                        $$ = createTmpSTableEntry();
                        $$->node = $1; 
                        $$->type =  Variable_Type;
                    }
              | int-type-specifier  {                         
                        $$ = createTmpSTableEntry();
                        $$->node = $1; 
                        $$->type =  Variable_Type;
                    }
              | struct-type-specifier  {                         
                        $$ = createTmpSTableEntry();
                        $$->node = $1; 
                        $$->type = Variable_Type;
                    }
              | typedef-type-specifier  {                         
                        $$ = createTmpSTableEntry();
                        $$->node = $1; 
                        $$->type =  Typedef_Name;
                    } 
              | union-type-specifier  {                         
                        $$ = createTmpSTableEntry();
                        $$->node = $1; 
                        $$->type =  Variable_Type;
                    }
              | void-type-specifier  {                         
                        $$ = createTmpSTableEntry();
                        $$->node = $1; 
                        $$->type =  Variable_Type;
                    }
              ;

int-type-specifier: signed-type-specifier       { $$ = $1; }
                  | unsigned-type-specifier     { $$ = $1; }
                  | character-type-specifier    { $$ = $1; }
                  | bool-type-specifier         { $$ = $1; }
                  ;

signed-type-specifier: short-signed-type    { $$ = $1; }
                     | reg-signed-type      { $$ = $1; }
                     | long-signed-type     { $$ = $1; }
                     | longlong-signed-type { $$ = $1; }
                     ; 

short-signed-type: SHORT            { $$ = newNode_scalarType(Short, 1); }
                 | SHORT INT        { $$ = newNode_scalarType(Short, 1); }
                 | SIGNED SHORT     { $$ = newNode_scalarType(Short, 1); }
                 | SIGNED SHORT INT { $$ = newNode_scalarType(Short, 1); }
                 ;

reg-signed-type: INT        { $$ = newNode_scalarType(Int, 1); }
               | SIGNED INT { $$ = newNode_scalarType(Int, 1); }
               | SIGNED     { $$ = newNode_scalarType(Int, 1); }
               ;

long-signed-type: LONG              { $$ = newNode_scalarType(Long, 1); }
                | LONG INT          { $$ = newNode_scalarType(Long, 1); }
                | SIGNED LONG       { $$ = newNode_scalarType(Long, 1); }
                | SIGNED LONG INT   { $$ = newNode_scalarType(Long, 1); }
                ;

longlong-signed-type: LONG LONG             { $$ = newNode_scalarType(LongLong, 1); }
                    | LONG LONG INT         { $$ = newNode_scalarType(LongLong, 1); }
                    | SIGNED LONG LONG      { $$ = newNode_scalarType(LongLong, 1); }
                    | SIGNED LONG LONG INT  { $$ = newNode_scalarType(LongLong, 1); }
                    ;

unsigned-type-specifier: short-unsigned-type    { $$ = $1; }
                       | int-unsigned-type      { $$ = $1; }
                       | long-unsigned-type     { $$ = $1; }
                       | longlong-unsigned-type { $$ = $1; }
                       ; 

short-unsigned-type: UNSIGNED SHORT     { $$ = newNode_scalarType(Short, 0); }         
                   | UNSIGNED SHORT INT { $$ = newNode_scalarType(Short, 0); }
                   ;

int-unsigned-type: UNSIGNED     { $$ = newNode_scalarType(Int, 0); }
                 | UNSIGNED INT { $$ = newNode_scalarType(Int, 0); }
                 ;

long-unsigned-type: UNSIGNED LONG       { $$ = newNode_scalarType(Long, 0); }
                  | UNSIGNED LONG INT   { $$ = newNode_scalarType(Long, 0); }
                  ;

longlong-unsigned-type: UNSIGNED LONG LONG      { $$ = newNode_scalarType(LongLong, 0); }
                      | UNSIGNED LONG LONG INT  { $$ = newNode_scalarType(LongLong, 0); }
                      ;

/* a plain 'char' was chosen to be an 'unsigned char' */
character-type-specifier: CHAR          { $$ = newNode_scalarType(Char, 0); }
                        | SIGNED CHAR   { $$ = newNode_scalarType(Char, 1); }
                        | UNSIGNED CHAR { $$ = newNode_scalarType(Char, 0); }
                        ;

bool-type-specifier: _BOOL { $$ = newNode_scalarType(Bool, 0); }
                   ;

float-type-specifier: FLOAT                  { $$ = newNode_scalarType(Float, 1);       }
                    | DOUBLE                 { $$ = newNode_scalarType(Double, 1);      }
                    | LONG DOUBLE   { $$ = newNode_scalarType(LongDouble, 1);  }
                    | complex-type-specifier { $$ = $1; }
                    | imag-type-specifier    { $$ = $1; }
                    ;

complex-type-specifier: FLOAT _COMPLEX       { $$ = newNode_scalarType(FloatComplex, 1);     }
                      | DOUBLE _COMPLEX      { $$ = newNode_scalarType(DoubleComplex, 1);    }
                      | LONG DOUBLE _COMPLEX { $$ = newNode_scalarType(LongDoubleComplex, 1);}
                      ;

imag-type-specifier: FLOAT _IMAGINARY       { $$ = newNode_scalarType(FloatImag, 1);     }
                   | DOUBLE _IMAGINARY      { $$ = newNode_scalarType(DoubleImag, 1);    }
                   | LONG DOUBLE _IMAGINARY { $$ = newNode_scalarType(LongDoubleImag, 1);}
                   ;

void-type-specifier: VOID        { $$ = newNode_scalarType(Void, 1); }
                   ;

/* for now not implementing enums, typedefs. */
enum-type-specifier: ENUM        { $$ = newNode_scalarType(Int, 1); }
                   ;
typedef-type-specifier: TYPEDEF  { $$ = newNode_scalarType(Int, 1); } 
                      ;

/* struct-type-specifier is a symbol table entry, not just a astnode type */
struct-type-specifier: struct-type-def  { $$ = $1; }
                     | struct-type-ref  { $$ = $1; }
                     ;

struct-type-def: STRUCT '{' field-list '}' {   
                    /* NOTE: we aren't inserting this struct into the scope symbol table */
                    TmpSymbolTableEntry *new_struct = createTmpSTableEntry();
                    new_struct->type = S_Tag_Type;
                    new_struct->su_tag_is_defined = 1;
                    $$ = newNode_sTableEntry(new_struct);

                    /* insert fields into the struct */
                    for (int i = 0; i < $3->len; ++i)
                        sTableInsert($$->stable_entry.sutag.su_table, $3->list[i], 0);
                }
               | STRUCT struct-tag  {
                        /* NOTE: this struct will be inserted into the symbol table (if not there already) */
                        $<astnode_p>$ = sTableLookUp(scope_stack.innermost_scope->tables[SU_TAG_NAMESPACE], $2.str);
                    
                        if  (   $<astnode_p>$ && 
                                $<astnode_p>$->stable_entry.type == S_Tag_Type && 
                                !$<astnode_p>$->stable_entry.sutag.is_defined
                            ) {     /* if struct was declared but not defined */
                            
                                $<astnode_p>$->stable_entry.file_name = cur_file_name;
                                $<astnode_p>$->stable_entry.line_num = cur_line_num;
                            }
                        else if (!$<astnode_p>$) {  /* if struct has not been declared previously */

                            TmpSymbolTableEntry *new_struct = createTmpSTableEntry(); 
                            new_struct->type = S_Tag_Type;
                            new_struct->su_tag_is_defined = 0; /* still not fully defined */
                            
                            $<astnode_p>$ = newNode_sTableEntry(new_struct);
                            $<astnode_p>$->stable_entry.ident = $2.str;

                        }
                        else
                            yyerror("This struct was already defined");
                    } '{' field-list '}' {
                        $$ = $<astnode_p>3;
                        
                        for (int i = 0; i < $5->len; ++i) {
                            if  (   $5->list[i]->stable_entry.node->nodetype == STABLE_SU_TAG           &&
                                    !strcmp($5->list[i]->stable_entry.node->stable_entry.ident, $2.str) &&
                                    !$5->list[i]->stable_entry.node->stable_entry.sutag.is_defined 
                                ) {
                                yyerror("Cannot declare variable of incomplete type. Perhaps you meant to create a pointer to it?");
                            }
                            
                            sTableInsert($$->stable_entry.sutag.su_table, $5->list[i], 0);
                        }

                        $$->stable_entry.sutag.is_defined = 1;

                        if (!searchStackScope(SU_TAG_NAMESPACE, $$->stable_entry.ident))
                            sTableInsert(scope_stack.innermost_scope->tables[SU_TAG_NAMESPACE], $$, 0);
                }
               ;

struct-type-ref: STRUCT struct-tag {                    
                    if (!($$ = sTableLookUp(scope_stack.innermost_scope->tables[SU_TAG_NAMESPACE], $2.str))) {  
                        //* create a forward, incomplete declaration */
                        TmpSymbolTableEntry *new_struct = createTmpSTableEntry();
                        new_struct->type = S_Tag_Type;
                        new_struct->su_tag_is_defined = 0;
                        $$ = newNode_sTableEntry(new_struct);
                        $$->stable_entry.ident = $2.str;
                        if(sTableInsert(scope_stack.innermost_scope->tables[SU_TAG_NAMESPACE], $$, 0) < 0)
                            yyerror("Unable to insert incomplete struct into symbol table");
                    }
                }
               ;

struct-tag: IDENT   { $$ = $1; }
          ;

field-list: member-declaration              { $$ = $1; }
          | field-list member-declaration   { 
                    $$ = newASTnodeList($1->len + $2->len, $1);
                    
                    for (int i = $1->len, k=0 ; i < $$->len ; ++i, ++k)  
                        $$->list[i] = $2->list[k];
                }
          ;

member-declaration: type-specifier member-declarator-list ';' { 
                            $1->type = SU_Member_Type;
                            if (!isTmpSTableEntryValid($1))
                                yyerror("Invalid struct declaration specifiers.");
                            else
                                $$ = combineSpecifierDeclarator($1, $2); 
                        }
                  ;

member-declarator-list: member-declarator { 
                                $$ = newASTnodeList(1, NULL); 
                                $$->list[0] = $1;
                            }
                      | member-declarator-list ',' member-declarator {
                                $$ = newASTnodeList($1->len+1, $1);
                                $$->list[$1->len] = $3;
                            }
                      ;

member-declarator: declarator   { $$ = $1; }
                 /* we will not be implementing bit-fields in struct definitions */
                 ;

union-type-specifier: union-type-def { $$ = $1; }
                    | union-type-ref { $$ = $1; }
                    ;

union-type-def: UNION '{' field-list '}' {   
                    /* NOTE: we aren't inserting this union into the scope symbol table */
                    TmpSymbolTableEntry *new_struct = createTmpSTableEntry();
                    new_struct->type = U_Tag_Type;
                    new_struct->su_tag_is_defined = 1;
                    $$ = newNode_sTableEntry(new_struct);

                    /* insert fields into the union */
                    for (int i = 0; i < $3->len; ++i)
                        sTableInsert($$->stable_entry.sutag.su_table, $3->list[i], 0);
                }
              | UNION union-tag {
                        /* NOTE: this union will be inserted into the symbol table (if not there already) */
                        $<astnode_p>$ = sTableLookUp(scope_stack.innermost_scope->tables[SU_TAG_NAMESPACE], $2.str);
                    
                        if  (   $<astnode_p>$ && 
                                $<astnode_p>$->stable_entry.type == U_Tag_Type && 
                                !$<astnode_p>$->stable_entry.sutag.is_defined
                            ) {     /* if union was declared but not defined */
                            
                                $<astnode_p>$->stable_entry.file_name = cur_file_name;
                                $<astnode_p>$->stable_entry.line_num = cur_line_num;
                            }
                        else if (!$<astnode_p>$) {  /* if union has not been declared previously */

                            TmpSymbolTableEntry *new_struct = createTmpSTableEntry(); 
                            new_struct->type = U_Tag_Type;
                            new_struct->su_tag_is_defined = 0; /* still not fully defined */
                            
                            $<astnode_p>$ = newNode_sTableEntry(new_struct);
                            $<astnode_p>$->stable_entry.ident = $2.str;

                        }
                        else
                            yyerror("This union was already defined");
                    } '{' field-list '}' {
                        $$ = $<astnode_p>3;
                        
                        for (int i = 0; i < $5->len; ++i) {
                            if  (   $5->list[i]->stable_entry.node->nodetype == STABLE_SU_TAG           &&
                                    !strcmp($5->list[i]->stable_entry.node->stable_entry.ident, $2.str) &&
                                    !$5->list[i]->stable_entry.node->stable_entry.sutag.is_defined 
                                ) { 
                                yyerror("Cannot declare variable of incomplete type. Perhaps you meant to create a pointer to it?");
                            }
                            
                            sTableInsert($$->stable_entry.sutag.su_table, $5->list[i], 0);
                        }

                        $$->stable_entry.sutag.is_defined = 1;

                        if (!searchStackScope(SU_TAG_NAMESPACE, $$->stable_entry.ident))
                            sTableInsert(scope_stack.innermost_scope->tables[SU_TAG_NAMESPACE], $$, 0);
                }
              ;

union-type-ref: UNION union-tag { 

                    if (!($$ = sTableLookUp(scope_stack.innermost_scope->tables[SU_TAG_NAMESPACE], $2.str))) {  /* create a forward, incomplete declaration */
                        TmpSymbolTableEntry *new_union = createTmpSTableEntry();
                        new_union->type = U_Tag_Type;
                        new_union->su_tag_is_defined = 0;
                        $$ = newNode_sTableEntry(new_union);
                        $$->stable_entry.ident = $2.str;
                        if(sTableInsert(scope_stack.innermost_scope->tables[SU_TAG_NAMESPACE], $$, 0) < 0)
                            yyerror("Unable to insert incomplete union into symbol table");
                    }
                }
              ;

union-tag: IDENT    { $$ = $1; }
         ;



decl-init-list: init-decl { 
                        $$ = newASTnodeList(1, NULL); 
                        $$->list[0] = $1;
                    }  
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


pointer-declarator: pointer direct-declarator   {  
                        if ($2->nodetype == STABLE_FNC_DECLARATOR) {
                            if (!$2->stable_entry.fnc.return_type) {
                                $2->stable_entry.fnc.return_type = $1;
                            }
                            else {
                                astnode *pacer = $2->stable_entry.fnc.return_type;
                                astnode *runner = pacer;
                                
                                while (runner && (runner->nodetype == PTR_TYPE || runner->nodetype == ARRAY_TYPE)) {
                                    pacer = runner;
                                    if (runner->nodetype == PTR_TYPE)
                                        runner = runner->ptr.pointee;
                                    else
                                        runner = runner->arr.ptr;
                                }                             
                                pacer->ptr.pointee = $1;
                            }
                        }   
                        else if ($2->stable_entry.node) {
                            astnode *pacer = $2;
                            astnode *runner = $2->stable_entry.node;
                            
                            while (runner && (runner->nodetype == PTR_TYPE || runner->nodetype == ARRAY_TYPE)) {
                                pacer = runner;
                                if (runner->nodetype == PTR_TYPE)
                                    runner = runner->ptr.pointee;
                                else
                                    runner = runner->arr.ptr;
                            } 
                            pacer->ptr.pointee = $1;
                        }
                        else {
                            $2->stable_entry.node = $1;
                        }

                        $$ = $2;
                    }
                  ;

pointer: '*'                        { $$ = newNode_ptr(None); }
       | '*' type-qualifier-list    { $$ = newNode_ptr($2); }
       | '*' pointer { 
            astnode *tmp = $2;
            astnode *tmp2;
            while (tmp) {
                tmp2 = tmp;
                tmp = tmp->ptr.pointee;
            }
            tmp2->ptr.pointee = newNode_ptr(None);
            
            $$ = $2; 
        }
       | '*' type-qualifier-list pointer    { 
            
            astnode *tmp = $3;
            astnode *tmp2;
            while (tmp) {
                tmp2 = tmp;
                tmp = tmp->ptr.pointee;
            }
            tmp2->ptr.pointee = newNode_ptr($2);
            
            $$ = $3;
        }
       ;

type-qualifier-list: type-qualifier                     { $$ = $1; }
                   | type-qualifier-list type-qualifier { $$ = helperTypeQualifierAddition($1,$2);}
                   ;

direct-declarator: simple-declarator    { $$ = $1; }
                 | '(' declarator ')'   { $$ = $2; }
                 | fnc-declarator       { $$ = $1; }
                 | array-declarator     { $$ = $1; }
                 ;


simple-declarator: IDENT { 
                        $$ = newNode_sTableEntry(NULL);
                        $$->stable_entry.ident = $1.str; 
                    }
                 ;

array-declarator: direct-declarator '[' ']'         {  
                        astnode *pacer = $1;
                        
                        if ($1->stable_entry.node) {
                            astnode *runner = $1->stable_entry.node;
                            
                            while (runner && (runner->nodetype == PTR_TYPE || runner->nodetype == ARRAY_TYPE)) {
                                pacer = runner;
                                if (runner->nodetype == PTR_TYPE)
                                    runner = runner->ptr.pointee;
                                else
                                    runner = runner->arr.ptr;
                            } 
                            pacer->ptr.pointee = newNode_arr(-1);
                        }
                        else {
                            pacer->stable_entry.node = newNode_arr(-1);
                        }
                        $$ = $1;
                    }
                | direct-declarator '[' NUMBER ']'  {
                        astnode *pacer = $1;
                        
                        if ($1->stable_entry.node) {
                            astnode *runner = $1->stable_entry.node;
                            
                            while (runner && (runner->nodetype == PTR_TYPE || runner->nodetype == ARRAY_TYPE)) {
                                pacer = runner;
                                if (runner->nodetype == PTR_TYPE)
                                    runner = runner->ptr.pointee;
                                else
                                    runner = runner->arr.ptr;
                            } 
                            pacer->ptr.pointee = newNode_arr($3.val);
                        }
                        else 
                            pacer->stable_entry.node = newNode_arr($3.val);
                        $$ = $1;
                    }
                /* for now only allow these type of array declarations. We will 
                   also simplify by not implementing variable-length arrays. */
                ;

fnc-declarator: direct-declarator '(' ')' {
                        astnode *pacer = $1;

                        if ($1->stable_entry.node) {
                            astnode *runner = $1->stable_entry.node;
                            
                            while (runner && (runner->nodetype == PTR_TYPE || runner->nodetype == ARRAY_TYPE)) {
                                pacer = runner;
                                if (runner->nodetype == PTR_TYPE)
                                    runner = runner->ptr.pointee;
                                else
                                    runner = runner->arr.ptr;
                            } 
                            /* NOTE: -1 == unknown number of arguments */
                            pacer->ptr.pointee = newNode_fncType(-1);
                            $$ = $1;
                            }
                        else {
                            TmpSymbolTableEntry *tmp = createTmpSTableEntry();
                            tmp->type = Function_Type;
                            $$ = newNode_sTableEntry(tmp);
                            $$->stable_entry.ident = $1->stable_entry.ident;
                            $$->stable_entry.fnc.return_type = NULL;
                        }
                        
                }
              /* simpilify function declarations to only include (). */
              ;



/**********************************************************************
************************ FUNCTION DECLARATIONS ************************
**********************************************************************/

function-def: decl-specifiers declarator  {

            /* check if function is already declared first */
            if ($<astnode_p>$ = searchStackScope(GENERAL_NAMESPACE, $2->stable_entry.ident)) {
                $<astnode_p>$->stable_entry.type = Function_Type;
                $<astnode_p>$->nodetype = STABLE_FNC_DEFINITION;
                $<astnode_p>$->stable_entry.node->fnc_type.fnc_body = NULL;
                $<astnode_p>$->stable_entry.line_num = cur_line_num;
                $<astnode_p>$->stable_entry.file_name = cur_file_name;
            }
            else {
                struct astnode_list *tmp_list = newASTnodeList(1, NULL);
                tmp_list->list[0] = $2;

                astnode_list *tmp2 = combineSpecifierDeclarator($1, tmp_list);
                $<astnode_p>$ = tmp2->list[0];
                $<astnode_p>$->stable_entry.type = Function_Type;
                $<astnode_p>$->nodetype = STABLE_FNC_DEFINITION;
                $<astnode_p>$->stable_entry.fnc.function_body = NULL;

                /* adding function to the scope above it */
                sTableInsert(scope_stack.innermost_scope->tables[GENERAL_NAMESPACE], $<astnode_p>$, 0);
            }
        } function-body {
            $$ = $<astnode_p>3;
            $$->stable_entry.fnc.function_body = $4;
        }
       ;


/* the reason this is seperate from the compound statement grammar is
   so that we could specify that this is a function scope and not a 
   block scope... probably could be avoided but not too bad a case
   of code duplication anyways... */
function-body: '{' { createNewScope(Function); } decl-or-stmt-list '}' {
                $$ = newNode_compoundStmt();

                /* connect compound stmt to its astnodes */
                $$->compound_stmt.astnode_ll = $3;

                /* update scope stacks */
                $$->compound_stmt.scope_layer = scope_stack.innermost_scope;
                scope_stack.innermost_scope = scope_stack.innermost_scope->child;
             }
         ;


/**********************************************************************
************************** TOP-LEVEL GRAMMAR **************************
**********************************************************************/

declaration_or_fndef: /* empty */                           { /* NOTHING */ } 
                    | declaration_or_fndef declaration      {   
                            if ($2) {
                                for (int i = 0; i < $2->len; ++i)
                                    if ($2->list[i])
                                        printAST($2->list[i], NULL);
                            }
                        }
                    | declaration_or_fndef function-def     { 
                            printAST($2, NULL);
                        }
                    ;



%%
