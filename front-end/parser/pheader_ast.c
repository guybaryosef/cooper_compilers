/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * pheader_ast.c - This file contains the implementations
 * of the functions defined in pheader_ast.h.
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "../front_end_header.h"
#include "../lexer/lheader.h"
#include "../lexer/lheader2.h"
#include "symbol_table.h"
#include "pheader_ast.h"


/*
 *  newNode_num - Creates a new AST node of type num.
 */
struct astnode *newNode_num(struct YYnum num) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = NUM_TYPE;

    node->num.types = num.types;
    node->num.val = num.val;
    node->num.d_val = num.d_val;

    return node;
}


/*
 * newNode_str - Creates anew AST node of type identifier,
 * string literal, or character literal.
 * 
 * Because they all are contained in a struct str, we can
 * handle all of these differnet tokens together.
 */
struct astnode *newNode_str(int token_name, struct YYstr str) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    switch(token_name) {
        case IDENT:
            node->nodetype = IDENT_TYPE;
            node->ident.str = str.str;
            break;       
        case CHARLIT:
            node->nodetype = CHRLIT_TYPE;
            node->chrlit.c_val = str.char_val;
            break;
        case STRING:
            node->nodetype = STRLIT_TYPE;
            node->strlit.str = str.str;
            node->strlit.str_size = str.str_size;
            break;
    }
    return node;
}

/*
 * newNode_unop - Creates a new AST node for type unop 
 * (unary operation).
 */
struct astnode *newNode_unop(int token_name) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    switch(token_name) {
        case SIZEOF:
            node->nodetype = SIZEOF_TYPE;
            break;
        case '*':
            node->nodetype = DEREF_TYPE;
            break;
        case '&': 
            node->nodetype = ADDR_TYPE;
            break;
        default:
            node->nodetype = UNOP_TYPE;
    }
    node->unop.op = token_name;
    node->unop.expr = NULL;
    return node;
}

/*
 * newNode_binop - Creates a new AST node for type binop 
 * (binary operation).
 */
struct astnode *newNode_binop(int token_name) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    switch(token_name) {
        case '>':
        case '<':
        case LTEQ:
        case GTEQ:
        case EQEQ:
        case NOTEQ:
            node->nodetype = COMPARE_TYPE;
            break;
        case LOGAND:
        case LOGOR:
            node->nodetype = LOG_TYPE;
            break;
        default:
            node->nodetype = BINOP_TYPE;
    }

    node->binop.op = token_name;
    node->binop.left = NULL;
    node->binop.right = NULL;
    return node;
}

/*
 * newNode_func - Creates a new AST node
 * of type function.
 */
struct astnode *newNode_fnc() {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = FNC_CALL;
    node->fnc.arg_count = 0;
    node->fnc.arguments = NULL;
    node->fnc.ident = NULL;
    return node;
}

/*
 * newNode_arglist - creates a new AST node
 * for the argument list of a function.
 */
struct astnode *newNode_arglist() {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    node->nodetype = ARGLIST_TYPE;
    node->arglist.size = 0;
    node->arglist.list = NULL;
    return node;
}

/*
 * expand_arglist - Expands the list of arguments
 * by 1.
 */
void expand_arglist(struct astnode *node) {
    struct astnode **new_arg_list = malloc((node->arglist.size+1)*sizeof(struct astnode *));

    memcpy(new_arg_list, node->arglist.list, sizeof(struct astnode *)*node->arglist.size);

    free(node->arglist.list);
    node->arglist.list = new_arg_list;
}

/*
 * newNode_arg - creates a new AST node
 * of type function argument.
 */
struct astnode *newNode_arg(int num) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = ARG_TYPE;
    node->arg.num = num;
    node->arg.expr = NULL;
    return node;
}

/*
 * newNode_slct - creates a new AST node
 * of type component selection.
 * 
 * Type:
 *  - 0: Direct component selection.
 */
struct astnode *newNode_slct() {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = SLCT_TYPE;
    node->slct.left = NULL;
    node->slct.right = NULL;
    return node;
}


/*
 * newNode_ternary - Creates an AST node for
 * the ternary operator (expr ? res1 : res 2).
 */
struct astnode *newNode_ternary() {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = TERNARY_TYPE;
    node->ternary.if_expr = NULL;
    node->ternary.then_expr = NULL;
    node->ternary.else_expr = NULL;
    return node;
}


/*
 * newNode_assment - Creates an AST node for the
 * assignment expression operator (=).
 */
struct astnode *newNode_assment(int op) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = ASS_TYPE;
    node->assignment.op = op;
    node->assignment.left = NULL;
    node->assignment.right = NULL;
    return node;    
}

/*
 * newNode_ptr - Creates an AST node for a pointer.
 */
struct astnode *newNode_ptr() {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = PTR_TYPE;
    node->ptr.pointee = NULL;
    return node;
}


/*
 * newNode_arr - Creates an AST node for an array.
 */
struct astnode *newNode_arr(int size) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = ARRAY_TYPE;
    node->arr.size = size;
    node->arr.ptr = NULL;
    return node;
}

/*
 * newNode_type - Creates a new AST node for a scalar type.
 */
struct astnode *newNode_type(enum Types type, _Bool is_signed) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = SCALAR_TYPE;
    node->type.sign = is_signed;
    node->type.type = type;
    return node;
}


/*
 * newNode_fnc_type - Creates a new AST node for a
 * function type.
 */
struct astnode *newNode_fnc_type(int arg_len) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = FNC_TYPE;
    node->fnc_type.arg_count = arg_len;
    node->fnc_type.args_types = calloc(arg_len, sizeof(struct astnode *));
    node->fnc_type.return_type = NULL;
    return node;
}


/*
 * newNode_su_type - Creates a new AST node for the type
 * of a struct or union.
 */
struct astnode *newNode_su_type() {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = SU_TYPE;
    node->su_type.table = symbol_table_create();
    return node;
}


/*
 * token2op - Takes the token name of an operator and 
 * returns how the operator is printed in Hakne'r AST example.
 * 
 * Ex: token name 'DIVEQ' will return the string "/=", while
 * "++" will return "POSTINC". Use this function as a reference
 * as to what should return what.
 */
char *token2op(int token_name) {
    switch(token_name) {
        case DIVEQ:
            return "/=";
        case MODEQ:
            return "%=";
        case SHLEQ:
            return "<<=";
        case SHREQ:
            return ">>=";
        case ANDEQ:
            return "&=";
        case XOREQ:
            return "^=";
        case OREQ:
            return "|=";
        case INDSEL:
            return "->";
        case SHL:
            return "<<";
        case SHR:
            return ">>";
        case LTEQ:
            return "<=";
        case GTEQ:
            return ">=";
        case EQEQ:
            return "==";
        case NOTEQ:
            return "!=";
        case LOGAND:
            return "&&";
        case LOGOR:
            return "||";
        case PLUSEQ:
            return "+=";
        case MINUSEQ:
            return "-=";
        case TIMESEQ:
            return "*=";
        case PLUSPLUS:
            /* This will only appear in postincrements, b/c 
               preincrement "++" will get reduced to "+= 1".   */
            return "POSTINC"; 
        case MINUSMINUS:
            return "--";
    }
}



/*
 * printAST - Given the root node of an Abstract Syntax Tree,
 * this function prints out the tree in a format matching 
 * Hakner's in the assignment sheet.
 * 
 * This is in essense a preorder traversal.
 */
void printAST(struct astnode *root, FILE *output_file) {

    FILE *output = (output_file) ? output_file : stdout;
    preorderTraversal(root, output, 0);
    fprintf(output, "\n");  /* keeping consistent with Hakner's format */
}


/*
 * preorderTraversal - A helper function for printAST. This
 * function implements preorder traversal for the AST printing.
 */
void preorderTraversal(struct astnode *cur, FILE *output, int depth) {
    if (cur == NULL)    /* for error checking, might be beneficial to throw error here */
        return;

    /* format the tab spacing correctly */
    for (int i = 0; i < depth; ++i)
        fprintf(output, "  ");

    switch(cur->nodetype) {
        case IDENT_TYPE:
            fprintf(output, "IDENT  %s\n", cur->ident.str);
            break;
        case NUM_TYPE:
            if (cur->num.types & NUMMASK_INTGR)
                fprintf(output, "CONSTANT:  (type=int)%lld\n", cur->num.val);
            else if (cur->num.types & NUMMASK_DOUBLE)
                fprintf(output, "CONSTANT:  (type=double)%g\n", cur->num.d_val);
            else
                fprintf(output, "CONSTANT:  (type=float)%g\n", cur->num.d_val);
            break;
        case CHRLIT_TYPE:
            fprintf(output, "CONSTANT:  (type=int)%d\n", cur->chrlit.c_val);
            break;
        case STRLIT_TYPE:
            fprintf(output, "STRING\t%s\n", cur->strlit.str);
            break;
        case BINOP_TYPE:
            if (cur->binop.op < 258)
                fprintf(output, "BINARY  OP  %c\n", cur->binop.op);
            else
                fprintf(output, "BINARY  OP  %s\n", token2op(cur->binop.op));
            preorderTraversal(cur->binop.left, output, depth+1);
            preorderTraversal(cur->binop.right, output, depth+1);
            break;
        case COMPARE_TYPE:
            if (cur->binop.op < 258)
                fprintf(output, "COMPARISON  OP  %c\n", cur->binop.op);
            else
                fprintf(output, "COMPARISON  OP  %s\n", token2op(cur->binop.op));
            preorderTraversal(cur->binop.left, output, depth+1);
            preorderTraversal(cur->binop.right, output, depth+1);
            break;
        case LOG_TYPE:
            fprintf(output, "LOGICAL  OP  %s\n", token2op(cur->binop.op));
            preorderTraversal(cur->binop.left, output, depth+1);
            preorderTraversal(cur->binop.right, output, depth+1);            break;
        case UNOP_TYPE:
            if (cur->binop.op < 258)
                fprintf(output, "UNARY  OP  %c\n", cur->unop.op);
            else
                fprintf(output, "UNARY  OP  %s\n", token2op(cur->unop.op));
            preorderTraversal(cur->unop.expr, output, depth+1);
            break;
        case ADDR_TYPE:
            fprintf(output, "ADDRESSOF\n");
            preorderTraversal(cur->unop.expr, output, depth+1);
            break;
        case DEREF_TYPE:
            fprintf(output, "DEREF\n");
            preorderTraversal(cur->unop.expr, output, depth+1);
            break;
        case SIZEOF_TYPE:
            fprintf(output, "SIZEOF\n");
            preorderTraversal(cur->unop.expr, output, depth+1);
            break;
        case FNC_CALL:
            fprintf(output, "FNCALL,  %d  arguments\n", cur->fnc.arg_count);
            preorderTraversal(cur->fnc.ident, output, depth + 1);
            for (int i = 0 ; i < cur->fnc.arg_count ; ++i)
                preorderTraversal(cur->fnc.arguments[i], output, depth);
            break;
        case ARG_TYPE:
            fprintf(output, "arg  #%d=\n", cur->arg.num);
            preorderTraversal(cur->arg.expr, output, depth+1);
            break;
        case SLCT_TYPE:
            fprintf(output, "SELECT\n");
            preorderTraversal(cur->slct.left, output, depth+1);
            preorderTraversal(cur->slct.right, output, depth+1);
            break;
        case TERNARY_TYPE:
            fprintf(output, "TERNARY  OP,  IF:\n");
            preorderTraversal(cur->ternary.if_expr, output, depth+1);
            for (int i = 0; i < depth; ++i)
                fprintf(output, "  ");
            fprintf(output, "THEN:\n");
            preorderTraversal(cur->ternary.then_expr, output, depth+1);
            for (int i = 0; i < depth; ++i)
                fprintf(output, "  ");
            fprintf(output, "ELSE:\n");
            preorderTraversal(cur->ternary.else_expr, output, depth+1);
            break;
        case ASS_TYPE:
            fprintf(output, "ASSIGNMENT\n");
            preorderTraversal(cur->assignment.left, output, depth+1);
            preorderTraversal(cur->assignment.right, output, depth+1);
            break;
        /********* NEEDS WORK *****/
        case PTR_TYPE:
            fprintf(output, "TYPE DESIGNATOR: %s\n", stringFromTokens(cur->type.type));
            break;
        case ARRAY_TYPE:
            break;
        case SCALAR_TYPE:
            break;
        case FNC_TYPE:
            break;
        case SU_TYPE:
            break;
    }
}


/* 
 * freeTree - Frees the dynamically allocated memory 
 * that was used by the Abstract Syntax Tree.
 *
 * This function does not return anything.
 */
void freeTree(struct astnode *root) {
    if (root == NULL)
        return;
    
    /* node will only have children if it is an operator */
    if (root->nodetype == BINOP_TYPE) {
        freeTree(root->binop.left);
        freeTree(root->binop.right);
    }
    else if (root->nodetype == FNC_CALL) {
        freeTree(root->fnc.ident);
        for (int i = 0; i < root->fnc.arg_count ; ++i)
            freeTree(root->fnc.arguments[i]);
    }
    else if (root->nodetype == UNOP_TYPE)
        freeTree(root->unop.expr);
    else if (root->nodetype == PTR_TYPE)
        freeTree(root->ptr.pointee);
    else if (root->nodetype == ARRAY_TYPE)
        freeTree(root->arr.ptr);
    else if (root->nodetype == FNC_TYPE) {
        freeTree(root->fnc_type.return_type);
        for (int i = 0 ; i < root->fnc_type.arg_count ; ++i)
            freeTree(root->fnc_type.args_types[i]);
    }

    free(root);
}