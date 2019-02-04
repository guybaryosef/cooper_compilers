/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * pheader_ast.h - A header file for the parser
 * that contains the abstract syntax tree structure
 * and associated functions:
 *  -   Create a new node.
 *  -   Print out the AST in the hakner-designated format.
 *  -   Free an AST.
 */

//#include "../lexer/lheader.h"


#ifndef PARSER_AST
#define PARSER_AST


/* struct for a token Identifer */
struct astnode_ident {
    char *str;
};

/* struct for a token numer */
struct astnode_num {
    int types;
    unsigned long long val;
    double d_val;
}; 

/* struct for a token binary operator */
struct astnode_binop {
    int op;
    struct astnode *left, *right;
};

/* struct for a token uniary operator */
struct astnode_unop {
    int op;
    struct astnode *right;
};

/* struct for a token string literal */
struct astnode_strlit {
    char *str;
    int str_size;
};

/* struct for a token character literal */
struct astnode_chrlit {
    char c_val;
};


/* struct for an Abstract Syntax Tree */
struct astnode {
    int nodetype;   /* will contain the token name from the lexer       */
    union {         /* anonymous union for the different token values   */
        struct astnode_num num;
        struct astnode_ident ident;
        struct astnode_chrlit chrlit;
        struct astnode_strlit strlit;
        struct astnode_unop unop;
        struct astnode_binop binop;
    };
} astnode;


/* newNode - Creates a new node for the AST.
 *
 * Given a token name and value (struct YYSTYPE is defined in 
 * lexer_header.h), this function allocates a new node for the
 * abstract syntax tree and loads in the token information into it.
 */
struct astnode *newNode(int token_name, YYSTYPE yylval);


/*
 * printAST - Given the root node of an Abstract Syntax Tree,
 * this function prints out the tree in a format matching 
 * Hakner's in the assignment sheet.
 * 
 * This is in essense a preorder traversal.
 */
void printAST(struct astnode *root, FILE *output_file);


/*
 * preorderTraversal - A helper function for printAST. This
 * function implements preorder traversal for the AST printing.
 */
void preorderTraversal(struct astnode *cur, FILE *output, int depth);


/* 
 * freeTree - Frees the dynamically allocated memory 
 * that was used by the Abstract Syntax Tree.
 *
 * This function does not return anything.
 */
void freeTree(struct astnode *root);

#endif