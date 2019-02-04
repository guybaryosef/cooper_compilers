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

#include "../lexer/lheader.h"
#include "pheader_ast.h"


/* newNode - Creates a new node for the AST.
 *
 * Given a token name and value (struct YYSTYPE is defined in 
 * lexer_header.h), this function allocates a new node for the
 * abstract syntax tree and loads in the token information into it.
 */
struct astnode *newNode(int token_name, YYSTYPE yylval) {
    
    struct astnode *node;
    if ((node = malloc(sizeof(struct astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    node->nodetype = token_name;

    switch(token_name) {
        case IDENT:
            node->ident.str = yylval.str.str; break;
        case NUMBER:
            node->num.types = yylval.num.types;
            node->num.val = yylval.num.val;
            node->num.d_val = yylval.num.d_val;
            break;
        case CHARLIT:
            node->chrlit.c_val = yylval.str.char_val; break;
        case STRING:
            node->strlit.str = yylval.str.str;
            node->strlit.str_size = yylval.str.str_size;
            break;
    }
    return node;
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
        case IDENT:
            fprintf(output, "IDENT  %s\n", cur->ident.str); break;
        case NUMBER:
            if (cur->num.types & NUMMASK_INTGR)
                fprintf(output, "CONSTANT:  numtype=int)%lld\n", cur->num.val);
            else if (cur->num.types & NUMMASK_DOUBLE)
                fprintf(output, "CONSTANT:  numtype=double)%f\n", cur->num.d_val);
            else
                fprintf(output, "CONSTANT:  numtype=float)%f\n", cur->num.d_val);
            break;
        case CHARLIT:
            fprintf(output, "CONSTANT:  (type=int)%d\n", cur->chrlit.c_val); break;
        case STRING:
            fprintf(output, "STRING  %s\n", cur->strlit.str); break;
        default:
            if (cur->binop.left) {  /* its a binary operator */
                fprintf(output, "BINARY  OP  %s\n", stringFromTokens(cur->binop.op));
                preorderTraversal(cur->binop.left, output, depth+1);
                preorderTraversal(cur->binop.right, output, depth+1);
            }
            else {  /* its a unary operator  */
                fprintf(output, "UNARY   OP  %s\n", stringFromTokens(cur->unop.op));
                preorderTraversal(cur->unop.right, output, depth+1);
            }
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