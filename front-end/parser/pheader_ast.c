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

#include "pheader_lex_comp.h"
#include "../lexer/lheader.h"
#include "../lexer/lheader2.h"
#include "pheader_ast.h"


/*
 *  newNode_num - Creates a new AST node of type num.
 */
struct astnode *newNode_num(int token_name, struct YYnum num) {
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
 * newNode_op - Creates a new AST node for type unop 
 * (unary operation).
 */
struct astnode *newNode_unop(int token_name) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = UNOP_TYPE;
    node->unop.op = token_name;
    return node;
}


/*
 * newNode_op - Creates a new AST node for type binop 
 * (binary operation).
 */
struct astnode *newNode_binop(int token_name) {
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = BINOP_TYPE;
    node->binop.op = token_name;
    return node;
}


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
            return "++";
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
}


/*
 * preorderTraversal - A helper function for printAST. This
 * function implements preorder traversal for the AST printing.
 */
void preorderTraversal(struct astnode *cur, FILE *output, int depth) {

    /* format the tab spacing correctly */
    for (int i = 0; i < depth; ++i)
        fprintf(output, "\t");

    switch(cur->nodetype) {
        case IDENT_TYPE:
            fprintf(output, "IDENT  %s\n", cur->ident.str);
            break;
        case NUM_TYPE:
            if (cur->num.types & NUMMASK_INTGR)
                fprintf(output, "CONSTANT:  (numtype=int)%lld\n", cur->num.val);
            else if (cur->num.types & NUMMASK_DOUBLE)
                fprintf(output, "CONSTANT:  (numtype=double)%f\n", cur->num.d_val);
            else
                fprintf(output, "CONSTANT:  (numtype=float)%f\n", cur->num.d_val);
            break;
        case CHRLIT_TYPE:
            fprintf(output, "CONSTANT:  (type=int)%d\n", cur->chrlit.c_val);
            break;
        case STRLIT_TYPE:
            fprintf(output, "STRING  %s\n", cur->strlit.str);
            break;
        case BINOP_TYPE:
                if (cur->binop.op < 258)
                    fprintf(output, "BINARY  OP  %c\n", cur->binop.op);
                else
                    fprintf(output, "BINARY  OP  %s\n", token2op(cur->binop.op));
                preorderTraversal(cur->binop.left, output, depth+1);
                preorderTraversal(cur->binop.right, output, depth+1);
                break;
        case UNOP_TYPE:
                if (cur->binop.op < 258)
                    fprintf(output, "BINARY  OP  %c\n", cur->unop.op);
                else
                    fprintf(output, "BINARY  OP  %s\n", token2op(cur->unop.op));
                preorderTraversal(cur->unop.right, output, depth+1);
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
    if (root->binop.left) {
        freeTree(root->binop.left);
        freeTree(root->binop.right);
    }
    else if (root->unop.right)
        freeTree(root->unop.right);

    free(root);
}